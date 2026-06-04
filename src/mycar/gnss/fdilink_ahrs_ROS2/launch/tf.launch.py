from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    imu_tf_node = Node(
        package='fdilink_ahrs',
        executable='imu_tf',
        name='imu_data_to_tf',
        output='screen',
        parameters=[{
            'imu_topic': '/imu',
            'position_x': 0,
            'position_y': 0,
            'position_z': 0,
        }],
    )

    return LaunchDescription([
        imu_tf_node,
    ])
