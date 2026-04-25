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
"""
    需求分析:
        0.前提:准备一个多车环境,也可以是实体机器人
        1.在机器人A的正后方指定一个坐标系goal,该坐标系是机器人B实现跟随参考
        2.订阅坐标变换解析出机器人B和goal的位姿关系,并生成控制B运动的速度指令
"""
def generate_launch_description():
    # 1.在机器人A的正后方指定一个坐标系goal,该坐标系是机器人B实现跟随参考
    # 发布一个静态坐标变换
    car_a_frame = DeclareLaunchArgument(name="car_a_frame",default_value="base_link1")
    goal_frame = DeclareLaunchArgument(name="goal_frame",default_value="goal")
    goal_x = DeclareLaunchArgument(name="goal_x",default_value="-0.5")
    goal_tf = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        arguments=["--frame-id",LaunchConfiguration("car_a_frame"),"--child-frame-id",LaunchConfiguration("goal_frame"),"--x",LaunchConfiguration("goal_x")]
    )
    # 2.订阅坐标变换解析出机器人B和goal的位姿关系,并生成控制B运动的速度指令
    # 自行编写节点实现
    tf_listener = Node(
        package="my_exer10_tf_sub",
        executable="tf_listener",
        parameters=[{"frame_id":"base_link2"}],
        remappings=[("/robot_0/cmd_vel","/car2/cam_vel")]
    )
    return LaunchDescription([
        goal_tf,
        car_a_frame,
        goal_frame,
        goal_x,
        tf_listener
    ])