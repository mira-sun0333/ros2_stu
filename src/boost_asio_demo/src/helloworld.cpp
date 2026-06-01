/*
  需求:通过boost::asio实现上位机与下位机的通信
        1.实现读操作 --- 读取并打印下位机发送的数据;
        2.实现写操作 --- 向下位机发送数据控制电机运动。
  流程:
        1.创建串口通信相关对象;
        2.实现写操作;
        3.实现读操作;
        4.释放资源.

*/



#include <cstdio>
#include <boost/asio.hpp>
#include <iostream>
#include <array>
#include <thread>
#include <iomanip>
int main(int argc, char ** argv)
{
  (void) argc;
  (void) argv;
  //1.创建串口通信相关对象(打开串口,并且设置参数,以能够与串口通信);
  //1-1.创建通信对象;
  boost::asio::io_context io; //是boost中所有IO通信的基础,管理IO服务的生命周期
  boost::asio::serial_port serial(io); //是专用于串口通信的类,必须与io_context相结合
  boost::system::error_code error; //用于捕获串口通信时产生的异常
  //1-2.打开串口;
  serial.open("/dev/ttyUSB0",error); //使用命令:ll /dev/ttyUSB 查看 tal键补齐
  if(error){
    std::cout << "串口打开异常: " << error.message() << std::endl;
    return 1;
  }
  //1-3.设置参数;
  serial.set_option(boost::asio::serial_port_base::baud_rate(115200)); // 波特率
  serial.set_option(boost::asio::serial_port_base::character_size(8)); // 设置每个数据位大小
  serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none)); // 设置流控制
  serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none)); // 设置校验位(无)
  serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one)); // 设置每一帧数据的结束符

  // 2.写 --- 控制电机运动
  // 2-1.明确数据格式
  // FC 06 00 64 00 64 00 64 00 64 FA DF ---- 四个电机转速为 100
  // FC 06 00 00 00 00 00 00 00 00 FA DF ---- 四个电机转速为 0
  std::array<uint8_t,12> control_data = {0xFC, 0x06, 0x00, 0x64, 0x00, 0x64, 0x00, 0x64, 0x00, 0x64, 0xFA, 0xDF};
  std::array<uint8_t,12> stop_data = {0xFC, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0xDF};
  //2-2.数据写出去
  // 先控制电机运动
  boost::asio::write(serial,boost::asio::buffer(control_data),error);  //error可以省略
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  // 再让电机停止
  boost::asio::write(serial,boost::asio::buffer(stop_data));

  //3.读 --- 接收下位机发送的数据,并以十六进制格式输出在终端
  //3-1.准备缓冲区
  std::array<uint8_t, 512> recv_buffer;
  //3-2.将数据读入到缓冲区
  boost::asio::read(serial,boost::asio::buffer(recv_buffer),error);
  //写出数据
  std::cout << "读取到的数据: " << std::endl;
  //遍历
  for (auto arg : recv_buffer)
  {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(arg) << " ";
  }
  std::cout << std::endl;
  //4.释放资源
  serial.close();
  printf("hello world boost_asio_demo package\n");
  return 0;
}
