#include "conversion.h"

string int2String    (int i)
{
  ostringstream temp;
  temp << i;
  return temp.str( );
}
string double2String (double i)
{
  ostringstream temp;
  temp << i;
  return temp.str( );
}
int    string2Int    (string str)
{
  int res;
  istringstream iss( str );
  iss >> res;
  return res;
}
double string2Double (string str)
{
  double res;
  istringstream iss( str );
  iss >> res;
  return res;
}
