#include <iostream>
#include "simulation.h"
#include "trace.h"
#include "geom2D.h"
#include <math.h>
using namespace std;
int main()
{
     cout << " *** SIMU ROBOTIQUE 2006 ***\n";
				   
     Trace *t;
     Simulation *s;
     t = new Trace();
     if (t!=NULL)
     {
       s = new Simulation("data/simulation.xml",t);
       if (s!=NULL)
       {
         s->run();
       }
     }
     delete(s);
     return 0;   
}
