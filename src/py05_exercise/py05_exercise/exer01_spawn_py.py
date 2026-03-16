import rclpy
from rclpy.node import Node
from turtlesim.srv import Spawn

"""
    需求:编写客户端实现,发送请求生成一只新乌龟
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1使用参数服务声明新的乌龟信息
            3-2创建服务客户端
            3-3连接服务端
            3-4组织并发送数据
        4.创建自定义节点类对象,组织函数,处理响应结果
        5.释放资源
"""

class Exer01SpawnPy(Node):
    def __init__(self,):
        super().__init__("exer01_spawn_py_node_py")
        #3-1使用参数服务声明新的乌龟信息
        self.declare_parameter("x",3.5)
        self.declare_parameter("y",3.5)
        self.declare_parameter("theta",1.57)
        self.declare_parameter("turtle_name","turtle2")
        self.x = self.get_parameter("x").get_parameter_value().double_value
        self.y = self.get_parameter("y").get_parameter_value().double_value
        self.theta = self.get_parameter("theta").get_parameter_value().double_value
        self.turtle_name = self.get_parameter("turtle_name").get_parameter_value().string_value
        #3-2创建服务客户端
        self.client = self.create_client(Spawn,"/spawn")
        #3-3连接服务端
        while not self.client.wait_for_service(1.0):
            self.get_logger().info("服务连接中.....")


    #3-4组织并发送数据
    def request(self):
        #组织数据
        request = Spawn.Request()
        request.x = self.x
        request.y = self.y
        request.theta = self.theta
        request.name = self.turtle_name
        #发布数据
        self.future = self.client.call_async(request)

def main():
    #初始化ros2客户端
    rclpy.init()

    #4.创建自定义节点类对象,组织函数,处理响应结果
    spawn = Exer01SpawnPy()
    spawn.request()
    rclpy.spin_until_future_complete(spawn,spawn.future)
    #处理响应
    response = spawn.future.result()
    if len (response.name) == 0:
        spawn.get_logger().info("乌龟重名了")
    else:
        spawn.get_logger().info("乌龟生成了")
    
    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()