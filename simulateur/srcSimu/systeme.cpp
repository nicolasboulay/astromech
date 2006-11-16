
#include "systeme.h"
#include "module.h"
#include "senseur.h"
#include "algo.h"
#include "effecteur.h"
#include "moteur.h"
#include "guidageConstant.h"
#include "algoPICs.h"
#include "algoPC.h"
#include "comSeriePC_PIC.h"
#include "dynamiquePassive.h"
#include "chassis.h"
#include "contour.h"
#include "codeuse.h"
#include "tinyxml.h"
#include "simulation.h"
#include "trace.h"
#include "geom2D.h"
#include <math.h>
using namespace std;


Systeme::Systeme(string fichierSysteme,Simulation *simuPere,Trace * t)
{
  trace = t;
  src = "systeme.cpp";
  cl = "Systeme";
  message = "debut";
  simulationPere = simuPere;
  trace->print(src,cl,"Systeme",message);
  TiXmlDocument doc( fichierSysteme );
  if(!doc.LoadFile())
  {
    message = "Problème doc.LoadFile() "+ fichierSysteme;
    trace->print(src,cl,"Systeme",message);
    exit(1);   
  }
  else
  {
    if(!chargerXML(doc))
    {
      message = "Problème après appel chargerXML ";
      trace->print(src,cl,"Systeme",message);
      exit(1);
    }
    else
    {
      //les modules échangent leurs cartes de visite !
      connectModules(); 
      initPositionModules();
    }
  }
  message = "fin";
  trace->print(src,cl,"Systeme",message);
}

Systeme::~Systeme(void)
{
  message = "debut";
  trace->print(src,cl,"~Systeme",message);
  vectorModule::iterator it;
  for (it = tabModule.begin();it!=tabModule.end();++it)
  {
    delete(*it);
  }
  delete(positionCG);
  message = "fin";
  trace->print(src,cl,"~Systeme",message);
}

int Systeme::chargerXML(TiXmlDocument & doc)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  TiXmlHandle docHandle( &doc );
  //chargement du nom
  TiXmlElement* pElt = docHandle.FirstChild( "Systeme" ).FirstChildElement("nom").Element();
  if (pElt)
  {
    nom = pElt->GetText();
    message = "nom : " + nom;
    trace->print(src,cl,"chargerXML",message);
  }
  else	{return 0;}

  //chargement du nombre de modules
  pElt = docHandle.FirstChild( "Systeme" ).FirstChildElement("nbModules").Element();
  if (pElt)
  {
    istringstream iss( pElt->GetText() );
    iss >> nbModules;
    message = "nbModules : " + iss.str();
    trace->print(src,cl,"chargerXML",message);
  }
  else	{return 0;}
  
  //chargement position centre de gravité initial et attitude
  pElt = docHandle.FirstChild( "Systeme" ).FirstChildElement("positionCG").Element();
  if (pElt)
  {
    TiXmlElement* pPt;
    TiXmlHandle handlePosition(0);
    handlePosition = TiXmlHandle(pElt);
    pPt = handlePosition.FirstChildElement("Point2D").Element();
    
    positionCG = new Point2D(pPt);
  }
  else	{return 0;}
  
  pElt = docHandle.FirstChild( "Systeme" ).FirstChildElement("attitude").Element();
  if (pElt)
  {
    istringstream iss( pElt->GetText() );
    iss >> attitude_rad;
    message = "attitude : " + iss.str();
    trace->print(src,cl,"chargerXML",message);
    attitude_rad *= (M_PI/180);
  }
  else	{return 0;}
  
  //chargement des modules
  pElt = docHandle.FirstChild( "Systeme" ).FirstChildElement("Module").Element();
  for( pElt; pElt; pElt=pElt->NextSiblingElement())
  {
    string nomModule;
    Module *m;
    TiXmlHandle hEltModule(0);
    TiXmlElement* pEltNom;
    
    hEltModule = TiXmlHandle(pElt);
    pEltNom = hEltModule.FirstChild("nom_classe").Element();
    m = creerModule(pEltNom->GetText());
    if (m!=NULL)
    {
      m->chargerXML(pElt);
      tabModule.push_back(m);
    }
    else
    {
      message = "échec creerModule";
      trace->print(src,cl,"chargerXML",message);
      return 0;
    }
  }
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  return 1;
}
void Systeme::run(void)
{
  message = "debut";
  trace->print(src,cl,"run",message);
}

