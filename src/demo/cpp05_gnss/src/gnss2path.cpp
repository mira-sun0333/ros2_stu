#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/path.hpp"
#include "sensor_msgs/msg/nav_sat_fix.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
using namespace std::placeholders;
/*
    需求:订阅gnss消息,生成并发布Path消息
          核心 -- 将gnss的以度为单位的坐标,转换成path中以米为单位的坐标
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建path发布方
            3-2创建gnss订阅方,回调函数中实现gnss到path的转换并发布
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class Gnss2Path :public rclcpp::Node{
public:
    Gnss2Path():Node("gnss2path_node_cpp"){
      is_origion = true;
      path.header.frame_id = "gnss_link";
      // 3-1创建path发布方
      path_pub_ = this->create_publisher<nav_msgs::msg::Path>("path",10);
      // 3-2创建gnss订阅方,回调函数中实现gnss到path的转换并发布
      nav_sub_ = this->create_subscription<sensor_msgs::msg::NavSatFix>("/gps/fix",10,std::bind(&Gnss2Path::nav_cb,this,_1));
    }

private:
    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_pub_;
    rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr nav_sub_;
    nav_msgs::msg::Path path;
    bool is_origion;
    double origion_la,origion_lo,origion_al;
    void nav_cb(const sensor_msgs::msg::NavSatFix & nav_msg)
    {
      // 1.生成path数据
      // path.header.frame_id
      path.header.stamp = this->now();
      geometry_msgs::msg::PoseStamped ps;
      ps.header.frame_id = "gnss_link";
      ps.header.stamp = this->now();
      RCLCPP_INFO(this->get_logger(),"录制的坐标点(%.9f,%.9f,%.9f)",nav_msg.longitude,nav_msg.latitude,nav_msg.altitude);
      // 确定原点,xyz都是0
      if(is_origion)
      {
        // 记录经纬度和海拔高度
        origion_la = nav_msg.latitude,
        origion_lo = nav_msg.longitude,
        origion_al = nav_msg.altitude;

        //修改标签
        is_origion = false;
      }
      else
      {
        // 获取当前坐标相对原点的经纬度海拔高度的改变
        double delta_lo = nav_msg.longitude - origion_lo; //经度
        double delta_la = nav_msg.latitude - origion_la; //纬度
        double delta_al = nav_msg.altitude - origion_al; //海拔
        // 转换为米的单位数据
        // 第三方工具
        // 简单方式:采集区域很小
        

        // 赋值给ps
        ps.pose.position.x = delta_lo / 0.000008983;
        ps.pose.position.y = delta_la * 0.000008993;
        ps.pose.position.z = delta_al;
      }



      path.poses.push_back(ps);
      // 关注点 xyz(以米为单位)
      // 2.发布
      path_pub_->publish(path);
    }

};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<Gnss2Path>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}