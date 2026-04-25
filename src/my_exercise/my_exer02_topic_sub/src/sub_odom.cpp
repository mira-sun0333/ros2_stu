#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/odometry.hpp"
using namespace std::placeholders;


/*
    需求:订阅里程计消息,每当机器人位移超过X米时,输出机器人坐标
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1订阅里程计消息;
            3-2解析里程计消息并计算位移是否超出X米(和上一次的记录点作比较)
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class SubOdom :public rclcpp::Node{
public:
    SubOdom():Node("sub_odom_node_cpp"){
        RCLCPP_INFO(this->get_logger(),"创建成功");
        last_x = 0.0;
        last_y = 0.0;
        // 动态调参 不写死
        this->declare_parameter<double>("distance",0.5);
        // 3-1订阅里程计消息
    }

private:
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub_;
    double last_x,last_y; // 上一次的记录点的x坐标和y坐标
    // 3-2解析里程计消息并计算位移是否超出X米(和上一次的记录点作比较)
    void on_timer(const nav_msgs::msg::Odometry &odom)
    {
      // 里程计包含机器人的坐标信息和速度信息
      double current_x = odom.pose.pose.position.x; // 当前x坐标
      double current_y = odom.pose.pose.position.y; // 当前y坐标
      // 计算间距
      double distance = sqrt(pow(current_x - last_x,2) + pow(current_y - last_y,2));
      // 计算是否大于指定间距
      if(distance >= this->get_parameter("distance").as_double()) // 动态调参
      {
        RCLCPP_INFO(this->get_logger(),"当前机器人坐标(%.2f,%.2f)",current_x,current_y);
        //易错点
        last_x = current_x;
        last_y = current_y;
      }
      // 如果大于,那么将当前坐标在终端输出,并且把当前坐标赋值给记录点
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<SubOdom>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}
