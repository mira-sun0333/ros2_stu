import rclpy
from rclpy.node import Node
from base_interfaces_demo.msg import Student
"""
    需求:以固定频率发布学生信息
    流程:
        1.导包
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建发布方
            3-2创建定时器
            3-3组织并发布学生信息
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class TalkerStu(Node):
    def __init__(self,str1):
        super().__init__("talkerstu_node_py")
        self.count = 0
        #3-1创建发布方
        self.publisher = self.create_publisher(Student,"chatter_stu",10)
        #3-2创建定时器
        self.timer = self.create_timer(0.5,self.on_timer)
       
        self.get_logger().info(f"成功创建节点{str1}")

    def on_timer(self,msgs):#回调函数
        
        #3-3组织并发布学生信息
        stu = Student()
        stu.name = "aotuman"
        stu.age = self.count
        stu.height = 1.70
        self.publisher.publish(stu)

        self.count +=1
        self.get_logger().info("学生数据是: (%s,%d,%.2f)" (stu.name,stu.agae,stu.height))
                               


def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin (TalkerStu())

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()