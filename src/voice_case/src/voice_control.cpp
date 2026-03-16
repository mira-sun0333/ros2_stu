#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp" //导航消息
#include "geometry_msgs/msg/twist.hpp"
using namespace std::placeholders;
using namespace std::chrono_literals;


/*
    改进后的版本,通过发送导航action请求来实现导航功能,并且增加了取消导航的功能
    需求:通过语音控制机器人
          需要调用者下达语音指令,可以控制机器人运动,并且机器人会播报接受指令后即将进入的运动状态
          ps: "前进"
          robot:  status:前进  播报:"小车即将前进"
          自然语音 -> 被语音识别节点捕获 ---生成文本--> 到当前节点 
              分支1 提取关键字->生成速度(导航)指令
              分支2 生成文本--> 发送给语音合成节点
              语音合成节点-->播报
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建订阅方(提取关键字,并生成指令)
            3-2创建速度消息发布方,结合定时器发布速度指令
            3-3创建导航消息发布方
            3-4创建发布方,订阅到语音指令的文本时,生成消息即将进入的状态,发布给消息语音合成节点
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class VoiceControl :public rclcpp::Node{
public:
      VoiceControl():Node("voice_control_node_cpp"),linear(0.0),angular(0.0){
        //3-1创建订阅方(提取关键字,并生成指令)
        voice_words_sub_ = this->create_subscription<std_msgs::msg::String>("voicewords",10,std::bind(&VoiceControl::words_cd,this,_1));
        //3-2创建速度消息发布方,结合定时器发布速度指令
        twist_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel",10);
        // 3-2创建定时器,定时发布速度指令
        timer_ = this->create_wall_timer(50ms,std::bind(&VoiceControl::timer_cb,this));
        //3-3创建导航消息发布方
        words_pub_ = this->create_publisher<std_msgs::msg::String>("ttswords",10);

    }

private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr voice_words_sub_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr twist_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr words_pub_;
    double linear,angular;
    void timer_cb(){
      // 发布速度指令
      geometry_msgs::msg::Twist twist;
      twist.linear.x = linear;
      twist.angular.z = angular;
        
      twist_pub_->publish(twist);

    }


    void words_cd(const std_msgs::msg::String & msg){
      // 提取关键词
      std::string txt = msg.data;
      std_msgs::msg::String words;
      if (txt.find("前进") != std::string::npos)
      {
        // 生成速度指令
        linear = 0.2;
        angular = 0.0;
        words.data = "小车开始前进";
      }
       // 生成后退指令
      else if(txt.find("后退") != std::string::npos)
      {
        linear = 0.2;
        angular = 0.0;
        words.data = "小车开始后退";

      }
       // 生成左转指令
      else if(txt.find("左转") != std::string::npos)
      {
        linear = 0.0;
        angular = 0.2;
        words.data = "小车开始左转";

      }
       // 生成速度指令
      else if(txt.find("右转") != std::string::npos)
      {
        linear = 0.0;
        angular = -0.2;
        words.data = "小车开始右转";

      }
      // 生成停止指令
      else if(txt.find("停止") != std::string::npos)
      {
        linear = 0.0;
        angular = -0.2;
        words.data = "小车停止";
      }
      // 生成被语音合成节点订阅的文本,并发布
      words_pub_->publish(words);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<VoiceControl>());

    //释放资源
    rclcpp::shutdown();
    return 0;
}