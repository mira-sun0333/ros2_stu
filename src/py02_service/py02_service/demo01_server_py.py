import rclpy
from rclpy.node import Node
from base_interfaces_demo.srv import AddInts

"""
    需求:创建服务器,解析客户端提交的数据并响应结果
    流程:
        1.导包
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建服务端
            3-2实现回调函数处理请求并产生响应
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class AddIntsServer(Node):
    def __init__(self):
        super().__init__("add_ints_server_node_py")
        self.get_logger().info("服务端创建了！(python)")
        #3-1创建服务端
        self.server = self.create_service(AddInts,"add_ints",self.add)
        # 3-2实现回调函数处理请求并产生响应
    def add(self,requset,response):
        response.sum = requset.num1 + requset.num2
        self.get_logger().info("%d + %d = %d" % (requset.num1, requset.num2, response.sum))
        return response
  
def main():
    #初始化ros2客户端
    rclpy.init()
    

    #调用spin函数,传入自定义类对象
    rclpy.spin(AddIntsServer())

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()