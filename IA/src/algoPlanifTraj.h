#ifndef AlgoPlanifTrajectoireH
#define AlgoPlanifTrajectoireH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

//class TiXmlDocument;
//class Trace;
class Obstacle;
class WayPoint;
class Segment2D;
class Arc2D;

typedef vector<Obstacle *> vectorObstacles;
typedef vector<WayPoint *> vectorWayPoint;
typedef vector<Segment2D *> vectorSegment2D;

#define N_VERTEX 100
#define MATRIX_INIT -1

typedef double Matrix[N_VERTEX][N_VERTEX];

class AlgoPlanifTrajectoire
{
  protected: //accessible uniquement par héritage
//    Trace * trace;
    string src;
    string cl;
    string message;
    double accelerationNormaleMax_m_par_s2;
    
  public : // accessible partout
    double longueurTerrain_m;
    double largeurTerrain_m;
    double rayonRobot_m;
    vectorObstacles tableDesObstacles;
    vectorWayPoint listeWayPoints;
    Matrix matrice;
    vectorSegment2D segBorduresTerrain;
	
    AlgoPlanifTrajectoire(double accelerationNormaleMax_m_par_s2,
			  double longueurTerrain_m,
			  double largeurTerrain_m,
			  double rayonRobot_m//,
//			  Trace * t
			  );
    ~AlgoPlanifTrajectoire(void);
//    int chargerXML(TiXmlDocument & doc);
    int calculeTrajectoire(//IN
                           vectorObstacles &tableDesObstacles,
                           WayPoint * wayPointDepart,
                           WayPoint * wayPointArrivee,
                           //OUT
                             vectorWayPoint &tableauWPTrajectoire);
    
	void initialiserMatrice ();
   	int CalculerWaypointWaypoint ( WayPoint *wp1, double rayon1, WayPoint *wp2, double rayon2, vectorObstacles &tableDesObstacles, Segment2D *segLL, Segment2D *segRR, Segment2D *segLR, Segment2D *segRL );
	int CalculerWaypointObstacle ( WayPoint *wp1, double rayon1, Obstacle *obs,                vectorObstacles &tableDesObstacles, Segment2D *segLL, Segment2D *segRR, Segment2D *segLR, Segment2D *segRL );
	int CalculerObstacleWaypoint ( Obstacle *obs, WayPoint *wp2, double rayon2,                vectorObstacles &tableDesObstacles, Segment2D *segLL, Segment2D *segRR, Segment2D *segLR, Segment2D *segRL );
	int CalculerObstacleObstacle ( Obstacle *obs1, Obstacle *obs2,                             vectorObstacles &tableDesObstacles, Segment2D *segLL, Segment2D *segRR, Segment2D *segLR, Segment2D *segRL );

	int AddLegCLC ( WayPoint *wp1, Arc2D *arc1, int sens1, WayPoint *wp2, Arc2D *arc2, int sens2, Segment2D *seg, vectorObstacles &tableDesObstacles);
	int AddLegCL ( WayPoint *wp1, Arc2D *arc1, int sens1, Segment2D *seg, vectorObstacles &tableDesObstacles, Obstacle *obs);
	int AddLegLC ( WayPoint *wp1, Arc2D *arc1, int sens1, Segment2D *seg, vectorObstacles &tableDesObstacles, Obstacle *obs);
	int AddLegL ( Segment2D *seg, vectorObstacles &ableDesObstacles, Obstacle *obs1, Obstacle *obs2);

	void AjouterWayPoint(WayPoint *wp);
	
	int CollisionArc ( Arc2D *arc, vectorObstacles &tableDesObstacles );
	int CollisionSegment ( Segment2D *seg, vectorObstacles &tableDesObstacles );
	int relierVertexObstacles( vectorObstacles &tableDesObstacles );
	int Dijkstra ( vectorWayPoint &v );
};
#endif
