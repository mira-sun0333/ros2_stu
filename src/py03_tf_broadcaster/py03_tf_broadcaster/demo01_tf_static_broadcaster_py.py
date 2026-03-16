import rclpy
from rclpy.node import Node
import sys
from rclpy.logging import get_logger
from tf2_ros.static_transform_broadcaster import StaticTransformBroadcaster
from geometry_msgs.msg import TransformStamped
import tf_transformations
"""
    需求:自实现静态坐标变换广播器,执行改程序时需要传入参数
          ros2 run 包 可执行程序 x y z roll pitch yaw frame child_frame
    流程:
        0.判断传入的参数是否合法
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建广播对象
            3-2组织并发布数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class TFStaticBroadcasterPy(Node):
    def __init__(self,argv):
        super().__init__("tf_static_broadcaster_py_node_py")
            #3-1创建广播对象
        self.broadcaster = StaticTransformBroadcaster(self)
            #3-2组织并发布数据
        self.pub_static_tf(argv)
    def pub_static_tf(self,argv):
        ts = TransformStamped()
        # 设置参数
        ts.header.stamp  =self.get_clock().now().to_msg() #设置时间戳
        ts.header.frame_id = argv[7]#设置父系坐标系
        ts.child_frame_id = argv[8]#设置子坐标系
        #设置平移
        ts.transform.translation.x = float(argv[1])
        ts.transform.translation.y = float(argv[2])
        ts.transform.translation.z = float(argv[3])
        #设置四元数
        #将欧拉角转换为四元数
        qtn = tf_transformations.quaternion_from_euler(
            float(argv[4]),
            float(argv[5]),
            float(argv[6])
        )
        ts.transform.rotation.x = qtn[0]
        ts.transform.rotation.y = qtn[1]
        ts.transform.rotation.z = qtn[2]
        ts.transform.rotation.w = qtn[3]



        self.broadcaster.sendTransform(ts)
       

    

def main():
    if len(sys.argv) !=9:
        get_logger("rclpy").error("传入的参数不合法!")
        return
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(TFStaticBroadcasterPy(sys.argv))

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()