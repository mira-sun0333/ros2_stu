import rclpy
from rclpy.node import Node

"""
    需求:演示参数API使用
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建参数对象
            3-2解析参数
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class MyParam(Node):
    def __init__(self):
        super().__init__("my_param_node_py")
        self.get_logger().info("参数API使用(python)")
         #3-1创建参数对象
        p1 = rclpy.Parameter("car_name",value="Tiger")
        p2 = rclpy.Parameter("widht",value=1.5)
        p3 = rclpy.Parameter("wheels",value=2)

    
          #  3-2解析参数
        self.get_logger().info("car_name = %s" % p1.value)
        self.get_logger().info("width = %s" % p2.value)
        self.get_logger().info("wheels = %s" % p3.value)

        self.get_logger().info("key = %s" % p1.name)
def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(MyParam())

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()