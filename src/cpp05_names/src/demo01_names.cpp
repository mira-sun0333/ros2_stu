#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"


/*
    需求:
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

class  MyNode:public rclcpp::Node{
public:
    MyNode():Node("zhen_keng","zuoxie"){
        //全局话题: 和命名空间,节点名称无关
       //pub_ = this->create_publisher<std_msgs::msg::String>("/shi",10);
       //相对话题:
       //pub_ = this->create_publisher<std_msgs::msg::String>("kaihui",10);

       //私有话题
       pub_ = this->create_publisher<std_msgs::msg::String>("~/vip",10);
    }
private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;

};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<MyNode>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}