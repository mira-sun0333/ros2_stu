#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

/*
    需求:编写一个ROS2的lifecycle Node 并执行
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.创建一个生命周期节点类
        4.执行生命周期节点对象
        5.释放资源
*/

class MyNode :public rclcpp_lifecycle::LifecycleNode{
public:
    MyNode():LifecycleNode("demo_lifecycle_node"){
        RCLCPP_INFO(this->get_logger(),"生命周期节点对象创建了");
    }
    // 要重写转换过程相关的函数
rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_configure(const rclcpp_lifecycle::State & previous_state)
{
    // 标记未使用参数，消除警告
    (void)previous_state;

    // 日志输出
    RCLCPP_INFO(this->get_logger(),"节点配置.....");
    return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
}

rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_activate(const rclcpp_lifecycle::State & previous_state)
{
    // 标记未使用参数
    (void)previous_state;

    RCLCPP_INFO(this->get_logger(),"节点激活.....");
    return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
}

rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_deactivate(const rclcpp_lifecycle::State & previous_state)
{
    // 标记未使用参数
    (void)previous_state;

    RCLCPP_INFO(this->get_logger(),"节点待激活.....");
    return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
}

rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_cleanup(const rclcpp_lifecycle::State & previous_state)
{
    // 标记未使用参数
    (void)previous_state;

    RCLCPP_INFO(this->get_logger(),"节点清空配置.....");
    return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
}

rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_shutdown(const rclcpp_lifecycle::State & previous_state)
{
    // 标记未使用参数
    (void)previous_state;

    RCLCPP_INFO(this->get_logger(),"节点销毁.....");
    return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
}
private:
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    auto mynode = std::make_shared<MyNode>();
    rclcpp::spin(mynode->get_node_base_interface());

    //释放资源
    rclcpp::shutdown();
    return 0;
}