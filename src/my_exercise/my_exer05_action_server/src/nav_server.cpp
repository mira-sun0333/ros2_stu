#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "my_exer_interfaces/action/nav.hpp"
#include "nav_msgs/msg/odometry.hpp"
using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders;    //占位符命名空间
using my_exer_interfaces::action::Nav;
std::mutex odom_mutex_;               //互斥锁


/*
    动作通信服务端(最大请求数据不能超过3米,只能前进)
    需求:处理服务端提交的数据,控制机器人运动指定的距离,连续反馈剩余距离和当前坐标,
        最终返回机器人运动的距离和提交的距离和当前坐标
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建动作通信服务端
            3-2处理请求数据(判断是否合法)
            3-3向机器人控制节点发布速度消息
            3-4订阅里程计消息,计算当前运动距离
            3-5处理客户端取消请求
            3-6生成最终响应+连续反馈
        4.调用spin函数,并传入节点对象指针
        5.释放资源

    模拟客户端
    ros2 action send_goal /my_car/nav_action my_exer_interfaces/action/Nav "{'distance': 1}" -f
*/

class NavServer :public rclcpp::Node{
public:
    NavServer():Node("nav_server_node_cpp"){
        x = 0.0;
        y = 0.0;
        last_x = 0.0;
        RCLCPP_INFO(this->get_logger(),"创建成功");
        sub_ = this->create_subscription<nav_msgs::msg::Odometry>("/odom",10,std::bind(&NavServer::do_odom,this,_1));
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

        // 3-1创建动作通信服务端
        action_server_ = rclcpp_action::create_server<Nav>(
           this,
            "/nav",
            /*
                rclcpp_action::Server<ActionT>::GoalCallback handle_goal, 
                rclcpp_action::Server<ActionT>::CancelCallback handle_cancel, 
                rclcpp_action::Server<ActionT>::AcceptedCallback handle_accepted

                using GoalCallback = std::function<GoalResponse(
                        const GoalUUID &, std::shared_ptr<const typename ActionT::Goal>)>;
                using CancelCallback = std::function<CancelResponse(std::shared_ptr<ServerGoalHandle<ActionT>>)>;
                using AcceptedCallback = std::function<void (std::shared_ptr<ServerGoalHandle<ActionT>>)>;
            */
            std::bind(&NavServer::handle_goal,this,_1,_2),//处理请求数据
            std::bind(&NavServer::handle_cancel,this,_1),//处理客户端取消请求
            std::bind(&NavServer::handle_accepted,this,_1)//处理最终响应和连续反馈
        );
        // 3-2处理请求数据(判断是否合法)
        // 3-5处理客户端取消请求
        // 3-6生成最终响应+连续反馈

    }

private:
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub_;
    rclcpp::AsyncParametersClient::SharedPtr parameters_client_;
    rclcpp_action::Server<Nav>::SharedPtr action_server_;
    double x,y;
    double last_x; // 存储每次导航结束时,机器人的 x 坐标
    void do_odom(const nav_msgs::msg::Odometry& odom)
    {
        // 解析 x 和 y 坐标
        x = odom.pose.pose.position.x;
        y = odom.pose.pose.position.y;
    }
    rclcpp_action::GoalResponse handle_goal(const rclcpp_action::GoalUUID & uuid,std::shared_ptr<const Nav::Goal> goal)
    {
      (void)uuid;
      // 处理请求数据实现
      RCLCPP_INFO(this->get_logger(),"处理请求数据");
      // 需要判断客户端提交的数据是否合法(大于0)
      // 1.获取数据
      double goal_num = goal->goal;
      // 2.非法判断
      if(goal_num <= 0)
      {
        RCLCPP_INFO(this->get_logger(),"提交的数据非法,必须提交正数,小车只能前进,不能后退.......");
        return rclcpp_action::GoalResponse::REJECT;
      }
      // 3.让机器人运动
      parameters_client_->set_parameters({rclcpp::Parameter("linear",0.1),rclcpp::Parameter("angular",0.0)});
      return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }
    rclcpp_action::CancelResponse handle_cancel(std::shared_ptr<rclcpp_action::ServerGoalHandle<Nav>>goal_handle)
    {
      RCLCPP_INFO(this->get_logger(),"客户端发送了取消请求......");
      parameters_client_->set_parameters({rclcpp::Parameter("linear",0.0),rclcpp::Parameter("angular",0.0)});
      return rclcpp_action::CancelResponse::ACCEPT;
    }
    void execute(std::shared_ptr<rclcpp_action::ServerGoalHandle<Nav>>goal_handle)
    {
      // 获取前进距离
      double goal_num = goal_handle->get_goal()->goal;
      // 连续反馈对象
      auto feedback_msg = std::make_shared<Nav::Feedback>();
      // 生成最终响应对象
      auto nav_resu = std::make_shared<Nav::Result>();
      // 1.生成连续发聩
      // 1-1.循环中发布剩余距离,循环体有休眠时间;
      // 1-2.剩余距离怎么计算? 订阅里程计(通过目标点和里程计坐标计算剩余距离)
      rclcpp::Rate rate(1.0);
      while (rclcpp::ok())
      {
        // 计算剩余距离
        // 目标值 - 当前机器人x坐标
        // goal_num ------ 目标值,前进距离
        // x ------------- 机器人的实时坐标
        // x 0 -- 0.48
        // double distance = goal_num - x;  // 有问题
        // double distance = goal_num + 导航开始时x - x ;
        double distance = goal_num + last_x - x;
        // 组织发布数据
        feedback_msg->distance = distance;
        goal_handle->publish_feedback(feedback_msg);
        RCLCPP_INFO(this->get_logger(),"连续发聩中,剩余距离:%.2f",distance);
        if(goal_handle->is_canceling())
        {
          last_x = x;
          RCLCPP_INFO(this->get_logger(),"任务被取消");
          nav_resu->x = x;
          nav_resu->y = y;
          goal_handle->canceled(nav_resu);
          return;
        }
        // 设置退出逻辑
        if(distance <= 0.1)
        {
          RCLCPP_INFO(this->get_logger(),"到达目标点......");
          // 让机器人停止
          parameters_client_->set_parameters({rclcpp::Parameter("linear",0.0),rclcpp::Parameter("angular",0.0)});
          last_x = x;
          break; // 退出循环
        }
        rate.sleep();
      }
      // 2.生成最终响应结果
      if (rclcpp::ok())
      {
        nav_resu->x = x;
        nav_resu->y = y;
        goal_handle->succeed(nav_resu);
      }
    }
    void handle_accepted(std::shared_ptr<rclcpp_action::ServerGoalHandle<Nav>>goal_handle)
    {
      // 生成连续反馈以及最终响应(子线程单独实现)
      std::thread{std::bind(&NavServer::execute,this,_1),goal_handle}.detach();
    } 
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<NavServer>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}