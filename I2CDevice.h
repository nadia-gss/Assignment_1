#ifndef I2C_H_
#define I2C_H_

#include <ctime>
#include <cstdint>
#define I2C_0 "/dev/i2c-0"
#define I2C_1 "/dev/i2c-1"


enum SquareWaveFrequency {
    SQW_1_HZ = 0b0000,
    SQW_1024_HZ = 0b0001,
    SQW_4096_HZ = 0b0010,
    SQW_8192_HZ = 0b0011
};


namespace EE513{

/**
 * @class I2CDevice
 * @brief Generic I2C Device class that can be used to connect to any type of I2C device and read or write to its registers
 */
class I2CDevice{
private:
	unsigned int bus;
	unsigned int device;
	int file;
public:
	I2CDevice(unsigned int bus, unsigned int device);
	virtual uint8_t decToBcd(uint8_t val);
	virtual int open();
	virtual int write(unsigned char value);
	virtual unsigned char readRegister(unsigned int registerAddress);
	virtual unsigned char* readRegisters(unsigned int number, unsigned int fromAddress=0);
	virtual int writeRegister(unsigned int registerAddress, unsigned char value);
	virtual void debugDumpRegisters(unsigned int number = 0xff);
	virtual tm* setCurrentDateTime();
	virtual void printDateTime();
	virtual void printTemperature();
	virtual tm* getSystemDateTime();
	virtual void setAlarm1(int second,int minute,int hour,int date);
	virtual void setAlarm2(int minute,int hour,int date);
	virtual void enableSquareWaveOutput(bool enable, SquareWaveFrequency frequency);
	virtual void close();
	virtual ~I2CDevice();
};

} /* namespace EE513*/

#endif /* I2C_H_ */


