#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "my_exer_interfaces/action/nav.hpp" //包含自定义的动作头文件

/*
    取消请求使用自定义终止变量实现
    需求:向服务端发送数据(前进距离),并处理服务端发送的连续反馈和最终响应
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.处理终端提交的数据
        4.自定义节点类
            4-1创建运动客户端对象
            4-2编写发布请求响应(函数)
            4-3处理服务端返回的请求数据是否合法
            4-4接收服务端发送的连续反馈数据和最终响应
            4-5特定条件下发送取消请求响应
                a.客户端按下ctrl+c   [判断条件:rclcpp::ok()]
                b.发送取消任务请求    action_client_->async_cancel_goal(goal_handle)
        5.调用自定义节点类对象向服务端发送请求
        6.调用spin函数,并传入节点对象指针
        7.释放资源
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间
using my_exer_interfaces::action::Nav; //使用自定义动作接口

static std::atomic_bool g_stop{false};//全局变量,用于接管Ctrl+C信号

class NavClient :public rclcpp::Node{
public:
    NavClient():Node("nav_client_node_cpp"){
      // 4-1创建运动客户端对象
      action_client_ = rclcpp_action::create_client<Nav>(this,"/nav");

    }
    // 4-2编写发布请求响应(函数)
    void send_goal(double goal)
    {
      // 1.保证可以连接到服务端
      if(!action_client_->wait_for_action_server(10s))
      {
        RCLCPP_ERROR(this->get_logger(),"连接超时!");
        return;
      }
      RCLCPP_INFO(this->get_logger(),"连接成功!");
      // 2.组织被发送的请求对象
      auto request = Nav::Goal();
      request.goal = goal;
      // 3.封装回调函数
      rclcpp_action::Client<Nav>::SendGoalOptions options;
        /*
            using GoalHandle = ClientGoalHandle<ActionT>;
            using GoalResponseCallback = std::function<void (typename GoalHandle::SharedPtr)>;
            using FeedbackCallback = typename GoalHandle::FeedbackCallback;
            using ResultCallback = typename GoalHandle::ResultCallback;

        */
        options.goal_response_callback = std::bind(&NavClient::goal_response_callback,this,_1);
        options.feedback_callback = std::bind(&NavClient::feedback_callback,this,_1,_2);
        options.result_callback = std::bind(&NavClient::result_callback,this,_1);
      // 4.发送
      action_client_->async_send_goal(request,options); // 核心API
    }

private:
    rclcpp_action::Client<Nav>::SharedPtr action_client_;
    // 4-3处理服务端返回的请求数据是否合法
    void goal_response_callback(rclcpp_action::ClientGoalHandle<Nav>::SharedPtr goal_handle)
    {
      //
      if(!goal_handle)
      {
        RCLCPP_ERROR(this->get_logger(),"请求数据非法,请先检查提交是否是正数");
      }
      else
      {
        RCLCPP_INFO(this->get_logger(),"请求数据被受理");
        // 单独一根线程 循环判断终端状态
        std::thread{std::bind(&NavClient::cancel,this,_1),goal_handle}.detach();
      }
    }
    void cancel(rclcpp_action::ClientGoalHandle<Nav>::SharedPtr goal_handle)
    {
      // 当rclcpp::ok()返回false时,取消任务
      while(rclcpp::ok())
      {

      }
      action_client_->async_cancel_goal(goal_handle);
    }
    // 4-4接收服务端发送的连续反馈数据和最终响应
    void feedback_callback(rclcpp_action::ClientGoalHandle<Nav>::SharedPtr goal_handle,const std::shared_ptr<const Nav::Feedback> feedback)
    {
      // 直接将机器人和目标点之间的剩余距离输出
      (void)goal_handle;
      RCLCPP_INFO(this->get_logger(),"距离目标点还有%.2f米",feedback->distance);
    }
    // 处理最终响应
    void result_callback(const rclcpp_action::ClientGoalHandle<Nav>::WrappedResult & result)
    {
      // 输出机器人的停止坐标
      if(result.code == rclcpp_action::ResultCode::SUCCEEDED)
      {
        RCLCPP_INFO(this->get_logger(),"导航完毕,机器人的停止坐标:(%.2f,%.2f)",result.result->x,result.result->y);
      }
      else
      {
        RCLCPP_INFO(this->get_logger(),"机器人没有到达目标点");
      }
      rclcpp::shutdown();
    }

};
int main(int argc, char * argv[])
{
    // 初始化ros2客户端
    rclcpp::init(argc,argv);
    // 处理终端提交的数据
    if(argc != 2)
    {
      RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"请提交一个距离数据");
      return 1;
    }
    auto nav_client = std::make_shared<NavClient>();
    // 5.调用自定义节点类对象向服务端发送请求
    nav_client->send_goal(atof(argv[1]));
    // 调用spin函数,使用自定义类对象指针
    rclcpp::spin(nav_client);

    // 释放资源
    rclcpp::shutdown();
    return 0;
}