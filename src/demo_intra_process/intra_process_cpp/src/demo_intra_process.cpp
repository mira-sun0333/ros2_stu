/*
    需求:将发布和订阅方集成到同一进程内,以实现数据的首发
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.创建发布方和订阅方
        4.将发布方对象和订阅方对象集成到同一进程
        5.释放资源
*/

// 1.包含头文件；
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
using namespace std::chrono_literals;
using std::placeholders::_1;
// 3.定义节点类；
// 通信时,节点默认通过DDS实现,如果要实现高效的进程内通信,需要在构造函数中设置参数:rclcpp::NodeOptions().use_intra_process_comms(true)
// 允许进程内通信
class MinimalPublisher : public rclcpp::Node
{
  public:
    MinimalPublisher()
    : Node("minimal_publisher",rclcpp::NodeOptions().use_intra_process_comms(true)), count_(0)
    {
      // 3-1.创建发布方；
      publisher_ = this->create_publisher<std_msgs::msg::String>("topic", 10);
      // 3-2.创建定时器；
      timer_ = this->create_wall_timer(500ms, std::bind(&MinimalPublisher::timer_callback, this));
    }

  private:
    void timer_callback()
    {
      // 3-3.组织消息并发布。
      auto message = std_msgs::msg::String();
      message.data = "Hello, world! " + std::to_string(count_++);
      RCLCPP_INFO(this->get_logger(), "发布的消息：'%s'", message.data.c_str());
      publisher_->publish(message);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    size_t count_;
};
class MinimalSubscriber : public rclcpp::Node
{
  public:
    MinimalSubscriber()
    : Node("minimal_subscriber",rclcpp::NodeOptions().use_intra_process_comms(true))
    {
      // 3-1.创建订阅方；
      subscription_ = this->create_subscription<std_msgs::msg::String>("topic", 10, std::bind(&MinimalSubscriber::topic_callback, this, _1));
    }

  private:
    // 3-2.处理订阅到的消息；
    void topic_callback(const std_msgs::msg::String & msg) const
    {
      RCLCPP_INFO(this->get_logger(), "订阅的消息： '%s'", msg.data.c_str());
    }
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};
int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<MinimalPublisher>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}