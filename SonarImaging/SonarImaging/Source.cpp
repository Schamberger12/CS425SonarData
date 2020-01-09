// Gregory Schamberger, gscham2

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include "SonarPoint.h"
#include "HSV_RGB_CONVERT.h"

using namespace std; 

#define LEFT_EDGE 0
#define RIGHT_EDGE 450
#define TOP_EDGE 450
#define BOTTOM_EDGE 0


vector<SonarPoint> points;

double maxLat;
double minLat;
double maxLong;
double minLong;
double maxDep;
double minDep; 


void dealWithParameters(int argc, char** argv); // Process command line args.
void update(int value); // Modify the data periodically.  ( Move the triangle )
void display(void);  // This draws the graphics.
void output(int x, int y, string str); // Outputs string at coordinates x, y
void keyboard(unsigned char c, int x, int y); // Keyboard event handler
void mouseClick(int button, int state, int x, int y); // Mouse button handler
void mouseMove(int x, int y); // Mouse motion handler
void reshape(int width, int height); // Window reshape handler
void NormalizePoints(vector<SonarPoint> &points); // normalized all of the points
void ParseFile(vector<SonarPoint> &points, vector<string> &lines); // parses the read file
void ReadFile(vector<SonarPoint> &points, const string &fileName); // reads a file from either the included default or the command line
float NormalToMap(double n_val); // maps a normalized value to a point
void SetColor(vector<SonarPoint> & points); 

