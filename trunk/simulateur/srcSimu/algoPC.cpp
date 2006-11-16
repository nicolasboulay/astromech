#include "module.h"
#include "algo.h"
#include "algoPC.h"
#include "systeme.h"
#include "trace.h"
#include "tinyxml.h"
#include "comSeriePC_PIC.h"
#include "geom2D.h"
#include <math.h>

using namespace std;

AlgoPC::AlgoPC(Systeme *s,Trace * t):Algo(s,t)
{
  trace = t;
  src = "algoPC.cpp";
  cl = "AlgoPC";
  message = "debut";
  trace->print(src,cl,"AlgoPC",message);
  tempsDerniereExecution_ms = -100000;
  message = "fin";
  trace->print(src,cl,"AlgoPC",message);
}

AlgoPC::~AlgoPC(void)
{
  message = "debut";
  trace->print(src,cl,"~AlgoPC",message);
  message = "fin";
  trace->print(src,cl,"~AlgoPC",message);
}

void AlgoPC::execute(int tempsCourant_ms)
{
  message = "debut";
  trace->print(src,cl,"execute",message);
  
  if ((tempsCourant_ms - tempsDerniereExecution_ms) >= 1000/(frequenceExecution_Hz))
  {
    if (comSerie->tramePIC_PC_recue())
    {
      TramePIC_PC * trameRecue;
      trameRecue = comSerie->getTramePIC_PC();
      //lecture trame et sauvegarde des données en local
      delete trameRecue;
    }
    
    //traitements
    
    //création d'une trame pour les PICs
    TramePC_PIC * trameEnvoyee = new TramePC_PIC();
    trameEnvoyee->numeroSequenceWP = numeroSequenceWP;
    for (int indWP = 0;indWP < listeWP.size();indWP++)
    {
      //recopie des wp dans la trame
      WayPoint * wp = new WayPoint(listeWP[indWP]->pt->x,
                                   listeWP[indWP]->pt->y,
				   listeWP[indWP]->attitude_rad,
				   listeWP[indWP]->vitesse_m_par_s);
      trameEnvoyee->tabWayPoint.push_back(wp);
    } 
    cout << "trameEnvoyee "<<trameEnvoyee<<endl;
    cout << "listeWP "<<&(trameEnvoyee->tabWayPoint)<<endl;
    comSerie->sendToPIC(trameEnvoyee);
    
    tempsDerniereExecution_ms = tempsCourant_ms;
    log << tempsCourant_ms << endl;
  }
  
  message = "fin";
  trace->print(src,cl,"execute",message);
}

void AlgoPC::chargerXML(TiXmlElement* pModuleXML)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  Algo::chargerXML(pModuleXML);
      
  TiXmlElement* pElt;
  TiXmlHandle handleModule(0);
  handleModule = TiXmlHandle(pModuleXML);
  
  pElt = handleModule.FirstChild("frequenceExecution_Hz").Element();
  istringstream iss( pElt->GetText() );
  iss >> frequenceExecution_Hz;
  message = "frequenceExecution_Hz : " + iss.str();
  trace->print(src,cl,"chargerXML",message);

  pElt = handleModule.FirstChild("numeroSeqWP").Element();
  istringstream iss1( pElt->GetText() );
  iss1 >> numeroSequenceWP;
  message = "numeroSequenceWP : " + iss1.str();
  trace->print(src,cl,"chargerXML",message);
  
  //chargement des segments
  TiXmlElement* pWP = handleModule.FirstChildElement("wayPoint").Element();
  for( pWP; pWP; pWP=pWP->NextSiblingElement("wayPoint"))
  {
    WayPoint * wp = new WayPoint(pWP);
    listeWP.push_back(wp);
  }
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
}

void AlgoPC::connectModules(void)
{
  message = "debut";
  trace->print(src,cl,"connectModules",message);
  
  string nomModule;
  vectorString::iterator it;
  it = tabRelation.begin();
  if (tabRelation.size() == 1)
  {
    nomModule = *it;
    comSerie = (ComSeriePC_PIC *)systPere->getPtrModule(nomModule);
    message = "connection à :" + nomModule;
    trace->print(src,cl,"connectModules",message);
  }
  
  message = "fin";
  trace->print(src,cl,"connectModules",message);
}


