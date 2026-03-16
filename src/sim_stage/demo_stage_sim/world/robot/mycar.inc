# 组装机器人
incluce "robot/car_base.inc"
incluce "robot/laser.inc"
incluce "robot/camera.inc"


# 组织
# 1.底盘 + 摄像头 + 雷达位姿
define mycar car_base(
    my_camera(pose [0.15 0.0 0.0 0.0])   # 在底盘上安装摄像头
    my_laser()    # 在底盘上安装雷达
    #my_laser(pose [0.15 0.0 0.0 0.0])
    #my_laser(pose [-0.15 0.0 0.0 0.0])

)