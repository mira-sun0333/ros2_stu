#include "rclcpp/rclcpp.hpp"
using namespace std::placeholders;

/*
    需求:节点内设置一参数,当参数被修改时,需要监听到该行为并作出处理
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1声明参数
            3-2为参数添加监听器
            3-3设置事件处理
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class ParamEventDemo :public rclcpp::Node{
public:
    ParamEventDemo():Node("param_event_demo_node_cpp"){
        RCLCPP_INFO(this->get_logger(),"创建成功");
        // 3-1声明参数
        this->declare_parameter("num",10);
        // 3-2为参数添加监听器
        // 参数1:被监听的参数名称
        // 参数2:回调函数
        // 参数3:节点名称,不设置时默认认为当前节点
        param_event = std::make_shared<rclcpp::ParameterEventHandler>(this);
        param_cb = param_event->add_parameter_callback("num",std::bind(&ParamEventDemo::cb,this,std::placeholders::_1));
        param_cb_other = param_event->add_parameter_callback("background_r",std::bind(&ParamEventDemo::cb_other,this,std::placeholders::_1),"/turtlesim");

    }

private:
    std::shared_ptr<rclcpp::ParameterEventHandler> param_event;
    std::shared_ptr<rclcpp::ParameterCallbackHandle> param_cb;
    std::shared_ptr<rclcpp::ParameterCallbackHandle> param_cb_other;

    // 3-3设置事件处理
    // void (const rclcpp::Parameter &)
    void cb(const rclcpp::Parameter& param)
    {
      // 处理修改后的参数
      RCLCPP_INFO(this->get_logger(),"修改的参数name:%s,修改后的value:%ld",param.get_name().c_str(),param.as_int());
    }
     void cb_other(const rclcpp::Parameter& param)
    {
      // 处理修改后的参数
      RCLCPP_INFO(this->get_logger(),"修改的参数name:%s,修改后的value:%ld",param.get_name().c_str(),param.as_int());
    }

};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<ParamEventDemo>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}