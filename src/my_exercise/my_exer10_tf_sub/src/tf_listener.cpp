#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"
#include "geometry_msgs/msg/twist.hpp"
using namespace std::chrono_literals;
/*
    需求:订阅坐标变换解析出机器人B和goal的位姿关系,并生成控制B运动的速度指令
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建一个缓存池
            3-2创建坐标变换监听方,会将订阅的数据存入缓存池
            3-3创建速度指令发布方
            3-4创建一个定时器,周期性的解析坐标变换,生成并发布速度指令
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class TFListener :public rclcpp::Node{
public:
    TFListener():Node("tf_listener_node_cpp"){
      // 将参与坐标变换的两个坐标系封装为参数
      this->declare_parameter("frame_id","robot_0/base_link");
      this->declare_parameter("child_frame_id","goal");
      // 3-1创建一个缓存池
      buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
      // 3-2创建坐标变换监听方,会将订阅的数据存入缓存池
      listener_ = std::make_shared<tf2_ros::TransformListener>(*buffer_);
      // 3-3创建速度指令发布方
      pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/robot_0/cmd_vel",10);
      // 3-4创建一个定时器,周期性的解析坐标变换,生成并发布速度指令
      timer_ = this->create_wall_timer(0.1s,std::bind(&TFListener::on_timer,this));
    }

private:
    std::shared_ptr<tf2_ros::Buffer> buffer_;
    std::shared_ptr<tf2_ros::TransformListener> listener_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    void on_timer()
    {
      // 周期性的解析坐标变换,生成并发布速度指令
      // 解析坐标变换
      geometry_msgs::msg::TransformStamped ts;
      try
      {
        auto ts =buffer_->lookupTransform(this->get_parameter("frame_id").as_string(),this->get_parameter("child_frame_id").as_string(),tf2::TimePointZero);
      }
      catch(const std::exception& e)
      {
        RCLCPP_ERROR(this->get_logger(),e.what());
        return;
      }
      // 生成并发布速度指令
      geometry_msgs::msg::Twist twist;
      // 组织数据
      // 在 TransformStamped中包含两个坐标系的x y 和偏移量,可以根据这两个值生成角速度和线速度
      twist.linear.x = 0.5 * sqrt(pow(ts.transform.translation.x,2) + pow(ts.transform.translation.y,2));
      twist.angular.z = 1.0 * atan2(ts.transform.translation.y,ts.transform.translation.x);
      pub_->publish(twist);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<TFListener>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}