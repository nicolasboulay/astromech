#ifndef TraceH
#define TraceH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

class Trace
{
  public : // accessible partout
  Trace(void);
  ~Trace(void);
  void print(string nomSource, string nomClasse, string nomMethode, string message);
};
#endif