int main(int argc, char **argv) {

	// Get all data points from file, place into vector of SonarPoints
	

	if (argc > 1) {
		ReadFile(points, argv[1]);
	}
	else {
		ReadFile(points, "R48");
	}
	
	// Normalize Points

	NormalizePoints(points);

	// Sort points according to date and then time

	sort(points.begin(), points.end(),
		[](const SonarPoint &a,const SonarPoint &b)
		{
			if (a.year < b.year)
				return true;
			else if (a.year == b.year) {
				if (a.month < b.month)
					return true;
				else if (a.month == b.month) {
					if (a.day < b.day)
						return true;
					else if (a.day == b.day) {
						if (a.hour < b.hour)
							return true;
						else if (a.hour == b.hour) {
							if (a.minute < b.minute)
								return true;
							else if (a.minute == b.minute) {
								if (a.second < b.second)
									return true;
								else
									return false;
							}
							else
								return false;
						}
						else
							return false;
					}
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
		});

	for (SonarPoint &p : points) {
		p.mappedX = NormalToMap(p.nLong);
		p.mappedY = NormalToMap(p.nLat); 

	}

	SetColor(points); 







	// Initialize glut, passing along command-line parameters  
	glutInit(&argc, argv);

	// Deal with any command line options for this program
	dealWithParameters(argc, argv);

	// Initialize the display mode, double buffered with RGBA color
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	// Initialize the physical window size.  ( Pixels on screen. )
	glutInitWindowSize(500, 500);

	// Now create a graphics window, and give it a title
	glutCreateWindow("OpenGL Sample - Sliding Triangle");

	// Set up the logical graphics space
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(LEFT_EDGE, RIGHT_EDGE, BOTTOM_EDGE, TOP_EDGE);
	// Set the clear color - The background when the screen is cleared
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// Register the callback event handlers
	glutDisplayFunc(display); // To redraw the screen as needed
	//glutIdleFunc( display ); // When there is nothing else to do
	glutKeyboardFunc(keyboard);
	//glutMouseFunc( mouseClick );
	//glutMotionFunc( mouseMove );
	//glutReshapeFunc( reshape );

	// set up a timer to regularly update the scene
	glutTimerFunc(50, update, 1);

	// Once everything is set up, the last step is to start the simulation loop

	glutMainLoop();

	return 0;

} // main

void ReadFile(vector<SonarPoint> &points, const string &fileName) {

	ifstream inFile; 

	vector<string> lines; 

	string s; 

	inFile.open(fileName); 
	if (!inFile.is_open()) {
		cout << "File could not be found, opening default file" << endl;
		inFile.open("R48.txt");
		while (getline(inFile, s)) {
			if (s.size() > 0) {
				lines.push_back(s); 
			}
		}
		inFile.close(); 
	}
	else {
		while (getline(inFile, s)) {
			if (s.size() > 0) {
				lines.push_back(s); 
			}
		}
		inFile.close(); 
	}


	ParseFile(points, lines); 
	
	return; 
}

void ParseFile(vector<SonarPoint> &points, vector<string> &lines) {

	vector<string> tokens; 

	SonarPoint sp; 

	string rawDate;
	string stringPart; 
	string rawTime; 
	
	stringstream ss; 
	

	for (string line : lines) {
		ss.str(line); 
		ss >> sp.latitude >> sp.longitude >> sp.depth >> rawDate
			>> rawTime >> sp.speed >> sp.heading;

		
		ss.clear();

		ss.str(rawDate);
		while (getline(ss, stringPart, '/')) {
			tokens.push_back(stringPart); 
		}
		sp.month = stoi(tokens[0]);
		sp.day = stoi(tokens[1]);
		sp.year = stoi(tokens[2]); 

		ss.clear();
		tokens.clear(); 
		
		ss.str(rawTime);
		while (getline(ss, stringPart, ':')) {
			tokens.push_back(stringPart); 
		}
		
		sp.hour = stoi(tokens[0]);
		sp.minute = stoi(tokens[1]);
		sp.second = stoi(tokens[2]);
		
		points.push_back(sp); 
		// setup for next line to parse
		ss.clear(); 
		tokens.clear(); 
	}
	return; 
}

void NormalizePoints(vector<SonarPoint> &points) {
	
	double p_maxLat = points[0].latitude;
	double p_minLat = p_maxLat;
	double p_maxLong = points[0].longitude;
	double p_minLong = p_maxLong;
	double p_maxDep = points[0].depth;
	double p_minDep = p_maxDep;

	for (SonarPoint &p : points) {
		if (p.latitude > p_maxLat) {
			p_maxLat = p.latitude;
		}
		if (p.latitude < p_minLat) {
			p_minLat = p.latitude;
		}
		if (p.longitude > p_maxLong) {
			p_maxLong = p.longitude;
		}
		if (p.longitude < p_minLong) {
			p_minLong = p.longitude; 
		}
		if (p.depth > p_maxDep) {
			p_maxDep = p.depth;
		}
		if (p.depth < p_minDep) {
			p_minDep = p.depth; 
		}
	}

	

	double n_LatDenom = p_maxLat - p_minLat;
	double n_LongDenom = p_maxLong - p_minLong;
	double n_DepDenom = p_maxDep - p_minDep;


	for (SonarPoint &p : points) {
		p.nLat = (p.latitude - p_minLat) / n_LatDenom;
		p.nLong = (p.longitude - p_minLong) / n_LongDenom;
		p.nDepth = (p.depth - p_minDep) / n_DepDenom;
	}

	maxLat = p_maxLat;
	minLat = p_minLat;
	maxLong = p_maxLong;
	minLong = p_minLong;
	maxDep = p_maxDep;
	minDep = p_minDep;

	return; 


}

void SetColor(vector<SonarPoint> & points) {
	hsv pointHSV;
	pointHSV.s = 1.0; 
	pointHSV.v = 1.0; 
	rgb pointRGB;


	for (SonarPoint &p : points) {
		pointHSV.h = (p.nDepth * 360.0);
		pointRGB = hsv2rgb(pointHSV); 

		p.red = pointRGB.r;
		p.green = pointRGB.g;
		p.blue = pointRGB.b; 

		cout << p.nDepth << ' ' << pointHSV.h << ' ' << p.red << ' ' << p.green << ' ' << p.blue << endl;
	}

}


float NormalToMap(double nVal) {

	return (nVal * 375.0) + 25;

}
void dealWithParameters(int argc, char **argv) {

	return;

} // dealWithParameters

//---------------------------------------------------------------------- update
/* Update everything that needs to be updated in the scene - This
 * function should be called via a timer to ensure that updates
 * happen at a regular rate, independent of the graphics update rate */

void update(int value) {

	// Reset the timer before leaving, so this will run again soon.
	glutTimerFunc(50, update, 1);

	// Tell the system the window is ready to be redrawn
	glutPostRedisplay();

	return;

} // update 

//--------------------------------------------------------------------- display
// This function is called each time the screen is to be drawn

void display(void) {

	// Clear the screen to the clear colour 
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw everything 

	// Draw long gradient of the colors according to depth on right side

	// 4 different quads that have one color on top and one color on bottom
	// opengl creates a gradient between these 2 colors

	glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(425.0f, 400.0f); // top-left | red
		glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(450.0f, 400.0f); // top-right | red
		glColor3f(1.0f, 1.0f, 0.0f); glVertex2f(450.0f, 300.0f); // bottom-left | yellow
		glColor3f(1.0f, 1.0f, 0.0f); glVertex2f(425.0f, 300.0f); // bottom-right | yellow
	glEnd(); 

	glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 0.0f); glVertex2f(425.0f, 300.0f); // top-left | yellow
		glColor3f(1.0f, 1.0f, 0.0f); glVertex2f(450.0f, 300.0f);  // top-right | yellow
		glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(450.0f, 200.0f); // bottom-left | green
		glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(425.0f, 200.0f); // bottom-right | green
	glEnd();

	glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(425.0f, 200.0f);  // top-left | green
		glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(450.0f, 200.0f);  // top-right | green
		glColor3f(0.0f, 1.0f, 1.0f); glVertex2f(450.0f, 100.0f);  // bottom-left | cyan
		glColor3f(0.0f, 1.0f, 1.0f); glVertex2f(425.0f, 100.0f);  // bottom-right | cyan
	glEnd();

	glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 1.0f); glVertex2f(425.0f, 100.0f);  // bottom-left | cyan
		glColor3f(0.0f, 1.0f, 1.0f); glVertex2f(450.0f, 100.0f);  // bottom-right | cyan
		glColor3f(0.0f, 0.0f, 1.0f); glVertex2f(450.0f, 0.0f);  // bottom-left | blue
		glColor3f(0.0f, 0.0f, 1.0f); glVertex2f(425.0f, 0.0f);  // bottom-right | blue
	glEnd();

	// Boundary Lines
	glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2f(0.0f, 407.5f);
		glVertex2f(500.0f, 407.5f);
	glEnd();

	// Create 2 lines to represent x/y coordinates on the map

	// x-axis

	// Line
	glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f); 
		glVertex2f(25.0f, 25.0f);
		glVertex2f(400.0f, 25.0f);
	glEnd();
	// Arrow
	glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2f(400.0f, 25.0f);
		glVertex2f(377.5f, 32.5f);
	glEnd();
	glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2f(400.0f, 25.0f);
		glVertex2f(377.5f, 17.5f);
	glEnd();


	// y-axis
	glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2f(25.0f, 25.0f);
		glVertex2f(25.0f, 400.0f);
	glEnd();
	// arrow
	glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2f(25.0f, 400.0f);
		glVertex2f(17.5f, 377.5f);
	glEnd();
	glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2f(25.0f, 400.0f);
		glVertex2f(32.5f, 377.5f);
	glEnd();

	// Write x and y coordinates
	glColor3f(0.0f, 0.0f, 0.0f);

	string s_maxLat = to_string(maxLat);
	string s_minLat = to_string(minLat);
	string s_maxLong = to_string(maxLong);
	string s_minLong = to_string(minLong); 
	string s_maxDep = to_string(maxDep);
	string s_minDep = to_string(minDep); 

	string xMin_yMin = "( " + s_minLat + " , " + s_minLong + " )"; 
	string xMin_yMax = "( " + s_maxLat + " , " + s_minLong + " )";
	string xMax_yMin = "( " + s_minLat + " , " + s_maxLong + " )";
	string xMax_yMax = "( " + s_maxLat + " , " + s_maxLong + " )";

	output(12.5f, 12.5f, xMin_yMin);
	output(12.5f, 412.5f, xMin_yMax);
	output(250.0f, 12.5f, xMax_yMin); 
	output(250.0f, 412.5f, xMax_yMax); 

	float x, y; 

	for (SonarPoint &p : points) {
		x = p.mappedX;
		y = p.mappedY; 
		glBegin(GL_QUADS);
			glColor3f(p.red, p.green, p.blue);
			glVertex2f(x - 3.0f, y + 3.0f);
			glVertex2f(x + 3.0f, y + 3.0f);
			glVertex2f(x + 3.0f, y - 3.0f);
			glVertex2f(x - 3.0f, y - 3.0f);
		glEnd();
	}



	glutSwapBuffers();

	return;

} // display

//---------------------------------------------------------------------- output
// Code by Mark Kilgard

void output(int x, int y, string str) {
	int len, i;

	glRasterPos2f(x, y);
	len = str.size(); 
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
	}
	return;

} // output

//-------------------------------------------------------------------- keyboard
// Deal with the keyboard input to the program.  

void keyboard(unsigned char c, int x, int y) {
	// escape key exits the program
	if (c == 27)
		exit(0);


	return;

} // keyboard

//------------------------------------------------------------------ mouseClick
// This function is called whenever a mouse button is pressed or released
void mouseClick(int button, int state, int x, int y) {

	return;

} // mouseClick

//------------------------------------------------------------------- mouseMove
// This function is called whenever the mouse moves within the window
void mouseMove(int x, int y) {

	return;

} // mouseMove

//--------------------------------------------------------------------- reshape
// This function is called when the window is reshaped
void reshape(int width, int height) {

	return;

} // reshape
