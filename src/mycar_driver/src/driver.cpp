#include "rclcpp/rclcpp.hpp"
#include "mycar_driver/my_serial.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/u_int16.hpp"
#include "std_msgs/msg/u_int16_multi_array.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "nav_msgs/msg/odometry.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include "geometry_msgs/msg/transform_stamped.hpp"
using namespace std::placeholders;

/*
    需求:在使用串口通信工具类,结合ROS2创建机器人底盘驱动
        功能点1:订阅速度指令并控制机器人运动(设置PID数据);
        功能点2:发布电池电压相关数据;
        功能点3:发布四轮编码器检测值;
        功能点4:生成并发布里程计;
        功能点5:生成并发布imu消息(描述机器人姿态).
    流程:
        先搭框架,然后按照功能点逐一实现
        0.打开串口;
        1.订阅速度指令并控制电机运动;
            1-1.订阅速度指令;
            1-2.为速度指令设置时效,比如超过2s没有新的速度指令电机停止;
            1-3.将速度指令(单位是 m/s )转换成电机可用的参数(一个PID控制周期内期望的编码器计数)
          实现思路:
            a.创建订阅方订阅速度指令,回调函数中记录最近的速度指令的下发时间(last_twist_time);
            b.创建一个定时器,定时器中,时时获取当前时刻(current_time),并且与last_twist_time比较,超时后,就不执行速度指令了;
            c.在定时器中执行速度指令时,实现单位的转换;
          提供设置 PID 参数的接口
        2.读取电池电压数据,并以话题通信的方式在ROS2中发布;
          读取并发布数据的通用流程;
          2-1.使用工具类读取下位机发送的数据; --- 使用多线程以实现异步操作,解析数据
          2-2.获取所需的数据帧对象;
          2-3.将帧对象中的数据封装成ROS2接口并发布.
              使用什么样的接口类型? --- #include "std_msgs/msg/u_int16.hpp"
              uint16类型 --- #include "std_msgs/msg/u_int16.hpp"
        3.编码器检测
          3-1.读取电机的编码器数据并发布
                 使用什么接口类型?
                 使用Uiint16的数组存储数据 --- #include "std_msgs/msg/u_int16_multi_array.hpp"
          3-2.计算里程计
                 根据编码器计数结合车辆运动学模型,获取车辆的位移信息和姿态信息
        4.IMU数据解析并发布
          把角速度,加速度以及欧拉角数据合并为  IMU  数据并发布
          角速度: x,y,z 三轴角速度
          加速度: x,y,z 三轴加速度
          欧拉角: y,x,z 三轴旋转角度(使用的单位是度,需要转换成弧度)

          使用指令可以查看ros2中的IMU消息格式:ros2 interface show(porot) sensor_msgs/msg/Imu
          该消息包含角速度,加速度,欧拉角三部分内容.

          数据合并发送思路:
            1.创建一个 IMU 类型的成员变量;
               每解析到一条数据直接赋值给IMU对应的字段
            2.保证数据的一致性?
              创建一个长度为3的 bool 类型数组,默认值是 false
              每一个元素都对应一条消息的读取状态,当前读取到某条数据时,将值改为 true
              三个元素都为 true 时,就发送IMU数据,并且将数组值置为 false
*/

