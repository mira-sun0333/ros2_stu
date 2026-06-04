#install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
#<exec_depend>ros2launch</exec_depend>
#from glob import glob 用于setup.py配置多个launch文件
#('share/' + package_name, glob('launch/py/*_launch.py')),

from launch import LaunchDescription
from launch_ros.actions import Node
# 封装终端指令相关类--------------
# from launch.actions import ExecuteProcess
# from launch.substitutions import FindExecutable   #FindExecutable(name="ros2")
# 参数声明与获取-----------------
# from launch.actions import DeclareLaunchArgument
# from launch.substitutions import LaunchConfiguration
# 文件包含相关-------------------
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
# 分组相关----------------------
# from launch_ros.actions import PushRosNamespace
# from launch.actions import GroupAction
# 事件相关----------------------
# from launch.event_handlers import OnProcessStart, OnProcessExit
# from launch.actions import ExecuteProcess, RegisterEventHandler,LogInfo
# 获取功能包下share目录路径-------
from ament_index_python.packages import get_package_share_directory
import os
"""
    有条件的集成底盘驱动,雷达驱动,摄像头驱动的launch文件

    条件:
        底盘: Arduion Stm32
        雷达: SL_A1 LS_N10
        摄像头: 有 无
    要求:
        launch文件尽量 通用
    实现:
        1.以共享的环境变量储存机器人的各项参数
        2.launch中解析变量,并根据解析结果动态的包含对应的驱动文件
"""
def generate_launch_description():

    #解析变量
    mycar_model = os.environ["MYCAR_MODEL"] # 底盘类型
    lidar = os.environ["LIDAR"] # 雷达类型
    use_cam = os.environ["USE_CAM"] # 是否使用摄像头
    # 创建临时变量存储底盘的驱动文件路径
    driver_file = ""
    # 根据解析的 mycar_model 结果生成驱动文件路径
    if mycar_model == "arduino": # 注意这里是Arduino的底盘,不是Stm32
        driver_file = os.path.join(
            get_package_share_directory("ros2_arduino_bridge"),
            "launch",
            "ros2_arduino.launch.py"
        )
    else:
        driver_file = os.path.join(
            get_package_share_directory("ros2_stm32_bridge"),
            "launch",
            "driver.launch.py"
        )
    # 优化 
    # return LaunchDescription([])
    ld = LaunchDescription()
    # 包含底盘驱动
    mycar_launch = IncludeLaunchDescription(
        launch_description_sources=PythonLaunchDescriptionSource(
            driver_file
            # os.path.join(
            #     get_package_share_directory("ros2_stm32_bridge"),
            #     "launch",
            #     "driver.launch.py"
            # )
        )
    )
    ld.add_action(mycar_launch)
    # 包含雷达驱动
    lidar_launch = IncludeLaunchDescription(
        launch_description_sources=PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory("sllidar_ros2"),
                "launch",
                "sllidar_a1_launch.py"
            )
        )
    )
    ld.add_action(lidar_launch)
    if int(use_cam):
    # 包含摄像头驱动
        cam_launch = IncludeLaunchDescription(
            launch_description_sources=PythonLaunchDescriptionSource(
                os.path.join(
                    get_package_share_directory("my_usb_cam"),
                    "launch",
                    "my_usb_cam.launch.py"
                )
            )
        )
        ld.add_action(cam_launch)

    # ----------------------------------------------------
    # 优化
    #   odom:车辆出发点(原点)
    #   base_footprint:底盘几何中心在地面的投影
    # 当设置传感器位姿关系时,需要参考谁?
    #       1.参考 base_footprint 是可以的(不建议)
    #       2.参考底盘的几何中心base_link,相对于 base_footprint 在x和y上无偏移,在z上有偏移
    # 优化:
    #   1.发布 base_link 相对于 base_footprint 的坐标变换
    #   2.发布雷达相对于 base_link的坐标变换
    # 发布坐标变换实现
    # ld.add_action(
    #     Node(
    #         package="tf2_ros",
    #         executable="static_transform_publisher",
    #         name="laser2base_footprint",
    #         arguments= ["--frame-id", "base_footprint", "--child-frame-id", "laser" "--z" "0.12"]
    #     )
    # )

    ld.add_action(
        Node(
            package="tf2_ros",
            executable="static_transform_publisher",
            name="base_link2base_footprint",
            arguments= ["--frame-id", "base_footprint", "--child-frame-id", "base_link" "--z" "0.06"]
        )
    )
    ld.add_action(
        Node(
            package="tf2_ros",
            executable="static_transform_publisher",
            name="laser2base_link",
            arguments= ["--frame-id", "base_link", "--child-frame-id", "laser" "--z" "0.06"]
        )
    )
    ld.add_action(
        Node(
            package="tf2_ros",
            executable="static_transform_publisher",
            name="camera2base_link",
            arguments= ["--frame-id", "base_link", "--child-frame-id", "camera" "--z" "0.05", "--x", "0.13"] # 这里的数据需要自己去查看,修改
        )
    )
    return ld