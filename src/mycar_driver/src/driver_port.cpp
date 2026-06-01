#include "rclcpp/rclcpp.hpp"
#include "mycar_driver/my_serial.hpp"


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
        1.......;
*/

class MyCarDriver :public rclcpp::Node{
public:
      MyCarDriver():Node("mycar_driver"){
        // 创建工具类对象,即可打开串口

        // 将参数设置为只读模式(程序运行后,参数不可修改)
        rcl_interfaces::msg::ParameterDescriptor desc;
        desc.read_only = true;
        // 参数声明 --------------------------------------------------------
        // 将端口号和波特率设置为参数
        this->declare_parameter<std::string>("port","/dev/ttyUSB0",desc);
        this->declare_parameter<int>("baud_rate",115200,desc);

        //参数解析 ---------------------------------------------------------
        port = this->get_parameter("port").as_string();
        baud_rate = this->get_parameter("baud_rate").as_int();

        serial_port_ = std::make_shared<my_serial::SerialPortComm>(port,baud_rate);
    }

private:
    std::shared_ptr<my_serial::SerialPortComm> serial_port_;
    std::string port; //端口号
    int baud_rate; //波特率
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