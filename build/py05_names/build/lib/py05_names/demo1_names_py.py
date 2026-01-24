import rclpy
from rclpy.node import Node
from std_msgs.msg import String

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

class MyNode(Node):
    def __init__(self,str1):
        super().__init__("zhenkeng_py",namespace="zuoxie_py")
        #全局话题
        self.pub = self.create_publisher(String,"/shi",10)
        #相对话题
        self.pub = self.create_publisher(String,"kaihui",10)
        #私有话题
        self.pub = self.create_publisher(String,"~/vip",10)
        
def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(MyNode())

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()