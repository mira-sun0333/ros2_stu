#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include "sensor_msgs/msg/imu.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
using namespace std::placeholders;
/*
    需求:订阅imu消息,提取旋转角度数据,生成坐标变换(机器人mycar相对于地面world投影)消息并发布
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建坐标变换广播器
            3-2订阅imu消息,在回调函数中,实现数据的转换,生成以及广播
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class Imu2TF :public rclcpp::Node{
public:
    Imu2TF():Node("imu2tf_node_cpp"){
      // 3-1创建坐标变换广播器
      tf_bro_ = std::make_shared<tf2_ros::TransformBroadcaster>(*this);
      // 3-2订阅imu消息,在回调函数中,实现数据的转换,生成以及广播
      imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>("imu",10,std::bind(&Imu2TF::imu_cb,this,_1));
    }

private:
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_bro_;
    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_;
    void imu_cb(const sensor_msgs::msg::Imu & imu)
    {
      // 组织一个坐标帧消息
      auto ts = geometry_msgs::msg::TransformStamped();
      ts.header.stamp = this->now();
      ts.header.frame_id = "world";
      ts.child_frame_id = "mycar";
      ts.transform.translation.z = 1.0;
      ts.transform.rotation = imu.orientation;
      // 发布
      tf_bro_->sendTransform(ts);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<Imu2TF>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}