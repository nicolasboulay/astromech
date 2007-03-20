#include <iostream>
#include "simulation.h"
#include "algoPlanifTraj.h"
#include "trace.h"
#include "geom2D.h"
#include "bit.h"
#include <math.h>
#include <string.h>
using namespace std;
int main()
{
     cout << " *** SIMU ROBOTIQUE 2006 ***\n";
				   
     Trace *t;
     Simulation *s;
     //AlgoPlanifTrajectoire *a;
     t = new Trace();
     if (t!=NULL)
     {     
       /*a = new AlgoPlanifTrajectoire("data/appart.xml",t);
       a->CalculeGrapheStatique(0.25);*/
       s = new Simulation("data/simulation.xml",t);
       if (s!=NULL)
       {
         s->run();
       }
     }
          //delete(a);
     delete(s);
     return 0;   
}
