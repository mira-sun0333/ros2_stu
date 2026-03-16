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
"""
    导航服务器的核心节点实现

    1.基础部分实现
        1-1.生命周管理器节点
            在Nav2中所有节点都是具有生命周期的节点
            生命周管理器节点会提供一套标准的方法来管理nav2中的节点
        1-2.行为树服务器节点
            控制导航的执行流程

    2.规划器实现
        生成从当前位置到目标点的路径
        2-1.规划器服务节点
        2-2.全局代价地图
        规划器节点依赖于全局代价地图

    3.运动控制实现
        控制机器人按照规划的路径移动
        3-1.运动控制节点
        3-2.局部地图
        运动控制节点依赖于局部代价地图

    4.恢复行为实现
        当机器人陷入困境时,自行脱困
    5.路点跟踪
        设置一系列的目标点集合，机器人可以一次到达这些目标点

    6.平滑路径
        平滑路径，使机器人运行更流畅，安全且可以减少硬件磨损

    7.速度平滑
        平滑速度，使机器人运行更流畅，安全可以减少硬件磨损

"""
def generate_launch_description():

    use_sim_time = DeclareLaunchArgument("use_sim_time",default_value="true")
    current_pkg = get_package_share_directory("mycar_navigation2")
    bt_yaml = os.path.join(current_pkg,"params","bt.yaml")
    planner_yaml = os.path.join(current_pkg,"params","planner.yaml")
    controller_yaml = os.path.join(current_pkg,"params","controller.yaml")
    behavior_yaml = os.path.join(current_pkg,"params","behavior.yaml")
    waypoint_yaml = os.path.join(current_pkg,"params","waypoint.yaml")
    waypoint_node = Node(
        package='nav2_waypoint_follower',
        executable='waypoint_follower',
        name='waypoint_follower',
        parameters=[
            #加载yaml文件
            waypoint_yaml,
            {'use_sim_time': LaunchConfiguration('use_sim_time')},
        ]
    )


    # 恢复行为节点
    behavior_node = Node(
        package="nav2_behavior",
        executable="behavior_server",
        name="behavior_server",
        parameters=[
            behavior_yaml,
            {"use_sim_time":LaunchConfiguration("use_sim_time")},
        ]
    )


    # 运动控制节点
    controller_node = Node(
        package="nav2_controller",
        executable="controller_server",
        name="controller_server",
        parameters=[
            controller_yaml,
            {"use_sim_time":LaunchConfiguration("use_sim_time")},

        ]
    )



    # 创建规划器节点
    planner_server = Node(
        package="nav2_planner",
        executable="planner_server",
        name="planner_server",
        parameters=[
            planner_yaml,
            {"use_sim_time":LaunchConfiguration("use_sim_time")},
            
        ]
    )


    # 行为树服务器
    bt_server = Node(
        package="nav2_bt_navigator",
        executable="bt_navigator",
        name="bt_navigator",
        parameters=[
            # 加载yaml文件
            bt_yaml,
            {"use_sim_time":LaunchConfiguration("use_sim_time"),
            # 自定义行为树并载入
            "default_nav_to_pose_bt_xml" : os.path.join(current_pkg,"bts","nav2_pose.xml"),
            "default_nav_through_poses_bt_xml" : os.path.join(current_pkg,"bts","nav2_poses.xml")},
        ]
    )
    # 声明生命周期管理器节点
    life_manager = Node(
        package="nav2_lifecycle_manager",
        executable="lifecycle_manager",
        name="lifecycle_manager_navigation",
        parameters=[
            {"use_sim_time":LaunchConfiguration("use_sim_time")},
            {"autostart":True},
            {"node_names":[  # 被托管的节点列表
                "bt_navigator",
                "planner_server",
                "controller_server",
                "behavior_server",
                "waypoint_follower",
            ]}
        ]
    )


    return LaunchDescription([
        use_sim_time,
        waypoint_node,
        behavior_node,
        controller_node,
        bt_server,
        planner_server,
        life_manager,
        
    ])