#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
using namespace std::chrono_literals;
/*
    需求:发布一张单目相机图片
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建图片发布方
            3-2创建定时器,周期性发布消息
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class SimCam :public rclcpp::Node{
public:
    SimCam():Node("sim_cam_node_cpp"){
      // 3-1创建图片发布方
      image_pub_ = this->create_publisher<sensor_msgs::msg::Image>("image",10);
      // 3-2创建定时器,周期性发布消息
      timer_ = this->create_wall_timer(0.2s,std::bind(&SimCam::on_timer,this));
    }

private:
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr image_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    //回调函数
    void on_timer(){
      // 1.组织一张图片数据
      auto image = sensor_msgs::msg::Image();
      // 字段设置
      // 头
      image.header.stamp = this->get_clock()->now();
      image.header.frame_id = "camera";
      // 分辨率
      image.height = 480;
      image.width = 640;
      // 编码集
      image.encoding = "rgb8"; // 一个像素由三原色描述,每个原色占8个bit(一个字节)
      // 是否是大端字节序
      image.is_bigendian = false;
      // 一行数据的字节数
      image.step = image.width * 3;
      // 储存图片图像数据的字节数组
      // 应该是使用step * rows
      // image.data = 
      // [255，0，0]
      image.data.resize(image.step * image.height);
      for(size_t i = 0; i < image.data.size(); i += 3)
      {
        image.data[i] = 255;
        image.data[i+1] = 0;
        image.data[i+2] = 0;
      }
      for(size_t i = image.data.size(); i < image.data.size() / 3 * 2; i += 3)
      {
        image.data[i] = 0;
        image.data[i+1] = 255;
        image.data[i+2] = 0;
      }
      for(size_t i = image.data.size() / 3 * 2; i < image.data.size(); i += 3)
      {
        image.data[i] = 0;
        image.data[i+1] = 0;
        image.data[i+2] = 255;
      }
      // 2.发布
      image_pub_->publish(image);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<SimCam>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}