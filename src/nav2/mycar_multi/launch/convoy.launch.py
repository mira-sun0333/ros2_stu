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
# from ament_index_python.packages import get_package_share_directory

"""
    组织参与编队的车辆的
    每有一辆车加入编队时,都需要调用一次 Convoy 节点.

"""
def generate_launch_description():

    # 发布两辆从车的目标坐标系
    robot_1_goal = Node(
        package="mycar_multi",
        executable="convoy",
        name="robot_1_goal",
        parameters=[
            {"x": -1.0},
            {"y": 0.8},
            {"yaw": 0.0},
            {"frame_id": "robot_0/base_link"},
            {"child_frame_id": "g1"},

        ]
    )
    robot_2_goal = Node(
        package="mycar_multi",
        executable="convoy",
        name="robot_2_goal",
        parameters=[
            {"x": -1.0},
            {"y": -0.8},
            {"yaw": 0.0},
            {"frame_id": "robot_0/base_link"},
            {"child_frame_id": "g2"},
        ],
        remappings=[("/robot_1/goal_pose","/robot_2/goal_pose")]
    )


    return LaunchDescription([
        robot_1_goal,
        robot_2_goal,
    ])