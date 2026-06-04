import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
import numpy
"""
    需求:
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建发布方
            3-2创建定时器
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class SimCamPy(Node):
    def __init__(self):
        super().__init__("sim_cam_node_py")
        # 3-1创建发布方
        self.image_pub = self.create_publisher(Image,"image",10)
        # 3-2创建定时器
        self.timer = self.create_timer(0.2,self.on_timer)
    def on_timer(self):
        # 组织图片数据
        image = Image()
        image.header.stamp = self.get_clock().now().to_msg()
        image.header.frame_id = "camera"
        image.height = 480
        image.width = 640
        image.encoding = "rgb8"
        image.is_bigendian = False
        image.step = image.width * 3
        # 字节数组设置
        data = numpy.zeros((image.height,image.width,3),dtype=numpy.uint8)
        data[:image.height // 3,:,:] = [255,0,0]
        data[image.height // 3:image.height // 3 * 2,:,:] = [0,255,0]
        data[image.height // 3 * 2:image.height,:,:] = [0,0,255]
        image.data = data.tobytes()
        # 发布
        self.image_pub.publish(image)
        pass
def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(SimCamPy())

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()