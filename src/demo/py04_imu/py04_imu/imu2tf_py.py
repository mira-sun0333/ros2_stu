import rclpy
from rclpy.node import Node
from tf2_ros import TransformBroadcaster
from sensor_msgs.msg import Imu
from geometry_msgs.msg import TransformStamped
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

class Imu2TF_py(Node):
    def __init__(self):
        super().__init__("imu2tf_node_py")
        self.tf_bro = TransformBroadcaster(self)
        self.imu_sub = self.create_subscription(Imu,"imu",self.imu_cb,10)
    def imu_cb(self,imu: Imu):
        # 组织
        ts = TransformStamped()
        ts.header.stamp = self.get_clock().now().to_msg()
        ts.header.frame_id = "world"
        ts.child_frame_id = "mycar"
        ts.transform.translation.z = 1.0
        ts.transform.rotation = imu.orientation
        # 发布
        self.tf_bro.sendTransform(ts)
def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(Imu2TF_py())

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()