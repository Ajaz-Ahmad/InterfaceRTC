#ifndef DS1307_H_
#define DS1307_H_
#include<string>
#define BUFFER_SIZE 19
class DS1307{
private: unsigned char seconds, minutes, hour, day, month, year, date;
private: char temp[19];
public:
DS1307();
char buf[BUFFER_SIZE];
int file;
int setBusConnection();
int setSensorConnection();
int setReadAddress();
int getReadData();
void display();
void endConnection();
int bcdToDec(char b);
int getCurrentTime();
int decToBcd(int dec);
int writeTimeData();
void setAlarm(unsigned char  minute, unsigned char hour, unsigned char day, unsigned char date, unsigned char month, unsigned char year);
void hitAlarm();
void displayFormat();
void setHourMode(int mode);
void SquareWaveGeneration();
void changeFrequencySQW(int f);
void validateFrequencyChange();
};



#endif /* DS1307_H_ */

