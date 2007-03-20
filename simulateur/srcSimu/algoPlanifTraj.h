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
			   
                           
};
#endif
