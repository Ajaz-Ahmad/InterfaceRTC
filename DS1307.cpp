#include <iostream>
#include <ctime>
#include <string.h>
#include "DS1307.h"
#include<fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>

#define BUFFER_SIZE 19
using namespace std;

	DS1307::DS1307()
	{
		date=0;
		seconds=0;
		minutes=0;
		hour=0;
		year=99;
		day=0;
		month=0;
		file=-1;
	}
	
	//converting Binary Coded Decimal to Decimal Values
	int DS1307::bcdToDec(char b){
		return (b / 16) * 10 + (b % 16);
	}
	//Converting Decimal values to BCD values
	int DS1307::decToBcd(int dec){
		int aa=(((dec/10)*16)|(dec%10));
		return (((dec/10)*16) | (dec % 10));
	}

	//Return String array Current system time
	int DS1307::getCurrentTime()
	{
	string days[7]={"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
	string months[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	time_t currentTime=time(0);
	string dates[5];
	string time[3];
	int count=0;
	char* dt=strtok(ctime(&currentTime)," ");
	while(dt!=NULL){
	dates[count]=dt;
	dt=strtok(NULL," ");
	count++;
	}
	count=0;
	char* ptr=const_cast<char*>(dates[3].c_str());
	char* tme=strtok(ptr,":");
	while(tme!=NULL)
	{
	cout<<tme<<endl;
	time[count]=tme;
	tme=strtok(NULL,":");
	count++;
	}
	for(int i=0;i<7;i++){
	if(days[i].compare(dates[0])==0)
	{
		i=i+1;
		day=(unsigned char)i;
	}
	}
	for(int i=0;i<12;i++){
	if(months[i].compare(dates[1])==0){
		i=i+1;
		month=(char)i;
	}
	}
	//date
	date=stoi(dates[2]);
	//hours:MM:ss
	hour=stoi(time[0]);
	minutes=stoi(time[1]);
	seconds=stoi(time[2]);
	//Year
	year=stoi(dates[4].substr(2));
	cout<<"Month is "<<(int)month<<" day is "<<(int)day<<" Year is "<<(int)year<<" Date is "<<(int)date<<" Time is "<<(int)hour<<":"<<(int)minutes<<":"<<(int)seconds<<endl;
	return 0;
	}
	int DS1307:: setBusConnection(){
		if ((file = open("/dev/i2c-1", O_RDWR)) < 0)
		{
			perror("failed to open the bus\n");
			return 1;
		}
		else
		return 0;
	}

	int DS1307::setSensorConnection(){
		if (ioctl(file, I2C_SLAVE, 0x68) < 0)
		{
			perror("Failed to connect to the sensor\n");
			return 1;
		}
			return 0;
	}

	int DS1307::setReadAddress(){
		char writeBuffer[1]={0x00};
		if (write(file, writeBuffer, 1) != 1)
		{
			perror("Failed to reset the read address\n");
			return 1;
		}
		else
			return 0;
	}

	int DS1307::getReadData(){

		if (read(file, buf, BUFFER_SIZE) != BUFFER_SIZE)
		{
			perror("Failed to read in the buffer\n");
			return 1;
		}
		else
			return 0;
	}
	int DS1307::writeTimeData()
	{
	//Get Current time
	getCurrentTime();
				//seconds------------//  //minutes//    //hour----- //---day-//  //Date//......//month//----//Year//
	unsigned char myData[8]={decToBcd(0),decToBcd((int)seconds),decToBcd((int)minutes),decToBcd((int)hour),decToBcd((int)day),decToBcd((int)date),decToBcd((int)month),decToBcd((int)year)};
	char writeBuffer[1]={0x00};
	cout<<write(file,writeBuffer,1)<<endl;
	if(write(file,writeBuffer,1)==1)
	{
	 int n=write(file,myData,8);
	 cout<<"value of n is "<<n<<endl;
	}
	return 0;
	}
	//Novel Functionality Print Time in 12 hour format
	void DS1307::displayFormat(){
	unsigned char writeAddress[1]={0x00};
	unsigned char hourBuffer[3];
	int mode=1;
	if(mode==1){
		if(write(file,writeAddress,1)!=1){cout<<"Failed to set read address"<<endl;}
		if(read(file,hourBuffer,3)!=3){cout<<"Failed to read into Buffer"<<endl;}
	}
	int hh=bcdToDec(hourBuffer[2]);
	minutes=bcdToDec(hourBuffer[1]);
	seconds=bcdToDec(hourBuffer[0]);
	if(hh<12){
	if(hh==0)
		hh=12;
	mode=0;
	}
	if(hh>12){
	hh=hh-12;
	mode=1;}
	if(mode==0)
	cout<<"Time in 12 hour Format is "<<hh<<":"<<(int)minutes<<":"<<(int)seconds<<" AM"<<endl;
	else
	cout<<"Time in 12 hour Format is "<<hh<<":"<<(int)minutes<<":"<<(int)seconds<<" PM"<<endl;
	}

	//Set Alarm 
	void DS1307::setAlarm(unsigned char  minute,unsigned char hour,unsigned char day, unsigned char date, unsigned char month, unsigned char year){
	 char writeAddress[1]={0x10};
	//Setting new address to store Alarm time
		if(write(file,writeAddress,1)!=1){
			perror("Failed to reset to new address");
		}
	//Writing new values
	unsigned char setTime[8]={0x10,0x00,minute,hour,day,date,month,year};
		if(write(file,setTime,8)!=8)
		{
			perror("Failed to write values");
		}
	}

//Turn Led on-off
	void DS1307::hitAlarm(){
		char writeAddress[1]={0x10};
		int alarmtime[19];
		int currenttime[7];
	//Setting new address to store Alarm Time
		if(write(file,writeAddress,1)!=1)
			cout<<"Failed to reset to new address"<<endl;
	//Reading set alarm time
		if(read(file,temp,19)!=19)
			cout<<"Failed to read alarm time"<<endl;
		alarmtime[0]=bcdToDec(temp[0]);//seconds
		alarmtime[1]=bcdToDec(temp[1]);//minutes
		alarmtime[2]=bcdToDec(temp[2]);//hour
		alarmtime[3]=bcdToDec(temp[3]);//day
		alarmtime[4]=bcdToDec(temp[4]);//date
		alarmtime[5]=bcdToDec(temp[5]);//month
		alarmtime[6]=bcdToDec(temp[6]);//year
		cout<<"Waiting for Alarm to hit to continue "<<endl;
			 while(true){
				setReadAddress();
				getReadData();
				currenttime[0]=bcdToDec(buf[1]);//minute
				currenttime[1]=bcdToDec(buf[2]);//hour
				currenttime[2]=bcdToDec(buf[3]);//day
				currenttime[3]=bcdToDec(buf[4]);//date
				currenttime[4]=bcdToDec(buf[5]);//month
				currenttime[5]=bcdToDec(buf[6]);//year

				if(currenttime[5]==alarmtime[6])
				{
					if(currenttime[4]==alarmtime[5])
						if(currenttime[3]==alarmtime[4])
							if(currenttime[1]==alarmtime[2])
								if(currenttime[0]==alarmtime[1]){
									cout<<"Alarm Ringing"<<endl;
									break;
								}
				}
					}

	}

	void DS1307::SquareWaveGeneration(){

	unsigned char controlAddress[2]={0x07,0x10};
	if(write(file,controlAddress,2)==2){
	cout<<"set control adress successfull"<<endl;
	//if(write(file,enableSQWE,1)==1){
	cout<<"Write successfull"<<endl;
	}
	else
	cout<<"Failed to set and write to Control Register"<<endl;
	}
	//change frequency validation
	void DS1307::validateFrequencyChange(){
	char setControlReadAddress[1]={0x07};
	unsigned char readBuffer[10];
	if(write(file,setControlReadAddress,1)!=1)
		cout<<"Failed to reset read Address of control register"<<endl;
	else
		cout<<"Set the control register read address"<<endl;
	if(read(file,readBuffer,10)==10)
	cout<<"Buffer value is "<<readBuffer[0]<<" bcdTodec value "<<bcdToDec(readBuffer[0])<<" int value is "<<(int)readBuffer[0]<<endl;
	}
	//Change Frequency of Square Wave 
	void DS1307::changeFrequencySQW(int f){
	unsigned char controlBuffer[2];
	char readAddress[1]={0x07};
	if(f==1){
	unsigned char controlAddress[2]={0x07,0x10};
	if(write(file,controlAddress,2)==2)
		cout<<"Changed Frquency to 1 KHz"<<endl;
	}
	else if(f==2){
	unsigned char controlAddress[2]={0x07,0x11};
	if(write(file,controlAddress,2)==2)
		cout<<"Changed frequency to 4.096 KHz"<<endl;
	}
	else if(f==3){
	unsigned char controlAddress[2]={0x07,0x12};
	if(write(file,controlAddress,2)==2)
		cout<<"Changed frequency to 8.192 KHz"<<endl;
	}
	else if(f==4){
	unsigned char controlAddress[2]={0x07,0x13};
	if(write(file,controlAddress,2)==2)
		cout<<"Changed frequenct to 32.768 KHz"<<endl;
	}
	}
	void DS1307::display()
	{
	cout<<"The RTC time is "<<bcdToDec(buf[2])<<":"<<bcdToDec(buf[1])<<":"<<bcdToDec(buf[0])<<" Day "<<bcdToDec(buf[3])<<" Date "<<bcdToDec(buf[4])<<" Month "<<bcdToDec(buf[5])<<" Year "<<bcdToDec(buf[6])<<endl;
	}

	void DS1307::endConnection(){
		close(file);
	}


