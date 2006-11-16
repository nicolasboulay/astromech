#include "trace.h"

using namespace std;

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
