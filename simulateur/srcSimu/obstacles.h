#ifndef ObstaclesH
#define ObstaclesH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

class TiXmlDocument;
class Trace;
class Segment2D;
class Point2D;
class TiXmlElement;

typedef vector<Segment2D *> vectorSegment2D;
typedef vector<Point2D *> vectorpoint2D;
typedef vector<int> vectorInt;

class Obstacles
{
  protected: //accessible uniquement par héritage
    Trace * trace;
    string src;
    string cl;
    string message;
    vectorSegment2D tabSegments;
    
  public : // accessible partout
    //Obstacles(string fichierObstacles, Trace * t);
    Obstacles(vectorSegment2D tabSegments, Trace * t);
    Obstacles(TiXmlElement* pObs, Trace * t);
    ~Obstacles(void);
    int chargerXML(TiXmlElement* pObs);
    Obstacles* dilateObstacle(double dilatation_m);
    Obstacles* autoFusion(void);
    int FusionneAvecObstacle(Obstacles* obs);
    void dumpObstacle(void);
    void dumpObstacleFormatExcel(void);
    bool pointInPolygon(Point2D *ptATester); 

};
#endif
