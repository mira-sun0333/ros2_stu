#include "rclcpp/rclcpp.hpp"
#include "my_exer_interfaces/srv/cru.hpp"
using my_exer_interfaces::srv::Cru;
using namespace std::chrono_literals;
/*
    需求:需要提交一个整型数据到服务端,并处理服务端的结果响应
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.判断终端的指令是否提交了整型数据
        4.自定义节点类
        5.创建类对象
        6.连接服务端 T -> 继续, F -> 程序退出
        7.发送请求
        8.处理响应
        9.释放资源
*/

class CruClient :public rclcpp::Node{
public:
    CruClient():Node("cru_client_node_cpp"){
        RCLCPP_INFO(this->get_logger(),"创建成功");
      client_ = this->create_client<Cru>("/cruising");
    }
    // 连接服务
    bool connect_server()
    {
      while(client_->wait_for_service(1s))
      {
        RCLCPP_INFO(this->get_logger(),"服务连接中");
        if (!rclcpp::ok())
        {
          return false;
        }
      }
      return true;
    }
    // 发送请求的函数(封装)
    rclcpp::Client<my_exer_interfaces::srv::Cru>::FutureAndRequestId send_request(int32_t flag)
    {
      // 1.将flag封装成cru的请求对象
      auto req = std::make_shared<Cru::Request>();
      req->flag = flag;
      // 2.发送到服务端(服务端给响应)
      return client_->async_send_request(req);
    }

private:
    rclcpp::Client<Cru>::SharedPtr client_;
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    // 3.判断终端的指令是否提交了整型数据
    if(argc != 2)
    {
      RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"请提交一个整型数据:");
      return 1;
    }
    // 4.自定义节点类

    // 5.创建类对象
    auto cru_client = std::make_shared<CruClient>();
    // 6.连接服务端 T -> 继续, F -> 程序退出
    bool flag = cru_client->connect_server();
    if(!flag)
    {
      RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"服务连接不上,程序退出");
      return 1;
    }
    // 7.发送请求
    auto response = cru_client->send_request(atoi(argv[1]));
    // 8.处理响应
    if(rclcpp::spin_until_future_complete(cru_client,response) == rclcpp::FutureReturnCode::SUCCESS)
    {
      auto response_ = response.get();
      RCLCPP_INFO(rclcpp::get_logger("client"),"响应成功,机器人的坐标:(%.2f,%.2f)",response_->x,response_->y);
    }
    else
    {
      RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"响应失败");
    }
    // 9.释放资源
    rclcpp::shutdown();
    return 0;
}