class MyCarDriver :public rclcpp::Node{
public:
      MyCarDriver():Node("mycar_driver"),flag(true),first_flag(true){
        // 创建工具类对象,即可打开串口

        // 将参数设置为只读模式(程序运行后,参数不可修改)
        rcl_interfaces::msg::ParameterDescriptor desc;
        desc.read_only = true;
        // 参数声明 --------------------------------------------------------
        // 将端口号和波特率设置为参数
        this->declare_parameter<std::string>("port","/dev/ttyUSB0",desc);
        this->declare_parameter<int>("baud_rate",115200,desc);
        this->declare_parameter<double>("control_rate",10.0);
        this->declare_parameter<double>("wheel_distance",0.2);
        this->declare_parameter<double>("wheel_d",0.08);
        this->declare_parameter<int>("jian_su_bi",90,desc);
        this->declare_parameter<int>("fen_bian_lv",44,desc);
        this->declare_parameter<int>("max_enc",100);
        this->declare_parameter<int>("kp",300);
        this->declare_parameter<int>("ki",0);
        this->declare_parameter<int>("kd",200);
        this->declare_parameter<std::string>("odom_frame","odom");
        this->declare_parameter<std::string>("base_frame","base_footprint");
        this->declare_parameter<bool>("pub_odom_frame",true);

        //参数解析 ---------------------------------------------------------
        port = this->get_parameter("port").as_string();
        baud_rate = this->get_parameter("baud_rate").as_int();
        control_rate = this->get_parameter("control_rate").as_double();
        wheel_distance = this->get_parameter("wheel_distance").as_double();
        wheel_d = this->get_parameter("wheel_d").as_double();
        jian_su_bi = this->get_parameter("jian_su_bi").as_int();
        fen_bian_lv = this->get_parameter("fen_bian_lv").as_int();
        max_enc = this->get_parameter("max_enc").as_int();
        kp = this->get_parameter("kp").as_int();
        ki = this->get_parameter("ki").as_int();
        kd = this->get_parameter("kd").as_int();
        odom_frame = this->get_parameter("odom_frame").as_string();
        base_frame = this->get_parameter("base_frame").as_string();
        pub_odom_frame = this->get_parameter("pub_odom_frame").as_bool();


        last_twist_time = this->get_clock()->now();

        serial_port_ = std::make_shared<my_serial::SerialPortComm>(port,baud_rate);

        // 写入PID
        set_pid(kp,ki,kd);

        // 创建速度订阅方
        cmd_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>("cmd_vel",10,std::bind(&MyCarDriver::cmd_vel_cb,this,_1));
        // 定时器是需要设置下位机的控制频率,控制频率最好可以动态调整(动态参数)
        timer_ = this->create_wall_timer(std::chrono::milliseconds(1000 / (int)control_rate),std::bind(&MyCarDriver::on_timer,this));
        //创建电压发布方
        vol_pub_ = this->create_publisher<std_msgs::msg::UInt16>("voltage",10);
        // 创建电机编码器发布方
        encode_pub_ = this->create_publisher<std_msgs::msg::UInt16MultiArray>("encode",10);
        // 创建 IMU 消息发布方
        imu_pub_ = this->create_publisher<sensor_msgs::msg::Imu>("imu",10);
        // 创建Odom 消息发布方
        odom_pub_ = this->create_publisher<nav_msgs::msg::Odometry>("odom",10);
        // 创建坐标变换广播器
        tf_bro_ = std::make_shared<tf2_ros::TransformBroadcaster>(*this);
        // 需要解析数据(循环操作的过程)
        run();
    }
    ~MyCarDriver(){
        flag = false;
    }

private:
    std::shared_ptr<my_serial::SerialPortComm> serial_port_;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_sub_;
    // 电压发布
    rclcpp::Publisher<std_msgs::msg::UInt16>::SharedPtr vol_pub_;
    // 车轮编码器计数发布
    rclcpp::Publisher<std_msgs::msg::UInt16MultiArray>::SharedPtr encode_pub_;
    // IMU消息发布方
    rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr imu_pub_;
    // Odom 消息发布方
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;
    // 坐标变化广播器
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_bro_;
    rclcpp::TimerBase::SharedPtr timer_;
    /*
        以多线程的方式解析数据
    */
    void run(){
        std::thread(std::bind(&MyCarDriver::parse_msg,this)).detach();
    }
    // 存储解析数据的业务逻辑
    void parse_msg(){
        // 调用循环工具类中读取数据帧的函数,取出每一帧数据并作针对向的处理
        while (rclcpp::ok() && flag)  //添加flag判断主线程是否活跃(防止主线程死掉,子线程还在运行)
        {
            // 读取数据帧,可能会返回空指针nullptr
            auto msg = serial_port_->read_message();
            // 处理空指针
            if (msg == nullptr)
            {
                continue;
            }
            //处理有效数据
            if (msg->type == my_serial::MessageType::VOLTAGE)
            {
                // 组织电压数据
                std_msgs::msg::UInt16 vol;
                // 将两个字节合并成一个short数据
                vol.data = ((msg->data[0] << 8) & 0xff00) | (msg->data[1] & 0x00ff);
                // 发布
                vol_pub_->publish(vol);
            }
            else if(msg->type == my_serial::MessageType::WHEEL_ENCODER)
            {
                // 1.读取到编码器计数并发布
                // 组织数据
                std_msgs::msg::UInt16MultiArray encode;
                // 将8个数据位两两一组变成4个Uint16类型数据,然后添加进 encode.data(vector)
                for (size_t i = 0; i < 4; i++)
                {
                    encode.data.push_back(((msg->data[i*2] << 8) & 0xff00) | (msg->data[i*2 + 1]) & 0x00ff);
                }
                
                // 发布数据
                encode_pub_->publish(encode);
                // 2.计算里程计
                handle_encode_message(encode.data[0],encode.data[1]);
            }
            else if(msg->type == my_serial::MessageType::ANGULAR_VEL)
            {
                // 要将标记改为true
                imu_msg_flag[0] = true;
                // 将数据解析并赋值给 imu 对象
                imu.angular_velocity.x = (int16_t)(((msg->data[0] << 8) & 0xff00) | (msg->data[1] & 0x00ff)) * 0.1;
                imu.angular_velocity.y = (int16_t)(((msg->data[2] << 8) & 0xff00) | (msg->data[3] & 0x00ff)) * 0.1;
                imu.angular_velocity.z = (int16_t)(((msg->data[4] << 8) & 0xff00) | (msg->data[5] & 0x00ff)) * 0.1;
                // 判断其他数据状态,并作出处理(都是true,那么数据就发布)
                check_pub_imu();
            }
            else if(msg->type == my_serial::MessageType::ACCEL)
            {
                // 要将标记改为true
                imu_msg_flag[1] = true;
                // 将数据解析并赋值给 imu 对象
                imu.linear_acceleration.x = (int16_t)(((msg->data[0] << 8) & 0xff00) | (msg->data[1] & 0x00ff)) * 0.1;
                imu.linear_acceleration.y = (int16_t)(((msg->data[2] << 8) & 0xff00) | (msg->data[3] & 0x00ff)) * 0.1;
                imu.linear_acceleration.z = (int16_t)(((msg->data[4] << 8) & 0xff00) | (msg->data[5] & 0x00ff)) * 0.1;
                // 判断其他数据状态,并作出处理(都是true,那么数据就发布)
                check_pub_imu();
            }
            else if(msg->type == my_serial::MessageType::EULAR)
            {
                // 要将标记改为true
                imu_msg_flag[2] = true;
                // 将数据解析并赋值给 imu 对象
                // 将欧拉角转换成四元数
                tf2::Quaternion qtn;
                qtn.setRPY(
                    // R (翻滚角度)
                    (int16_t)(((msg->data[2] << 8) | 0xff) | (msg->data[3] & 0x00ff)) * 0.1 * (2 * M_PI / 360),
                    // P
                    (int16_t)(((msg->data[0] << 8) | 0xff) | (msg->data[1] & 0x00ff)) * 0.1 * (2 * M_PI / 360),
                    // Y
                    (int16_t)(((msg->data[4] << 8) | 0xff) | (msg->data[5] & 0x00ff)) * 0.1 * (2 * M_PI / 360)
                );
                imu.orientation.x = qtn.getX();
                imu.orientation.y = qtn.getY();
                imu.orientation.z = qtn.getZ();
                imu.orientation.w = qtn.getW();
                // 判断其他数据状态,并作出处理(都是true,那么数据就发布)
                check_pub_imu();
            }
            
        }
        
    }
    /*
        计算两个记录点之间,车轮旋转的量(由编码器计数获取)
        参数1:current_encode 当前编码器计数
        参数2:上一次记录点的编码器计数
        返回值:差值
    */
    int get_delta(int current_encode,int last_encode)
    {
        // 1.获取二者差的绝对值,判断绝对值是否小于等于 max_enc * 1.5(系数)
        // 如果小于等于为true,那么是正常旋转
        if (std::abs(current_encode - last_encode) <= max_enc * 1.5)
        {
            return current_encode - last_encode;
        }
        else if (current_encode < last_encode) // 否则,如果正转..... 100 65000
        {
            return current_encode + 65535 - last_encode;
        }
        else if (current_encode > last_encode) // 反转..... 65000 100
        {
            return current_encode - 65535 - last_encode;
        } 
    }
    /*
        根据编码器读数,计算里程计
        计算原理:
            1.(当前编码器计数 - 上次编码器计数) 获取某个时段内的位移信息;
            当前编码器计数和上次编码器计数还有时间差值,进而就可以计算速度信息了
    */
    void handle_encode_message(int left_encode, int right_encode){
        // 判断是不是第一次处理编码器数据,如果是,设置里程计运算所需要的初始值
        if (first_flag)
        {
            // 初始化机器人的初始姿态信息
            x = y = theta = 0.0;
            last_left_encode = left_encode;
            last_right_encode = right_encode;
            last_encode_time = this->get_clock()->now();
            first_flag = false;
            return;
        }
        // 根据当前编码器计数以及时间计算里程计.
        // ------------------------------------------------------
        // 获取左右轮的位移和时间差,进而计算车的位移量和姿态,以及速度
        // 计算左右轮运行了多少?
        /*
            简单的运算会出现BUG,是在临界值时出现BUG
            BUG描述:运行一段距离后,里程计出现跳变甚至飞了.
            BUG原因:对临界值处理不当.
                    举例1:前转时,上一次记录点65000,当前记录点100,实际旋转的量 100 + (65535 - 65000),当前算法: 100 - 65000;
                    举例2:后转时,上一次记录点1000,当前记录点65000,实际旋转的量 -(65335 - 65000) - 100,当前算法 65000 - 100;
            BUG解决:单独处理临界值情况:

        */
        //int left_delta = left_encode - last_left_encode;
        //int right_delta = right_encode - last_right_encode;
        int left_delta = get_delta(left_encode,last_left_encode);
        int right_delta = get_delta(right_encode,last_right_encode);
        // 时间差
        rclcpp::Time current_time = this->get_clock()->now();
        double time_delta = (current_time - last_encode_time).seconds();

        // 单位转换: 将编码器计数转换成 m
        // 计算每个编码器计数对应行进距离 = 周长 / 车轮旋转一圈的编码器计数 = PI * 车轮直径 / (电机减速比 * 编码器分辨率)
        double resoulation = M_PI * wheel_d / (jian_su_bi * fen_bian_lv);
        // 车辆行进距离(m)
        // double delta_dis = (left_delta * resoulation + right_delta * resoulation) / 2;
        double delta_dis = (left_delta + right_delta) * resoulation / 2;
        // 旋转角度(rad)
        // double delta_theta = (right_delta * resoulation - left_delta * resoulation) / wheel_distance;
        double delta_theta = (right_delta - left_delta) * resoulation / wheel_distance;

        // 计算车俩的速度(线速度和角速度)
        double vel_dis = delta_dis / time_delta;
        double vel_theta = delta_theta / time_delta;

        // --------------------------------------------------------
        // 计算车辆在 X 和 Y 轴上的偏移(相当于上一个记录点)
        double delta_x, delta_y;
        if (delta_theta == 0)
        {
            delta_x = delta_dis;
            delta_y = 0.0;
        }
        else
        {
            delta_x = delta_dis * (sin(delta_theta) / delta_theta);
            delta_y = delta_dis * (1 - cos(delta_theta)) / delta_theta;
        }
        // 车辆相对于原点 x 坐标, y 坐标以及旋转角度
        x += cos(theta) * delta_x - sin(theta) * delta_y;
        y += sin(theta) * delta_x + cos(theta) * delta_y;
        theta += delta_theta;

        // 计算完毕后,需要使用left_encode,right_encode,current_time去覆盖对应的 last_xxxx
        last_left_encode = left_encode;
        last_right_encode = right_encode;
        last_encode_time = current_time;

        // 和ROS2集成
        // 发布里程计消息
        nav_msgs::msg::Odometry odom;
        odom.header.stamp = current_time;
        odom.header.frame_id = odom_frame; // 原点
        odom.child_frame_id = base_frame;      // 机器人的基坐标系
        // 里程计中机器人的位置信息
        odom.pose.pose.position.x = x;
        odom.pose.pose.position.y = y;
        odom.pose.pose.position.z = 0.0;     // 在地面不用设置坐标
        // 里程计中机器人的位姿信息(四元数) --- 根据欧拉角转换
        tf2::Quaternion qtn;
        qtn.setRPY(0.0, 0.0, theta);
        odom.pose.pose.orientation.x = qtn.getX();
        odom.pose.pose.orientation.y = qtn.getY();
        odom.pose.pose.orientation.z = qtn.getZ();
        odom.pose.pose.orientation.w = qtn.getW();
        // 设置里程计的速度数据
        odom.twist.twist.linear.x = vel_dis;
        odom.twist.twist.linear.y = 0.0;
        odom.twist.twist.linear.z = 0.0;

        odom.twist.twist.angular.x = 0.0;
        odom.twist.twist.angular.y = 0.0;
        odom.twist.twist.angular.z = vel_theta;

        odom_pub_->publish(odom);

        // 发布坐标变换 -- 不是必须的
        if (!pub_odom_frame)
        {
            return;
        }
        // 坐标变换具体实现
        geometry_msgs::msg::TransformStamped ts;
        // 组织内容
        ts.header.stamp = current_time;
        ts.header.frame_id = odom_frame;
        ts.child_frame_id = base_frame;
        ts.transform.translation.x = x;
        ts.transform.translation.y = y;
        ts.transform.translation.z = 0.0;
        ts.transform.rotation.x = qtn.getX();
        ts.transform.rotation.y = qtn.getY();
        ts.transform.rotation.z = qtn.getZ();
        ts.transform.rotation.w = qtn.getW();
        tf_bro_->sendTransform(ts);

    }

