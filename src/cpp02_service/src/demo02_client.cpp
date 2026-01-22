#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/srv/add_ints.hpp"
using base_interfaces_demo::srv::AddInts;
using namespace std::chrono_literals;
/*
    需求:编写客户端,组织数据并提交然后处理响应结果
    流程:
    前提:main函数中需要判断提交的参数是否正确

        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建客户端
            3-2连接服务器(对于服务端,如果客户端连接不到服务器,那么不能发送请求)
            3-3发送请求
        4.直接创建对象指针
        需要配调连接服务的函数,根据连接结果做下一步的处理
        连接服务后,调用请求发送函数
        在处理响应结果
        5.释放资源
*/

class AddIntsClient :public rclcpp::Node{
public:
    AddIntsClient():Node("add_ints_client_node_cpp"){
        RCLCPP_INFO(this->get_logger(),"客户端创建");
        //3-1创建客户端
        /*
        模板:服务接口
        参数:服务话题名称;
        返回值:服务对象指针
        
        
        */
        client_ = this->create_client<AddInts>("add_ints");
       

    }
    //3-2连接服务器(对于服务端,如果客户端连接不到服务器,那么不能发送请求)
    /*
    连接服务器实现,如果连接成功返回true,否则返回false
    */
    bool connect_server(){
        //在指定超时时间内连接服务器,如果连接上了,那俺么返回true,否则返回false
        //client_->wait_for_service(1s);
        while (!client_->wait_for_service(2s))//循环以1秒为超时时间连接服务器,知道连接服务器才退出循环
        {
            //对ctrl+c 这个操作进行判断做出特殊处理
            //1.怎么判断 ctrl+c按下 2.如何处理
            //按下ctrl+c 意味着释放资源关闭程序
            if (!rclcpp::ok())
            {
                RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"强行终止客户端");

                return false;

            }
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"服务连接中");
        }
        




        return true;
    }
    //3-3发送请求
    //编写发送请求函数.....参数是两个整形数据 返回值是提交请求后的服务器的返回结果
    rclcpp::Client<AddInts>::FutureAndRequestId send_requset(int num1,int num2){
        //在组织请求数据


        //发送
        /*
        rclcpp::Client<base_interfaces_demo::srv::AddInts>::FutureAndRequestId
        async_send-request(std::shared_ptr<base_interfaces_demo::srv::AndInts_Requset> request)//AddInts::Request
        */
       auto request = std::make_shared<AddInts::Request>();
       request->num1 = num1;
       request->num2 = num2;
       return client_->async_send_request(request);
    }


private:
    rclcpp::Client<AddInts>::SharedPtr client_;

};

int main(int argc, char const * argv[])
{
    if (argc != 3)
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"请提交两个整型数字");
        return 1;
    }
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
   // rclcpp::spin(std::make_shared<AddIntsClient>());
   //创建客户端对象
   auto client = std::make_shared<AddIntsClient>();
   //调用客户端对象的连接服务器功能
   bool flag = client->connect_server();
   //根据连接结果做进一步处理
   if (!flag)
   {
    /*
        rclcpp::get_logger("name")创建logger 对象不依赖于context
    */
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"服务器连接失败,程序退出!");
    //RCLCPP_INFO(client->get_logger("rclcpp"),"服务器连接失败,程序退出!");

    return 0;
   }
   //执行后续操作.......
   //调用请求提交函数 接收并处理响应结果
   auto future = client->send_requset(atoi(argv[1]),atoi(argv[2]));
   //处理响应
   if (rclcpp::spin_until_future_complete(client,future) == rclcpp::FutureReturnCode::SUCCESS)//成功
   {
    RCLCPP_INFO(client->get_logger(),"响应成功！sum = %d",future.get()->sum);
   }
   else//失败
   {
        RCLCPP_INFO(client->get_logger(),"响应失败！");
   }
   

    //释放资源
    rclcpp::shutdown();
    return 0;
}