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
from launch_ros.actions import PushRosNamespace
from launch.actions import GroupAction
# 事件相关----------------------
# from launch.event_handlers import OnProcessStart, OnProcessExit
# from launch.actions import ExecuteProcess, RegisterEventHandler,LogInfo
# 获取功能包下share目录路径-------
from ament_index_python.packages import get_package_share_directory
import os
"""
    需求: 多车定位时，每一辆车都安装了该launch
    文件所属的功能包,并且执行该文件,以实现每辆车的定位功能
    实现分析:
        1.删除地图服务,因为所有车共享一张地图
        2.定位节点进行 区分,每辆车的定位实现,都需要有自己的节点名称,话题名称以及参数等内容
    技术点:
        使用launch分组功能,为每一辆车设置不同的namespace,以及yaml参数

"""
def generate_launch_description():

    use_sim_time = DeclareLaunchArgument("use_sim_time",default_value="true")

    # 使用环境变量导入车辆编号,以作为命名空间或其他标识
    car_num = ""
    try:
        car_num = os.environ["CAR_NUM"]
    except Exception:
        pass

    group = GroupAction(
        actions=[
            PushRosNamespace(namespace=car_num),
            Node(
                package="nav2_amcl",
                executable="amcl",
                name="amcl_node",
                parameters=[
                    os.path.join(get_package_share_directory("mycar_localization"),"params",car_num,"amcl.yaml"),
                    {"use_sim_time" : LaunchConfiguration("use_sim_time")}
                ],
                # 切忌需要重映地图话题
                remappings=[
                    ("map","/map")
                ]
            ),
            Node(
                package="nav2_lifecycle_manager",
                executable="lifecycle_manager",
                name="lifecycle_manager_amcl",
                parameters=[
                    {"use_sim_time" : LaunchConfiguration("use_sim_time")},
                    {"autostart" : True},
                    {"node_names" : ["amcl_node"]}
                ]
            )

        ]
    )
    

    return LaunchDescription([
        use_sim_time,
        group,
    ])