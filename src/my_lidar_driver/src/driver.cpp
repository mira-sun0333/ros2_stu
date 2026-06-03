#include "rclcpp/rclcpp.hpp"
#include "../sdk/include/bluesea.h"
#include "sensor_msgs/msg/laser_scan.cpp"
#include "my_lidar_srv/srv/control.cpp"
using namespace std::placeholders;
/*
    需求: 自实现激光雷达驱动,主要功能点如下:
        1.能够广播激光雷达消息;
        2.能够控制激光雷达起停,并能设置刷新频率;
    分析:
        1.结合ROS2的publisher和雷达的SDK广播雷达消息;
        2.结合ROS2的service和SDK设置雷达参数;
            通过客户端发送请求到该节点,该节点在调用SDK的发送指令的功能控制雷达,并响应结果给客户端;
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1. 通过SDK连接雷达;
            3-2. 创建发布对象并发布雷达消息;
            3-3. 创建服务端,解析请求并设置雷达参数;
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class LidarDriver :public rclcpp::Node{
public:
    LidarDriver():Node("lidar_driver_node"){
        // 3-1. 通过SDK连接雷达;
        connect_lidar();
        // 3-2. 创建发布对象并发布雷达消息;
        laser_pub_ = this->create_publisher<sensor_msgs::msg::LaerScan>("scan",10);
        timer_ = this->create_wall_timer(std::chrono::millseconds(100),std::bind(&LidarDriver::on_timer,this));
        // 3-3. 创建服务端,解析请求并设置雷达参数;
        control_service_ = this->create_service<my_lidar_srv::srv::Control>("control",std::bind(&LidarDriver::control,this,_1,_2));
    }

private:
    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr laser_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Service<my_lidar_srv::srv::Control>::SharedPtr control_service_;
    std::shared_ptr<BlueSeaLidarDriver> sdk_driver_;
    ArgData argdata;
    // 向雷达发送指令的回调函数
    void control(const my_lidar_srv::srv::Control::Request::SharedPtr req, const my_lidar_srv::srv::Control::Response::SharedPtr res)
    {
        std::string topic = "scan";
        std::string cmd;
        ushort proto = 0x0043;
        // 解析客户端请求数据
        // start
        // stop
        // rate 10.0
        if(req->func == "start")
        {
            cmd = "LSTARH";
        }
        else if(req->func == "stop")
        {
            cmd = "LSTOPH"
        }
        else if(req->func == "rate")
        {
            // 获取刷新频率,再将频率转换成雷达电机的转速
            int rpm = int(req->param * 60);
            cmd = "LSRPM:" + std::to_string(rpm) + "H";
            proto = 0x0053;
        }
        // 向雷达发送指令
        // sendCmd("scan",xxxx,proto);
        bool flag = sdk_driver_->sendCmd(topic,cmd,proto);
        // 产生响应结果
        if(flag)
        {
            res->code = 1;
            res->value = "雷达设置成功";
        }
        else
        {
            res->code = 1;
            res->value = "雷达设置失败";
        }
    }
    // 发布雷达消息的回调函数
    void on_timer()
    {
        // 获取SDK读取的雷达消息,然后转换成ROS2的LaserScan,最后再发布
        // 获取一帧雷达数据
        auto hub = sdk_driver_->getHub(0);
        // 判断雷达是否正常运转
        if(hub->nfan == 0)
        {
            return;
        }
        if(hub->offsetangle == -1)
        {
            sdk_driver_->checkIsRun(0);
            return;
        }
        // 解析数据
        int8_t falg = false;
        int fans = sdk_driver_->GetAllFans(hub,argdata,falg);
        if(fans > 0)
        {
            // 将一帧雷达数据,转换成LaserScan消息
            /*
            SDK中的数据结构
struct PubHub // 存储一帧扫描到的数据
{
    pthread_mutex_t mtx;           // 互斥锁，用于线程安全
    int nfan;                      // 扇区数量
    RawData *fans[MAX_FANS];       // 扇区数组，指向RawData的指针
    int error_num;                 // 错误数量
    int offsetangle;               // 偏移角度
    int offsetidx;                 // 偏移索引
    std::vector<DataPoint> consume;// 统计完剩余的点数
    uint32_t ts_beg[2];            // 开始时间戳，两个32位无符号整数
    uint32_t ts_end[2];            // 结束时间戳，两个32位无符号整数
};
struct ConnectArg // 设置端口、波特率、雷达话题、点云话题等参数
{
    std::string scan_topics;   // 扫描数据的话题名称，用于指定发布或订阅扫描数据的话题
    std::string cloud_topics;  // 点云数据的话题名称，用于指定发布或订阅点云数据的话题
    std::string arg1;          // 第一个自定义参数
    int arg2;                  // 第二个自定义参数，为整数类型
};
            ROS2的数据结构
            
            */
            // 消息转换
            sensor_msgs::msg::LaserScan laser_scan;
            int points = hub->consume.size();
            // 设置消息字段
            // 设置消息头
            laser_scan.header.frame_id = argdata.frame_id;
            laser_scan.header.stamp.sec = hub->ts_beg[0];
            laser_scan.header.stamp.nanosec = hub->ts_beg[1];
            // 基本的雷达参数
            laser_scan.angle_min = argdata.min_angle; // 最小扫描角度
            laser_scan.angle_max = argdata.max_angle; // 最大扫描角度
            laser_scan.angle_increament = M_PI * 2 / points; // 角距离(两个脉冲的角度间隔)扫描范围 / 扫描点数
            // 获取开始时间和结束时间
            double beg = double(hub->ts_beg[0]) + double(hub->ts_beg[1]) / 1000000000.0;
            double end = double(hub->ts_end[0]) + double(hub->ts_end[1]) / 1000000000.0;
            double scan_time = end - beg;
            laser_scan.scan_time = scan_time;
            laser_scan.time_increment = scan_time / points;
            // 最小扫描距离
            laser_scan.range_min = argdata.min_dist;
            laser_scan.range_max = argdata.max_dist;
            // 距离和强度
            laser_scan.ranges.resize(points);
            laser_scan.intensities.resize(points);
            // 遍历
            // 消息发布扫描点数据,分别设置
            for(size_t i = 0; i < hub->consume.size(); i++)
            {
                auto data_point = hub->consume[i];
                laser_scan.ranges[i] = data_point.distance;  // 单位换算,将mm换算成m
                laser_scan.intensities[i] = data_point.confidence;
            }
            // 数据反转
            if(argdara.reversed)
            {
                std::reverse(laser_scan.ranges.begin(),laser_scan.ranges.end());
                std::reverse(laser_scan.intensities.begin(),laser_scan.intensities.end());
            }
            // 消息发布
            laser_pub_->publish(laser_scan);
        }
        // 最后清空缓存
        hub->consume.clear();
    }
    void connect_lidar()
    {
        // 创建SDK雷达驱动
        sdk_driver_ = std::make_shared<BlueSeaLidarDriver>();
        // 设置初始化参数
        init_argdata();
        sdk_driver_->getInitCmds(argdata);
        // 打开雷达线程
        sdk_driver_->openLidarThread();
    }
    // 初始化argdata
    void init_argdata()
    {
        // 雷达连接参数要灵活配置,可以通过ROS2的参数服务从外部导入数据,然后传递给argdata
         // 设置参数
        this->declare_parameter("num", 1);
        this->declare_parameter("type", "uart");
        this->declare_parameter("frame_id", "laser");
        this->declare_parameter("dev_id", -1);
        argdata.num = this->get_parameter("num").as_int(); // 雷达个数
        argdata.type = this->get_parameter("type").as_string();
        argdata.frame_id = this->get_parameter("frame_id").as_string();
        argdata.dev_id = this->get_parameter("dev_id").as_int();
        this->declare_parameter("port", "/dev/LHLiDAR");
        this->declare_parameter("baud_rate", -1);
        this->declare_parameter("scan_topic", "scan");
        this->declare_parameter("cloud_topic", "cloud");
        ConnectArg arg;
        arg.arg1 = this->get_parameter("port").as_string();
        arg.arg2 = this->get_parameter("baud_rate").as_int();
        arg.scan_topics = this->get_parameter("scan_topic").as_string();
        arg.cloud_topics = this->get_parameter("cloud_topic").as_string();
        argdata.connectargs.push_back(arg);
        this->declare_parameter("localport", 6668);
        this->declare_parameter("is_group_listener", false);
        this->declare_parameter("group_ip", "224.1.1.91");
        this->declare_parameter("raw_bytes", 3);
        this->declare_parameter("inverted", false);
        this->declare_parameter("reversed", false);
        this->declare_parameter("output_scan", false);
        this->declare_parameter("output_cloud", false);
        this->declare_parameter("output_cloud2", false);
        argdata.localport = this->get_parameter("localport").as_int();
        argdata.custom.is_group_listener = this->get_parameter("is_group_listener").as_bool();
        argdata.custom.group_ip = this->get_parameter("group_ip").as_string();
        argdata.raw_bytes = this->get_parameter("raw_bytes").as_int();
        argdata.inverted = this->get_parameter("inverted").as_bool();
        argdata.reversed = this->get_parameter("reversed").as_bool();
        argdata.output_scan = this->get_parameter("output_scan").as_bool();
        argdata.output_cloud = this->get_parameter("output_cloud").as_bool();
        argdata.output_cloud2 = this->get_parameter("output_cloud2").as_bool();
        this->declare_parameter("output_360", true);
        this->declare_parameter("with_angle_filter", false);
        this->declare_parameter("from_zero", false);
        this->declare_parameter("fitter_isopen", false);
        argdata.output_360 = this->get_parameter("output_360").as_bool();
        argdata.with_angle_filter = this->get_parameter("with_angle_filter").as_bool();
        argdata.from_zero = this->get_parameter("from_zero").as_bool();
        argdata.fitter.isopen = this->get_parameter("fitter_isopen").as_bool();
        this->declare_parameter("min_angle", -M_PI);
        this->declare_parameter("max_angle", M_PI);
        this->declare_parameter("min_dist", 0.1);
        this->declare_parameter("max_dist", 12.0);
        this->declare_parameter("custom_error_scale", 0.9);
        argdata.min_angle = this->get_parameter("min_angle").as_double();
        argdata.max_angle = this->get_parameter("max_angle").as_double();
        argdata.min_dist = this->get_parameter("min_dist").as_double();
        argdata.max_dist = this->get_parameter("max_dist").as_double();
        argdata.custom.error_scale = this->get_parameter("custom_error_scale").as_double();
        this->declare_parameter("time_mode", 0);
        this->declare_parameter("custom_error_circle", 3);
        this->declare_parameter("fitter_type", 1);
        this->declare_parameter("fitter_filter_window", 1);
        argdata.time_mode = this->get_parameter("time_mode").as_int();
        argdata.custom.error_circle = this->get_parameter("custom_error_circle").as_int();
        argdata.fitter.type = this->get_parameter("fitter_type").as_int();
        argdata.fitter.filter_window = this->get_parameter("fitter_filter_window").as_int();
        this->declare_parameter("fitter_max_range", 7.0);
        this->declare_parameter("fitter_min_range", 0.15);
        this->declare_parameter("fitter_max_range_difference", 0.1);
        argdata.fitter.max_range = this->get_parameter("fitter_max_range").as_double();
        argdata.fitter.min_range = this->get_parameter("fitter_min_range").as_double();
        argdata.fitter.max_range_difference = this->get_parameter("fitter_max_range_difference").as_double();
        //
        this->declare_parameter("uuid", -1);
        this->declare_parameter("rpm", -1);
        argdata.uuid = this->get_parameter("uuid").as_int();
        argdata.rpm = this->get_parameter("rpm").as_int();
        //
        this->declare_parameter("hard_resample", false);
        this->declare_parameter("soft_resample", false);
        argdata.hard_resample = this->get_parameter("hard_resample").as_bool(); 
        argdata.soft_resample = this->get_parameter("soft_resample").as_bool();
        this->declare_parameter("resample", -1.0);
        argdata.resample = this->get_parameter("resample").as_double();
        this->declare_parameter("with_smooth", -1);
        this->declare_parameter("with_deshadow", -1)
        this->declare_parameter("alarm_msg", -1);
        this->declare_parameter("direction", -1);
        this->declare_parameter("unit_is_mm", -1);
        this->declare_parameter("with_confidence", -1);
        this->declare_parameter("ats", -1);
        argdata.with_smooth = this->get_parameter("with_smooth").as_int();
        argdata.with_deshadow = this->get_parameter("with_deshadow").as_int();
        argdata.alarm_msg = this->get_parameter("alarm_msg").as_int();
        argdata.direction = this->get_parameter("direction").as_int();
        argdata.unit_is_mm = this->get_parameter("unit_is_mm").as_int();
        argdata.with_confidence = this->get_parameter("with_confidence").as_int();
        argdata.ats = this->get_parameter("ats").as_int();
        this->declare_parameter("range_min", -180.0);
        this->declare_parameter("range_max", 180.0);
        Range range;
        range.min = this->get_parameter("range_min").as_double();
        range.max = this->get_parameter("range_max").as_double();
        argdata.masks.push_back(range);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<LidarDriver>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}