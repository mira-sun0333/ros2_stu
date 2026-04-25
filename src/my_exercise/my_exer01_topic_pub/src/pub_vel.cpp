#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
using namespace std::chrono_literals;

/*
    需求:编写发布方实现,发布速度指令,控制机器人作圆周运动
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1 创建消息发布方(发布速度指令)
            3-2 创建定时器
            3-3 定时器回调函数中组织并发布速度指令
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class PubVel :public rclcpp::Node{
public:
    PubVel():Node("pub_vel_node_cpp"){
        RCLCPP_INFO(this->get_logger(),"创建成功!");
        // 3-0 设置动态参数修改
        this->declare_parameter<double>("linear",0.0);
        this->declare_parameter<double>("angular",0.0);
        // 3-1 创建消息发布方(发布速度指令)
        pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel",10);
        // 3-2 创建定时器
        timer_ = this->create_wall_timer(100ms,std::bind(&PubVel::cb,this));

    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    // 3-3 定时器回调函数中组织并发布速度指令
    void cb()
    {
      geometry_msgs::msg::Twist twist;
      twist.linear.x = this->get_parameter("linear").as_double();
      twist.angular.z = this->get_parameter("angular").as_double();
      pub_->publish(twist);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<PubVel>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}
