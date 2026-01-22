#include "rclcpp/rclcpp.hpp"
using namespace std::chrono_literals;


/*
    需求:修改turtlesim_node的背景色
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建参数客户端
            3-2连接参数服务端
            3-3更新参数
        4.创建节点对象指针,并调用其函数
        5.释放资源
*/

class Exer06Param :public rclcpp::Node{
public:
    Exer06Param():Node("exer06_param_node_cpp"){
        RCLCPP_INFO(this->get_logger(),"参数客户端");
        //3-1创建参数客户端
        client_ = std::make_shared<rclcpp::SyncParametersClient>(this,"/turtlesim");
       

    }
    //3-2连接参数服务端
    bool connect_server(){
        while (client_->wait_for_service(1s))
        {
            if (!rclcpp::ok())
            {
                RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"客户端强制退出");
                return false;
            }
            

            RCLCPP_INFO(this->get_logger(),"服务连接中");
        }
        
        return true;
    }
    //3-3更新参数
    void update_param(){
        //背景色要递进修改
        //background_r [0,255] 
        //1.获取参数
        int red = client_->get_parameter<int>("background_r");

        //2.编写循环,修改参数(通过休眠控制修改频率)
        rclcpp::Rate rate(30.0);
        /*
            需求:背景色渐变,由浅变深,在反之
            实现:
                1.一个完整周期,计数是511个,中间值是255;
                2.创建一个计数器,初始值和red相同,递增的步进值也和red相同,取值就是[0,511]
                3.当计数器在[0,255]之间时,red 递增,如果在[256,511] red 递减
                4.当计数器大于等于511时 归零
        */
       int count = red;
        while (rclcpp::ok())
        {
            //red += 5;
            count <= 255 ? red += 5: red -= 5;
            count += 5;
            if (count > 511) count = 0;
          
            
            //修改服务端参数
            client_->set_parameters({rclcpp::Parameter("background_r",red)});

            rate.sleep();
        }
        
    }
private:
    rclcpp::SyncParametersClient::SharedPtr client_;

};

int main(int argc, char const * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    auto client = std::make_shared<Exer06Param>();
    if (!client->connect_server()){
        return 1;
    }
    //rclcpp::spin(std::make_shared<Exer06Param>());
    //调用其函数
    client->update_param();

    //释放资源
    rclcpp::shutdown();
    return 0;
}