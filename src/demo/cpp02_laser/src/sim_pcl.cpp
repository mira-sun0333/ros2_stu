#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "sensor_msgs/msg/point_field.hpp"
#include  "cstring"
using namespace std::chrono_literals;
/*
    需求:生成并发布多线激光雷达消息
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建发布方
            3-2创建定时器,周期性的发布消息
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class SimPointCloud2 :public rclcpp::Node{
public:
    SimPointCloud2():Node("sim_pointcloud2_node_cpp"){
        counter = 0;
      // 3-1创建发布方
      pcl_pub_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("pcl",10);
      // 3-2创建定时器,周期性的发布消息
      timer_ = this->create_wall_timer(0.1s,std::bind(&SimPointCloud2::on_timer,this));
    }

private:
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pcl_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    uint32_t counter;
    void on_timer()
    {
        // 生成点云数据
        auto pcl = sensor_msgs::msg::PointCloud2();
        // 设置头
        pcl.header.stamp = this->now();
        pcl.header.frame_id = "lidar";

        // 设置宽度和高度
        pcl.width = 60; // 列
        pcl.height = 30; //行
        
        //设置点的描述字段
        // 创建四个 PointField 对象(x y z in) 将四个对象添加进 fields[]
        auto x = sensor_msgs::msg::PointField();
        x.name = "x";
        x.offset = 0;
        x.datatype = sensor_msgs::msg::PointField::FLOAT32;
        x.count = 1;
        auto y = sensor_msgs::msg::PointField();
        y.name = "y";
        y.offset = 4;
        y.datatype = sensor_msgs::msg::PointField::FLOAT32;
        y.count = 1;
        auto z = sensor_msgs::msg::PointField();
        z.name = "z";
        z.offset = 8;
        z.datatype = sensor_msgs::msg::PointField::FLOAT32;
        z.count = 1;
        auto in = sensor_msgs::msg::PointField();
        in.name = "intensity";
        in.offset = 12;
        in.datatype = sensor_msgs::msg::PointField::FLOAT32;
        in.count = 1;

        pcl.fields.push_back(x);
        pcl.fields.push_back(y);
        pcl.fields.push_back(z);
        pcl.fields.push_back(in);

        // 是否是大端字节序
        pcl.is_bigendian = false;
        // 一个点的步长值
        pcl.point_step = 16;
        // 一行的步长值
        pcl.row_step = pcl.point_step * pcl.width;
        // 是否有无效数据
        pcl.is_dense = true;

        // 设置点数据
        // 储存点的数据,每个点对应四个元素
        std::vector<float> points(4 * pcl.height * pcl.width);
        // 遍历所有点,为每个point设置数据
        for(uint32_t row = 0; row < pcl.height; row++) // 行
        {
            for(uint32_t col = 0; col < pcl.height; col++) //列
            {
                // 获取当前点的初始位索引
                uint32_t index = (row * pcl.width + col) * 4;
                // 一个点占四个元素
                // x
                points[index] = 1.0 - counter * 0.01;
                // y
                points[index + 1] = 1.0 - col * 0.02;
                // z
                points[index + 2] = 1.0 - row * 0.02;
                // in
                points[index + 3] = row * col;
            }
        }
        pcl.data.resize(16 * pcl.height * pcl.width);
        // 数据复制
        std::memcpy(&(pcl.data[0]),points.data(),pcl.data.size());
        // 发布点云数据
        pcl_pub_->publish(pcl);
        counter++;
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<SimPointCloud2>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}