#include "obstacles.h"
#include "tinyxml.h"
#include "geom2D.h"
#include "conversion.h"

using namespace std;


/*Obstacles::Obstacles(string fichierObstacles,Trace * t)
{
  trace = t;
  src = "obstacles.cpp";
  cl = "Obstacles";
  message = "debut";
  trace->print(src,cl,"Obstacles",message);
  TiXmlDocument doc( fichierObstacles );
  if(!doc.LoadFile())
  {
    message = "Problème doc.LoadFile() "+ fichierObstacles;
    trace->print(src,cl,"Obstacles",message);
    exit(1);   
  }
  else
  {
    if(!chargerXML(doc))
    {
      message = "Problème après appel chargerXML ";
      trace->print(src,cl,"Obstacles",message);
      exit(1);
    }
  }
  message = "fin";
  trace->print(src,cl,"Obstacles",message);
}*/
Obstacle::Obstacle(double xCentreObstacle_m,
                     double yCentreObstacle_m,
	             double in_rayonObstacle_m)
{
  centreObstacle = new Point2D(xCentreObstacle_m,yCentreObstacle_m);
  rayonObstacle_m = in_rayonObstacle_m;
}
Obstacle::Obstacle(TiXmlElement* pObs)
{
  if(!chargerXML(pObs))
  {
    cout<<"Problème après appel chargerXML "<<endl;
    exit(1);
  }
}

Obstacle::~Obstacle(void)
{
  delete centreObstacle;
}

int Obstacle::chargerXML(TiXmlElement* pObs)
{  
  TiXmlHandle handleObstacle(0);
  handleObstacle = TiXmlHandle(pObs);
/*  Point2D *pt1;
  Point2D *pt2;
  vectorpoint2D tabPointObstacle;
  
  //chargement des points
  TiXmlElement* pPt = handleObstacle.FirstChildElement("Point2D").Element();

  for( pPt; pPt; pPt=pPt->NextSiblingElement("Point2D"))
  {
    pt1 = new Point2D(pPt);
    tabPointObstacle.push_back(pt1);
  }
  
  //création des segments à partir des points
  for (int indicePt=0;indicePt<tabPointObstacle.size()-1;indicePt++)
  {
    pt2 = new Point2D(tabPointObstacle[indicePt+1]->x,tabPointObstacle[indicePt+1]->y);
    Segment2D *s2D = new Segment2D(tabPointObstacle[indicePt],pt2);
    tabSegments.push_back(s2D);
  }
  pt2 = new Point2D(tabPointObstacle[0]->x,tabPointObstacle[0]->y);
  Segment2D *s2D = new Segment2D(tabPointObstacle[tabPointObstacle.size()-1],pt2);
  tabSegments.push_back(s2D);
  
  message = "fin";
  trace->print(src,cl,"chargerXML",message);*/
  return 1;
}

