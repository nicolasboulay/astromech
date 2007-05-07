#include "geom2D.h"
#include "algoPlanifTraj.h"
#include "tinyxml.h"
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

int AlgoPlanifTrajectoire::calculeTrajectoire(
              //IN
              vectorObstacles tableDesObstacles,
              WayPoint & wayPointDepart, 
              WayPoint & wayPointArrivee,
              //OUT
              vectorWayPoint tableauWPTrajectoire)
{
  message = "debut";
  trace->print(src,cl,"calculeTrajectoire",message);
  // Step 1 : Tester trajectoire directe
  // Step 1.1 : Construction des deux cercles de chaque WP
  Arc2D *arc;
  Arc2D *arcDepartL = new Arc2D(&wayPointDepart, 1, 0.2);
  Arc2D *arcDepartR = new Arc2D(&wayPointDepart, -1, 0.2);
  Arc2D *arcArriveeL = new Arc2D(&wayPointArrivee, 1, 0.2);
  Arc2D *arcArriveeR = new Arc2D(&wayPointArrivee, -1, 0.2);
  
  arc = arcDepartL;  printf("Arc Depart L de centre [%f %f] de rayon %f\n", arc->pt->x, arc->pt->y, arc->rayon);
  arc = arcDepartR;  printf("Arc Depart R de centre [%f %f] de rayon %f\n", arc->pt->x, arc->pt->y, arc->rayon);
  arc = arcArriveeL; printf("Arc Arrive L de centre [%f %f] de rayon %f\n", arc->pt->x, arc->pt->y, arc->rayon);
  arc = arcArriveeR; printf("Arc Arrive R de centre [%f %f] de rayon %f\n", arc->pt->x, arc->pt->y, arc->rayon);
  
  
  
  // Step 2 : Construire le graphe à travers les obstacles et tester les collisions à la volée
  
  // Step 3 : Rechercher le plus court chemin
  
  // Step 4 : Générer la route
  
  message = "fin";
  trace->print(src,cl,"calculeTrajectoire",message);
  return 1;
}

int AlgoPlanifTrajectoire::CalculerWaypointWaypoint ( WayPoint *wp1, double rayon1, WayPoint *wp2, double rayon2, Segment2D *segLL, Segment2D *segRR, Segment2D *segLR, Segment2D *segRL ) {

  Arc2D *arc1L = new Arc2D(wp1,  1, rayon1);
  Arc2D *arc1R = new Arc2D(wp1, -1, rayon1);
  Arc2D *arc2L = new Arc2D(wp2,  1, rayon2);
  Arc2D *arc2R = new Arc2D(wp2, -1, rayon2);
  
  // Initilialisation
  Segment2D *segVide  = new Segment2D(new Point2D(-1,-1), new Point2D(-1,-1));
  Segment2D *segUtile = new Segment2D(new Point2D(-1,-1), new Point2D(-1,-1));
  
  // SegmentLL
  if ( 2 <= arc1L->CalculerSegmentsTangents( arc2L, segUtile, segVide, segVide, segVide) ) {
    segLL->copySegment(segUtile);
  }
  
  // SegmentRR
  if ( 2 <= arc1R->CalculerSegmentsTangents( arc2R, segVide, segUtile, segVide, segVide) ) {
    segRR->copySegment(segUtile);
  }
  
  // SegmentLR
  if ( 4 <= arc1L->CalculerSegmentsTangents( arc2R, segVide, segVide, segUtile, segVide) ) {
    segLR->copySegment(segUtile);
  }
  
  // SegmentRL
  if ( 4 <= arc1R->CalculerSegmentsTangents( arc2L, segVide, segVide, segVide, segUtile) ) {
    segRL->copySegment(segUtile);
  }
  
  return 0;
  
}


int AlgoPlanifTrajectoire::CalculerWaypointObstacle ( WayPoint *wp1, double rayon1, Obstacle *obs, Segment2D *segLL, Segment2D *segRR, Segment2D *segLR, Segment2D *segRL ) {
  
  //printf("Obstacle %f %f %f\n", obs->centreObstacle->x, obs->centreObstacle->y, obs->rayonObstacle_m);
  
  return 0;
}
/*
int AlgoPlanifTrajectoire::CalculerObstacleWaypoint () {
}

int AlgoPlanifTrajectoire::CalculerObstacleObstacle () {
}

int AlgoPlanifTrajectoire::CollisionLegBordures () {
}

int AlgoPlanifTrajectoire::CollisionLegObstacles ( Leg *leg, vectorObstacles vecObstacles ) {

}
*/
