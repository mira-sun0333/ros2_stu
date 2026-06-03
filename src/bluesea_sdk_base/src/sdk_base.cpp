/*
    需求: 调用SDK的核心API,将雷达扫描到的数据,在终端输出
    流程:
      0.包含头文件;
      1.创建SDK中的雷达驱动对象;
      2.设置雷达连接参数
      3.连接雷达;
      4.读数据(解析雷达扫描的数据);
      5.写数据(控制雷达起停)
*/
#include <cstdio>
#include <memory>
#include <thread>
// 0.包含头文件;
#include "../sdk/include/bluesea.h"
int main(int argc, char ** argv)
{
  (void) argc;
  (void) argv;

  printf("调用SDK核心API,输出扫描点数据\n");
  // 1.创建SDK中的雷达驱动对象;
  std::shared_ptr<BlueSeaLidarDriver> driver = std::make_shared<BlueSeaLidarDriver>();
  ArgData argdata;
  // 设置必须参数
  argdata.num = 1; // 连接几个雷达
  argdata.type = "uart"; // 设置连接方式
  argdata.dev_id = -1; // 设备编号
  argdata.frame_id = "laser"; // 雷达坐标系
  
  // 设置连接信息
  ConnectArg connect;
  connect.arg1 = "/dev/ttyUSB0"; //设置端口号
  connect.arg2 = -1; // 设置波特率
  connect.scan_topics = "scan"; //扫描话题
  connect.cloud_topics = "cloud"; // 点云话题
  argdata.connectargs.push_back(connect);

  // 设置角分辨率
  argdata.resample = -1.0;
  // 2.设置雷达连接参数
  driver->openLidarThread();
  // 3.连接雷达;
  driver->getInitCmds(argdata);
  // 4.读数据(解析雷达扫描的数据);
  int count = 0;
  while(true) // 由于雷达数据是不断输出的,所以循环解析数据
  {
    // 休眠
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // 4-1.获取一帧雷达数据
    auto hub = driver->getHub(0); // 参数是雷达的索引(可能有多个雷达)
    // 4-2.判断雷达运转是否正常
    if(hub->nfan == 0) // 扇区数为0,没有启动成功
    {
      continue; // 继续下次循环
    }
    if(hub->offsetangle == -1) // 角度偏移量
    {
      // 检测雷达是否运行中
      driver->checkIsRun(0);
      continue;
    }
    // 4-3.获取扫描点,并打印
    int8_t flag = false;
    int n = driver->GetAllFans(hub,argdata,flag); // 获取扇区数量,并将数据写入hub
    if(n>0)
    {
      for(size_t i = 0; i < hub->consume.size(); i++) // 遍历所有扫描点
      {
        // 获取一个扫描点
        auto point = hub->consume[i];
        // 打印
        printf("这是第%ld个扫描点,角度:%.2f,距离:%d,强度:%d",i,point.degree,point.distance,point.confidence);
      }
    }
    hub->consume.clear(); //清空缓存数组
    count++;
    if(count >= 100)
    {
      break;
    }
  }
  // 5.写数据(控制雷达起停)
  // 雷达解析(10s)后,跳出循环并关闭雷达
  bool stop = driver->sendCmd("scan","LSTOPH",0x0043);
  if (stop)
  {
    printf("雷达关闭成功!\n");
  }
  return 0;
}
