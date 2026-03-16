#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "rosbag2_cpp/writer.hpp"

/*
    需求:录制控制乌龟运动的速度指令
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建录制对象
            3-2设置磁盘文件
            3-3写数据(创建一个速度订阅方,回调函数中执行写出操作)
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class SimpleBagRecorder :public rclcpp::Node{
public:
    SimpleBagRecorder():Node("simple_bag_recorder_node_cpp"){
        RCLCPP_INFO(this->get_logger(),"消息录制对象创建");
          //3-1创建录制对象
          writer_ = std::make_unique<rosbag2_cpp::Writer>();
          //3-2设置磁盘文件-
          writer_->open("my_bag");//相对路径,是工作空间的直接子级
          //3-3写数据(创建一个速度订阅方,回调函数中执行写出操作)
          //writer_->write(,)
          sub_ = this->create_subscription<geometry_msgs::msg::Twist>("/turtle1/cmd_vel",10,std::bind(&SimpleBagRecorder::do_write_msg,this,std::placeholders::_1));


    }

private:
    void do_write_msg(std::shared_ptr<rclcpp::SerializedMessage> msg){
      //write(std::shared_ptr<rclcpp::SerializedMessage> message, 被写出的消息
      //const std::string &topic_name, 话题名称
      //const std::string &type_name, 话题类型
      //const rclcpp::Time &time) 时间戳
      RCLCPP_INFO(this->get_logger(),"数据写出....");
      writer_->write(msg,"/turtle1/cmd_vel","geometry_msgs/msg/Twist",this->now());

    }
    std::unique_ptr<rosbag2_cpp::Writer>writer_;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_;
  
};

int main(int argc, char const * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<SimpleBagRecorder>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}
