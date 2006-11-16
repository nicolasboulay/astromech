#include "module.h"
#include "systeme.h"
#include "effecteur.h"
#include "senseur.h"
#include "dynamiquePassive.h"
#include "trace.h"
#include "tinyxml.h"
#include "contour.h"
#include <math.h>
using namespace std;

DynamiquePassive::DynamiquePassive(Systeme *s,Trace * t):Effecteur(s,t)
{
  trace = t;
  src = "dynamiquePassive.cpp";
  cl = "DynamiquePassive";
  message = "debut";
  trace->print(src,cl,"DynamiquePassive",message);
    
  message = "fin";
  trace->print(src,cl,"DynamiquePassive",message);
}

DynamiquePassive::~DynamiquePassive(void)
{
  message = "debut";
  trace->print(src,cl,"~DynamiquePassive",message);
  message = "fin";
  trace->print(src,cl,"~DynamiquePassive",message);
}

void DynamiquePassive::execute(int tempsCourant_ms)
{
  message = "debut";
  trace->print(src,cl,"execute",message);
  
  //log << tempsCourant_ms <<"\t"<< new_pos_x_m <<"\t"<< new_pos_y_m <<"\t"<< att_deg <<"\t"<< attNormalise_deg << endl;
  
  message = "fin";
  trace->print(src,cl,"execute",message);
}


void DynamiquePassive::chargerXML(TiXmlElement* pModuleXML)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  Effecteur::chargerXML(pModuleXML);
  
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
}

void DynamiquePassive::connectModules(void)
{
  message = "debut";
  trace->print(src,cl,"connectModules",message);
  
  string nomModule;
  vectorString::iterator it;
  it = tabRelation.begin();
  if (tabRelation.size() == 1)
  {
    nomModule = *it;
    contour = (Contour *)systPere->getPtrModule(nomModule);
    message = "connection à :" + nomModule;
    trace->print(src,cl,"connectModules",message);
  }
  message = "fin";
  trace->print(src,cl,"connectModules",message);
}
