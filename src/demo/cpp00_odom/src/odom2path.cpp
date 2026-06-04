#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "nav_msgs/msg/path.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "geometry_msgs/msg/pose.hpp"

using namespace std::placeholders;
/*
    需求:订阅机器人里程计消息,生成并发布路径消息(path)
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建订阅里程计消息
            3-2发布轨迹消息
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class Odom2Path :public rclcpp::Node{
public:
    Odom2Path():Node("odom2path_node_cpp"){
      RCLCPP_INFO(this->get_logger(),"订阅里程计消息并生成机器人的运行轨迹");
      // 将原点添加进轨迹
      append_pose();
      //  3-1创建订阅里程计消息
      odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>("odom",10,std::bind(&Odom2Path::odom_cb,this,_1));
      //  3-2发布轨迹消息
      path_pub_ = this->create_publisher<nav_msgs::msg::Path>("path",10);
    }

private:
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_pub_;
    // 存储轨迹核心数据(一些坐标点)的vector
    std::vector<geometry_msgs::msg::PoseStamped> poses;
    // 记录vector中的 最后一个坐标点
    geometry_msgs::msg::Pose last_pose;
    // 处理订阅到的里程计消息,然后将数据转换成path并发布
    void odom_cb(const nav_msgs::msg::Odometry & odom)
    {
      // 1.获取机器人位姿
        auto current_pose = odom.pose.pose;
      // 2.有条件的将当前位姿添加进轨迹(poses)
      // 条件:将当前坐标与poses中的最后一个坐标进行计算,如果位移距离大于某个指定间距(0.1或0.5m),认为符合
        if(get_distance(current_pose) >= 0.1)
        {
          // 将当前坐标赋值给last_pose
          last_pose = current_pose;
          // 条件成立则添加进poses
          append_pose();
        }
      // 3.发布轨迹
      publish_path();
    }
// 发布轨迹
void publish_path()
{
    // 1.组织path消息数据
    nav_msgs::msg::Path path;
    path.header.stamp = this->get_clock()->now();
    path.header.frame_id = "odom";
    path.poses = poses;
    // 2.发布path消息
    path_pub_->publish(path);
}
// 将符合条件的坐标添加进轨迹
void append_pose()
{
    // 将lase_pose转换成PoseStamped数据
    auto pose_stamped = geometry_msgs::msg::PoseStamped();
    pose_stamped.header.stamp = this->get_clock()->now();
    pose_stamped.header.frame_id = "odom";
    pose_stamped.pose = last_pose;
    // 将pose_stamped添加进poses
    poses.push_back(pose_stamped);
}
double get_distance(geometry_msgs::msg::Pose& current_pose)
{
    // 计算
    // 距离 = 开方(两点x的差的平方 + 两点y的差的平方 + 两点z的差的平方)
    return sqrt(pow(current_pose.position.x - last_pose.position.x,2) + pow(current_pose.position.y - last_pose.position.y,2));
}
};  
int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<Odom2Path>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}