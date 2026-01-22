import rclpy
from rclpy.node import Node

"""
    需求:创建参数服务端并操作参数
    流程:
        1.导包
        2.初始化ros2客户端
        3.自定义节点类
            3-1增
            3-2查
            3-3改
            3-4删
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class ParamServer(Node):
    def __init__(self):
        #如果要允许删除参数,需要提前声明
        super().__init__("param_server_node_py",allow_undeclared_parameters = True)
        self.get_logger().info("参数服务端")
        #3-1增
        def declare_param(self):
            self.get_logger().info("----------新增参数---------")
            self.declare_parameter("car_name","tiger")
            self.declare_parameter("width",1.55)
            self.declare_parameter("wheels",5)

            self.set_parameters([rclpy.Parameter("haha",value="xixi")])
            
        #3-2查
        def get_param(self):
            self.get_logger().info("----------查询参数---------")
            #获取指定参数
            car_name =self.get_parameter("car_name")
            self.get_logger().info("%s=%s" %(car_name.name,car_name.value))

            #获取多个参数
            params = self.get_parameters(["car_name","wheels","width"])
            for param in params:
                self.get_logger().info("%s=======%s" % (param.name, param.value))



            #判断是否包含某个参数
            self.get_logger().info("包含 car_name吗%d" % self.has_parameter("car_name"))
            self.get_logger().info("包含 height吗%d" % self.has_parameter("height"))

        #3-3改
        def update_param(self):
            self.get_logger().info("----------修改参数---------")
            self.set_parameters([rclpy.Parameter("car_name",value="Mouse")])
            self.get_logger().info("修改后%s=%s" %(car_name.name,car_name.value))
        #3-4删
        def del_param(self):
            self.get_logger().info("----------删除参数---------")
            self.get_logger().info("删除前,包含 car_name吗%d" % self.has_parameter("car_name"))
            self.undeclare_parameter("car_name")
            self.get_logger().info("删除后,包含 car_name吗%d" % self.has_parameter("car_name"))
  
def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    node = ParamServer()
    node = declare_param()
    node = get_param()
    node = update_param()
    node = del_param()
    rclpy.spin(node)

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()