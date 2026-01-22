#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/srv/add_ints.hpp"
using base_interfaces_demo::srv::AddInts;
using std::placeholders::_1;
using std::placeholders::_2;



/*
    需求:服务端实现,解析提交的请求数据,将解析的数据相加并响应到客户端
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建服务端
            3-2实现回调函数
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class AddIntsServer :public rclcpp::Node{
public:
    AddIntsServer():Node("add_ints_server_node_cpp"){
        RCLCPP_INFO(this->get_logger(),"服务端节点创建");
        //  3-1创建服务端
        /*
        模板:
        1.服务接口
        2.回调函数
    返回值:服务对象指针
        */
        server_ = this->create_service<AddInts>("add_ints",std::bind(&AddIntsServer::add,this,_1,_2));
        

    }
   
private:
      void add(const AddInts::Request::SharedPtr req, const AddInts::Response::SharedPtr res){
         //  3-2实现回调函数
        res->sum = req->num1 + req->num2;
        RCLCPP_INFO(this->get_logger(),"%d + %d = %d",req->num1, req->num2,res->sum);
    }
   rclcpp::Service<AddInts>::SharedPtr server_;


};
int main(int argc, char const * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<AddIntsServer>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}