#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
using namespace std::placeholders;
using namespace std::chrono_literals;
#define PI 3.1415926535
/*
    需求:实现激光雷达仿真
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建消息发布方
            3-2创建定时器,定时器的回调函数中发布消息
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class SimLaser :public rclcpp::Node{
public:
    SimLaser():Node("sim_laser_node_cpp"){
        RCLCPP_INFO(this->get_logger(),"激光雷达仿真节点创建成功");
        samples = 100;
        rate = 5.0;
        radius = 3.0;
        //  3-1创建消息发布方
        laser_scan_pub_ = this->create_publisher<sensor_msgs::msg::LaserScan>("scan",10);
        //  3-2创建定时器,定时器的回调函数中发布消息
        timer_ = this->create_wall_timer(200ms,std::bind(&SimLaser::timer_cb,this));
    }

private:
    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr laser_scan_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    int samples;
    float rate;
    float radius;
    //回调函数
    void timer_cb(){
      // 1.组织laser_scan消息数据
      auto msg = sensor_msgs::msg::LaserScan();
      msg.header.stamp = this->get_clock()->now();
      msg.header.frame_id = "laser";
      msg.angle_min = -PI;
      msg.angle_max = PI;
      msg.angle_increment = PI * 2 / 100;
      msg.range_min = 0.1;
      msg.range_max = 12.0;
      msg.scan_time = 1 / rate;
      msg.time_increment = msg.scan_time / samples;
      for(int i = 0;i < samples;i++)
      {
        // 设置每个采样点通过测距获取的距离以及强度信息
        msg.ranges.push_back(radius);
        msg.intensities.push_back(40.0);
      }
      if(radius >= 0.5)
      {
        radius -= 0.1;
      }
      
      // 2.将laser_scan消息发布
      laser_scan_pub_->publish(msg);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<SimLaser>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}