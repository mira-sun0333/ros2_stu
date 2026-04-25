#include "rclcpp/rclcpp.hpp"
#include "my_exer_interfaces/srv/cru.hpp"
#include "nav_msgs/msg/odometry.hpp"
using namespace std::placeholders;
using namespace std::placeholders;
using namespace std::chrono_literals;

/*
    需求:编写服务通信的服务端
        功能点1分析: 提取请求的数据并处理
                0 ------- 机器人终止巡航
                1 ------- 机器人开始巡航
                核心: 怎么向机器人发送速度指令
        功能点1实现: 
                思路: 复用my_exer01_topic_pub实现
                如何复用?
                    1.my_exer01_topic_pub以动态参数的方式提供了对外接口
                    2.创建参数客户端向my_exer01_topic_pub注入参数即可
                优点:
                    1.增强了代码的复用性
                    2.增强了可维护性
        
        功能点2 动态生成响应结果
                通过里程计获取机器人坐标
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建服务端
            3-2处理请求数据,并生成响应结果
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class CruServer :public rclcpp::Node{
public:
    CruServer():Node("cru_server_node_cpp"){
        x = 0.0;
        y = 0.0;
        RCLCPP_INFO(this->get_logger(),"创建成功");
        // 创建参数客户端
        parameters_client_ = std::make_shared<rclcpp::AsyncParametersClient>(this,"pub_vel_node_cpp");
        // 连接参数服务端
         while (!parameters_client_->wait_for_service(1s))
        {
            if (!rclcpp::ok())
            {
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"客户端连接被中断");
                return;
            }
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"连接参数服务器中...");
        }

        // RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        // 设置参数

        // 3-1创建服务端
      service_ = this->create_service<my_exer_interfaces::srv::Cru>("/cruising",std::bind(&CruServer::do_cru,this,_1,_2));
      sub_ = this->create_subscription<nav_msgs::msg::Odometry>("/odom",10,std::bind(&CruServer::do_odom,this,_1));
    }

private:
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub_;
    rclcpp::AsyncParametersClient::SharedPtr parameters_client_;
    rclcpp::Service<my_exer_interfaces::srv::Cru>::SharedPtr service_;
    double x,y;
    void do_odom(const nav_msgs::msg::Odometry& odom)
    {
        // 解析 x 和 y 坐标
        x = odom.pose.pose.position.x;
        y = odom.pose.pose.position.y;
    }
        // 3-2处理请求数据,并生成响应结果
        void do_cru(const my_exer_interfaces::srv::Cru::Request::SharedPtr request,const my_exer_interfaces::srv::Cru::Response::SharedPtr response){
            // 提取请求数据
            uint32_t flag = request->flag;
            RCLCPP_INFO(this->get_logger(),"客户端提交的数据:%d",flag);
            if(flag == 0)
            {
                // 终止巡航,线速度和角速度设置为0
                // vel_msg->linear.x = 0.1;
                // vel_msg->angular.z = 0.5;
                parameters_client_->set_parameters
                ({
                    rclcpp::Parameter("linear",0.0),
                    rclcpp::Parameter("angular",0.0)
                });
            }
            else
            {
                //开始巡航,线速度和角速度设置为非0值
                // vel_msg->linear.x = 0.0;
                // vel_msg->angular.z = 0.0;
                parameters_client_->set_parameters
                ({
                    rclcpp::Parameter("linear",0.1),
                    rclcpp::Parameter("angular",0.3)
                });
            }
            // 组织响应结果
            response->x = x;
            response->y = y;
        }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<CruServer>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}