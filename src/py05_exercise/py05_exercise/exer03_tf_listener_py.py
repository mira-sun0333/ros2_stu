import rclpy
from rclpy.node import Node
from tf2_ros.buffer import Buffer
from tf2_ros.transform_listener import TransformListener
from geometry_msgs.msg import Twist
import math
"""
    需求:监听坐标变换广播数据,并且生成turtle1相对于turtle2的坐标数据,进而生成控制
        turtle2运动的速度指令
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1声明参数服务
            3-2创建一个缓存
            3-3创建监听器
            3-4创建速度发布方
            3-5创建一个定时器,实现坐标的变换,并且生成速度指令并发布
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class Exer03TFListenerPy(Node):
    def __init__(self):
        super().__init__("exer03_tf_listener_py_node_py")
        #3-1声明参数服务
        self.declare_parameter("father_frame","turtle2")
        self.declare_parameter("child_frame","turtle1")
        self.father_frame = self.get_parameter("father_frame").get_parameter_value().string_value()
        self.child_frame = self.get_parameter("child_frame").get_parameter_value().string_value()
        #3-2创建一个缓存
        self.buffer = Buffer()
        #3-3创建监听器
        self.listener = TransformListener(self.buffer,self)
        #3-4创建速度发布方
        self.cmd_pub = self.create_publisher(Twist,self.father_frame + "cmd_vel",10)
        #3-5创建一个定时器,实现坐标的变换,并且生成速度指令并发布
        self.timer = self.create_timer(1.0,self.on_timer)

    def on_timer(self):
        #1.实现坐标变换
        if self.buffer.can_transform(self.father_frame,self.child_frame,rclpy.time.Time()):
            #1.实现坐标变换
            ts = self.buffer.lookup_transform(self.father_frame,self.child_frame,rclpy.time.Time())


            #2.组织速度指令
            twist = Twist()
            twist.linear.x = 0.5 * math.sqrt(math.pow(ts.transform.translation.x,2) + math.pow(ts.transform.translation.y,2))
            twist.angular.z = 1.0 * math.atan2(ts.transform.translation.y,ts.transform.translation.x)

            #3.发布
            self.cmd_pub.publish(twist)

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(Exer03TFListenerPy())

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()