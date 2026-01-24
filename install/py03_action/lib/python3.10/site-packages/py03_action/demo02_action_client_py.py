import rclpy
from rclpy.node import Node
import sys
from rclpy.logging import get_logger
from rclpy.action import ActionClient
from base_interfaces_demo.action import Progress

"""
    需求:编写动作客户端,可以发送一个整型数据到服务端,并处理服务端的连续反馈和最终响应结果
    流程:
    前提:可以解析终端下动态传入的参数
        1.导包
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建动作客户端
            3-2发送请求
            3-3处理关于目标值的服务器响应(回调函数)
            3-4处理连续反馈(回调函数)
            3-5处理最终反馈(回调函数)
        4.调用spin函数,并传入节点对象指针
        5.释放资源
"""

class ProgressActionClient(Node):
    def __init__(self):
        super().__init__("progress_action_client_node_py")
        self.get_logger().info("动作通信客户端创建")
        # 3-1创建动作客户端
        #node: Any, action_type: Any, action_name
        self.client = ActionClient(self,Progress,"get_sum")

    # 3-2发送请求
    def send_goal(self,num):
        #连接服务端
        self.client.wait_for_server()

        #发送请求
        goal = Progress.Goal()
        goal.num = num
        self.future = self.client.send_goal_async(goal,self.fb_callback)
        self.future.add_done_callback(self.goal_response_callback)

    # 3-3处理关于目标值的服务器响应(回调函数)
    def goal_response_callback(self,future):
        #获取目标句柄
        goal_handle = future.result()
        #self.get_logger().info(goal_handle.__str__())
        #判断目标是否被正常接收
        if not goal_handle.accepted:
            self.get_logger().error("目标被拒绝")
            return
        self.get_logger().info("目标被接受,正在处理中......")
        #处理最终响应结果
        self.result_future = goal_handle.get_result_async()
        self.result_future.add_done_callback(self.get_result_callback)

    #3-5处理最终反馈(回调函数)
    def get_result_callback(self,future):
        result = future.result().result
        self.get_logger().info("最终结果:%d" % result.sum)

    # 3-4处理连续反馈(回调函数)
    def fb_callback(self,fb_msg):
        progress = fb_msg.feedback.progress
        self.get_logger().info("连续反馈数据:%.2f" % progress)

        

   
def main():
    #动态解析传入的参数
    if len(sys.argv) != 2:
        get_logger("rclpy").error("请提交一个整型数据")
        return
    #初始化ros2客户端
    rclpy.init()

    #调用spin函数,传入自定义类对象
    action_client = ProgressActionClient()
    action_client.send_goal(int(sys.argv[1]))
    rclpy.spin(action_client)

    #释放资源
    rclpy.shutdown()

if __name__ == "__main__":
    main()