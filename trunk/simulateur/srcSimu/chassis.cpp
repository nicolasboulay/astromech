#include "module.h"
#include "systeme.h"
#include "effecteur.h"
#include "chassis.h"
#include "trace.h"
#include "tinyxml.h"
#include "moteur.h"
#include "geom2D.h"
#include <math.h>
using namespace std;

Chassis::Chassis(Systeme *s,Trace * t):Effecteur(s,t)
{
  trace = t;
  src = "chassis.cpp";
  cl = "Chassis";
  message = "debut";
  trace->print(src,cl,"Chassis",message);
    
  message = "fin";
  trace->print(src,cl,"Chassis",message);
}

Chassis::~Chassis(void)
{
  message = "debut";
  trace->print(src,cl,"~Chassis",message);
  message = "fin";
  trace->print(src,cl,"~Chassis",message);
}

void Chassis::execute(int tempsCourant_ms)
{
  message = "debut";
  trace->print(src,cl,"execute",message);
  
  double deltaRotMoteurDroit_rad;
  double deltaRotMoteurGauche_rad;
  double deltaPosDroit_m;
  double delatPosGauche_m;
  double deltaAv_m;
  double new_pos_x_m;
  double new_pos_y_m;
  double new_att_rad;
  double att_deg;
  double attNormalise_deg;
  double pos_x_m = positionCourante->getX();
  double pos_y_m = positionCourante->getY();
  double attitude_rad = *attitudeCourante_rad;

  deltaRotMoteurDroit_rad = moteurDroit->getDeltaRot_rad();
  deltaRotMoteurGauche_rad = moteurGauche->getDeltaRot_rad();
  
  deltaPosDroit_m = deltaRotMoteurDroit_rad * rayonRoueDroite_m / rapportReduction;
  delatPosGauche_m = deltaRotMoteurGauche_rad * rayonRoueGauche_m / rapportReduction ;
  
  deltaPosDroit_m = deltaPosDroit_m * (1 - glissementRoueDroite_mm_par_m/1000);
  delatPosGauche_m = delatPosGauche_m * (1 - glissementRoueGauche_mm_par_m/1000);
  
  deltaAv_m = (deltaPosDroit_m + delatPosGauche_m)/2;
  new_pos_x_m = pos_x_m + deltaAv_m * sin(attitude_rad);
  new_pos_y_m = pos_y_m + deltaAv_m * cos(attitude_rad);
  new_att_rad = attitude_rad + atan((delatPosGauche_m - deltaPosDroit_m)/ecartEntreRoues_m);
  
  //mise à jour des variables partagees
  positionCourante->setXY(new_pos_x_m,new_pos_y_m);
  *attitudeCourante_rad = new_att_rad;
  
  att_deg = attitude_rad*(180/M_PI);
  attNormalise_deg = att_deg;
  while (attNormalise_deg >= 360){attNormalise_deg -= 360;}
  while (attNormalise_deg < 0){attNormalise_deg += 360;}
  
  log << tempsCourant_ms <<"\t"<< new_pos_x_m <<"\t"<< new_pos_y_m <<"\t"<< att_deg <<"\t"<< attNormalise_deg << endl;
  
  message = "fin";
  trace->print(src,cl,"execute",message);
}

void Chassis::chargerXML(TiXmlElement* pModuleXML)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  Effecteur::chargerXML(pModuleXML);
  
  TiXmlElement* pElt;
  TiXmlHandle handleModule(0);
  handleModule = TiXmlHandle(pModuleXML);
  
  pElt = handleModule.FirstChild("rapportReduction").Element();
  istringstream iss( pElt->GetText() );
  iss >> rapportReduction;
  message = "rapportReduction : " + iss.str();
  trace->print(src,cl,"chargerXML",message);
  
  pElt = handleModule.FirstChild("rayonRoueDroite_m").Element();
  istringstream iss2( pElt->GetText() );
  iss2 >> rayonRoueDroite_m;
  message = "rayonRoueDroite_m : " + iss2.str();
  trace->print(src,cl,"chargerXML",message);
  
  pElt = handleModule.FirstChild("rayonRoueGauche_m").Element();
  istringstream iss3( pElt->GetText() );
  iss3 >> rayonRoueGauche_m;
  message = "rayonRoueGauche_m : " + iss3.str();
  trace->print(src,cl,"chargerXML",message);
  
  pElt = handleModule.FirstChild("ecartEntreRoues_m").Element();
  istringstream iss4( pElt->GetText() );
  iss4 >> ecartEntreRoues_m;
  message = "ecartEntreRoues_m : " + iss4.str();
  trace->print(src,cl,"chargerXML",message);
  
  pElt = handleModule.FirstChild("glissementRoueDroite_mm_par_m").Element();
  istringstream iss5( pElt->GetText() );
  iss5 >> glissementRoueDroite_mm_par_m;
  message = "glissementRoueDroite_mm_par_m : " + iss5.str();
  trace->print(src,cl,"chargerXML",message);
  
  pElt = handleModule.FirstChild("glissementRoueGauche_mm_par_m").Element();
  istringstream iss6( pElt->GetText() );
  iss6 >> glissementRoueGauche_mm_par_m;
  message = "glissementRoueGauche_mm_par_m : " + iss6.str();
  trace->print(src,cl,"chargerXML",message);
    
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
}

void Chassis::connectModules(void)
{
  message = "debut";
  trace->print(src,cl,"connectModules",message);
  
  string nomModule;
  vectorString::iterator it;
  it = tabRelation.begin();
  if (tabRelation.size() == 2)
  {
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
