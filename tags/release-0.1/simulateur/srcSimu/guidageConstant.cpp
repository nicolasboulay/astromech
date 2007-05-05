#include "module.h"
#include "systeme.h"
#include "algo.h"
#include "guidageConstant.h"
#include "trace.h"
#include "tinyxml.h"
#include "effecteur.h"
#include "moteur.h"
using namespace std;

GuidageConstant::GuidageConstant(Systeme *s,Trace * t):Algo(s,t)
{
  trace = t;
  src = "guidageConstant.cpp";
  cl = "GuidageConstant";
  message = "debut";
  trace->print(src,cl,"GuidageConstant",message);
  message = "fin";
  trace->print(src,cl,"GuidageConstant",message);
}

GuidageConstant::~GuidageConstant(void)
{
  message = "debut";
  trace->print(src,cl,"~GuidageConstant",message);
  message = "fin";
  trace->print(src,cl,"~GuidageConstant",message);
}

void GuidageConstant::execute(int tempsCourant_ms)
{
  message = "debut";
  trace->print(src,cl,"execute",message);
  
  if ((tempsCourant_ms >=1000)&&(tempsCourant_ms <=20000))
  {
    moteurDroit->setConsigneVitesse(vitConsigneMoteurDroit_rad_s);
    moteurGauche->setConsigneVitesse(vitConsigneMoteurGauche_rad_s);
  }
  else
  {
    moteurDroit->setConsigneVitesse(0.0);
    moteurGauche->setConsigneVitesse(0.0);
  }
  log << tempsCourant_ms <<"\t"<< vitConsigneMoteurDroit_rad_s <<"\t"<< vitConsigneMoteurGauche_rad_s << endl;
  
  message = "fin";
  trace->print(src,cl,"execute",message);
}

void GuidageConstant::chargerXML(TiXmlElement* pModuleXML)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  Algo::chargerXML(pModuleXML);
  
  TiXmlElement* pElt;
  TiXmlHandle handleModule(0);
  handleModule = TiXmlHandle(pModuleXML);
  
  pElt = handleModule.FirstChild("vitMoteurDroit_rad_s").Element();
  istringstream iss( pElt->GetText() );
  iss >> vitConsigneMoteurDroit_rad_s;
  message = "vitConsigneMoteurDroit_rad_s : " + iss.str();
  trace->print(src,cl,"chargerXML",message);
  
  pElt = handleModule.FirstChild("vitMoteurGauche_rad_s").Element();
  istringstream iss2( pElt->GetText() );
  iss2 >> vitConsigneMoteurGauche_rad_s;
  message = "vitConsigneMoteurGauche_rad_s : " + iss2.str();
  trace->print(src,cl,"chargerXML",message);
    
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
}

void GuidageConstant::connectModules(void)
{
  message = "debut";
  trace->print(src,cl,"connectModules",message);
  
  string nomModule;
  vectorString::iterator it;
  it = tabRelation.begin();
  if (tabRelation.size() == 2)
  {//reinterpret_cast<GuidageConstant *>
    nomModule = *it;
    moteurDroit = (Moteur *)systPere->getPtrModule(nomModule);
    message = "connection à :" + nomModule;
    trace->print(src,cl,"connectModules",message);
  
    it++;
  
    nomModule = *it;
    moteurGauche = (Moteur *)systPere->getPtrModule(nomModule);
    message = "connection à :" + nomModule;
    trace->print(src,cl,"connectModules",message);
  }
  
  message = "fin";
  trace->print(src,cl,"connectModules",message);
}
