#include "module.h"
#include "systeme.h"
#include "senseur.h"
#include "camera.h"
#include "trace.h"
#include "tinyxml.h"
#include <math.h>
using namespace std;

Camera::Camera(Systeme *s,Trace * t):Senseur(s,t)
{
  trace = t;
  src = "camera.cpp";
  cl = "Camera";
  message = "debut";
  trace->print(src,cl,"Camera",message);
    
  message = "fin";
  trace->print(src,cl,"Camera",message);
}

Camera::~Camera(void)
{
  message = "debut";
  trace->print(src,cl,"~Camera",message);
  
  message = "fin";
  trace->print(src,cl,"~Camera",message);
}

void Camera::execute(int tempsCourant_ms)
{
  message = "debut";
  trace->print(src,cl,"execute",message);
  
  
  //log << tempsCourant_ms <<"\t"<< nbImpulsionComptees << endl;
  
  message = "fin";
  trace->print(src,cl,"execute",message);
}


void Camera::chargerXML(TiXmlElement* pModuleXML)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  Senseur::chargerXML(pModuleXML);
  
  TiXmlElement* pElt;
  TiXmlHandle handleModule(0);
  handleModule = TiXmlHandle(pModuleXML);
  
/*  pElt = handleModule.FirstChild("nbPointsParTour").Element();
  istringstream iss( pElt->GetText() );
  iss >> nbPointsParTour;
  message = "nbPointsParTour : " + iss.str();
  trace->print(src,cl,"chargerXML",message);*/
    
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
}

void Camera::connectModules(void)
{
  message = "debut";
  trace->print(src,cl,"connectModules",message);
  
  string nomModule;
  vectorString::iterator it;
  it = tabRelation.begin();
/*  if (tabRelation.size() == 1)
  {
    nomModule = *it;
    moteur = (Moteur *)systPere->getPtrModule(nomModule);
    message = "connection à :" + nomModule;
    trace->print(src,cl,"connectModules",message);
  }*/

  message = "fin";
  trace->print(src,cl,"connectModules",message);
}
