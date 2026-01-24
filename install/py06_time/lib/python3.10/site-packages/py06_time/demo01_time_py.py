import rclpy
from rclpy.node import Node
#import time
import threading
from rclpy.time import Time
from rclpy.duration import Duration

class MyNode(Node):
    def __init__(self):
        super().__init__("time_node_py")
        #self.demo_rate()
        #self.demo_time()
        #self.demo_duration()
        self.demo_opt()

    def demo_opt(self):
        t1 = Time(seconds=20)
        t2 = Time(seconds=15)

        du1 = Duration(seconds=7)
        du2 = Duration(seconds=13)

        self.get_logger().info("t1>=t2? %d" % (t1>=t2))
        self.get_logger().info("t1<t2? %d" % (t1<t2))

        t3 = t1 - t2
        t4 = t1 + du1
        t5 = t1 - du1
        self.get_logger().info("t3 = %d" % t3.nanoseconds)
        self.get_logger().info("t4 = %d" % t4.nanoseconds)
        self.get_logger().info("t5 = %d" % t5.nanoseconds)

        #duration之间运算
        self.get_logger().info("du1>=du2? %d" % (du1>=du2))
        self.get_logger().info("du1<du2? %d" % (du1<du2))


        




    def demo_duration(self):
        #创建Duration对象
        du1 = Duration(seconds=10,nanoseconds=800000000)

        #调用函数
        self.get_logger().info("ns = %d" % du1.nanoseconds)

    def demo_time(self):
        #创建time对象
        t1 = Time(seconds=5,nanoseconds=500000000)
        right_now = self.get_clock().now()

        #调用time函数
        self.get_logger().info("s = %.2f, ns = %d" % (t1.seconds_nanoseconds()[0],t1.seconds_nanoseconds()[1]))
        self.get_logger().info("s = %.2f, ns = %d" % (right_now.seconds_nanoseconds()[0],right_now.seconds_nanoseconds()[1]))
        self.get_logger().info("ns = %d" % t1.nanoseconds)
        self.get_logger().info("ns = %d" % right_now.nanoseconds)



    def demo_rate(self):
        #创建Rate对象
        self.rate = self.create_rate(1.0)

        #调用sleep函数----导致程序挂起堵塞
        #解决1使用time休眠
        #解决2 创建子线程
        #while rclpy.ok():
        #   self.get_logger().info("+++++++++++++++++++")
        #  self.rate.sleep()
        #time.sleep(1.0)
        

        #创建子线程实现运行频率控制
        thread = threading.Thread(target=self.do_some)
        thread.start()

    def do_some(self):
        while rclpy.ok():
            self.get_logger().info("+++++++++++++++++++")
            self.rate.sleep()
      
 
def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(MyNode())

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()