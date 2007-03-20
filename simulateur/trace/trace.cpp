#include "trace.h"

using namespace std;

string intToString (int number)
{
  ostringstream oss;
  oss<< number;
  return oss.str();
}

Trace::Trace()
{
  cout << "new Trace\n";
}

Trace::~Trace(void)
{
  cout << "delete Trace\n";
}

void Trace::print(string nomSource, string nomClasse, string nomMethode, string message)
{
  cout << "### "<<nomSource<<" ### "<<nomClasse<<"::"<<nomMethode<<" Message : "<<message<<endl;
}


