#include "DS1307.h"
#include <iostream>
using namespace std;
int main(){
	DS1307 object=DS1307();
	object.setBusConnection();
	object.setSensorConnection();
	object.setReadAddress();
	object.getReadData();
	object.display();
//	object.setHourMode(1);
	object.writeTimeData();
	object.display();
	object.setAlarm(0x53,0x21,0x01,0x18,0x03,0x19);
//	object.getCurrentTime();
//	object.SquareWaveGeneration();
//	object.changeFrequencySQW(1);
//	object.hitAlarm();
	object.validateFrequencyChange();
	object.displayFormat();
	object.endConnection();
}
