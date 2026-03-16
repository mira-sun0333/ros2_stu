import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PointStamped

"""
    需求:发布相对于laser坐标系的坐标点数据
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建发布方
            3-2创建定时器
            3-3在回调中组织并发布消息
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class PointBroadcasterPy(Node):
    def __init__(self):
        super().__init__("point_broadcaster_py_node_py")
        #3-1创建发布方
        self.point_pub = self.create_publisher(PointStamped,"point",10)
        #3-2创建定时器
        self.x = 0.2
        self.timer = self.create_timer(1.0,self.on_timer)
        #3-3在回调中组织并发布消息
    def on_timer(self):
        #组织消息
        ps = PointStamped()
        ps.header.stamp = self.get_clock().now().to_msg()
        ps.header.frame_id = "laser"
        self.x += 0.05
        ps.point.x = self.x
        ps.point.y = 0.0
        ps.point.z = 0.3



        #发布数据
        self.point_pub.publish(ps)


def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(PointBroadcasterPy())

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()