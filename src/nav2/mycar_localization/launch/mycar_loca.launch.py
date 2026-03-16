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
def generate_launch_description():

    use_sim_time = DeclareLaunchArgument("use_sim_time",default_value="true")
    # 1.加载地图数据
    map_server_launch = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            launch_file_path=os.path.join(get_package_share_directory("mycar_map_server"),"launch","map_server.launch.py")
        ),
        launch_arguments=[
            ("use_sim_time",LaunchConfiguration("use_sim_time"))
        ]
    )


    # 2.启动amcl节点
    amcl_node = Node(
        package="nav2_amcl",
        executable="amcl",
        name="amcl_node",
        parameters=[
            os.path.join(get_package_share_directory("mycar_localization"),"params","amcl.yaml"),
            {"use_sim_time" : LaunchConfiguration("use_sim_time")}
        ]
    )

    # 3.启动生命周期管理器
    manager_amcl = Node(
        package="nav2_lifecycle_manager",
        executable="lifecycle_manager",
        name="lifecycle_manager_amcl",
        parameters=[
            {"use_sim_time" : LaunchConfiguration("use_sim_time")},
            {"autostart" : True},
            {"node_names" : ["amcl_node"]}
        ]
    )


    return LaunchDescription([
        use_sim_time,
        map_server_launch,
        amcl_node,
        manager_amcl
    ])