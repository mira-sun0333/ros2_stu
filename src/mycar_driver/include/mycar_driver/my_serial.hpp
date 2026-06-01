#ifndef MY_SERIAL_H
#define MY_SERIAL_H
#include <iostream>
#include <boost/asio.hpp>
#include <array>
#include <iomanip>
/*
    串口通信功能点分析
        1.打开串口 --- 构造函数实现
        2.释放资源 --- 析构函数实现
        3.读操作
            3-1.读取所有数据以16进制输出,方便测试
            3-2.读取数据,对数据进行解析并封装,封装成对应的对象.
        4.写操作
            4-1.写出电机速度指令;
            4-2.写出PID参数;
            4-3.写出舵机控制指令.


*/
namespace my_serial{
// 封装当前解析状态
enum ParseStatus{HEADER, TYPE, DATA, CHECKSUM, END};
// 封装消息类型
enum MessageType{VOLTAGE = 1, WHEEL_ENCODER = 2, ANGULAR_VEL = 3, ACCEL = 4, EULAR = 5};
// 封装消息
struct Message{
    MessageType type;
    std::array<uint8_t,8> data;
};
class SerialPortComm{
public:
    // 打开串口
    SerialPortComm(const std::string& port, int baud_rate):io(),serial(io),error(),status(HEADER),pid_rate(25){ // 1.创建串口通信对象
        std::cout << "打开串口" << std::endl;
        // 2.打开串口
        serial.open(port,error);
        if (error)
        {
            // 抛出一个运行时异常
            throw std::runtime_error("串口" + port + "打开失败!");
        }
        
        // 3.设置通信参数
        serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
        serial.set_option(boost::asio::serial_port_base::character_size(8));
        serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
        serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
        serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));

