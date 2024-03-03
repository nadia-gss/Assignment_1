#include "I2CDevice.h"
#include <iostream>
#include <ctime>
#include <unistd.h>

using namespace std;
using namespace EE513;

int main() {
	// 创建一个 I2CDevice 对象并连接到指定的 I2C 设备
	I2CDevice device(1, 0x68);

	/**    // 写数据
	 device.writeRegister(0x00, 0xFF);

	 // 读取一个字节的数据并打印输出
	 unsigned char data = device.readRegister(0x00);
	 cout << "Data read from register: " << hex << (int)data << endl;

	 // 读取多个数据并打印输出
	 unsigned char *registers = device.readRegisters(4, 0x00);
	 cout << "Data read from multiple registers: ";
	 for (int i = 0; i < 4; ++i) {
	 cout << hex << (int)registers[i] << " ";
	 }
	 cout << endl;

	 // 调试输出寄存器的内容
	 device.debugDumpRegisters(16); */

	//打印输出RTC模块的当前时间和日期
	device.printDateTime();

	//Read and display the current temperature
	device.printTemperature();
	//Get current date and time
	tm *currentDateTime = device.getSystemDateTime();
	//Set date and time of RTC
	tm *time_now = device.setCurrentDateTime();
	//Set an alarm
	device.setAlarm1(0,47, 13, 3);
	device.enableSquareWaveOutput(true, SQW_1_HZ);
	sleep(10);
	device.enableSquareWaveOutput(false, SQW_1_HZ);

	return 0;
}