    // 检测不同消息的状态,以及处理数据发布
    void check_pub_imu(){
        // 判断三个标记,如果都是这个true,那么发布 IMU,把所有标记设置为 false
        if(imu_msg_flag[0] && imu_msg_flag[1] && imu_msg_flag[2])
        {
            // 组织并发布消息
            imu.header.stamp = this->get_clock()->now();
            imu.header.frame_id = "imu_link";
            // imu 其他数据字段在下位机数据被解析时已经设置了
            imu_pub_->publish(imu);
            // 把所有标记设置为 false
            std::fill_n(std::begin(imu_msg_flag),3,false);
        }
    }
    // 设置PID的函数
    void set_pid(int kp,int ki,int kd)
    {
        serial_port_->write_pid(kp,ki,kd);
    }
    void on_timer(){
        double linear, angular; // 存储线速度和角速度
        // 1.判断速度指令时效
        rclcpp::Time current_time = this->get_clock()->now();
        if ((current_time - last_twist_time).seconds() <= 1.0)
        {
            // RCLCPP_INFO(this->get_logger(),"速度下达"); 日志
            linear = twist_timer.linear.x;
            angular = twist_timer.angular.z;
        }
        else
        {
            // RCLCPP_INFO(this->get_logger(),"停止"); 日志
            // 控制电机停止
            serial_port_->write_motor_stop();
            return;
        }
        // 2.实现速度转换
        // 将速度指令(单位是 m/s )转换成电机可用的参数(一个PID控制周期内期望的编码器计数)
        // 2-0.将速度转换成左右车轮的速度(m/s)
        //double left_wheel = linear - (角速度相关计算公式);
        //double wright_wheel = linear + (角速度相关计算公式)
        double left_wheel = linear - angular * wheel_distance / 2;
        double right_wheel = linear + angular * wheel_distance / 2;
        // 2-1.将 m/s 转换成转速 r/s 
        // 速度 /  车轮的周长
        double left_wheel_r = left_wheel / (M_PI * wheel_d);
        double right_wheel_r = right_wheel / (M_PI * wheel_d);

        // 2-2.将 r/s 转换成编码器计数/s
        // 乘 车轮旋转一圈的编码器计数
        // double left_wheel_enc = left_wheel_r * (一圈编码器计数 = 减速比 * 编码器分辨率);
        double left_wheel_enc = left_wheel_r * (jian_su_bi * fen_bian_lv);
        double right_wheel_enc = right_wheel_r * (jian_su_bi * fen_bian_lv);

        // 2-3.将编码器计数/s 转换成 编码器计数/PID周期(40ms)
        // 编码器计数/s/控制频率(25Hz) 或 编码器计数/s * 时间(0.04s)
        double left = left_wheel_enc / serial_port_->get_pid_rate();
        double right = right_wheel_enc / serial_port_->get_pid_rate();
        /*
            BUG描述: 当速度比较快时,不能既有线速度又有角速度(左前转弯实现不了,只能前进)
            原   因: 电机转速是有上限的,当给定的线速度过高时,即便左右轮计算的结果速度有区别,
                     但是由于计算的值较大,左右电机都只能极速运转(但到不了给定速度),因此出现
                     不能转弯的BUG了
            修   改: 
                     1.电机速度最值 100
                     2.获取左右电机速度,并且取出最大绝对值
                     3.让步骤2中的最大值和步骤1的最大值相除,获取一个比例 如果小于等于1 取 1.0,否则就取计算的比例
                     4.速度下发之前,让左右电机都除以这个比例
        */
        double bi_li = std::max(std::max(std::abs(left),std::abs(right)) / max_enc, 1.0);
        short l = static_cast<short>(left / bi_li);
        short r = static_cast<short>(right / bi_li);

        // 3.速度写出
        serial_port_->write_motor_diff(l,r);
    }
    // 订阅速度的回调
    void cmd_vel_cb(const geometry_msgs::msg::Twist & twist)
    {
        // RCLCPP_INFO(this->get_logger(),"线速度:%.2f,角速度:%.2f",twist.linear.x,twist.angular.z);
        // 记录订阅到当前指令的时刻
        last_twist_time = this->get_clock()->now();
        twist_timer = twist;
    };
 
