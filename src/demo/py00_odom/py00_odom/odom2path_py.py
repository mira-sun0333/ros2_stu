import rclpy
from rclpy.node import Node
from nav_msgs.msg import Odometry,Path
from geometry_msgs.msg import Pose,PoseStamped
from std_msgs.msg import Header,Time
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

class Odom2PathPy(Node):
    def __init__(self,str1):
        super().__init__("odom2path_node_py")
        # 设置储存轨迹坐标点的列表
        self.poses = list()
        # 设置储存轨迹中最后一个坐标点的临时变量
        self.last_pose = Pose()
        # 将起点添加进轨迹中
        self.append_pose()
        # 订阅里程计消息
        self.odom_sub = self.create_subscription(Odometry, "odom", self.odom_cb, 10)
        # 发布路径消息
        self.path_pub = self.create_publisher(Path, "path", 10)
    def odom_cb(self,odom:Odometry):#回调函数
        # 将里程计中位姿信息按照条件取出,储存在轨迹中,并发布
        # 1.获取机器人位姿
        current_pose = odom.pose.pose
        # 2.有条件的将当前位姿添加进轨迹中
        if self.get_distance(current_pose) > 0.1:
            # 将当前坐标点赋值给 last_pose
            self.last_pose = current_pose
            # 将当前坐标点添加进 poses
            self.append_pose(current_pose)

        # 3.发布路径消息
        self.publish_path()
    # 发布轨迹
    def publish_path(self):
        # 将poses 转换成Path类型的,然后再发布
        # 1.创建Path类型的对象
        path = Path()
        path.header.frame_id = "odom"
        path.header.stamp = self.get_clock().now().to_msg()
        path.poses = self.poses
        # 发布路径消息
        self.path_pub.publish(path)
    # 将一个坐标点添加进 poses
    def append_pose(self):
        # 将last_pose 转换成PoseStamp类型的,然后再添加
        ps = PoseStamped()
        ps.header.frame_id = "odom"
        ps.header.stamp = self.get_clock().now().to_msg()
        ps.pose = self.last_pose
        # 添加到 poses 中
        self.poses.append(ps)
    # 计算两个坐标点的距离
    def get_distance(self,current_pose):
        dx = self.last_pose.position.x - current_pose.position.x
        dy = self.last_pose.position.y - current_pose.position.y
        return math.sqrt(dx**2 + dy**2)


def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(Odom2PathPy())

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()