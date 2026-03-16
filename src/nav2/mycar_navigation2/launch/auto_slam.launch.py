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
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
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
    需求: 实现自主探索的SLAM
"""

def generate_launch_description():

    use_sim_time = DeclareLaunchArgument("use_sim_time",default_value="true")

    # 1.包含SLAM对应的launch  (替换AMCL.二者都可以定位和建图)
    slam_pkg = get_package_share_directory("mycar_slam_slam_toolbox")
    slam_launch = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            launch_file_path=os.path.join(slam_pkg,"launch","online_sync.launch.py")
        ),
        launch_arguments=[
            ("use_sim_time",LaunchConfiguration("use_sim_time"))
        ]
    )

    # 2.包含导航的核心实现
    nav2_pkg = get_package_share_directory("mycar_navigation2")
    nav2_launch = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            launch_file_path=os.path.join(nav2_pkg,"launch","nav2.launch.py")
        ),
        launch_arguments=[("use_sim_time",LaunchConfiguration("use_sim_time"))]
    )

    rviz2_node = Node(
        package="rviz2",
        executable="rviz2",
        arguments=["-d",os.path.join(nav2_pkg,"rviz2","nav2_rviz")]
    )





    return LaunchDescription([
        use_sim_time,
        slam_launch,
        nav2_launch,
        rviz2_node,

    ])