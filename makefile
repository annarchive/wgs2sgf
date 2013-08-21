CC=g++
CXX=g++
CXXFLAGS= -Wall
OBJ = main.o wgs.o common.o getmoves.o
getmoves.o:getmoves.cpp wgs.h
	$(CXX) $(CXXFLAGS) -c getmoves.cpp
common.o:common.cpp wgs.h
	$(CXX) $(CXXFLAGS) -c common.cpp
main.o:wgs.h main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp
wgs.o:wgs.h wgs.cpp
	$(CXX) $(CXXFLAGS) -c wgs.cpp
all:$(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o wgs2sgf

clean:
	rm -rf *.o