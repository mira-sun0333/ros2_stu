/*
    需求:实现编队通用逻辑
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1发布目标坐标系(通过静态坐标系变换实现的);
            3-2将目标坐标系转换成当前从车导航的目标点.
                a.目标点是什么消息类型?  geometry_msgs/msg/PoseStamped
                b.怎么从坐标变换中生成该目标点? 将goal1 goal2 坐标系转换成计算出相对于 map 的位姿
                    然后再生成坐标点即可
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/
#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/static_transform_broadcaster.h"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "tf2_ros/transform_listener.hpp" //坐标变换监听
#include "tf2_ros/buffer.hpp"  //坐标变换缓存

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class Convoy :public rclcpp::Node{
public:
      Convoy():Node("convoy_node_cpp"){
        // 静态坐标变换广播器
        tf_bro_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);
        //x y z pith roll yaw 父级坐标系 子级坐标系
        //通过参数动态设置
        this->declare_parameter("x",0.0);
        this->declare_parameter("y",0.0);
        this->declare_parameter("z",0.0);
        this->declare_parameter("pith",0.0);
        this->declare_parameter("roll",0.0);
        this->declare_parameter("yaw",0.0);
        this->declare_parameter("frame_id","robot_0/base_link");
        this->declare_parameter("child_frame_id","goal_1");
        this->declare_parameter("map_frame_id","map");

        x = this->get_parameter("x").as_double();
        y = this->get_parameter("y").as_double();
        z = this->get_parameter("z").as_double();
        pith = this->get_parameter("pith").as_double();
        roll = this->get_parameter("roll").as_double();
        yaw = this->get_parameter("yaw").as_double();


        frame_id = this->get_parameter("frame_id").as_string();
        child_frame_id = this->get_parameter("child_frame_id").as_string();
        map_frame_id = this->get_parameter("map_frame_id").as_string();
        // 发布坐标变换
        this->bro_tf();
        // 实现坐标变换监听,将监听结果转换成点数据并发布
        //创建坐标变换监听器
        buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        listener_ = std::make_shared<tf2_ros::TransformListener>(*buffer_);
        //创建定时器,周期为1s
        timer_ = this->create_wall_timer(1s,std::bind(&Convoy::on_timer,this));
        // 初始化目标点发布方
        pose_pub_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("/robot_1/goal_pose",10);

    }
private:
    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> tf_bro_;
    double x,y,z,pith,roll,yaw;
    std::string frame_id,child_frame_id,map_frame_id;
    
    rclcpp::TimerBase::SharedPtr timer_;//定时器
    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr pose_pub_;
    

    std::shared_ptr<tf2_ros::Buffer> buffer_;//坐标变换缓存
    std::shared_ptr<tf2_ros::TransformListener> listener_;//坐标变换监听器

    // 根据传入的参数发布坐标变换(目标坐标系相对于主车的坐标变换的变换)
    void bro_tf(){
        // 组织消息
        geometry_msgs::msg::TransformStamped ts;
        ts.header.stamp = this->get_clock()->now();
        ts.header.frame_id = frame_id;

        ts.child_frame_id = child_frame_id;
        

        ts.transform.translation.x = x;
        ts.transform.translation.y = y;
        ts.transform.translation.z = z;


        // 将欧拉角转换四元数
        tf2::Quaternion qtn;
        qtn.setRPY(roll,pith,yaw);

        ts.transform.rotation.x = qtn.getX();
        ts.transform.rotation.y = qtn.getY();
        ts.transform.rotation.z = qtn.getZ();
        ts.transform.rotation.w = qtn.getW();


        //发布消息
        tf_bro_->sendTransform(ts);
    }
    //发布导航请求的回调函数
    void on_timer(){
        //获取目标坐标系相对于从车坐标系的变换
        try
        {
            //实现目标坐标系于map的坐标变换
            geometry_msgs::msg::TransformStamped ts =  buffer_->lookupTransform(map_frame_id, child_frame_id, tf2::TimePointZero);
            //将TransStamped 数据转换为 PoseStamped 数据
            geometry_msgs::msg::PoseStamped ps;
            ps.header.frame_id = map_frame_id;
            ps.header.stamp = this->now();

            //设置三轴偏移量    
            ps.pose.position.x = ts.transform.translation.x;
            ps.pose.position.y = ts.transform.translation.y;
            ps.pose.position.z = ts.transform.translation.z;

            // 设置旋转角度
            ps.pose.orientation = ts.transform.rotation;
            // 将作为目标点发布
            pose_pub_->publish(ps);
        }
        catch(const std::exception& e)
        {

        }
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<Convoy>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}
