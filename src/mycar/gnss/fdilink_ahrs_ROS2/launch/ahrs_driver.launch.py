import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    pkg_dir = get_package_share_directory('fdilink_ahrs')

    ahrs_driver_node = Node(
        package='fdilink_ahrs',
        executable='ahrs_driver',
        name='ahrs_driver',
        output='screen',
        namespace='ahrs_imu',
        parameters=[{
            'port': '/dev/ttyUSB1',
            'baud': 921600,
            'device_type': 1,
            'imu_topic': 'imu',
            'imu_frame': 'imu',
            'mag_pose_2d_topic': 'mag_pose_2d',
            'imu_topic_trueEast': 'imu_trueEast',
            'mag_topic': 'magnetic_field',
            'yaw_offset': -2.094,
            'debug': False,
        }],
    )

    return LaunchDescription([
        ahrs_driver_node,
    ])
