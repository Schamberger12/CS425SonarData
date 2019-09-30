CXX = g++

default: SonarGraph


SonarGraph: Source.o SonarPoint.o HSV_RGB_CONVERT.o
		$(CXX) -o SonarGraph Source.o SonarPoint.o HSV_RGB_CONVERT.o -lglut -lGLU -lGL

Source.o: Source.cpp SonarPoint.cpp SonarPoint.h HSV_RGB_CONVERT.cpp HSV_RGB_CONVERT.h
		$(CXX) -c Source.cpp

SonarPoint.o: SonarPoint.cpp SonarPoint.h
		$(CXX) -c SonarPoint.cpp

HSV_RGB_CONVERT.o: HSV_RGB_CONVERT.cpp HSV_RGB_CONVERT.h
		$(CXX) -c HSV_RGB_CONVERT.cpp


clean:
		$(RM) SonarGraph *.o *~