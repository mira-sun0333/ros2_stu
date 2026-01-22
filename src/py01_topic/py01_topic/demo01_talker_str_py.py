"""
需求:以某个固定频率发送文本"hello world",文本后缀编号,每发布一条,编号+1
流程:
1.导包
2.初始化ros2客户端
3.自定义节点类;
3-1.创建消息发布方
3-2创建定时器
3-3组织并发布消息
4.使用spin函数,传入自定义类指针对象
5.释放资源

"""
#导包
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
#3.自定义节点类;
class Talker(Node):
    def __init__(self):
        super().__init__("talker_node_py")
        self.get_logger().info("发布方创建了(python)!")
        #设置计数器
        self.count = 0
        #3-1.创建消息发布方
        """
        参数:
        1.消息类型
        2.话题名称
        3.QOS 队列长度
        返回值:发布对象
        """
        self.publisher = self.create_publisher(String,"chatter",10)
        #3-2创建定时器
        """
        参数:
        1.时间间隔
        2.回调函数
        返回值: 定时器对象
        """
        self.timer = self.create_timer(1.0,self.on_timer)
        #3-3组织并发布消息

    def on_timer(self):
         #3-3组织并发布消息
         message = String()
         message.data = "hello world(python)!" + str(self.count)
         self.publisher.publish(message)
         self.count += 1
         self.get_logger().info("发布的数据:%s" % message.data)
def main():
   #2.初始化ros2客户端
   rclpy.init()
   #4.使用spin函数,传入自定义类指针对象
   rclpy.spin(Talker())
   #5.释放资源
   rclpy.shutdown()


if __name__ == '__main__':
    main()
