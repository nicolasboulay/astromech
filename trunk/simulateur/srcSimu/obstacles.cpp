#include "obstacles.h"
#include "tinyxml.h"
#include "geom2D.h"
#include "trace.h"

using namespace std;


Obstacles::Obstacles(string fichierObstacles,Trace * t)
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
}

Obstacles::~Obstacles(void)
{
  message = "debut";
  trace->print(src,cl,"~Obstacles",message);
  vectorSegment2D::iterator it;
  for (it = tabSegments.begin();it!=tabSegments.end();++it)
  {
    delete(*it);
  }
  message = "fin";
  trace->print(src,cl,"~Obstacles",message);
}

int Obstacles::chargerXML(TiXmlDocument & doc)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  TiXmlHandle docHandle( &doc );
  
  //chargement des segments
  TiXmlElement* pSeg = docHandle.FirstChildElement("Segment2D").Element();
  for( pSeg; pSeg; pSeg=pSeg->NextSiblingElement("Segment2D"))
  {
    Segment2D * s2D = new Segment2D(pSeg);
    message = "segment";
    trace->print(src,cl,"chargerXML",message);
    tabSegments.push_back(s2D);
  }
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  return 1;
}
