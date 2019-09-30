#ifndef SONARPOINT_H
#define SONARPOINT_H

class SonarPoint {
public:
	double latitude;
	double nLat;
	double longitude;
	double nLong;
	float depth; 
	float nDepth; 
	int month;
	int day;
	int year; 
	int hour;
	int minute;
	int second; 
	float speed; 
	int heading;
	float mappedX;
	float mappedY;
	double red;
	double green;
	double blue; 
	int shape; 

	void print();


};

#endif
