import rclpy
from rclpy.node import Node
from sensor_msgs.msg import LaserScan
import math
"""
    需求:
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建
            3-2实现回调函数
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class SimLaserPy(Node):
    def __init__(self):
        super().__init__("sim_laser_node_py")
        self.get_logger().info(f"成功创建节点sim_laser_node_py")
        # 设置采样点
        self.samples = 100
        self.rate = 5.0
        self.radius = 3.0
        # 订阅激光雷达消息创建发布方
        self.laser_sub = self.create_subscription(LaserScan, "scan",10)
        # 定时器
        self.timer = self.create_timer(0.1, self.timer_cb)

    # 回调函数
    def timer_cb(self):#回调函数
        msg = LaserScan()
        msg.header.frame_id = "laser"
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.angle_min = -math.pi
        msg.angle_max = math.pi
        msg.angle_increment = math.pi * 2 /self.samples
        msg.range_min = 0.15
        msg.range_max = 12.0
        msg.scan_time = 1.0 / self.rate
        msg.time_increment = msg.scan_time / self.samples
        for i in range(0,self.samples):
            msg.ranges.append(self.radius)
            msg.intensities.append(40.0)
        if self.radius > 0.5:
            self.radius -= 0.1
        # 组织并发布
        self.publish_scan()
        # 发布激光雷达消息
        self.laser_pub.publish(msg)
def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(SimLaserPy())

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()