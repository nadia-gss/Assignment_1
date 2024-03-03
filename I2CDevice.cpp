#include"I2CDevice.h"
#include<iostream>
#include<sstream>
#include<fcntl.h>
#include<stdio.h>
#include<iomanip>
#include<unistd.h>
#include<sys/ioctl.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>
#include <chrono>
#include <ctime>
#include <array>
#include <cstdint>
using namespace std;

#define HEX(x) setw(2) << setfill('0') << hex << (int)(x)

namespace EE513 {

/**
 * Constructor for the I2CDevice class. It requires the bus number and device number.
 * The constructor opens a file handle to the I2C device, which is destroyed when
 * the destructor is called
 * @param bus The bus number.
 * @param device The device ID on the bus.
 */
I2CDevice::I2CDevice(unsigned int bus, unsigned int device) {
	this->file = -1;
	this->bus = bus;
	this->device = device;
	this->open();
}

/**
 * Open a connection to an I2C device
 * @return 1 on failure to open to the bus or device, 0 on success.
 */
int I2CDevice::open() {
	string name;
	if (this->bus == 0)
		name = I2C_0;
	else
		name = I2C_1;

	if ((this->file = ::open(name.c_str(), O_RDWR)) < 0) {
		perror("I2C: failed to open the bus\n");
		return 1;
	}
	if (ioctl(this->file, I2C_SLAVE, this->device) < 0) {
		perror("I2C: Failed to connect to the device\n");
		return 1;
	}
	return 0;
}

/**
 * Write a single byte value to a single register.
 * @param registerAddress The register address
 * @param value The value to be written to the register
 * @return 1 on failure to write, 0 on success.
 */

int I2CDevice::writeRegister(unsigned int registerAddress,
		unsigned char value) {
	unsigned char buffer[2];
	buffer[0] = registerAddress;
	buffer[1] = value;
	if (::write(this->file, buffer, 2) != 2) {
		perror("I2C: Failed write to the device\n");
		return 1;
	}
	return 0;
}

/**
 * Write a single value to the I2C device. Used to set up the device to read from a
 * particular address.
 * @param value the value to write to the device
 * @return 1 on failure to write, 0 on success.
 */
int I2CDevice::write(unsigned char value) {
	unsigned char buffer[1];
	buffer[0] = value;
	if (::write(this->file, buffer, 1) != 1) {
		perror("I2C: Failed to write to the device\n");
		return 1;
	}
	return 0;
}

/**
 * Read a single register value from the address on the device.
 * @param registerAddress the address to read from
 * @return the byte value at the register address.
 */
unsigned char I2CDevice::readRegister(unsigned int registerAddress) {
	this->write(registerAddress);
	unsigned char buffer[1];
	if (::read(this->file, buffer, 1) != 1) {
		perror("I2C: Failed to read in the value.\n");
		return 1;
	}
	return buffer[0];
}

/**
 * Method to read a number of registers from a single device. This is much more
 * efficient than reading the registers individually. The from address is the
 * starting address to read from, which defaults to 0x00.
 * @param number the number of registers to read from the device
 * @param fromAddress the starting address to read from
 * @return a pointer of type unsigned char* that points to the first element in the block of registers
 */
unsigned char* I2CDevice::readRegisters(unsigned int number,
		unsigned int fromAddress) {
	this->write(fromAddress);
	unsigned char *data = new unsigned char[number];
	if (::read(this->file, data, number) != (int) number) {
		perror("IC2: Failed to read in the full buffer.\n");
		return NULL;
	}
	return data;
}

/**
 * Method to dump the registers to the standard output. It inserts a return
 * character after every 16 values and displays the results in hexadecimal to give
 * a standard output using the HEX() macro that is defined at the top of this file.
 * The standard output will stay in hexadecimal format, hence
 * the call on the last like.
 * @param number the total number of registers to dump, defaults to 0xff
 */

void I2CDevice::debugDumpRegisters(unsigned int number) {
	cout << "Dumping Registers for Debug Purposes:" << endl;
	unsigned char *registers = this->readRegisters(number);
	for (int i = 0; i < (int) number; i++) {
		cout << HEX(*(registers + i)) << " ";
		if (i % 16 == 15)
			cout << endl;
	}
	cout << dec;
}
/**
 *Method to get time and date.
 */
void I2CDevice::printDateTime() {
	unsigned char *dateTime = this->readRegisters(7, 0x00);
	if (dateTime != nullptr) {
		cout << "Current Date & Time: ";
		//解析并打印时间
		cout << HEX(dateTime[4]) << "-" << HEX(dateTime[5]) << "-" << "20"
				<< HEX(dateTime[6]) << " "; // date-month-year
		cout
				<< HEX(
						dateTime[2]
								& 0x3F) << ":" << HEX(dateTime[1]) << ":" << HEX(dateTime[0]); // 时:分:秒
		cout << endl;
		delete[] dateTime; // 释放内存
	} else {
		cout << "Failed to read Time & Date." << endl;
	}
}
/**
 * Get current date and time.
 */
tm* I2CDevice::getSystemDateTime() {
	auto now = chrono::system_clock::now();
	time_t now_time = chrono::system_clock::to_time_t(now);
	// 将时间转换为本地时间
	tm *local_time = localtime(&now_time);

	// 输出日期和时间的数字格式
	cout << dec << local_time->tm_mday << "-"; // 输出日
	cout << dec << local_time->tm_mon + 1 << "-"; // 输出月
	cout << dec << local_time->tm_year + 1900 << " "; // 输出年

	cout << local_time->tm_hour << ":" << local_time->tm_min << ":"
			<< local_time->tm_sec << endl;
	return local_time;
}

/**
 * Set the current time and date.
 */
tm* I2CDevice::setCurrentDateTime() {
	tm *system_time = getSystemDateTime();
	unsigned char dateTime[7];

	//Day
	dateTime[4] = ((system_time->tm_mday / 10) << 4)
			+ (system_time->tm_mday % 10);
	//Month
	dateTime[5] = ((system_time->tm_mon + 1) / 10 << 4)
			+ ((system_time->tm_mon + 1) % 10);
	//Year
	dateTime[6] = (((system_time->tm_year + 1900) % 100) / 10 << 4)
			+ ((system_time->tm_year + 1900) % 10);
	// Seconds
	dateTime[0] = ((system_time->tm_sec / 10) << 4)
			+ (system_time->tm_sec % 10);

	// Minutes
	dateTime[1] = ((system_time->tm_min / 10) << 4)
			+ (system_time->tm_min % 10);

	// Hours (24-hour format)
	dateTime[2] = ((system_time->tm_hour / 10) << 4)
			+ (system_time->tm_hour % 10);
	// Day of week (not used in setting)
	dateTime[3] = 0; // Not setting the day of the week

	//Write
	for (int i = 0; i < 7; ++i) {
		writeRegister(i, dateTime[i]);
	}
	return system_time;
}
/**
 * Read and display the current temperature.
 */
void I2CDevice::printTemperature() {
	unsigned char *data = this->readRegisters(2, 0x11);
	float temperature = data[0] * 1.0 + (data[1] >> 6) * 0.25;
	cout << "Current temperature : " << temperature << endl;
}

/**
 * Method to set an alarm.
 * Alarm 1 Registers:0x07-0x0A seconds minutes hours date
 * Alarm 2 Registers:0x0B-0x0D minutes hours date
 */
void I2CDevice::setAlarm1(int second, int minute, int hour, int date) { //time on date
	writeRegister(0x07, decToBcd(second) | 0b00000000);  // second
	writeRegister(0x08, decToBcd(minute) | 0b00000000);  // minute
	writeRegister(0x09, decToBcd(hour) | 0b00000000);    // hour
	writeRegister(0x0A, decToBcd(date) | 0b00000000);

	writeRegister(0x0F, 0b10001000); //The listening bit of alarm 1 is set to 0

	// 读取当前0E寄存器的值
	unsigned char currentValue = readRegister(0x0E);
	// 将新值写回0E寄存器
	writeRegister(0x0E,0b00011111);

	unsigned char value = readRegister(0x0E);
	cout << HEX(value) << endl;

	cout << dec << second << " " << minute << " " << hour << " " << date << endl;

	while (true) {
		unsigned char flg = readRegister(0x0F); // 读取日期寄存器
		if (flg & 0x01) { // 检查日期寄存器的第一位，如果是1表示闹钟已经触发
			cout << "0x0F:" << HEX(flg) << endl;
			cout << "Alarm triggered!" << endl;
			unsigned char value = readRegister(0x0F);
			cout << "Alarm1" << HEX(value) << endl;
			// 将新值写回0E寄存器
        		writeRegister(0x0E,0b00011100);
			break;
		}
		sleep(1); // 每秒钟检查一次
	}
}

void I2CDevice::setAlarm2(int minute, int hour, int date) {
    // Set alarm time registers
    writeRegister(0x0B, decToBcd(minute) | 0b10000000);  // Minute (set bit 7 to 1 for Alarm2)
    writeRegister(0x0C, decToBcd(hour) | 0b10000000);    // Hour (set bit 7 to 1 for Alarm2)
    writeRegister(0x0D, decToBcd(date) | 0b10000000);    // Date (set bit 7 to 1 for Alarm2)

    writeRegister(0x0F, 0b10001000); // Control register for Alarm2

    // Set interrupt enable for Alarm2
    unsigned char currentValue = readRegister(0x0E);
    writeRegister(0x0E, currentValue | 0b00000100); // Set A2IE bit (bit 2) to enable Alarm2 interrupt

    while (true) {
        unsigned char flg = readRegister(0x0F); // Read Control/Status register
        if (flg & 0b00000010) { // Check Alarm2 flag
            cout << "Alarm triggered!" << endl;
            break; // Exit the loop after the alarm is triggered
        }
        sleep(1); // Check every second
    }
}

uint8_t I2CDevice::decToBcd(uint8_t val) {
	return ((val / 10 * 16) + (val % 10));
}


/**
方波
*/
void I2CDevice::enableSquareWaveOutput(bool enable, SquareWaveFrequency frequency) {
        // 读取当前控制寄存器值
        unsigned char controlRegisterValue = readRegister(0x0E);
	
	writeRegister(0x0F,0b10001000); //A1F,A2F设置为0

        // 设置或清除OUT引脚以启用或禁用方波输出
	if (enable) {
        	controlRegisterValue |= (1 << 1); // 将第2位设置为0以启用方波输出
	// 设置方波频率
        controlRegisterValue &= 0b11110011; // 清除第2-3位以设置频率
        controlRegisterValue |= (frequency << 2); // 设置频率位
    	} else {
		cout << "关闭方波输出" << endl;
        	controlRegisterValue = 0x1c; // 将第2位清零以禁用方波输出
    	}

        // 写回修改后的控制寄存器值
	writeRegister(0x0F, 0b10001000); //The listening bit of alarm 1 is set to 0
        writeRegister(0x0E, controlRegisterValue);
	controlRegisterValue = readRegister(0x0E);
	cout << "修改后的寄存器值：" << HEX(controlRegisterValue) << endl;
    }








/**
 * Close the file handles and sets a temporary state to -1.
 */
void I2CDevice::close() {
	::close(this->file);
	this->file = -1;
}

/**
 * Closes the file on destruction, provided that it has not already been closed.
 */
I2CDevice::~I2CDevice() {
	if (file != -1)
		this->close();
}

} /* namespace EE513*/
