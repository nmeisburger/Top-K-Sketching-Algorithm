CXX = g++

CXXFLAGS = -Wall -g

main: main.o CMS.o
	$(CXX) $(CXXFLAGS) -o main main.o CMS.o

main.o: main.cpp CMS.h
	$(CXX) $(CXXFLAGS) -c main.cpp

CMS.o: CMS.h