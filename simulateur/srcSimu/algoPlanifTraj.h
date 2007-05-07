#ifndef AlgoPlanifTrajectoireH
#define AlgoPlanifTrajectoireH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

class TiXmlDocument;
class Trace;
class Obstacle;
class WayPoint;
class Segment2D;

typedef vector<Obstacle *> vectorObstacles;
typedef vector<WayPoint *> vectorWayPoint;

class AlgoPlanifTrajectoire
{
  protected: //accessible uniquement par héritage
    Trace * trace;
    string src;
    string cl;
    string message;
    double accelerationNormaleMax_m_par_s2;
    double longueurTerrain_m;
    double largeurTerrain_m;
    double rayonRobot_m;
    
  public : // accessible partout
    AlgoPlanifTrajectoire(double accelerationNormaleMax_m_par_s2,
			  double longueurTerrain_m,
			  double largeurTerrain_m,
			  double rayonRobot_m,
			  Trace * t);
    ~AlgoPlanifTrajectoire(void);
    int chargerXML(TiXmlDocument & doc);
    int calculeTrajectoire(//IN
                           vectorObstacles tableDesObstacles,
                           WayPoint & wayPointDepart,
                           WayPoint & wayPointArrivee,
                           //OUT
                             vectorWayPoint tableauWPTrajectoire);
    int CalculerWaypointWaypoint ( WayPoint *wp1, double rayon1, WayPoint *wp2, double rayon2, Segment2D *segLL, Segment2D *segRR, Segment2D *segLR, Segment2D *segRL );
    int CalculerWaypointObstacle ( WayPoint *wp1, double rayon1, Obstacle *obs, Segment2D *segLL, Segment2D *segRR, Segment2D *segLR, Segment2D *segRL );

};
#endif
