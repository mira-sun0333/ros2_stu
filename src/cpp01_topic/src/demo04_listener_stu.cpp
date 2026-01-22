#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/msg/student.hpp"


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
using base_interfaces_demo::msg::Student;
class ListenerStu :public rclcpp::Node{
public:
   ListenerStu():Node("listenerstu_node_cpp"){
        //3-1创建
        subscription_ = this->create_subscription<Student>("chatter_stu",10,std::bind(&ListenerStu::do_cb,this,std::placeholders::_1));
        //3-2实现回调函数

    }
    private:
    void do_cb(const Student &stu){
        RCLCPP_INFO(this->get_logger(),"订阅的学生信息：name=%s,age=%d,height=%.2f",stu.name.c_str(),stu.age,stu.height);
    }
    rclcpp::Subscription<Student>::SharedPtr subscription_;


};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<ListenerStu>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}