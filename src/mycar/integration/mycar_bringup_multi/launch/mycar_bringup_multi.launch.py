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
from launch_ros.actions import PushRosNamespace
from launch.actions import GroupAction
# 事件相关----------------------
# from launch.event_handlers import OnProcessStart, OnProcessExit
# from launch.actions import ExecuteProcess, RegisterEventHandler,LogInfo
# 获取功能包下share目录路径-------
from ament_index_python.packages import get_package_share_directory
import os
"""
    需求分析:
        1.不同机器人的相同模块,要有唯一的坐标系标识
          odom1 base_footprint1 base_link1 laser1 camera1
          odom2 base_footprint2 base_link2 laser2 camera2
        2.不同机器人要发布相对于map的坐标系关系
          map ---> odom1
          map ---> odom2
        3.需要为机器人发布订阅话题设置命名空间
          /car1/scan
          /car2/scan
    实   现:
        1.每个机器人都需要创建一个针对编队而实现的功能包
        2.在该机器人的功能包中重新设置坐标系名称,发布里程计相对于map的坐标系变换,并为当前机器人节点设置命名空间
        3.分模块实现
"""
def generate_launch_description():
    # 1.启动机器人的各个模块(不要去包含之前功能包提供launch文件,而是要直接调用节点,并为之设计新的配置文件)
    # 启动底盘
    # 解析变量
    mycar_model = os.environ("MYCAR_MODEL") # 底盘类型
    car_num = "1"
    try:
        car_num = os.environ["CAR_NUM"]
    except:
        pass
    base_driver = Node
    if mycar_model == "arduino":
        # 调用 ros2_arduino_bridge 的驱动节点启动 atduino 底盘
        base_driver = Node(package="ros2_arduino_bridge", 
            executable="arduino_node", 
            name="ros2_arduino_node",
            parameters=[os.path.join(get_package_share_directory("mycar_bringup_multi"),"params","arduino.yaml")],
        )
    else:
        # 调用 ros2_stm32_bridge 的驱动节点启动 stm32 底盘
        base_driver = Node(
            package="ros2_stm32_bridge",
            executable="base_controller",
            parameters=[os.path.join(get_package_share_directory("mycar_bringup_multi"), "params", mycar_model + ".yaml")],
        )
    # 集成激光雷达,直接包含launch文件
    lidar_launch = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            launch_file_path=os.path.join(
                get_package_share_directory("sllidar_ros2"),
                "launch",
                "sllidar_a1_launch.py"
            )
        ),launch_arguments=[
            ("frame_id","laser" + car_num)
        ]
    )
    # 相机
    camera_node = Node(
        package="usb_cam",
        executable="usb_cam_node_exe",
        output="screen",
        name="camera_node",
        parameters=[os.path.join(get_package_share_directory("mycar_bringup_multi"),"params","params_1.yaml")]
    )
    # 2.以分组的方式为当前机器人各个模块设置相同的命名空间
    group = GroupAction([PushRosNamespace("car" + car_num),base_driver,lidar_launch])
    # 3.发布坐标变换
    odom2map = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        arguments=["--frame_id","map","--child_frame_id","odom" + car_num,"--x","0.3","--y","-0.4"]
    )
    base_link2base_footprint = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        arguments=["--frame_id","base_footprint" + car_num,"--child_frame_id","base_link" + car_num,"--z","0.05"]
    )
    laser2base_link = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        arguments=["--frame_id","base_link" + car_num,"--child_frame_id","laser" + car_num,"--z","0.07"]
    )
    camera2base_link = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        arguments=["--frame_id","base_link" + car_num,"--child_frame_id","camera" + car_num,"--z","0.05","--x","0.15"]
    )
    return LaunchDescription([
        base_driver,
        group,
        base_link2base_footprint,
        laser2base_link,
        camera_node,
        camera2base_link,
        odom2map
    ])