    std::string port;              // 端口号
    int baud_rate;                 // 波特率
    double control_rate;           // 上位机对下位机控制频率
    rclcpp::Time last_twist_time;  // 最新速度指令时刻
    // 接收最新速度指令的变量
    geometry_msgs::msg::Twist twist_timer;
    double wheel_distance;         // 轮间距
    double wheel_d;                // 车轮直径
    int jian_su_bi,fen_bian_lv;    // 电机的减速比和分辨率
    int max_enc;                   // 电机最大转速(一个PID控制周期内编码器的最大计数)
    int kp,ki,kd;                  // 电机整定的PID参数
    bool flag;                     // 标记:标记主线程是否处于活跃状态
    sensor_msgs::msg::Imu imu;     // 存储IMU消息的变量
    bool imu_msg_flag[3]{false,false,false};

    // 声明两个变量,分别用于储存左右车轮初始的编码器计数
    int last_left_encode, last_right_encode;
    rclcpp::Time last_encode_time;
    bool first_flag;               // 记录是不是第一次解析编码器计数
    // 机器人初始姿态信息(由于是2D机器人,只需要设计x,y坐标和航线角theta)
    double x,y,theta;
    std::string odom_frame,base_frame;   // 原点坐标系与小车基坐标系
    bool pub_odom_frame;                 // 是否发布里程计相关坐标变换

};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<MyCarDriver>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}