import rclpy
from rclpy.node import Node
from rclpy.action import ActionServer
from base_interfaces_demo.action import Progress
import time
"""
    需求:编写动作服务端,解析客户端提交的数据遍历该数据累加求和,最后结果响应回客户端,且请求响应过程中需要生成连续反馈
    流程:
        1.导包
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建动作服务端对象
            3-2.处理提交的目标值(回调函数)----默认实现
            3-3.生成连续反馈(回调函数)----默认实现
            3-4.响应最终结果(回调函数)
            3-5.处理取消请求
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class ProgressActionServer(Node):
    def __init__(self):
        super().__init__("progress_action_server_node_py")
        self.get_logger().info("动作通信服务端创建")
        #3-1创建动作服务端对象
        #node: Any, action_type: Any, action_name: Any, *, callback_group: Any | None = None,
        self.server = ActionServer(
            self,
            Progress,
            "get_sum",
            self.execute_callback
        )

    
    #3-4.响应最终结果(回调函数)
    def execute_callback(self,goal_handle):
        #1.生成连续反馈
        #//首先要获取目标值然后遍历 遍历累加,且每循环一次就计算进度,并作为连续反馈发布
        num = goal_handle.request.num
        sum = 0
        for i in range(1,num + 1):
            sum += 1
            feedback = Progress.Feedback()
            feedback.progress = i / num
            goal_handle.publish_feedback(feedback)
            self.get_logger().info("连续反馈:%.2f" % feedback.progress)
            time.sleep(1.0)

        #2.响应最终结果
        goal_handle.succeed()
        result = Progress.Result()
        result.sum = sum
        self.get_logger().info("最终日志结果:%d" % result.sum)
        return result
        
    

def main():
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    rclpy.spin(ProgressActionServer())

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()