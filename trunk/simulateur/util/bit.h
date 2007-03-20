#ifndef BitH
#define BitH
#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

void setChar(const unsigned char bit[8],
	     unsigned char & byte);

void setBit(const unsigned char numeroBit,
	    unsigned char & byte);	     

void clearBit(const unsigned char numeroBit,
	      unsigned char & byte);
	      
bool isBitSet(const unsigned char byte,
              const unsigned char numeroBit);
	      
string byteToString(const unsigned char byte);

#endif

