import rclpy
from rclpy.node import Node
from tf2_ros import TransformBroadcaster
from turtlesim.msg import Pose
from geometry_msgs.msg import TransformStamped
import tf_transformations
"""
    需求:启动turtlesim_node节点,编写程序,发布乌龟(turtle)相对于窗体(world）的位姿
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建动态的广播器
            3-2创建一个乌龟位姿订阅方
            3-3在回调中,获取乌龟位姿并生成相对相关关系然后发布
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class TFDynaBroadcasterPy(Node):
    def __init__(self):
        super().__init__("tf_dyna_broadcaster_py_node_py")
        #3-1创建动态的广播器
        self.broadcaster = TransformBroadcaster(self)

        #3-2创建一个乌龟位姿订阅方
        self.sub = self.create_subscription(
            Pose,
            "/turtle1/pose",
            self.do_pose,
            10)
        #3-3在回调中,获取乌龟位姿并生成相对相关关系然后发布
    def do_pose(self,pose):
            #组织 transform
            ts = TransformStamped()
            ts.header.stamp = self.get_clock().now().to_msg()
            ts.header.frame_id = "world"

            ts.child_frame_id = "turtle1"

            #偏移量
            ts.transform._translation.x = pose.x
            ts.transform._translation.y = pose.y
            ts.transform._translation.z = 0.0
            #四元数
            qtn = tf_transformations.quaternion_from_euler(0.0,0.0,pose.theta)
            ts.transform.rotation.x = qtn[0]
            ts.transform.rotation.y = qtn[1]
            ts.transform.rotation.z = qtn[2]
            ts.transform.rotation.w = qtn[3]
     
            #发布 transform
            self.broadcaster.sendTransform(ts)
        
        
def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(TFDynaBroadcasterPy())

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()