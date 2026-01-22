#include "rclcpp/rclcpp.hpp"
using namespace std::chrono_literals;

/*
    需求:创建参数客户端,查询或修改服务端参数
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建参数客户端对象并连接服务端
            3-2连接服务端
            3-3查询参数
            3-4修改参数
        4.创建自定义节点类对象,并调用函数实现
        5.释放资源
*/

class ParamClient :public rclcpp::Node{
public:
    ParamClient():Node("param_client_node_cpp"){
        RCLCPP_INFO(this->get_logger(),("参数客户端创建"));
        //3-1创建参数客户端对象并连接服务端
        //参数1:当前对象依赖的节点
        //参数2:参数服务端节点名称
        param_client_ = std::make_shared<rclcpp::SyncParametersClient>(this,"param_server_node_cpp");
        /*
        问题:服务通信不是通过服务话题关联吗?为什么参数客户端是通过参数服务端的节点名称关联?
        答:  
             1.参数服务端启动后,底层封装了多个服务通信的服务端
             2.每个服务端的话题,都是采用 /服务端节点名称/xxxx;
             3.参数客户端创建后,也会封装多个服务通信的客户端
             4.这些客户端与服务端相呼应,也要使用相同的话题,因此客户端再创建时需要使用服务端节点名称
        */
     

    }
    //3-2连接服务端
    bool connect_server(){
        while ( !param_client_->wait_for_service(1s))
        {
            if (!rclcpp::ok())
            {
                return false;
            }
            
            RCLCPP_INFO(this->get_logger(),"服务连接中....");
        }
        
    }
    //3-3查询参数
    void get_param(){
        RCLCPP_INFO(this->get_logger(),"---------------------参数查询操作---------------------");
        //获取某个参数
      std::string car_name =  param_client_->get_parameter<std::string>("car_name");
      double width  = param_client_->get_parameter<double>("width");
      RCLCPP_INFO(get_logger(),"car_name = %s",car_name.c_str());
      RCLCPP_INFO(get_logger(),"width = %.2f",width);
        //获取多个参数
      auto params =   param_client_->get_parameters({"car_name","width","wheels"});
      for (auto &&param : params)
      {
        RCLCPP_INFO(this->get_logger(),"%s = %s",param.get_name().c_str(),param.value_to_string().c_str());
      }
      


        //判断是否包含某个参数
        RCLCPP_INFO(this->get_logger(),"包含car_name吗?%d",param_client_->has_parameter("car_name"));
        RCLCPP_INFO(this->get_logger(),"包含height吗?%d",param_client_->has_parameter("height"));

        
    }
    //3-4修改参数
    void update_param(){
        RCLCPP_INFO(this->get_logger(),"---------------------参数修改操作---------------------");
        param_client_->set_parameters({rclcpp::Parameter("car_name","pig"),
            rclcpp::Parameter("width",3.0),
            //设置一个参数服务端不存在的参数
            //注意:如果允许此种操作,那么参数服务端必须声明:rclcpp::NOdeOptions().allow_undeclared_parameters(true)
            rclcpp::Parameter("length",5.0)
        });
        RCLCPP_INFO(this->get_logger(),"新设置的参数:%.2f",param_client_->get_parameter<double>("length"));
    }
private:
    rclcpp::SyncParametersClient::SharedPtr param_client_;


};

int main(int argc, char const * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);
    auto client = std::make_shared<ParamClient>();
    bool flag = client->connect_server();
    if (!flag)
    {
        return 0;
    }
    client->get_param();
    client->update_param();
    client->get_param();
    



    //释放资源
    rclcpp::shutdown();
    return 0;
}