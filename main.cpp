#include "I2CDevice.h"
#include <iostream>
#include <ctime>
#include <unistd.h>

using namespace std;
using namespace EE513;

int main() {
	I2CDevice device(1, 0x68);
/**
	 device.writeRegister(0x00, 0xFF);

	 unsigned char data = device.readRegister(0x00);
	 cout << "Data read from register: " << hex << (int)data << endl;

	 unsigned char *registers = device.readRegisters(4, 0x00);
	 cout << "Data read from multiple registers: ";
	 for (int i = 0; i < 4; ++i) {
	 cout << hex << (int)registers[i] << " ";
	 }
	 cout << endl;

	 device.debugDumpRegisters(16); */

	//print date and time
	device.printDateTime();

	//Read and display the current temperature
	cout << "Current temperature: " ;
	device.printTemperature();
	cout << "°C" << endl;
	//Get current date and time
	tm *currentDateTime = device.getSystemDateTime();
	//Set date and time of RTC
	tm *time_now = device.setCurrentDateTime();

	//Set an alarm
//	device.setAlarm1(0,23, 21, 3);

	//Set the square wave output frequency to 1
	/** cout << "Set the square wave output frequency to 1: ";
	device.enableSquareWaveOutput(true, SQW_1_HZ);
	sleep(5);
	device.enableSquareWaveOutput(false, SQW_1_HZ);
	//Set the square wave output frequency to 1024
	cout << "Set the square wave output frequency to 1024: ";
	device.enableSquareWaveOutput(true, SQW_1024_HZ);
	sleep(5);
	device.enableSquareWaveOutput(false, SQW_1024_HZ);
	cout << "Set the square wave output frequency to 4096: ";
	//Set the square wave output frequency to 4096
	device.enableSquareWaveOutput(true, SQW_4096_HZ);
	sleep(5);
	device.enableSquareWaveOutput(false, SQW_4096_HZ);
	//Set the square wave output frequency to 8192
	cout << "Set the square wave output frequency to 8192: ";
	device.enableSquareWaveOutput(true, SQW_8192_HZ);
	sleep(5);
	device.enableSquareWaveOutput(false, SQW_8192_HZ);
	*/
//	device.setAlarm2(50,17,3);
	device.monitorTemperature();
	return 0;
}
