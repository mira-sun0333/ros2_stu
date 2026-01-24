import rclpy
from rclpy.node import Node
from base_interfaces_demo.msg import Student

"""
    需求:订阅学生信息并打印在终端
    流程:
        1.导包
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建订阅方
            3-2解析订阅到的信息
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class ListenerStu(Node):
    def __init__(self,str1):
        super().__init__("listenerstu_node_py")
        #3-1创建订阅方
        self.subscription = self.create_subscription(Student,"chatter_stu",self.do_cb,10)
        #3-2解析订阅到的信息
    def do_cb(self,stu):
        self.get_logger().info("订阅到的学生信息:name= %s,age = %d,height = %.2f" % (stu.name,stu.age,stu.height))

    def call_back(self,msgs):#回调函数
        self.get_logger().info("数据是: %s"%msgs.data)

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(ListenerStu())

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()