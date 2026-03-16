
/*
    需求:广播不同乌龟相对于world的坐标系相对关系
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建
            3-2实现回调函数
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2/LinearMath/Quaternion.h"

class TFDynaBroadcaster:public rclcpp::Node{
public:
    TFDynaBroadcaster():Node("tf_dyna_broadcaster_node_cpp"){
        this->declare_parameter("turtle","thrtle1");
        turtle = this->get_parameter("turtle").as_string();
        //3-1创建动态的广播器
        broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        //3-2创建一个乌龟位姿订阅方
        pose_sub_ = this->create_subscription<turtlesim::msg::Pose>("/" + turtle + "/pose",10,
            std::bind(&TFDynaBroadcaster::do_pose,this,std::placeholders::_1)
        );
        

    }

private:
    std::string turtle;
    //3-3在回调中,获取乌龟位姿并生成相对相关关系然后发布
    void do_pose(const turtlesim::msg::Pose & pose){
        //组织消息
        geometry_msgs::msg::TransformStamped ts;

        ts.header.stamp = this->now();
        ts.header.frame_id = "world";

        ts.child_frame_id = turtle;

        ts.transform.translation.x = pose.x;
        ts.transform.translation.y = pose.y;
        ts.transform.translation.z = 0.0;

        //从欧拉角转换成四元数
        //乌龟的欧拉角只有yaw上的取值 没有roll和pitch
        tf2::Quaternion qtn;
        qtn.setRPY(0,0,pose.theta);
        
        ts.transform.rotation.x = qtn.x();
        ts.transform.rotation.y = qtn.y();
        ts.transform.rotation.z = qtn.z();
        ts.transform.rotation.w = qtn.w();

        
        //发布
        broadcaster_->sendTransform(ts);
    }
    std::shared_ptr<tf2_ros::TransformBroadcaster> broadcaster_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_sub_;
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<TFDynaBroadcaster>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}