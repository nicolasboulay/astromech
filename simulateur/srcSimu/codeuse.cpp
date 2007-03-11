#include "module.h"
#include "systeme.h"
#include "senseur.h"
#include "codeuse.h"
#include "trace.h"
#include "tinyxml.h"
#include "effecteur.h"
#include "moteur.h"
#include <math.h>
using namespace std;

Codeuse::Codeuse(Systeme *s,Trace * t):Senseur(s,t)
{
  trace = t;
  src = "codeuse.cpp";
  cl = "Codeuse";
  message = "debut";
  trace->print(src,cl,"Codeuse",message);
    
  message = "fin";
  trace->print(src,cl,"Codeuse",message);
}

Codeuse::~Codeuse(void)
{
  message = "debut";
  trace->print(src,cl,"~Codeuse",message);
  
  message = "fin";
  trace->print(src,cl,"~Contour",message);
}

void Codeuse::execute(int tempsCourant_ms)
{
  message = "debut";
  trace->print(src,cl,"execute",message);
  
  nbImpulsionComptees = (moteur->getDeltaRot_rad()/(2*M_PI))*nbPointsParTour;
  if (nbImpulsionComptees<0)
  {
    nbImpulsionComptees = -nbImpulsionComptees;
  }
  log << tempsCourant_ms <<"\t"<< nbImpulsionComptees << endl;
  
  message = "fin";
  trace->print(src,cl,"execute",message);
}


void Codeuse::chargerXML(TiXmlElement* pModuleXML)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  Senseur::chargerXML(pModuleXML);
  
  TiXmlElement* pElt;
  TiXmlHandle handleModule(0);
  handleModule = TiXmlHandle(pModuleXML);
  
  pElt = handleModule.FirstChild("nbPointsParTour").Element();
  istringstream iss( pElt->GetText() );
  iss >> nbPointsParTour;
  message = "nbPointsParTour : " + iss.str();
  trace->print(src,cl,"chargerXML",message);
    
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
}

void Codeuse::connectModules(void)
{
  message = "debut";
  trace->print(src,cl,"connectModules",message);
  
  string nomModule;
  vectorString::iterator it;
  it = tabRelation.begin();
  if (tabRelation.size() == 1)
  {
    nomModule = *it;
    moteur = (Moteur *)systPere->getPtrModule(nomModule);
    message = "connection à :" + nomModule;
    trace->print(src,cl,"connectModules",message);
  }

  message = "fin";
  trace->print(src,cl,"connectModules",message);
}

double Codeuse::getNbImpulsions(void)
{
  return nbImpulsionComptees;
}
