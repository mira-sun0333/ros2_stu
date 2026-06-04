#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2/LinearMath/Quaternion.h>
#include <string>

class ImuTfNode : public rclcpp::Node
{
public:
  ImuTfNode() : Node("imu_data_to_tf")
  {
    this->declare_parameter("imu_topic", std::string("/imu"));
    this->declare_parameter("position_x", 0);
    this->declare_parameter("position_y", 0);
    this->declare_parameter("position_z", 0);

    std::string imu_topic = this->get_parameter("imu_topic").as_string();
    position_x_ = this->get_parameter("position_x").as_int();
    position_y_ = this->get_parameter("position_y").as_int();
    position_z_ = this->get_parameter("position_z").as_int();

    tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(*this);
    sub_ = this->create_subscription<sensor_msgs::msg::Imu>(
      imu_topic, 10,
      std::bind(&ImuTfNode::imuCallback, this, std::placeholders::_1));
  }

private:
  void imuCallback(const sensor_msgs::msg::Imu::SharedPtr imu_data)
  {
    geometry_msgs::msg::TransformStamped transform_stamped;
    transform_stamped.header.stamp = this->now();
    transform_stamped.header.frame_id = "world";
    transform_stamped.child_frame_id = "imu";

    transform_stamped.transform.translation.x = static_cast<double>(position_x_);
    transform_stamped.transform.translation.y = static_cast<double>(position_y_);
    transform_stamped.transform.translation.z = static_cast<double>(position_z_);

    transform_stamped.transform.rotation.x = imu_data->orientation.x;
    transform_stamped.transform.rotation.y = imu_data->orientation.y;
    transform_stamped.transform.rotation.z = imu_data->orientation.z;
    transform_stamped.transform.rotation.w = imu_data->orientation.w;

    tf_broadcaster_->sendTransform(transform_stamped);
  }

  int position_x_;
  int position_y_;
  int position_z_;
  std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
  rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr sub_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ImuTfNode>());
  rclcpp::shutdown();
  return 0;
}
