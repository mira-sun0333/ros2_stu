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
# from launch.actions import IncludeLaunchDescription
# from launch.launch_description_sources import PythonLaunchDescriptionSource
# 分组相关----------------------
# from launch_ros.actions import PushRosNamespace
# from launch.actions import GroupAction
# 事件相关----------------------
# from launch.event_handlers import OnProcessStart, OnProcessExit
# from launch.actions import ExecuteProcess, RegisterEventHandler,LogInfo
# 获取功能包下share目录路径-------
from ament_index_python.packages import get_package_share_directory
import os
def generate_launch_description():
    # 1.启动两个激光雷达
    laser_up = Node(
        package='sllidar_ros2',
        executable='sllidar_node',
        name='laser_up_node',
        parameters=[{'channel_type':'serial',
                        'serial_port': '/dev/mylidar_up', 
                        'serial_baudrate': 115200, 
                        'frame_id': 'laser_up',
                        'inverted': False, 
                        'angle_compensate': True}],
        remapping={'/scan':'/scan_up'},
        output='screen')
    laser_down = Node(
        package='sllidar_ros2',
        executable='sllidar_node',
        name='laser_down_node',
        parameters=[{'channel_type':'serial',
                        'serial_port': '/dev/mylidar_down', 
                        'serial_baudrate': 115200, 
                        'frame_id': 'laser_down',
                        'inverted': False, 
                        'angle_compensate': True}],
        remapping={'/scan':'/scan_down'},
        output='screen')
    # 2.将两个激光雷达集成进坐标树
    base_link_laser_up = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='base_link_laser_up',
        arguments=["--frame-id", "base_link", "--child-frame-id", "laser_up", "--z", "0.35"],
        output='screen')
    base_link_laser_down = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='base_link_laser_down',
        arguments=["--frame-id", "base_link", "--child-frame-id", "laser_down", "--z", "0.05"],
        output='screen')
    # 3.融合
    ira_merger = Node(
        package='ira_laser_tools',
        executable='laserscan_multi_merger',
        name='ira_merger_node',
        parameters=[
            os.path.join(get_package_share_directory('mycar_laser_merger'), "params", 'ira_merger.yaml')
        ],
        output='screen')
    # 4.对融合后的数据进行过滤
    laser_filter = Node(
            package="laser_filters",
            executable="scan_to_scan_filter_chain",
            parameters=[
                os.path.join(
                    get_package_share_directory("mycar_laser_filters"),
                    "params", "box_filter.yaml"
                )
            ],
            remapping={'/scan':'/scan_multi'},
            output='screen'
    )
    return LaunchDescription([laser_up, laser_down, base_link_laser_up, base_link_laser_down, ira_merger,laser_filter])