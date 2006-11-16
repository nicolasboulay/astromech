#include "module.h"
#include "systeme.h"
#include "effecteur.h"
#include "trace.h"
using namespace std;

Effecteur::Effecteur(Systeme *s,Trace * t):Module(s,t)
{
  trace = t;
  src = "effecteur.cpp";
  cl = "Effecteur";
  message = "debut";
  trace->print(src,cl,"Effecteur",message);
  message = "fin";
  trace->print(src,cl,"Effecteur",message);
}

Effecteur::~Effecteur(void)
{
  message = "debut";
  trace->print(src,cl,"~Effecteur",message);
  message = "fin";
  trace->print(src,cl,"~Effecteur",message);
}

void Effecteur::execute(int tempsCourant_ms)
{
  message = "debut";
  trace->print(src,cl,"execute",message);
  message = "fin";
  trace->print(src,cl,"execute",message);
}

void Effecteur::chargerXML(TiXmlElement* pModuleXML)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  Module::chargerXML(pModuleXML);
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
}
