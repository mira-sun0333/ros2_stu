import argparse
import os
from pathlib import Path  # noqa: E402
import sys
from camera_config import CameraConfig, USB_CAM_DIR  # noqa: E402
from launch import LaunchDescription  # noqa: E402
from launch.actions import GroupAction  # noqa: E402
from launch_ros.actions import Node  # noqa: E402


CAMERAS = []
CAMERAS.append(
    CameraConfig(
        name='camera1',
        param_path=Path(USB_CAM_DIR, 'config', 'params_1.yaml')
    )
    # Add more Camera's here and they will automatically be launched below
)


def generate_launch_description():
    ld = LaunchDescription()

    parser = argparse.ArgumentParser(description='usb_cam demo')
    parser.add_argument('-n', '--node-name', dest='node_name', type=str,
                        help='name for device', default='usb_cam')
    




    # args, unknown = parser.parse_known_args(sys.argv[4:])
    # 需要改为自定义的功能包
    # usb_cam_dir = get_package_share_directory('my_usb_cam')

    # # get path to params file
    # params_path = os.path.join(
    #     usb_cam_dir,
    #     'config',
    #     'params.yaml'
    # )
    # node.name = args.node_name
    
    # print(params_path)
    # ld.add_action(Node(
    #     package='usb_cam',executable='usb_cam_node_exe',output='screen',
    #     name=node_name,
    #     # namespace=ns,
    #     parameters=[params_path]
    #     ))
    # 去掉下面的节点
    # ld.add_action(Node(
    #     package='usb_cam',executable='show_image.py',output='screen',
        # namespace=ns,
        # arguments=[image_manip_dir + "/data/mosaic.jpg"])
        # remapppings=[('image_in','image_raw')]
        # ))





    # camera_nodes = [
    #     Node(
    #         package='usb_cam', executable='usb_cam_node_exe', output='screen',
    #         name=camera.name,
    #         namespace=camera.namespace,
    #         parameters=[camera.param_path],
    #         remappings=camera.remappings
    #     )
    #     for camera in CAMERAS
    # ]

    # camera_group = GroupAction(camera_nodes)

    # ld.add_action(camera_group)
    return ld
