# ws03_exercise - ROS2 机器人学习与实践工作空间

本项目是一个基于 ROS2 的机器人学习与实践工作空间，围绕自主移动机器人（mycar），通过 **10 个递进式练习包** 系统学习 ROS2 核心通信机制（话题、服务、动作、坐标变换），并配套实体机器人驱动功能包，实现从仿真到实体的完整开发流程。

## 实践重点

**odom 与 map 坐标系的关系**（详见 `实践需求.md`）：
- **odom**：基于里程计定位，存在累计误差
- **map**：基于地图匹配定位，可能产生跳变，环境单一时效果差
- 在机器人系统中，map 与 odom 之间是动态变换过程（以机器人为视角）

## 工作空间结构

```
ws03_exercise/
├── src/
│   ├── my_exercise/              # 练习功能包（10 个递进式练习）
│   │   ├── my_exer_interfaces    # 自定义接口（Cru.srv, Nav.action）
│   │   ├── my_exer01_topic_pub   # 话题通信：发布速度指令
│   │   ├── my_exer02_topic_sub   # 话题通信：订阅里程计
│   │   ├── my_exer03_service_server # 服务通信：巡航服务端
│   │   ├── my_exer04_service_client # 服务通信：巡航客户端
│   │   ├── my_exer05_action_server  # 动作通信：导航服务端
│   │   ├── my_exer06_action_client  # 动作通信：导航客户端
│   │   ├── my_exer07_log            # 日志系统
│   │   ├── my_exer08_launch         # Launch 文件集成
│   │   ├── my_exer09_tf_pub         # TF 坐标变换发布
│   │   └── my_exer10_tf_sub         # TF 坐标变换订阅（跟随/护航）
│   ├── mycar/                    # 实体机器人功能包
│   │   ├── base/                 # 底盘驱动（Arduino / STM32）
│   │   ├── camera/               # 摄像头驱动（USB Cam）
│   │   ├── laser/                # 激光雷达驱动（思岚 SL 系列）
│   │   └── integration/          # 启动集成（单机器人/多机器人/模型描述）
│   ├── Stage/                    # Stage 仿真器（子模块）
│   └── stage_ros2/               # Stage ROS2 接口（子模块）
```

## 练习包详解

### 自定义接口 (my_exer_interfaces)

| 接口 | 类型 | 请求 | 响应/反馈 |
|------|------|------|-----------|
| `Cru.srv` | Service | `int32 flag` (0=停止, 1=开始) | `float64 x, y` |
| `Nav.action` | Action | `float64 goal` (前进距离) | 结果: `float64 x, y` / 反馈: `float64 distance` |

### 练习 01-02：话题通信

- **exer01_topic_pub**：向 `/cmd_vel` 发布 `Twist` 速度指令，支持动态参数 `linear` / `angular` 控制线速度和角速度
- **exer02_topic_sub**：订阅 `/odom` 里程计消息，位移超过阈值（默认 0.5m，可动态调整）时输出坐标

### 练习 03-04：服务通信

- **exer03_service_server**：提供 `/cruising` 巡航服务，通过参数客户端复用 exer01 节点控制运动
- **exer04_service_client**：向 `/cruising` 发送巡航请求，接收机器人坐标

### 练习 05-06：动作通信

- **exer05_action_server**：提供 `/nav` 导航动作，控制机器人前进指定距离，连续反馈剩余距离，支持取消任务
- **exer06_action_client**：向 `/nav` 发送前进距离，接收连续反馈和最终响应，Ctrl+C 自动取消

### 练习 07：日志系统

- **exer07_log**：周期性输出 5 种级别日志（FATAL / DEBUG / ERROR / WARN / INFO）

### 练习 08：Launch 文件

| Launch 文件 | 功能 |
|-------------|------|
| `exer00_bringup.launch.py` | 启动仿真环境或实体机器人 |
| `exer01_topic_pub.launch.py` | 集成环境 + 运动控制节点 |
| `exer02_topic_sub.launch.py` | 集成环境 + 里程计订阅 |
| `exer03_service.launch.py` | 集成环境 + 巡航服务端 |
| `exer04_action.launch.py` | 集成环境 + 导航动作服务端 |

### 练习 09-10：TF 坐标变换

- **exer09_tf_pub**：发布多车 odom 到 map 的静态坐标变换
- **exer10_tf_sub**：订阅坐标变换，解析位姿关系生成速度指令，实现：
  - **跟随**：机器人 B 跟随机器人 A 后方的 goal 坐标系
  - **护航**：V 字形编队，B 跟随左后方，C 跟随右后方

## 实体机器人功能包 (mycar)

| 模块 | 功能包 | 说明 |
|------|--------|------|
| 底盘 | `ros2_arduino_bridge` | Arduino 底盘驱动（Python） |
| 底盘 | `ros2_stm32_bridge` | STM32 底盘驱动（C++） |
| 摄像头 | `usb_cam` + `my_usb_cam` | USB 摄像头驱动与封装 |
| 雷达 | `sllidar_ros2` | 思岚 SL 系列激光雷达驱动 |
| 集成 | `mycar_bringup` | 单机器人启动集成 |
| 集成 | `mycar_bringup_multi` | 多机器人编队启动集成 |
| 模型 | `mycar_description` | 机器人 URDF 模型（简单版） |
| 模型 | `mycar_description_detailed` | 机器人 URDF 模型（详细版，含 STL 网格） |

## 编译与运行

```bash
# 编译工作空间
colcon build

# 加载环境
source install/setup.bash

# 示例：启动仿真 + 话题发布
ros2 launch my_exer08_launch exer01_topic_pub.launch.py

# 示例：启动多车跟随
ros2 launch my_exer09_tf_pub stage_ros2_multi.launch.py
ros2 launch my_exer10_tf_sub car_follow.launch.py

# 示例：启动护航编队
ros2 launch my_exer09_tf_pub stage_ros2_multi_escort.launch.py
ros2 launch my_exer10_tf_sub car_escort.launch.py

# 示例：启动实体机器人
export MYCAR_MODEL=stm32
export LIDAR=SL_A1
export USE_CAM=1
ros2 launch mycar_bringup mycar_bringup.launch.py
```

## 子模块

| 子模块 | 来源 |
|--------|------|
| `src/Stage` | [damuxt/Stage](https://github.com/damuxt/Stage) |
| `src/stage_ros2` | [damuxt/stage_ros2](https://github.com/damuxt/stage_ros2) |
| `src/mycar/base/ros2_arduino_bridge` | [damuxt/ros2_arduino_bridge](https://github.com/damuxt/ros2_arduino_bridge) |
| `src/mycar/base/ros2_stm32_bridge` | [damuxt/ros2_stm32_bridge](https://github.com/damuxt/ros2_stm32_bridge) |
| `src/mycar/camera/usb_cam` | [ros-drivers/usb_cam](https://github.com/ros-drivers/usb_cam) |
| `src/mycar/laser/sllidar_ros2` | [Slamtec/sllidar_ros2](https://github.com/Slamtec/sllidar_ros2) |

克隆时自动拉取子模块：

```bash
git clone --recurse-submodules https://github.com/mira-sun0333/ros2_stu.git
```

## 依赖

- ROS2 (Foxy/Humble)
- Stage 仿真器
- tf2_ros
- colcon 构建工具

## 作者

Mira (2157664041@qq.com)
