#include "rclcpp/rclcpp.hpp"
#include "mycar_driver/my_serial.hpp"

/*
    需求:实现小车的底盘驱动
    阶段1:搭建代码框架
      1.创建一个头文件,实现串口通信功能
      2.当前源文件,调用头文件,关注的时ROS2相关业务逻辑
      这样设计的优点:提高内聚性,降低耦合性,方便后期维护扩展.
    阶段2:编写并测试头文件功能点
      1.打开以及关闭串口
      2.读实现
      3.写实现
*/

class MyNode :public rclcpp::Node{
public:
      MyNode():Node("mynode_node_cpp"){
          serial_port_ = std::make_shared<my_serial::SerialPortComm>("/dev/ttyUSB0",115200);
          // serial_port_->read_print_hex();
          // serial_port_->write_motor_speed(0,0,0,0);
          // serial_port_->write_motor_speed(90,90,90,90);
          // serial_port_->write_motor_diff(90,90);
          // serial_port_->write_motor_stop(0,0);

          // pid设置
          // serial_port_->write_pid(300,0,200);默认
          // serial_port_->write_pid(30,100,200);
          // serial_port_->write_motor_diff(90,90); 不能和上一条同时使用 先运行这一条,在运行上一条.

          // 舵机测试
          //serial_port_->write_servo_control(60,0);

          //测试消息读取函数
          while (rclcpp::ok())
          {
                auto msg = serial_port_->read_message();
                if (msg != nullptr)
                {
                    RCLCPP_INFO(this->get_logger(),"消息类型:%d",msg->type);
                }
                else
                {
                    RCLCPP_INFO(this->get_logger(),"----------------------");
                }
                
          }
        
    }

private:
    std::shared_ptr<my_serial::SerialPortComm> serial_port_;
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<MyNode>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}