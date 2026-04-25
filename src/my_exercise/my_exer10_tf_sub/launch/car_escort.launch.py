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
        1.
          在机器人A的左后方指定一个固定坐标系left_back,该坐标系是机器人B实现护航的参考
          在机器人A的右后方指定一个固定坐标系right_back,该坐标系是机器人C实现护航的参考
        2.
          订阅坐标变换解析出机器人B和left_back的位姿关系,并生成控制B运动的速度指令
          订阅坐标变换解析出机器人C和right_back的位姿关系,并生成控制C运动的速度指令
"""
def generate_launch_description():
    # 在机器人A的左后方指定一个固定坐标系left_back,该坐标系是机器人B实现护航的参考
    # 在机器人A的右后方指定一个固定坐标系right_back,该坐标系是机器人C实现护航的参考
    car_a_frame = DeclareLaunchArgument(name="car_a_frame",default_value="robot_1/base_link")
    left_back = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        arguments=["--frame-id",LaunchConfiguration("car_a_frame"),"--child-frame-id","left_back_frame","--x","-0.5","--y","0.5"]
    )
    right_back = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        arguments=["--frame-id",LaunchConfiguration("car_a_frame"),"--child-frame-id","right_back_frame","--x","-0.5","--y","-0.5"]
    )
    # 订阅坐标变换解析出机器人B和left_back的位姿关系,并生成控制B运动的速度指令
    # 订阅坐标变换解析出机器人C和right_back的位姿关系,并生成控制C运动的速度指令
    left_listener = Node(
        package="my_exer10_tf_sub",
        executable="tf_listener",
        parameters=[{"frame_id":"robot_2/base_link","child_frame_id":"left_back_frame"}], # 左后 机器人B 和 left_back_frame
        remappings=[("/robot_0/cmd_vel","/robot_2/cmd_vel")]
    )
    right_listener = Node(
        package="my_exer10_tf_sub",
        executable="tf_listener",
        parameters=[{"frame_id":"robot_0/base_link","child_frame_id":"right_back_frame"}], # 右后 机器人C 和 right_back_frame
        # remappings=[("/robot_0/cmd_vel","/robot_2/cmd_vel")]
    )
    return LaunchDescription([
        car_a_frame,
        left_back,
        right_back,
        left_listener,
        right_listener
    ])