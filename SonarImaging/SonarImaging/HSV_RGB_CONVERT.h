// hsv_rgb_convert code taken from David H on stackoverflow
// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
#ifndef HSV_RGB_CONVERT_H
#define HSV_RGB_CONVERT_H

typedef struct {
	double r; 
	double g;
	double b;
} rgb;

typedef struct {
	double h;
	double s;
	double v;
} hsv;

rgb hsv2rgb(hsv in);

hsv rgb2hsv(rgb in); 


#endif