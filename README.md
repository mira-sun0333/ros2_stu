# ROS2 Progress Workspace

这是一个 ROS2 (Robot Operating System 2) 学习与练习的工作空间，包含了多个核心功能的演示示例。

## 工作空间结构

```
ros2_progress_ws/
├── src/
│   ├── demo_component/        # 组件演示
│   ├── demo_intra_process/    # 进程内通信演示
│   ├── demo_lifecycle/       # 生命周期节点演示
│   ├── demo_param_event/     # 参数事件演示
│   ├── demo_plugin/          # 插件机制演示
│   └── demo_topic_statistics/ # 话题统计演示
├── install/                  # 安装目录 (colcon build 生成)
├── log/                      # 日志目录
└── build/                    # 构建目录
```

## 演示包说明

### demo_component
组件演示包，展示 ROS2 组件的创建与使用。

### demo_intra_process
进程内通信演示，包含 Python 和 C++ 两种实现：
- `intra_process_py` - Python 版本
- `intra_process_cpp` - C++ 版本

### demo_lifecycle
生命周期节点演示，展示如何创建和管理 ROS2 生命周期节点：
- `lifecycle_py` - Python 版本
- `lifecycle_cpp` - C++ 版本

### demo_param_event
参数事件演示，展示参数监控与事件处理：
- `param_event_py` - Python 版本
- `param_event_cpp` - C++ 版本

### demo_plugin
插件机制演示，展示 ROS2 插件系统的使用方法：
- `nav_planner` - 导航规划器基类
- `planner_sample` - 规划器插件实现
- `load_planner` - 插件加载器

### demo_topic_statistics
话题统计演示，展示话题监控与数据统计：
- `talker` - 发布者
- `topic_statistics` - 统计功能包

## 环境要求

- ROS2 (建议 Humble 或更高版本)
- C++ 编译器 (支持 C++14 或更高)
- Python 3.8 或更高版本

## 构建与运行

### 编译工作空间

```bash
cd ~/ros2_progress_ws
colcon build
```

### 设置环境

```bash
source install/setup.bash
```

### 运行演示

根据需要运行各个演示包，例如：

```bash
# 运行 talker
ros2 run talker talker

# 运行 lifecycle 演示
ros2 run lifecycle_cpp lifecycle_demo

# 运行 plugin 演示
ros2 run load_planner load_planner
```

## 主要功能特性

- **生命周期管理**: 展示 ROS2 节点的生命周期状态转换
- **进程内通信**: 演示同一进程内节点间的高效通信
- **参数事件**: 监控参数变化并响应事件
- **插件系统**: 演示动态加载插件的机制
- **话题统计**: 展示话题数据传输的监控与统计

## 学习资源

- [ROS2 官方文档](https://docs.ros.org/)
- [ROS2 中文社区](https://doc.ros.org.cn/)

## 维护者

- iammira
