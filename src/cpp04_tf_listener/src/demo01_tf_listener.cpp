#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"
using namespace std::chrono_literals;


/*
    需求:先发布laser到base_link的坐标相对关系,再发布camera到base_link的坐标系相对关系
         求解laser到camera的坐标系相对关系
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建一个缓存对象, 融合多个坐标系相对关系为一个坐标系
            3-2创建一个监听器,绑定缓存对象,会将所有广播器广播的数据写入缓存
            3-3编写一个定时器,循环实现转换
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class TFListener :public rclcpp::Node{
public:
    TFListener():Node("tf_listener_node_cpp"){
      //3-1创建一个缓存对象, 融合多个坐标系相对关系为一个坐标系
      buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
      //3-2创建一个监听器,绑定缓存对象,会将所有广播器广播的数据写入缓存
      listener_ = std::make_shared<tf2_ros::TransformListener>(*buffer_, this);
      //3-3编写一个定时器,循环实现转换
      timer_ = this->create_wall_timer(1s,std::bind(&TFListener::on_timer, this));

       
    }

private:
    void on_timer(){
      //实现坐标系转换
      //geometry_msgs::msg::TransformStamped 返回值,新的坐标系
      //lookupTransform(const std::string &target_frame, 新坐标帧的父坐标系
      //const std::string &source_frame, 新坐标的子坐标系
      //const tf2::TimePoint &time) 转换的时间点 一般设置为:tf2::TimePointZero,意思是转换最新时刻的时间帧
      try
      {
        //geometry_msgs::msg::TransformStamped 
        //lookupTransform(const std::string &target_frame, 
        //const std::string &source_frame, 
        //const tf2::TimePoint &time)
        //当转换失败时,会抛出异常,可以使用try catch 处理
        auto ts = buffer_->lookupTransform("camera","laser",tf2::TimePointZero);
        RCLCPP_INFO(this->get_logger(),"-----------转换完成的坐标帧信息-----------");
        RCLCPP_INFO(this->get_logger(),
            "新坐标帧:父坐标系:%s,子坐标系:%s,偏移量(%.2f,%.2f,%.2f)",
            ts.header.frame_id.c_str(),//camera
            ts.child_frame_id.c_str(),//laser
            ts.transform.translation.x,
            ts.transform.translation.y,
            ts.transform.translation.z

        );
      }
      catch(const tf2::LookupException& e)
      {
        RCLCPP_INFO(this->get_logger(),"异常提示:%s",e.what());
      }
      
    
    }
    std::unique_ptr<tf2_ros::Buffer> buffer_;
    std::shared_ptr<tf2_ros::TransformListener> listener_;
    rclcpp::TimerBase::SharedPtr timer_;

  
};

int main(int argc, char const * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<TFListener>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}