Module * Systeme::creerModule(string nom_classe)
{
  if (nom_classe == "E_Moteur")
  {
    Moteur *m;
    m = new Moteur(this,trace);
    return m;
  }
  
  if (nom_classe == "E_Chassis")
  {
    Chassis *c;
    c = new Chassis(this,trace);
    return c;
  }
  
  if (nom_classe == "A_GuidageConstant")
  {
    GuidageConstant *g;
    g = new GuidageConstant(this,trace);
    return g;
  }
  
  if (nom_classe == "S_Contour")
  {
    Contour *c;
    c = new Contour(this,trace);
    contour = c;
    return c;
  }
  
  if (nom_classe == "E_DynamiquePassive")
  {
    DynamiquePassive *d;
    d = new DynamiquePassive(this,trace);
    return d;
  }
  if (nom_classe == "A_AlgoPC")
  {
    AlgoPC *pc;
    pc = new AlgoPC(this,trace);
    return pc;
  }
  if (nom_classe == "A_AlgoPIC")
  {
    AlgoPIC *pic;
    pic = new AlgoPIC(this,trace);
    return pic;
  }
  if (nom_classe == "A_ComSeriePC_PIC")
  {
    ComSeriePC_PIC *com;
    com = new ComSeriePC_PIC(this,trace);
    return com;
  }
  if (nom_classe == "S_Codeuse")
  {
    Codeuse *c;
    c = new Codeuse(this,trace);
    return c;
  }

  return NULL;
}

Module * Systeme::getPtrModule(string nom_module)
{
  message = "debut";
  trace->print(src,cl,"getPtrModule",message);
  
  // recherche du module qui porte le nom fourni
  vectorModule::iterator it;
  Module *m;
  for (it = tabModule.begin();it!=tabModule.end();++it)
  {
    m = *it;
    if (m->getNomModule() == nom_module)
    {
      message = "fin";
      trace->print(src,cl,"getPtrModule",message);
      return m;
    }
  }
  message = "pas de module nommé " + nom_module;
  trace->print(src,cl,"getPtrModule",message);  
  
  return NULL;
}
string Systeme::getNomSysteme(void)
{
  return nom;
}
Simulation * Systeme::getSimuPere(void)
{
  return simulationPere;
}
Contour * Systeme::getPt_Contour(void)
{
  return contour;
}
Point2D * Systeme::getPositionCourante(void)
{
  return positionCG;
}
double * Systeme::getAttitudeCourante_rad(void)
{
  return &attitude_rad;
}
void Systeme::connectModules(void)
{
  message = "debut";
  trace->print(src,cl,"connectModules",message);
  
  vectorModule::iterator it;
  Module *m;
  for (it = tabModule.begin();it!=tabModule.end();++it)
  {
    m = *it;
    if (m!=NULL)
    {
      m->connectModules();
      m->setPasTemps(simulationPere->getPasTemps());
    }
  }
  message = "fin";
  trace->print(src,cl,"connectModules",message); 
}

void Systeme::classerModules(vectorModule & tabSenseurs,
                             vectorModule & tabAlgos,
			     vectorModule & tabEffecteurs)
{
  message = "debut";
  trace->print(src,cl,"classerModules",message);
  
  vectorModule::iterator it;
  Module *m;
  string nom_classe;
  string idAlgo = "A_";
  string idSenseur = "S_";
  string idEffecteur = "E_";
  for (it = tabModule.begin();it!=tabModule.end();++it)
  {
    m = *it;
    nom_classe = m->getNomClasse();
    if (nom_classe.substr(0,2) == idAlgo)
    {
      tabAlgos.push_back(m);
    }
    else
    {
      if (nom_classe.substr(0,2) == idSenseur)
      {
        tabSenseurs.push_back(m);
      }
      else
      {
        if (nom_classe.substr(0,2) == idEffecteur)
	{
	  tabEffecteurs.push_back(m);
	}
	else
	{
	  message = "bizarre identifiant classe module non reconnu";
          trace->print(src,cl,"classerModules",message);
	}
      }
    }
  }
  message = "fin";
  trace->print(src,cl,"classerModules",message);
}

void Systeme::initPositionModules(void)
{
  message = "debut";
  trace->print(src,cl,"initPositionModules",message);
  
  vectorModule::iterator it;
  Module *m;
  
  for (it = tabModule.begin();it!=tabModule.end();++it)
  {
    m = *it;
    m->setPosition(positionCG);
    m->setAttitude(&attitude_rad);
  }
  
  message = "fin";
  trace->print(src,cl,"initPositionModules",message);
}
