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
    需求:
        启动stage的多车环境
        发布两辆车的起点相对于map的坐标变换
"""
def generate_launch_description():
    # 启动stage的多车环境
    stage_ros2_multi = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            launch_file_path=os.path.join(
                get_package_share_directory("stage_ros2"),
                "launch",
                "my_house_multi.launch.py"
            )
        )
    )
    # 发布两辆车的起点相对于map的坐标变换
    robot0odom2map = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="robot0odom2map",
        arguments=["--frame_id","map","--child_frame_id","robot_0/odom","--x","1","--y","-1","--yaw","-0.785"]
    )
    robot1odom2map = Node(
         package="tf2_ros",
        executable="static_transform_publisher",
        name="robot1odom2map",
        arguments=["--frame_id","map","--child_frame_id","robot_1/odom","--x","3","--y","-3","--yaw","-1.57"]
    )
    robot2odom2map = Node(
         package="tf2_ros",
        executable="static_transform_publisher",
        name="robot2odom2map",
        arguments=["--frame_id","map","--child_frame_id","robot_2/odom","--x","4","--y","-1.5","--yaw","-2.355"]
    )
    return LaunchDescription([
        stage_ros2_multi,
        robot0odom2map,
        robot1odom2map,
        robot2odom2map
    ])