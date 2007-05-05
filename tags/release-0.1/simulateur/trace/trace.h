#ifndef TraceH
#define TraceH

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string> 

using namespace std;

string intToString (int number);

class Trace
{
  public : // accessible partout
  Trace(void);
  ~Trace(void);
  void print(string nomSource, string nomClasse, string nomMethode, string message);  
};
#endif
