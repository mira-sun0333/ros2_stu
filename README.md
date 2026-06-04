# ros2_stu —— ROS2 入门教学演示项目

基于 ROS2 Humble 的教学演示项目，涵盖机器人感知与导航的常见传感器数据处理场景。每个主题同时提供 **C++** 和 **Python** 两种实现，适合对比学习。

## 项目结构

```
ws_capa/
├── src/
│   └── demo/
│       ├── cpp00_odom/         # 里程计 → 轨迹(Path) [C++]
│       ├── py00_odom/          # 里程计 → 轨迹(Path) [Python]
│       ├── cpp01_laser/        # 单线激光雷达仿真(LaserScan) [C++]
│       ├── py01_laser/         # 单线激光雷达仿真(LaserScan) [Python]
│       ├── cpp02_laser/        # 多线激光雷达仿真(PointCloud2) [C++]
│       ├── py02_laser/         # 多线激光雷达仿真(PointCloud2) [Python]
│       ├── cpp03_camera/       # 单目相机仿真(Image) [C++]
│       ├── py03_camera/        # 单目相机仿真(Image) [Python]
│       ├── cpp04_imu/          # IMU → TF 坐标变换 [C++]
│       ├── py04_imu/           # IMU → TF 坐标变换 [Python]
│       ├── cpp05_gnss/         # GNSS 定位 → 轨迹(Path) [C++]
│       └── py05_gnss/          # GNSS 定位 → 轨迹(Path) [Python]
├── build/                      # 编译产物
├── install/                    # 安装产物
└── log/                        # 运行日志
```

## 软件包说明

### 0. 里程计轨迹生成 (`cpp00_odom` / `py00_odom`)

- **可执行文件**: `odom2path` / `odom2path_py`
- **订阅话题**: `/odom` (`nav_msgs/Odometry`)
- **发布话题**: `/path` (`nav_msgs/Path`)
- **功能**: 订阅里程计消息，按 0.1 米间距采样位姿，累积生成 Path 轨迹。

### 1. 单线激光雷达仿真 (`cpp01_laser` / `py01_laser`)

- **可执行文件**: `sim_laser` / `sim_laser_py`
- **发布话题**: `/scan` (`sensor_msgs/LaserScan`)
- **功能**: 模拟 360° 单线激光雷达，100 个采样点，半径 0.5m~3.0m，强度固定 40.0。

### 2. 多线激光雷达仿真 (`cpp02_laser` / `py02_laser`)

- **可执行文件**: `sim_pcl` / `sim_plc_py`
- **发布话题**: `/pcl` (`sensor_msgs/PointCloud2`)
- **功能**: 模拟 60列×30行 三维点云，包含 x/y/z/intensity 字段，模拟多线激光雷达数据。

### 3. 单目相机仿真 (`cpp03_camera` / `py03_camera`)

- **可执行文件**: `sim_cam` / `sim_cam_py`
- **发布话题**: `/image` (`sensor_msgs/Image`)
- **功能**: 模拟 640×480 RGB8 彩色图像，输出三色条纹（红/绿/蓝），周期 0.2s，frame_id 为 "camera"。

### 4. IMU → TF 坐标变换 (`cpp04_imu` / `py04_imu`)

- **可执行文件**: `imu2tf`
- **订阅话题**: `/imu` (`sensor_msgs/Imu`)
- **功能**: 订阅 IMU 数据，提取四元数旋转，发布 `world → mycar` 的 TF 坐标变换。

### 5. GNSS 定位轨迹生成 (`cpp05_gnss` / `py05_gnss`)

- **可执行文件**: `gnss2path` / `gnss2path_py`
- **订阅话题**: `/gps/fix` (`sensor_msgs/NavSatFix`)
- **发布话题**: `/path` / `/gnss_path` (`nav_msgs/Path`)
- **功能**: 订阅 GNSS 经纬度，以第一条消息为原点，转换为 WGS84 平面坐标后累积路径轨迹。

## 环境要求

- **操作系统**: Ubuntu 22.04
- **ROS2 版本**: Humble Hawksbill
- **编译工具**: colcon
- **依赖**: `rclcpp` / `rclpy`, `sensor_msgs`, `nav_msgs`, `geometry_msgs`, `tf2_ros`

## 编译与运行

```bash
# 1. 进入工作空间
cd ~/ws_capa

# 2. 编译所有包
colcon build --packages-up-to cpp03_camera py03_camera ...

# 3. 设置环境变量
source install/setup.bash

# 4. 运行示例（以单目相机为例）
ros2 run cpp03_camera sim_cam
# 或 Python 版
ros2 run py03_camera sim_cam_py
```
