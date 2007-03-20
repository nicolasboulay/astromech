#include "algoPlanifTraj.h"
#include "tinyxml.h"
#include "geom2D.h"
#include "obstacles.h"
#include "trace.h"

using namespace std;


AlgoPlanifTrajectoire::AlgoPlanifTrajectoire(double in_accelerationNormaleMax_m_par_s2,
			                     double in_longueurTerrain_m,
			                     double in_largeurTerrain_m,
			                     double in_rayonRobot_m,
					     Trace * t)
{
  trace = t;
  src = "algoPlanifTraj.cpp";
  cl = "AlgoPlanifTrajectoire";
  message = "debut";
  trace->print(src,cl,"AlgoPlanifTrajectoire",message);
  
  accelerationNormaleMax_m_par_s2 = in_accelerationNormaleMax_m_par_s2;
  longueurTerrain_m = in_longueurTerrain_m;
  largeurTerrain_m = in_largeurTerrain_m;
  rayonRobot_m = in_rayonRobot_m;
/*  TiXmlDocument doc( fichierObstacles );
  if(!doc.LoadFile())
  {
    message = "Problème doc.LoadFile() "+ fichierObstacles;
    trace->print(src,cl,"AlgoPlanifTrajectoire",message);
    exit(1);   
  }
  else
  {
    if(!chargerXML(doc))
    {
      message = "Problème après appel chargerXML ";
      trace->print(src,cl,"AlgoPlanifTrajectoire",message);
      exit(1);
    }
  }*/
  message = "fin";
  trace->print(src,cl,"AlgoPlanifTrajectoire",message);
}

AlgoPlanifTrajectoire::~AlgoPlanifTrajectoire(void)
{
  message = "debut";
  trace->print(src,cl,"~AlgoPlanifTrajectoire",message);
  message = "fin";
  trace->print(src,cl,"~AlgoPlanifTrajectoire",message);
}

int AlgoPlanifTrajectoire::chargerXML(TiXmlDocument & doc)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
/*  TiXmlHandle docHandle( &doc );
  
  //chargement des obstacles
  TiXmlElement* pObst = docHandle.FirstChildElement("Obstacle").Element();
  for( pObst; pObst; pObst=pObst->NextSiblingElement("Obstacle"))
  {
    Obstacles * obs = new Obstacles(pObst,trace);
    message = "nouvel Obstacle";
    trace->print(src,cl,"chargerXML",message);
    tabObstaclesStatiques.push_back(obs);
  }
  message = "fin";
  trace->print(src,cl,"chargerXML",message);*/
  return 1;
}

int AlgoPlanifTrajectoire::calculeTrajectoire(//IN
                           vectorObstacles tableDesObstacles,
                           WayPoint & wayPointDepart,
			   WayPoint & wayPointArrivee,
			   //OUT
			   vectorWayPoint tableauWPTrajectoire)
{
  message = "debut";
  trace->print(src,cl,"calculeTrajectoire",message);
  
  message = "fin";
  trace->print(src,cl,"calculeTrajectoire",message);
  return 1;
}
