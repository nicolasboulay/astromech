#include "simulation.h"
#include "systeme.h"
#include "module.h"
#include "obstacles.h"
#include "tinyxml.h"
#include "trace.h"

using namespace std;


Simulation::Simulation(string fichierSimulation, Trace * t)
{
  trace = t;
  src = "simulation.cpp";
  cl = "Simulation";
  message = "debut";
  trace->print(src,cl,"Simulation",message);
  TiXmlDocument doc( fichierSimulation );
  if(!doc.LoadFile())
  {
    message = "Problème doc.LoadFile() "+ fichierSimulation;
    trace->print(src,cl,"Simulation",message);
    exit(1);  
  }
  else
  {
    if(!chargerXML(doc))
    {
      message = "Problème après appel chargerXML ";
      trace->print(src,cl,"Simulation",message);
      exit(1);
    }
    else
    {
      //rangement des modules dans les 3 tableaux
      classerModules();
    }
  }
  message = "fin";
  trace->print(src,cl,"Simulation",message);
}

Simulation::~Simulation(void)
{
  message = "debut";
  trace->print(src,cl,"~Simulation",message);
  vectorSysteme::iterator it;
  for (it = tabSysteme.begin();it!=tabSysteme.end();++it)
  {
    delete(*it);
  }
  
  delete (obstacles);
  
  message = "fin";
  tabSysteme.clear();
  trace->print(src,cl,"~Simulation",message);
}

int Simulation::getPasTemps(void)
{
  return pasTemps_ms;
}
vectorSysteme Simulation::getTabSystemes(void)
{
  return tabSysteme;
}
int Simulation::chargerXML(TiXmlDocument & doc)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  TiXmlHandle docHandle( &doc );
  
  //chargement du nombre de modules
  TiXmlElement* pElt = docHandle.FirstChild( "Simulation" ).FirstChildElement("pasTemps_ms").Element();
  if (pElt)
  {
    istringstream iss( pElt->GetText() );
    iss >> pasTemps_ms;
    message = "lecture pasTemps_ms : " + iss.str();
    trace->print(src,cl,"chargerXML",message);
  }
  else	{return 0;}
  
  pElt = docHandle.FirstChild( "Simulation" ).FirstChildElement("duree_s").Element();
  if (pElt)
  {
    istringstream iss( pElt->GetText() );
    iss >> dureeSimu_s;
    message = "lecture dureeSimu_s : " + iss.str();
    trace->print(src,cl,"chargerXML",message);
  }
  else	{return 0;}
  
  //chargement des systemes
  pElt = docHandle.FirstChild( "Simulation" ).FirstChildElement("fichierSysteme").Element();
  for( pElt; pElt; pElt=pElt->NextSiblingElement("fichierSysteme"))
  {
    string nomFichierSysteme;
    Systeme *sys;
    nomFichierSysteme = pElt->GetText();
    sys = new Systeme(nomFichierSysteme,this,trace);
    tabSysteme.push_back(sys);
  }
  
/*  pElt = docHandle.FirstChild( "Simulation" ).FirstChildElement("fichierObstacle").Element();
  string nomFichierObstacle;
  nomFichierObstacle = pElt->GetText();
  obstacles = new Obstacles(nomFichierObstacle,trace);*/
  
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
  
  return 1;
}

void Simulation::classerModules(void)
{
  message = "debut";
  trace->print(src,cl,"classerModules",message);
  
  //demande de classement aux différents systèmes
  vectorSysteme::iterator it;
  Systeme *s;
  for (it = tabSysteme.begin();it!=tabSysteme.end();++it)
  {
    s = *it;
    s->classerModules(tabSenseurs,tabAlgos,tabEffecteurs);
  }
  message = "fin";
  trace->print(src,cl,"classerModules",message);
}
void Simulation::run(void)
{
  message = "debut";
  trace->print(src,cl,"run",message);
  vectorModule::iterator it;
  Module *m;
  int finSimu_ms = dureeSimu_s*1000;
  int compteurTemps_ms;
  
  for (compteurTemps_ms = 0;compteurTemps_ms<=finSimu_ms;compteurTemps_ms+=pasTemps_ms)
  {
    ostringstream oss;
    oss << compteurTemps_ms;
    message = "t = " + oss.str();
    trace->print(src,cl,"run",message);
    oss.clear();
    for (it = tabSenseurs.begin();it!=tabSenseurs.end();++it)
    {
      m = *it;
      m->execute(compteurTemps_ms);
    }
    for (it = tabAlgos.begin();it!=tabAlgos.end();++it)
    {
      m = *it;
      m->execute(compteurTemps_ms);
    }
    for (it = tabEffecteurs.begin();it!=tabEffecteurs.end();++it)
    {
      m = *it;
      m->execute(compteurTemps_ms);
    }
  }
  message = "fin";
  trace->print(src,cl,"run",message);
}
