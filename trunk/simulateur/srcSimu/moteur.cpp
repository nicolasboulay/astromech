#include "module.h"
#include "systeme.h"
#include "effecteur.h"
#include "moteur.h"
#include "trace.h"
#include "tinyxml.h"
#include "senseur.h"
#include "contour.h"

using namespace std;

Moteur::Moteur(Systeme *s,Trace * t):Effecteur(s,t)
{
  trace = t;
  src = "moteur.cpp";
  cl = "Moteur";
  message = "debut";
  trace->print(src,cl,"Moteur",message);
  
  deltaRotPrec_rad = 0;
  
  message = "fin";
  trace->print(src,cl,"Moteur",message);
}

Moteur::~Moteur(void)
{
  message = "debut";
  trace->print(src,cl,"~Moteur",message);
  
  message = "fin";
  trace->print(src,cl,"~Moteur",message);
}

double Moteur::getDeltaRot_rad(void)
{
  return deltaRot_rad;
}

void Moteur::execute(int tempsCourant_ms)
{
  message = "debut";
  trace->print(src,cl,"execute",message);
  
  double ecartDeltaPrecEtConsigne_rad;
  double consigneDeltaRot_rad;
  double differentielMax_rad;
  deltaRot_rad = 0;
  //consigne associée au pas de temps
  consigneDeltaRot_rad = (consigneVitesse_rad_par_s * pasTemps_ms) / 1000;
  //acceleration max associée au pas de temps
  differentielMax_rad = (coeffSatAccelRot_rad_par_s * pasTemps_ms) / 1000;
  
  ecartDeltaPrecEtConsigne_rad = consigneDeltaRot_rad - deltaRotPrec_rad;
  deltaRot_rad = deltaRotPrec_rad + ecartDeltaPrecEtConsigne_rad * coeffRedErreur;
  //saturation de l'accélération
  if ((deltaRot_rad - deltaRotPrec_rad) > differentielMax_rad)
  {
    deltaRot_rad = deltaRotPrec_rad + differentielMax_rad;
  }
  //saturation de la décélération
  if ((deltaRot_rad - deltaRotPrec_rad) < -differentielMax_rad)
  {
    deltaRot_rad = deltaRotPrec_rad - differentielMax_rad;
  }
  
  //gestion des choc (collante)
  if (contour->intersectionEnCours())
  {
    deltaRot_rad = 0; //on arrete tout
  }
  deltaRotPrec_rad = deltaRot_rad;
  log << tempsCourant_ms <<"\t"<< consigneDeltaRot_rad <<"\t"<< deltaRot_rad << endl;
  message = "fin";
  trace->print(src,cl,"execute",message);
}

void Moteur::setConsigneVitesse(double vitesse_rad_par_s)
{
  consigneVitesse_rad_par_s = vitesse_rad_par_s;
}

void Moteur::chargerXML(TiXmlElement* pModuleXML)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  Effecteur::chargerXML(pModuleXML);
  
  TiXmlElement* pElt;
  TiXmlHandle handleModule(0);
  handleModule = TiXmlHandle(pModuleXML);
  
  pElt = handleModule.FirstChild("coeffRedErreur").Element();
  istringstream iss( pElt->GetText() );
  iss >> coeffRedErreur;
  message = "coeffRedErreur : " + iss.str();
  trace->print(src,cl,"chargerXML",message);
  
  pElt = handleModule.FirstChild("coeffSatAccelRot_rad_par_s").Element();
  istringstream iss2( pElt->GetText() );
  iss2 >> coeffSatAccelRot_rad_par_s;
  message = "coeffSatAccelRot_rad_par_s : " + iss2.str();
  trace->print(src,cl,"chargerXML",message);
  
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
}

void Moteur::connectModules(void)
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
