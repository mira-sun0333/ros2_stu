#include "rclcpp/rclcpp.hpp"
#include "turtlesim/srv/spawn.hpp"
using namespace std::chrono_literals;

/*
    需求:编写客户端实现,发送请求生成一只新乌龟
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1使用参数服务声明新的乌龟信息
            3-2创建服务客户端
            3-3连接服务端
            3-4组织并发送数据
        4.创建自定义节点类对象,组织函数,处理响应结果
        5.释放资源
*/

class Exer01Spawn :public rclcpp::Node{
public:
    Exer01Spawn():Node("exer01_spawn_node_cpp"){
        //3-1使用参数服务声明新的乌龟信息
        this->declare_parameter("x",3.0);
        this->declare_parameter("y",3.0);
        this->declare_parameter("theta",0.0);
        this->declare_parameter("turtle_name","turtle2");
        x = this->get_parameter("x").as_double();
        y = this->get_parameter("y").as_double();
        theta = this->get_parameter("theta").as_double();
        turtle_name = this->get_parameter("turtle_name").as_string();
        //3-2创建服务客户端
        spawn_client_ = this->create_client<turtlesim::srv::Spawn>("/spawn");
    }
        //3-3连接服务端
        bool connect_server(){
            while (!spawn_client_->wait_for_service(1s))
            {
                if (!rclcpp::ok())
                {
                    RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"强制退出");
                    return false;
                }
                
                RCLCPP_INFO(this->get_logger(),"服务连接中.....");
            }
            return true;
            
        }
        //3-4组织并发送数据
        rclcpp::Client<turtlesim::srv::Spawn>::FutureAndRequestId request(){
            auto req = std::make_shared<turtlesim::srv::Spawn_Request>();
            req->x = x;
            req->y = y;
            req->theta = theta;
            req->name = turtle_name;
            //rclcpp::Client<turtlesim::srv::Spawn>::FutureAndRequestId 
            //async_send_request(std::shared_ptr<turtlesim::srv::Spawn_Request> request)

            return spawn_client_->async_send_request(req);
        }

private:
    double_t x,y,theta;
    std::string turtle_name;
    rclcpp::Client<turtlesim::srv::Spawn>::SharedPtr spawn_client_;

  
};

int main(int argc, char const * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //4.创建自定义节点类对象,组织函数,处理响应结果
    auto client_ = std::make_shared<Exer01Spawn>();
    bool flag = client_->connect_server();
    if (!flag)
    {
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"服务连接失败");
        return 1;
    }
    //发送请求
    auto response = client_->request();


    //处理响应
    if (rclcpp::spin_until_future_complete(client_,response) == rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_INFO(client_->get_logger(),"响应成功");
        //如果生成新乌龟时,新乌龟重名了,那么也会响应成功,但是其实
        //没有生成新乌龟,且响应结果中,乌龟的名称是空串(正常应该是请求的乌龟的名称)
        std::string name = response.get()->name;
        if (name.empty())
        {
            RCLCPP_INFO(client_->get_logger(),"您生成的乌龟因为重名导致生成失败");
        }else{
            RCLCPP_INFO(client_->get_logger(),"您生成的乌龟成功");
        }
        
    }
    else{
        RCLCPP_INFO(client_->get_logger(),"响应失败");
    }
    
    
    //rclcpp::spin(std::make_shared<Exer01Spawn>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}