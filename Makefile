all : testuj

testuj : test
	./test

test : test.cpp virus_genealogy.cpp virus_genealogy.h
	g++ -std=c++11 -Wall test.cpp virus_genealogy.cpp -otest;
