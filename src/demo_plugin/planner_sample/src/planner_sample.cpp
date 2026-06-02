/*
    插件实现:
        1.继承基类,并重写基类函数;
        2.注册插件
*/
#include "nav_planner/base_planner.hpp"
#include "rclcpp/rclcpp.hpp"
#include "pluginlib/class_list_macros.hpp"
namespace planner_sample{
    class SamplePlanner:public nav_planner::BasePlanner,public rclcpp::Node{
        public:
            SamplePlanner():Node("sample_planner_node"){
                RCLCPP_INFO(this->get_logger(),"简单的路劲规划器插件创建了");
            }
            void init(double rate){
                RCLCPP_INFO(this->get_logger(),"简单的路劲规划器插件初始化,路径规划器频率是%.lfHZ",rate);
            }
            void path(){
                RCLCPP_INFO(this->get_logger(),"生成规划路径");
            }
    };
}
PLUGINLIB_EXPORT_CLASS(planner_sample::SamplePlanner,nav_planner::BasePlanner);