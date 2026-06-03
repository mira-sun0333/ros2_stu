# WS_PRO_DEMO — ROS2 激光雷达驱动与控制系统

基于 ROS2 Humble 的 BlueSea 激光雷达驱动与控制工作空间。

## 项目结构

```
WS_PRO_DEMO/
├── src/
│   ├── my_lidar_srv/          # 自定义服务接口包
│   ├── my_lidar_driver/       # 激光雷达驱动与客户端
│   └── bluesea_sdk_base/      # BlueSea SDK 基础库
├── build/                     # colcon 构建输出
├── install/                   # colcon 安装输出
└── README.md
```

## 功能包说明

### 1. my_lidar_srv — 自定义服务接口

定义了 `Control.srv` 服务接口，用于控制激光雷达的启停和参数设置。

- **请求 (Request):**
  - `string func`   — 功能指令（如 `start`、`stop`、`rate`）
  - `float64 param` — 功能参数（如频率值）
- **响应 (Response):**
  - `int32 code`    — 执行结果状态码
  - `string value`  — 返回信息

### 2. my_lidar_driver — 激光雷达驱动

实现了 BlueSea 激光雷达的 ROS2 驱动节点，主要功能：

- **雷达消息发布：** 通过 SDK 读取雷达数据，以 `sensor_msgs/msg/LaserScan` 格式发布到 `/scan` 话题
- **雷达控制服务：** 提供 `control` 服务，支持 start / stop / rate 等指令
- **客户端工具：** 提供命令行客户端，可通过终端指令控制雷达

**命令行用法：**

```bash
ros2 run my_lidar_driver client start
ros2 run my_lidar_driver client stop
ros2 run my_lidar_driver client rate 5.0
```

**启动文件：** `launch/driver.launch.py`

```bash
ros2 launch my_lidar_driver driver.launch.py
```

**参数配置：** `params/lidar_driver.yaml` — 包含雷达连接参数、滤波参数、扫描范围等配置。

### 3. bluesea_sdk_base — BlueSea SDK 基础库

BlueSea 激光雷达通信 SDK，提供与雷达硬件的底层通信能力（UART / UDP / TCP），供 `my_lidar_driver` 使用。

## 依赖

- ROS2 Humble
- rclcpp
- sensor_msgs
- rosidl_default_generators

## 构建

```bash
cd WS_PRO_DEMO
source /opt/ros/humble/setup.bash
colcon build
source install/setup.bash
```

## 运行

1. 先启动雷达驱动节点：

```bash
ros2 launch my_lidar_driver driver.launch.py
```

2. 通过客户端控制雷达：

```bash
ros2 run my_lidar_driver client start   # 启动雷达
ros2 run my_lidar_driver client stop    # 停止雷达
ros2 run my_lidar_driver client rate 5.0 # 设置频率
```

## 串口权限

若使用串口连接雷达，需配置 udev 规则（参考 `script/LHLiDAR.rules`）：

```bash
sudo cp src/my_lidar_driver/script/LHLiDAR.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
sudo udevadm trigger
```

## 作者

- iammira (2157664041@qq.com)
