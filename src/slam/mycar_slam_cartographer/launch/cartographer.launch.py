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
# 需求: 调用cartographer实现SLAM
def generate_launch_description():

    # 声明仿真时间参数
    use_sim_time = DeclareLaunchArgument("use_sim_time",default_value="True")
    dirc = DeclareLaunchArgument("dirc",default_value=os.path.join(get_package_share_directory("mycar_slam_cartographeer"),"params"))
    base_name = DeclareLaunchArgument("base_name",default_value="mycar.lus")
    # 启动节点1 : 子地图发布节点(cartographer_node)
    cartographer_node = Node(
        package="cartographer_ros",
        executalble="cartographer_node",
        parameters=[{
            "use_sim_time":LaunchConfiguration("use_sim_time")
        }],
        # 配置文件加载 --- 设置文件路径(所属目录 + 文件名称)
        arguments=[
            # 设置目录路径
            "-configuration_directory", LaunchConfiguration("dirc"),
            # 设置配置文件
            "-configuration_basename", LaunchConfiguration("base_name")
        ]
    )
    

    # 启动节点2 : 地图拼接节点(cartographer_occupancy_grid_node)
    cartographer_occupancy_grid_node=Node(
        package="cartographer_ros",
        executalble="cartographer_occupancy_grid_node",
         parameters=[{
            "use_sim_time":LaunchConfiguration("use_sim_time")
        }],
    )



    return LaunchDescription([
        dirc,
        base_name,
        use_sim_time,
        cartographer_node,
        cartographer_occupancy_grid_node
    ])