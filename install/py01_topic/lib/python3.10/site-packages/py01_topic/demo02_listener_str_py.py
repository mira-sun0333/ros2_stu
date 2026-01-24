"""
需求:订阅发布方发布的消息,并在终端输出
流程:
1.导包
2.初始化ros2客户端
3.自定义节点
3-1创建订阅方
3-2解析并输出数据
4.调用spin函数,并床=传入节点对象
5.资源释放
"""
#1.导包
import rclpy
from rclpy.node import Node
from std_msgs.msg import String

#3.自定义节点
class Listener(Node):
    def __init__(self):
        super().__init__("listener_node_py")
        self.get_logger().info("订阅方创建了(python)!")
        #3-1创建订阅方
        """
        参数:
        1.消息类型
        2.话题名称
        3.回调函数
        4.QOS队列长度
        返回值:订阅对象
        
        """
        self.subscription = self.subscription(String,"chatten",self.do_cb,10)
       
    def do_cb(self,msg):
        #3-2解析并输出数据
        self.get_logger().info("订阅的数据:%s" % msg.data)

def main():
    #2.初始化ros2客户端
    rclpy.init()
    #4.调用spin函数,并床=传入节点对象
    rclpy.spin(Listener())
    #5.资源释放
    rclpy.shutdown()


if __name__ == '__main__':
    main()
