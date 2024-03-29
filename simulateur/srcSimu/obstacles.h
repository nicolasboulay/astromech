#ifndef ObstacleH
#define ObstacleH

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
class WayPoint;
class TiXmlElement;

typedef vector<Segment2D *> vectorSegment2D;
typedef vector<Point2D *> vectorpoint2D;
typedef vector<int> vectorInt;
typedef vector<WayPoint *> vectorWayPoint;

class Obstacle
{
  protected: //accessible uniquement par h�ritage

  public : // accessible partout
    Point2D * centreObstacle;
    double rayonObstacle_m;
	vectorWayPoint waypoints;
	//vectorWayPoint waypointsR;
    //Obstacles(string fichierObstacles, Trace * t);
    Obstacle(double xCentreObstacle_m,
              double yCentreObstacle_m,
	      double rayonObstacle_m);
    Obstacle(TiXmlElement* pObs);
    ~Obstacle(void);
    int chargerXML(TiXmlElement* pObs);
    /*Obstacles* dilateObstacle(double dilatation_m);
    Obstacles* autoFusion(void);
    int FusionneAvecObstacle(Obstacles* obs);
    void dumpObstacle(void);
    void dumpObstacleFormatExcel(void);
    bool pointInPolygon(Point2D *ptATester); */

};
#endif
