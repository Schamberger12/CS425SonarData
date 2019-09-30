#include <iostream>
#include <iomanip>
#include "SonarPoint.h"

using namespace std; 

void SonarPoint::print() {
	cout << setprecision(10) << latitude << ' ' << longitude << ' ' << nLat << ' ' << nLong << ' '
		<< depth << ' ' << nDepth << ' ' << month << ' ' << day << ' ' << year << ' '
		<< hour << ' ' << minute << ' ' << second << ' ' << speed << ' ' << heading << endl;

	return; 
}