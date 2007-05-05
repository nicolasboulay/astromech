#include "module.h"
#include "systeme.h"
#include "senseur.h"
#include "trace.h"
using namespace std;

Senseur::Senseur(Systeme *s,Trace * t):Module(s,t)
{
  trace = t;
  src = "senseur.cpp";
  cl = "Senseur";
  
  message = "debut";
  trace->print(src,cl,"Senseur",message);
  message = "fin";
  trace->print(src,cl,"Senseur",message);
}

Senseur::~Senseur(void)
{
  message = "debut";
  trace->print(src,cl,"~Senseur",message);
  message = "fin";
  trace->print(src,cl,"~Senseur",message);
}

void Senseur::execute(int tempsCourant_ms)
{
  message = "debut";
  trace->print(src,cl,"execute",message);
  message = "fin";
  trace->print(src,cl,"execute",message);
}

void Senseur::chargerXML(TiXmlElement* pModuleXML)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  Module::chargerXML(pModuleXML);
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
}
