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
# from launch.actions import IncludeLaunchDescription
# from launch.launch_description_sources import PythonLaunchDescriptionSource
# 分组相关----------------------
# from launch_ros.actions import PushRosNamespace
# from launch.actions import GroupAction
# 事件相关----------------------
# from launch.event_handlers import OnProcessStart, OnProcessExit
# from launch.actions import ExecuteProcess, RegisterEventHandler,LogInfo
# 获取功能包下share目录路径-------
# from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    # 声明参数
    use_sim_time = DeclareLaunchArgument("use_sim_time",default_value="true")
    yaml_filename = DeclareLaunchArgument("yaml_filename",default_value="map/my_map.yaml")


    # 调用地图服务节点
    map_server_node = Node(
        package="nav2_map_server",
        executable="map_server",
        name="map_server",
        parameters=[
            {"yaml_filename": LaunchConfiguration("yaml_filename")},
            {"use_sim_time" : LaunchConfiguration("use_sim_time")}
        ]
    )


    # 调用生命周期管理节点
    manager_node = Node(
        package="nav2_lifecycle_manager",
        executable="lifecycle_manager",
        name="lifecycle_manager_map_server",
        parameters=[
            {"use_sim_time" : LaunchConfiguration("use_sim_time")},
            {"autostart" : True},
            {"node_names" : ["map_server"]}
        ]
    )

    return LaunchDescription([
        use_sim_time,
        yaml_filename,
        map_server_node,
        manager_node
    ])