        std::cout << "========================================" << port << "开启" << "==========================================" << std::endl;

    }
    // 释放资源
    ~SerialPortComm(){
        serial.close();
    }
    //获取PID控制频率
    int get_pid_rate(){
        return pid_rate;
    }
    // 读
    // 3-1.读取一定量的数据以16进制输出,方便测试
    void read_print_hex(){
        // 设置缓冲区
        std::array<uint8_t,512> buffer;
        // 从串口读取数据到缓冲区
        boost::asio::read(serial,boost::asio::buffer(buffer));
        // 遍历打印
        for (auto arg : buffer)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(arg) <<" "; 
        }
        std::cout << std::endl;
        

    }
    // 3-2.读取数据,对数据进行解析并封装,封装成对应的对象.
    /*
        读取数据需求:
            将有效的数据帧封装并返回
        怎么读(正常流程)?
            先获取帧头 fc
            获取功能位 01 --- 05
            获取八位的数据位
            读取校验位
            读取结束位
        怎么封装?
            解析的状态 ----- 查询帧头,功能位,数据位......
            功能位封装 ----- 电压,编码器...
            消息对象   ----- 功能位类型以及8个字节的数据
    */
    std::shared_ptr<Message> read_message(){
        // 解析逻辑实现
        // 先读取一个字节,查找是否是帧头
        // if true -> 再读一个字节,获取数据类型
        //         -> 如果数据类型合法 -> 读8 位数据 ->读取一位校验位,并且校验
        //                                                      ->成功 -> 读结束位
        //                                                                  -> 组织数据返回,并且重新读取帧头.
        //                                                      ->失败 -> 函数退出,重新执行
        //         -> 非法 -> 函数退出,重新执行
        // if false -> 函数退出,重新执行
        switch (status)
        {
        case HEADER: //查找帧头
            // 从串口读取一个字节 
            boost::asio::read(serial,boost::asio::buffer(&buffer_data[0],1));
            // 判断是否是帧头
            // 如果是,需要设置新的状态 -- 查找数据类型,并执行校验相关操作
            if (buffer_data[0] == 0xfc)
            {
                check_num = 0xfc;
                status = TYPE;
            }
            else
            {
                // 如果不是 直接break
                break;
            }
        case TYPE:
            // 从串口读取一个字节
            boost::asio::read(serial,boost::asio::buffer(&buffer_data[0],1));
            // 判断是否是合法的数据类型
            // 是 --> 进入下一个状态,执行校验相关操作,记录消息类型
            if (buffer_data[0] >= 1 && buffer_data[0] <= 5)
            {
                status = DATA;
                check_num ^= buffer_data[0];
                msg_type = MessageType(buffer_data[0]);
            }
            // 如果不是 --> 重新查找帧头(状态切换成HEADER)
            else
            { 
                status = HEADER;
                break;
            }
        case DATA:
            // 读取数据位上的数据(8个字节)
            boost::asio::read(serial,boost::asio::buffer(&data[0],8));
            // 异或校验
            for (size_t i = 0; i < 8; i++)
            {
                check_num ^= data[i];
            }
            // 设置状态
            status = CHECKSUM;
            //break;
        case CHECKSUM:
            // 读取一位校验位
            boost::asio::read(serial,boost::asio::buffer(&buffer_data[0],1));
            // 判断校验位数据与自己计算的check_num是否一致,如果一致,进入下一阶段
            if (check_num == buffer_data[0])
            {
                status = END;
            }
            // 否则,从头读取
            else
            {
                status = HEADER;
                break;
            }
        case END:
            // 读取一位数据
            boost::asio::read(serial,boost::asio::buffer(&buffer_data[0],1));
            // 设置状态为 HEADER
            status = HEADER;
            // 判断读取的数据是不是结束位,如果是,组织并返回Message对象
            if (buffer_data[0] == 0xDF)
            {
                std::shared_ptr msg = std::make_shared<Message>();
                msg->type = msg_type;
                msg->data = data;
                return msg;
            }
            // 如果不是无操作
            break;
        default:
            break;
        }
        return nullptr;
    }
    // 写
    // 抽取校验操作
    void checksum(std::array<uint8_t,12>& data){
        // 校验
        uint8_t checksum = 0;
        for (size_t i = 0; i < 10; i++)
        {
            checksum ^= data[i];
        }
        data[10] = checksum;
    }
    // 写出电机速度指令
    /*
        需要将每一个电机的速度转换成两个数据位并发送
        返回值:void
        参数:四个电机的速度.
    */
    void write_motor_speed(short motor_a,short motor_b,short motor_c,short motor_d){
        // 创建一个字节数组
        std::array<uint8_t,12> data = {0xFC, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0xDF};
        // 设置字节数组的元素
        data[2] = (motor_a >> 8) & 0xff;
        data[3] = motor_a & 0xff;
        data[4] = (motor_b >> 8) & 0xff;
        data[5] = motor_b & 0xff;
        data[6] = (motor_c >> 8) & 0xff;
        data[7] = motor_c & 0xff;
        data[8] = (motor_d >> 8) & 0xff;
        data[9] = motor_d & 0xff;
        // 校验
        checksum(data);
        // 写出
        boost::asio::write(serial,boost::asio::buffer(data,12));
    }
    // 差速小车电机控制
    void write_motor_diff(short motor_a,short motor_b){
        write_motor_speed(motor_a,motor_b,motor_a,motor_b);
    }
    // 停止电机运动
    void write_motor_stop(){
        write_motor_diff(0,0);
    }
    // 写出PID
    /*
        设置电机控制的PID
        返回值:void(无)
        参数:kp,ki和kd
    */
    void write_pid(short kp,short ki,short kd){
        // 创建存储PID的字节数组
        std::array<uint8_t,12> data = {0xFC, 0x07, 0x01, 0x2C, 0x00, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x1E, 0xDF};
        // 设置数组数据
        data[2] = (kp >> 8) & 0xff;
        data[3] = kp & 0xff;
        data[4] = (ki >> 8) & 0xff;
        data[5] = ki & 0xff;
        data[6] = (kd >> 8) & 0xff;
        data[7] = kd & 0xff;
        checksum(data);
        //写出
        boost::asio::write(serial,boost::asio::buffer(data,12));
    }
    // 写出舵机控制指令.
    /*
        返回值:void(无)
        参数:舵机1的旋转角度,舵机2的旋转角度
    */
    void write_servo_control(int8_t servo1_angle, int8_t servo2_angle){
        // 创建字节数组
        std::array<uint8_t,12> data = {0xFC, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF4, 0xDF};
        // 为数组设置数据
        data[2] = servo1_angle & 0xff;
        data[3] = servo2_angle & 0xff;
        checksum(data);
        // 写出
        boost::asio::write(serial,boost::asio::buffer(data,12));
    }
private:
    boost::asio::io_context io;
    boost::asio::serial_port serial;
    boost::system::error_code error;
    //存储解析状态,默认从查找帧头开始
    ParseStatus status;
    // 创建一个缓存数组,用于存储分析读取的数据
    std::array<uint8_t,512> buffer_data;
    // 创建一个存储Message数据的数组
    std::array<uint8_t,8> data;
    // 设置一个用于校验的变量
    uint8_t check_num;
    // 设置一个用于存储消息类型的变量
    MessageType msg_type;
    int pid_rate;
};
}

#endif