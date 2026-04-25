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
from launch.conditions import IfCondition,UnlessCondition

"""
    需求:
        在该launch文件中.需要启动仿真环境或实体机器人,然后再启动运动控制节点
    分析:
        关键点 -- 判断逻辑
        使用仿真环境吗?
        使用机器人 --- 本地启动?
                  --- 远程启动? x
        目前ros2无法再launch文件中远程启动,只能借助于类似ssh的工具启动远程
    实现:
        1.简单集成
        2.实现逻辑处理
"""
def generate_launch_description():

    exer00_bringup = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            launch_file_path=os.path.join(
                get_package_share_directory("my_exer08_launch"),
                "launch",
                "exer00_bringup.launch.py"
            )
        )
    )


    use_pub_vel = DeclareLaunchArgument(name="use_pub_vel",default_value="True")
    # 运动控制节点
    pub_vel_node = Node(
        package="my_exer01_topic_pub",
        executable="pub_vel",
        condition=IfCondition(LaunchConfiguration("use_pub_vel"))
    )
    return LaunchDescription([
        use_pub_vel,
        exer00_bringup,
        pub_vel_node
    ])
    # ld = LaunchDescription()
    # return ld