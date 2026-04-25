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
        启动机器人或仿真环境
"""
def generate_launch_description():

    # 1.定义参数,相当于是标记,标记是否使用仿真,是否使用机器人
    use_sim = DeclareLaunchArgument(name="use_sim",default_value="True")
    use_local_robot = DeclareLaunchArgument(name="use_local_robot",default_value="False")

    # 2.解析参数,根据参数结果决定某些节点是否启动

    # 仿真环境
    stage_launch = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            launch_file_path=os.path.join(
                # 包路径
                get_package_share_directory("stage_ros2"),
                # launch
                "launch",
                # launch文件
                "my_house.launch.py"
            )
        ),condition=IfCondition(LaunchConfiguration("use_sim"))
        # ),condition=UnlessCondition(LaunchConfiguration("use_sim"))

    )

    # 实体机器人
    mycar_launch = IncludeLaunchDescription(
        launch_description_source=PythonLaunchDescriptionSource(
            launch_file_path=os.path.join(
                # 包路径
                get_package_share_directory("mycar_bringup"),
                # launch
                "launch",
                # launch文件
                "mycar_bringup.launch.py"
            )
        ),condition=IfCondition(LaunchConfiguration("use_local_robot"))
    )
    return LaunchDescription([
        use_sim,
        use_local_robot,    
        stage_launch,
        mycar_launch
    ])
    # ld = LaunchDescription()
    # return ld