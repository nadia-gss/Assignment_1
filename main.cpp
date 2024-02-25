#include "I2CDevice.h"
#include <iostream>

using namespace std;
using namespace EE513;

int main() {
    // 创建一个 I2CDevice 对象并连接到指定的 I2C 设备
    I2CDevice device(1, 0x68);

    // 向指定寄存器写入一个字节的数据
    device.writeRegister(0x00, 0xFF);

    // 从指定寄存器读取一个字节的数据并打印输出
    unsigned char data = device.readRegister(0x00);
    cout << "Data read from register: " << hex << (int)data << endl;

    // 读取多个连续寄存器的数据并打印输出
    unsigned char *registers = device.readRegisters(4, 0x00);
    cout << "Data read from multiple registers: ";
    for (int i = 0; i < 4; ++i) {
        cout << hex << (int)registers[i] << " ";
    }
    cout << endl;

    // 调试输出寄存器的内容
    device.debugDumpRegisters(16);

    return 0;
}
