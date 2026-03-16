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
from ament_index_python.packages import get_package_share_directory
import os
# 编写launch文件,启动slam_toolbox的同步建图节点
def generate_launch_description():

    # 为节点加载配置文件
    # 问题: 高耦合,灵活性差,导致不方便代码移植
    #  params_path = os.path.join(get_package_share_directory("mycar_slam_slam_toolbox"),"params","online_sync_slam.yaml")
    # 解决: 使用   LaunchArgument  动态传参
    # 实现:
    # 1.DeclareLaunchArgument  声明  yaml  文件路径
    # 方便仿真与实体机器人来回切换的优化操作
    use_sim_time = DeclareLaunchArgument("use_sim_time",default_value="true")
    params_path = DeclareLaunchArgument("params_path",default_value=os.path.join(get_package_share_directory("mycar_slam_slam_toolbox"),"params","online_sync_slam_mycar.yaml"))
    # params_path = os.path.join(get_package_share_directory("mycar_slam_slam_toolbox"),"params","online_sync_slam.yaml")
    # 创建同步建图节点对应的node对象
    slam_node = Node(
        package="slam_toolbox",
        executable="sync_slam_toolbox_node",
        name="my_slam_toolbox",
        # parameters=[params_path]
        # 2.LaunchConfiguration  获取文件
        parameters=[LaunchConfiguration("params_path"),
                    {"use_sim_time": LaunchConfiguration("use_sim_time")}
                    
                    ]
    )

    return LaunchDescription([
        use_sim_time,
        params_path,
        slam_node
    ])

# 3.执行launch文件时,动态传入 yaml 文件路径