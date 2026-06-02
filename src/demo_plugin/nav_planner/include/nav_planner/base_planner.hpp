/*
    路劲规划器插件的共同基类,在该类中编写插件所需实现的函数
        1.初始化操作
        2.生成路径
*/
#ifndef NAV_PLANNER_BASE_PLANNER_HPP
#define NAV_PLANNER_BASE_PLANNER_HPP
namespace nav_planner{
    class BasePlanner{
        protected:
            // 插件基类必须提供无参构造
            BasePlanner(){}
        public:
            // 初始化函数
            virtual void init(double rate) = 0;
            // 路径生成函数
            virtual void path() = 0;
    };
}

#endif