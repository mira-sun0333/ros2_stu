#include "rclcpp/rclcpp.hpp"
#include "my-lidar_srv/srv/control.hpp"
using namespace std::chrono_literals;
/*
    需求:雷达客户端实现,可以向服务端提交指令,设置雷达参数
        ros2 run my_lidar_driver client start
        ros2 run my_lidar_driver client stop
        ros2 run my_lidar_driver client rate 5.0
    流程:
        1.包含头文件
        2.判断终端指令传入参数的合法性
        3.初始化ROS2客户端
        4.自定义节点类
            4-1.创建客户端
            4-2.封装连接函数
            4-3.封装发送请求实现
        5.连接服务端
        6.发送请求
        7.处理响应
        8.释放资源
*/

class LidarClient :public rclcpp::Node{
public:
    LidarClient():Node("lidar_client_node"){
        // 4-1.创建客户端
        client_ = this->create_client<my_lidar_srv::srv::Control>("control");
        
    }
    // 4-2.封装连接函数
    bool connect_server(){
        while(!client_->wait_for_service(1s))
        {
            if(!rclcpp::ok())
            {
                RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"连接被终止")
                return false;
            }
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"服务连接中,请稍等")
        }
        return true;
    }
    // 4-3.封装发送请求实现
    void send_request(std::string func,double param)
    {
        // 将参数组织成 request
        auto request = std::make_shared<my_lidar_srv::srv::Control::Request>();
        request->func = func;
        request->param = param;
        // 调用client 的发送功能
        client_->async_send_request(request);
    }
private:
    rclcpp::Client<my_lidar_srv::srv::Control>::SharedPtr client_;
   
   
    
};

int main(int argc, char const * argv[])
{
    // 2.判断终端指令传入参数的合法性
    if(argc != 2 && argc != 3)
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"调用语法异常,请按一下格式执行");
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"ros2 run my_lidar_driver client start");
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"ros2 run my_lidar_driver client stop");
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"ros2 run my_lidar_driver client rate 5.0(刷新频率,可按需设置)");
        return 1;
    }
    // 如果格式化合法,需要解析数据
    std:string func = argv[1];
    double param = 0.0;
    if(argc == 3)
    {
        param = atof(argv[2]);
        if(param < 5.0 || param >  10)
        {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"刷新频率请设置再[5.0-10.0]之间");
            return 1;
        }
    }
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    // rclcpp::spin(std::make_shared<LidarClient>());
    // 连接服务
    std::shared_ptr<LidarClient> client = std::make_shared<LidarClient>();
    bool flag = client->connect_server();
    // RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"连接成功了吗? %d",flag);
    if (flag)
    {
        auto response = client->send_request(func,param);
        if(rclcpp::spin_until_future_complete(client,response) == rclcpp::FutureReturnCode::SUCCESS)
        {
            // 获取返回的数据(状态码 + 描述信息)
            auto response = response.get();
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"响应状态码:%d,描述信息:%s",response_->code,response_->value.c_str());
        }
    }
    //释放资源
    rclcpp::shutdown();
    return 0;
}