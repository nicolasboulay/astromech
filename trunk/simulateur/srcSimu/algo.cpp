#include "module.h"
#include "systeme.h"
#include "algo.h"
#include "trace.h"
using namespace std;

Algo::Algo(Systeme *s,Trace * t):Module(s,t)
{
  trace = t;
  src = "algo.cpp";
  cl = "Algo";
  message = "debut";
  trace->print(src,cl,"Algo",message);
  message = "fin";
  trace->print(src,cl,"Algo",message);
}

Algo::~Algo(void)
{
  message = "debut";
  trace->print(src,cl,"~Algo",message);
  message = "fin";
  trace->print(src,cl,"~Algo",message);
}

void Algo::execute(int tempsCourant_ms)
{
  message = "debut";
  trace->print(src,cl,"execute",message);
  message = "fin";
  trace->print(src,cl,"execute",message);
}

void Algo::chargerXML(TiXmlElement* pModuleXML)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  Module::chargerXML(pModuleXML);
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
}
