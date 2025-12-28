import rclpy
from rclpy.node import Node

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

class person(Node):
    def __init__(self,str1):
        super().__init__(str1)
        self.get_logger().info(f"成功创建节点{str1}")

    def call_back(self,msgs):#回调函数
        self.get_logger().info("数据是: %s"%msgs.data)
"""//不推荐
def main():
    #初始化ros2客户端
    rclpy.init()

      #创建节点
    node = rclpy.create_node("helloworld_py_node")
    #输出文本
    node.get_logger().info("hello world!")

    #释放资源
    rclpy.shutdown()
"""
class MyNone(None):
    def __init__(self):
        super().__init__("hello_node_py")
        self.get_logger().info("hello world!(python的继承方式)")
    


def main():
    #初始化
    rclpy.init()

    #创建对象
    rclpy.shutdown()
    pass

    #资源释放
if __name__ == "__main__":
    main()

