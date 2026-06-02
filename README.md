# ROS2 Driver Workspace

ROS2 机器人驱动工作空间，包含串口通信驱动和底盘控制功能。

## 项目结构

```
ROS2_DRIVER_WS/
├── src/
│   ├── boost_asio_demo/     # Boost ASIO 串口通信演示
│   │   ├── CMakeLists.txt
│   │   ├── package.xml
│   │   └── src/
│   │       └── helloworld.cpp
│   └── mycar_driver/        # 机器人底盘驱动
│       ├── CMakeLists.txt
│       ├── package.xml
│       ├── include/
│       │   └── mycar_driver/
│       │       └── my_serial.hpp
│       └── src/
│           ├── driver.cpp
│           ├── driver_port.cpp
│           ├── driver_port_cmd.cpp
│           ├── driver_port_cmd_vol.cpp
│           ├── driver_port_cmd_vol_enc.cpp
│           ├── driver_port_cmd_vol_enc_imu.cpp
│           └── driver_test.cpp
└── .gitignore
```

## 功能特性

### mycar_driver

- **速度控制**：订阅 `cmd_vel` 话题，控制机器人运动
- **电池电压监测**：发布电池电压数据
- **编码器数据**：发布四轮编码器检测值
- **里程计**：根据编码器数据计算机器人位移和姿态
- **IMU 数据**：发布三轴角速度、加速度和欧拉角数据

### boost_asio_demo

- Boost ASIO 串口通信基础演示

## 依赖

- ROS2 (Humble/Jazzy)
- Boost ASIO
- geometry_msgs
- sensor_msgs
- nav_msgs
- tf2 / tf2_ros

## 构建

```bash
cd ~/ROS2_DRIVER_WS
colcon build
source install/setup.bash
```

## 运行

```bash
# 启动底盘驱动
ros2 run mycar_driver driver

# 启动测试节点
ros2 run mycar_driver driver_test
```

## 话题接口

| 话题名 | 类型 | 方向 | 描述 |
|--------|------|------|------|
| `/cmd_vel` | geometry_msgs/msg/Twist | 订阅 | 速度指令 |
| `/battery_voltage` | std_msgs/msg/UInt16 | 发布 | 电池电压 |
| `/encoder_values` | std_msgs/msg/UInt16MultiArray | 发布 | 编码器数据 |
| `/odom` | nav_msgs/msg/Odometry | 发布 | 里程计 |
| `/imu` | sensor_msgs/msg/Imu | 发布 | IMU数据 |

## License

MIT
