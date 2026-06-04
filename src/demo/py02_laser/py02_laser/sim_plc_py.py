import rclpy
from rclpy.node import Node
from sensor_msgs.msg import PointCloud2
from sensor_msgs.msg import PointField
import numpy
"""
    需求:生成并发布多线激光雷达消息
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

class SimPointCloud2Py(Node):
    def __init__(self):
        super().__init__("sim_pointcloud2_node_py")
        self.counter = 0
        # 发布方
        self.pcl_pub = self.create_publisher(PointCloud2,"pcl",10)
        # 定时器
        self.timer = self.create_timer(0.1,self.on_timer)
    def on_timer(self):
        # 生成点云数据
        pcl = PointCloud2()

        # 设置头
        pcl.header.stamp = self.get_clock().now().to_msg()
        pcl.header.frame_id = "lidar"
        # 设置宽度和高度
        pcl.height = 30
        pcl.width = 60  
        # 点的描述字段
        pcl.fields.append(PointField(name="x",offset=0,datatype=PointField.FLOAT32,count=1))
        pcl.fields.append(PointField(name="y",offset=4,datatype=PointField.FLOAT32,count=1))
        pcl.fields.append(PointField(name="z",offset=8,datatype=PointField.FLOAT32,count=1))
        pcl.fields.append(PointField(name="intensity",offset=12,datatype=PointField.FLOAT32,count=1))
        # 是否是大端字节序
        pcl.is_bigendian = False
        # 一个点的步长值
        pcl.point_step = 16
        # 一行的步长值
        pcl.row_step = pcl.point_step * pcl.width
        # 是否有无效数据
        pcl.is_dense = True
        # 设置点数据
        points = numpy.zeros(4 * pcl.height * pcl.width,dtype=numpy.float32)
        # 储存点的数据,每个点对应四个元素
        for row in range(0,pcl.height):
            for col in range(0,pcl.width):
                index = (row * pcl.width + col) * 4
                points[index] = 1.0 - self.counter * 0.01
                points[index + 1] = 1.0 - col * 0.05
                points[index + 2] = 1.0 - row * 0.05
                points[index + 3] = row * col
        pcl.data = points.tobytes()
        # 发布
        self.pcl_pub.publish(pcl)
        self.counter += 1
   
def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(SimPointCloud2Py())

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()