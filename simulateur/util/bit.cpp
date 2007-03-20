#include "bit.h"

void setChar(const unsigned char bit[8],
	     unsigned char & byte)
{
  for (int i=0;i<8;i++)
  {
    if (bit[i])
      byte |= (1<<(7-i));
    else
      byte &= ~(1<<(7-i));
  }
}

void setBit(const unsigned char numeroBit,
	    unsigned char & byte)
{
  byte |= (1<<numeroBit); 
}

void clearBit(const unsigned char numeroBit,
	      unsigned char & byte)
{
  byte &= ~(1<<numeroBit); 
}		     

bool isBitSet(const unsigned char byte,
              const unsigned char numeroBit)
{
  bool bit = byte & (1 << numeroBit);
  return bit;
}
	      
string byteToString(const unsigned char byte)
{
  string strByte="";
  for (int i=7;i>=0;i--)
  {
    if (isBitSet(byte,i))
      strByte+='1';
    else
      strByte+='0';
  }
  return strByte;
}
