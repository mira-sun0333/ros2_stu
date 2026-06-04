# MoveIt 2 Workspace

这是一个基于 [MoveIt 2](https://github.com/ros-planning/moveit2) 的 ROS 2 工作空间。

## 工作空间结构

```
ws_moveit2/
├── src/
│   ├── moveit2/                    # MoveIt 2 核心框架
│   │   ├── moveit_core/             # 核心功能模块
│   │   ├── moveit_ros/              # ROS 相关组件
│   │   │   ├── move_group/          # MoveGroup 节点
│   │   │   ├── planning/            # 规划接口
│   │   │   ├── perception/          # 感知模块
│   │   │   ├── robot_interaction/   # 机器人交互
│   │   │   ├── visualization/      # 可视化
│   │   │   ├── warehouse/           # 数据库
│   │   │   ├── moveit_servo/        # 伺服控制
│   │   │   └── hybrid_planning/     # 混合规划
│   │   ├── moveit_planners/         # 规划器
│   │   │   ├── ompl/                # OMPL 规划器
│   │   │   ├── chomp/               # CHOMP 规划器
│   │   │   ├── trajopt/             # TrajOpt 规划器
│   │   │   └── pilz_industrial_motion_planner/  # Pilz 工业规划器
│   │   ├── moveit_kinematics/       # 运动学插件
│   │   ├── moveit_setup_assistant/  # 配置向导
│   │   ├── moveit_py/               # Python 绑定
│   │   ├── moveit_configs_utils/    # 配置工具
│   │   └── moveit_runtime/          # 运行时组件
│   ├── moveit2_tutorials/           # 教程和文档
│   ├── moveit_resources/            # 机器人资源
│   │   ├── panda_description/       # Panda 机械臂描述
│   │   ├── panda_moveit_config/     # Panda MoveIt 配置
│   │   ├── fanuc_description/       # Fanuc 机器人描述
│   │   ├── fanuc_moveit_config/     # Fanuc MoveIt 配置
│   │   └── pr2_description/          # PR2 机器人描述
│   ├── moveit_task_constructor/     # 任务规划框架
│   ├── moveit_visual_tools/         # 可视化工具
│   ├── launch_param_builder/        # 启动参数构建器
│   ├── rosparam_shortcuts/          # ROS 参数快捷工具
│   └── srdfdom/                     # SRDF 解析器
├── build/                           # 构建目录
├── install/                        # 安装目录
└── log/                            # 日志目录
```

## 主要功能

### MoveIt 2 核心组件

- **move_group**: 规划执行的核心节点
- **运动规划**: 支持 OMPL、CHOMP、TrajOpt 等多种规划器
- **碰撞检测**: 提供 Bullet、FCL 等碰撞检测方法
- **运动学**: 支持 IKFast、KDL、SRDL 等运动学插件
- **视觉感知**: 集成 occupancy map monitor
- **任务规划**: MoveIt Task Constructor 高级任务框架
- **伺服控制**: MoveIt Servo 实时伺服控制

### 支持的机器人

- **Panda**: Franka Emika Panda 机械臂
- **Fanuc**: Fanuc 工业机器人
- **PR2**: Personal Robot 2

## 构建

### 前提条件

- ROS 2 (Humble / Iron / Rolling)
- colcon
- CMake 3.22+
- GCC 11+

### 构建工作空间

```bash
# 源 ROS 2 环境
source /opt/ros/humble/setup.bash

# 安装依赖
rosdep install --from-paths src --ignore-src -r -y

# 构建
colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release
```

## 使用

### 源环境

```bash
source install/setup.bash
```

### 运行示例

```bash
# 启动 Panda 演示
ros2 launch panda_moveit_config demo.launch.py

# 启动 Move Group
ros2 launch moveit2_tutorials demo.launch.py
```

