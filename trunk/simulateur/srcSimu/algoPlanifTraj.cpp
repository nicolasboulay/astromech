#include "math.h"
#include "geom2D.h"
#include "obstacles.h"
#include "algoPlanifTraj.h"
#include "tinyxml.h"
#include "trace.h"

#define DIST_PASSAGE_OBSTACLE 0.01
#define GARDE_RELIER_WP_OBSTACLE 0.001

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

  // Initialiser les bordures du terrain.
  this->segBorduresTerrain.clear();
  Point2D *pt1 = new Point2D(rayonRobot_m,                       rayonRobot_m);
  Point2D *pt2 = new Point2D(in_largeurTerrain_m - rayonRobot_m, rayonRobot_m);
  Point2D *pt3 = new Point2D(rayonRobot_m,                       in_longueurTerrain_m - rayonRobot_m);
  Point2D *pt4 = new Point2D(in_largeurTerrain_m - rayonRobot_m, in_longueurTerrain_m - rayonRobot_m);
  Segment2D *seg12 = new Segment2D(pt1, pt2);
  Segment2D *seg13 = new Segment2D(pt1, pt3);
  Segment2D *seg34 = new Segment2D(pt3, pt4);
  Segment2D *seg24 = new Segment2D(pt2, pt4);
  this->segBorduresTerrain.push_back(seg12);
  this->segBorduresTerrain.push_back(seg13);
  this->segBorduresTerrain.push_back(seg34);
  this->segBorduresTerrain.push_back(seg24);
  
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
  
  for ( unsigned int i = 0 ; i < this->segBorduresTerrain.size() ; i++ ) {
	//printf("**** Segments2D : %d/%d\n", Segment2D::nbPresents, Segment2D::nbCrees);
	//printf("**** Points2D   : %d/%d\n", Point2D::nbPtPresents, Point2D::nbPtCrees);
	Segment2D *seg = this->segBorduresTerrain[i];
	//Point2D *pt1 = seg->pt1;
	//Point2D *pt2 = seg->pt2;
	delete(seg);
	//delete(pt1);
	//delete(pt2);
	//printf("**** Segments2D : %d/%d\n", Segment2D::nbPresents, Segment2D::nbCrees);
	//printf("**** Points2D   : %d/%d\n", Point2D::nbPtPresents, Point2D::nbPtCrees);
  }
  
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

void AlgoPlanifTrajectoire::initialiserMatrice () {
	for ( int i = 0; i < N_VERTEX ; i++ ) {
		for( int j = 0; j < N_VERTEX ; j++ ) {
			this->matrice[i][j] = MATRIX_INIT;
		}
	}
}

int AlgoPlanifTrajectoire::calculeTrajectoire(
              //IN
              vectorObstacles &tableDesObstacles,
              WayPoint * wayPointDepart, 
              WayPoint * wayPointArrivee,
              //OUT
              vectorWayPoint &tableauWPTrajectoire)
{
  message = "debut";
  trace->print(src,cl,"calculeTrajectoire",message);
   
  // Initialisation
  this->initialiserMatrice(); // matrice 
  this->listeWayPoints.clear(); // liste des waypoints
  for ( unsigned int i = 0 ; i <= tableDesObstacles.size() ; i++ ) { // waypoints sur les obstacles
    tableDesObstacles[i]->waypoints.clear();
  }
  
  // Ajout des waypoint de départ et d'arrivée
  wayPointDepart->id = 0;
  wayPointArrivee->id = 1;
  this->listeWayPoints.push_back(wayPointDepart);
  this->listeWayPoints.push_back(wayPointArrivee);
  
  Segment2D segLL, segLR, segRL, segRR;
  
  // Calcul de WPDépart - WP Arrivée
  this->CalculerWaypointWaypoint ( wayPointDepart, 1, wayPointArrivee, 1, 
		tableDesObstacles, &segLL, &segLR, &segRL, &segRR);
  
  // Step 2 : Construire le graphe à travers les obstacles et tester les collisions à la volée
  
  printf("%d waypoints sur le premier obstacle (0)\n", tableDesObstacles[0]->waypoints.size());
  
  for ( unsigned int i = 0 ; i < tableDesObstacles.size() ; i++ ) {
	this->CalculerWaypointObstacle ( wayPointDepart, 1, tableDesObstacles[i], tableDesObstacles, &segLL, &segLR, &segRL, &segRR);
  }
  
  printf("%d waypoints sur le premier obstacle\n", tableDesObstacles[0]->waypoints.size());
  
  for ( unsigned int i = 0 ; i < tableDesObstacles.size() ; i++ ) {
    for ( unsigned int j = i+1 ; j < tableDesObstacles.size() ; j++ ) {
      this->CalculerObstacleObstacle ( tableDesObstacles[i], tableDesObstacles[j], tableDesObstacles, &segLL, &segLR, &segRL, &segRR);
	}
  }
  printf("%d waypoints sur le premier obstacle\n", tableDesObstacles[0]->waypoints.size());
  
  for ( unsigned int i = 0 ; i < tableDesObstacles.size() ; i++ ) {
	this->CalculerObstacleWaypoint ( tableDesObstacles[i], wayPointArrivee, 1, tableDesObstacles, &segLL, &segLR, &segRL, &segRR);
  }
  
  printf("%d waypoints sur le premier obstacle\n", tableDesObstacles[0]->waypoints.size());
  
  printf("Relier les vertex sur les obstacles\n");
  this->relierVertexObstacles(tableDesObstacles);
  
  // Step 3 : Rechercher le plus court chemin
  
  // Step 4 : Générer la route
  
  if ( this->Dijkstra(tableauWPTrajectoire) < 0 ) {
	printf("Impossible de trouver une trajectoire\n");
  } else {
	printf("Trajectoire finie en %d waypoints\n", tableauWPTrajectoire.size());
  }
  
  message = "fin";
  trace->print(src,cl,"calculeTrajectoire",message);
  return 1;
}

int AlgoPlanifTrajectoire::CalculerWaypointWaypoint ( 
			WayPoint *wp1, 
			double rayon1, 
			WayPoint *wp2, 
			double rayon2, 
			vectorObstacles &tableDesObstacles,
			Segment2D *segLL, 
			Segment2D *segRR, 
			Segment2D *segLR, 
			Segment2D *segRL )
{

  Arc2D *arc1L = new Arc2D(wp1,  1, rayon1);
  Arc2D *arc1R = new Arc2D(wp1, -1, rayon1);
  Arc2D *arc2L = new Arc2D(wp2,  1, rayon2);
  Arc2D *arc2R = new Arc2D(wp2, -1, rayon2);
  
  // Initilialisation
  Segment2D *segVide  = new Segment2D();
  Segment2D *segUtile = new Segment2D();
  
  // Calcul des segments d'intersection
  // SegmentLL
  if ( 2 <= arc1L->CalculerSegmentsTangents( arc2L, segUtile, segVide, segVide, segVide) ) {
	if ( this->AddLegCLC(wp1, arc1L, 1, wp2, arc2L, 1, segUtile, tableDesObstacles) == 0 ) {
		segLL->copySegment(segUtile);
	}
  }
  
  // SegmentRR
  if ( 2 <= arc1R->CalculerSegmentsTangents( arc2R, segVide, segUtile, segVide, segVide) ) {
	if ( this->AddLegCLC(wp1, arc1R, -1, wp2, arc2R, -1, segUtile, tableDesObstacles) == 0 ) {
		segRR->copySegment(segUtile);
	}
  }
  
  // SegmentLR
  if ( 4 <= arc1L->CalculerSegmentsTangents( arc2R, segVide, segVide, segUtile, segVide) ) {
	if ( this->AddLegCLC(wp1, arc1L, 1, wp2, arc2R, -1, segUtile, tableDesObstacles) == 0 ) {
		segLR->copySegment(segUtile);
	}
  }
  
  // SegmentRL
  if ( 4 <= arc1R->CalculerSegmentsTangents( arc2L, segVide, segVide, segVide, segUtile) ) {
	if ( this->AddLegCLC(wp1, arc1R, -1, wp2, arc2L, 1, segUtile, tableDesObstacles) == 0 ) {
		segRL->copySegment(segUtile);
	}
  }
  
  delete(arc1L);
  delete(arc1R);
  delete(arc2L);
  delete(arc2R);
  delete(segUtile);
  delete(segVide);
  
  return 0;
  
}

int AlgoPlanifTrajectoire::CalculerObstacleObstacle ( 
			Obstacle *obs1,
			Obstacle *obs2,
			vectorObstacles &tableDesObstacles,
			Segment2D *segLL, 
			Segment2D *segRR, 
			Segment2D *segLR, 
			Segment2D *segRL )
{

  Arc2D *arc1 = new Arc2D(obs1->centreObstacle, obs1->rayonObstacle_m + this->rayonRobot_m + DIST_PASSAGE_OBSTACLE, 0, 2*M_PI);
  Arc2D *arc2 = new Arc2D(obs2->centreObstacle, obs2->rayonObstacle_m + this->rayonRobot_m + DIST_PASSAGE_OBSTACLE, 0, 2*M_PI);
  
  // Initilialisation
  Segment2D *segVide  = new Segment2D();
  Segment2D *segUtile1 = new Segment2D();
  Segment2D *segUtile2 = new Segment2D();
  
  // Calcul des segments d'intersection
  // SegmentLL et SegmentRR (en même temps)
  if ( 2 <= arc1->CalculerSegmentsTangents( arc2, segUtile1, segUtile2, segVide, segVide) ) {
	if ( this->AddLegL(segUtile1, tableDesObstacles, obs1, obs2) == 0 ) {
		segLL->copySegment(segUtile1);
	}
	if ( this->AddLegL(segUtile2, tableDesObstacles, obs1, obs2) == 0 ) {
		segRR->copySegment(segUtile2);
	}
  }
  
  // SegmentLR et segment RL en même temps
  if ( 4 <= arc1->CalculerSegmentsTangents( arc2, segVide, segVide, segUtile1, segUtile2) ) {
	if ( this->AddLegL(segUtile1, tableDesObstacles, obs1, obs2) == 0 ) {
		segLR->copySegment(segUtile1);
	}
	if ( this->AddLegL(segUtile2, tableDesObstacles, obs1, obs2) == 0 ) {
		segRL->copySegment(segUtile2);
	}
  }
  
  delete(arc1);
  delete(arc2);
  delete(segVide);
  delete(segUtile1);
  delete(segUtile2);
  
  return 0;
  
}

int AlgoPlanifTrajectoire::AddLegCLC ( WayPoint *wp1, Arc2D *arc1, int sens1,  WayPoint *wp2, Arc2D *arc2, int sens2, Segment2D *seg, vectorObstacles &tableDesObstacles) {
	// Anticollision segment
	char antiCol = this->CollisionSegment(seg, tableDesObstacles); // 0 = pas de collision
	
	// 1 : wp1
	// 2 : wp2
	// 3 : wp sur le cercle du wp1
	// 4 : wp sur le cercle du wp2
	
	// Anticollision arcs
	Arc2D *arc13;
	if ( antiCol == 0 ) {
		if ( sens1 > 0 ) { // à gauche
			arc13 = new Arc2D(arc1->pt, wp1->pt, seg->pt1, arc1->rayon);
		} else { // à droite
			arc13 = new Arc2D(arc1->pt, seg->pt1, wp1->pt, arc1->rayon);
		}
		antiCol = this->CollisionArc(arc13, tableDesObstacles); 
	}
	
	Arc2D *arc42;
	if ( antiCol == 0 ) {
		if ( sens2 > 0 ) { // à gauche
			arc42 = new Arc2D(arc2->pt, seg->pt2, wp2->pt, arc2->rayon);
		} else { // à droite
			arc42 = new Arc2D(arc2->pt, wp2->pt, seg->pt2, arc2->rayon);
		}
		antiCol = this->CollisionArc(arc42, tableDesObstacles); 
	}
	
	int returnCode;
	if ( antiCol == 0 ) {
		WayPoint *wp3 = new WayPoint(seg->pt1->x, seg->pt1->y, rad2deg(convertirAngle(seg->getAngle())), 0);
		this->AjouterWayPoint(wp3);
		this->matrice[wp1->id][wp3->id] = arc13->longueur; // mise a jour de la matrice pour l'arc wp1-wp3
		
		printf("WP3 [%d] %f %f %f\n", wp3->id, wp3->pt->x, wp3->pt->y, wp3->cap_deg);
		
		WayPoint *wp4 = new WayPoint(seg->pt2->x, seg->pt2->y, wp3->cap_deg, 0);
		this->AjouterWayPoint(wp4);
		this->matrice[wp4->id][wp2->id] = arc42->longueur; // mise a jour de la matrice pour l'arc wp4-wp2
		
		printf("WP4 [%d] %f %f %f\n", wp4->id, wp4->pt->x, wp4->pt->y, wp4->cap_deg);
		
		this->matrice[wp3->id][wp4->id] = seg->getLongueur(); // mise a jour de la matrice pour l'arc wp3-wp4
		
		returnCode = 1; // insertion OK
	} else {
		returnCode = 0; // insertion non réalisée car collision
	}
	
	delete(arc13);
	delete(arc42);
	
	return returnCode;
	
}

int AlgoPlanifTrajectoire::AddLegCL ( WayPoint *wp1, Arc2D *arc1, int sens1, Segment2D *seg, vectorObstacles &tableDesObstacles, Obstacle *obs) {
	// Anticollision segment
	char antiCol = this->CollisionSegment(seg, tableDesObstacles); // 0 = pas de collision
	
	// 1 : wp1
	// 2 : wp sur le cercle du wp1
	// 3 : wp sur l'obstacle
	
	// Anticollision arcs
	Arc2D *arc12;
	if ( antiCol == 0 ) {
		if ( sens1 > 0 ) { // à gauche
			arc12 = new Arc2D(arc1->pt, wp1->pt, seg->pt1, arc1->rayon);
		} else { // à droite
			arc12 = new Arc2D(arc1->pt, seg->pt1, wp1->pt, arc1->rayon);
		}
		antiCol = this->CollisionArc(arc12, tableDesObstacles); 
	}
	
	int returnCode;
	
	if ( antiCol == 0 ) {
		WayPoint *wp2 = new WayPoint(seg->pt1->x, seg->pt1->y, rad2deg(convertirAngle(seg->getAngle())), 0);
		WayPoint *wp3 = new WayPoint(seg->pt2->x, seg->pt2->y, wp2->cap_deg, 0);
		
		this->AjouterWayPoint(wp2);
		this->AjouterWayPoint(wp3);
		
		printf("WP2 [%d] %f %f %f\n", wp2->id, wp2->pt->x, wp2->pt->y, wp2->cap_deg);
		printf("WP3 [%d] %f %f %f\n", wp3->id, wp3->pt->x, wp3->pt->y, wp3->cap_deg);
		
		this->matrice[wp1->id][wp2->id] = arc12->longueur; // mise a jour de la matrice pour l'arc wp1-wp3
		this->matrice[wp2->id][wp3->id] = seg->getLongueur(); // mise a jour de la matrice pour l'arc wp2-wp3
		
		obs->waypoints.push_back(wp3); // On ajoute à l'obstacle ce point pour ensuite faire les liens sur l'obstacle
		
		returnCode = 1; // insertion OK
	} else {
		returnCode = 0; // insertion non réalisée car collision
	}
	
	delete(arc12);
	
	return returnCode;
	
}

int AlgoPlanifTrajectoire::AddLegLC ( WayPoint *wp2, Arc2D *arc2, int sens2, Segment2D *seg, vectorObstacles &tableDesObstacles, Obstacle *obs) {
	// Anticollision segment
	char antiCol = this->CollisionSegment(seg, tableDesObstacles); // 0 = pas de collision
	
	// 1 : wp1 : sur l'obstacle
	// 2 : wp2 : destination
	// 3 : wp sur le cercle du wp2 - on a donc wp1 - wp3 et wp3 - wp2
	
	// Anticollision arcs
	Arc2D *arc32;
	if ( antiCol == 0 ) {
		if ( sens2 > 0 ) { // à gauche
			arc32 = new Arc2D(arc2->pt, seg->pt2, wp2->pt, arc2->rayon);
		} else { // à droite
			arc32 = new Arc2D(arc2->pt, wp2->pt, seg->pt2, arc2->rayon);
		}
		antiCol = this->CollisionArc(arc32, tableDesObstacles); 
	}
	
	int returnCode;
	
	if ( antiCol == 0 ) {
		WayPoint *wp1 = new WayPoint(seg->pt1->x, seg->pt1->y, rad2deg(convertirAngle(seg->getAngle())), 0);
		WayPoint *wp3 = new WayPoint(seg->pt2->x, seg->pt2->y, wp1->cap_deg, 0);
		
		this->AjouterWayPoint(wp1);
		this->AjouterWayPoint(wp3);
	
		printf("WP1 [%d] %f %f %f\n", wp1->id, wp1->pt->x, wp1->pt->y, wp1->cap_deg);
		printf("WP3 [%d] %f %f %f\n", wp3->id, wp3->pt->x, wp3->pt->y, wp3->cap_deg);
		
		this->matrice[wp3->id][wp2->id] = arc32->longueur; // mise a jour de la matrice pour l'arc wp4-wp2
		this->matrice[wp1->id][wp3->id] = seg->getLongueur(); // mise a jour de la matrice pour l'arc wp3-wp4
		
		obs->waypoints.push_back(wp1); // On ajoute à l'obstacle
		
		returnCode = 1; // insertion OK
	} else {
		returnCode = 0; // insertion non réalisée car collision
	}
	
	delete(arc32);
	
	return returnCode;
	
}


int AlgoPlanifTrajectoire::AddLegL ( Segment2D *seg, vectorObstacles &tableDesObstacles, Obstacle *obs1, Obstacle *obs2) {
	// Anticollision segment
	char antiCol = this->CollisionSegment(seg, tableDesObstacles); // 0 = pas de collision
	
	// 1 : wp sur seg sens positif, premier point
	// 2 : wp sur seg sens positif, deuxième point
	// 3 : wp sur seg sens négatif, premier point
	// 4 : wp sur seg sens négatif, deuxième point
	
	// Anticollision arcs
	if ( antiCol == 0 ) {
	
		WayPoint *wp1 = new WayPoint(seg->pt1->x, seg->pt1->y, rad2deg(convertirAngle(seg->getAngle())), 0);
		WayPoint *wp2 = new WayPoint(seg->pt2->x, seg->pt2->y, wp1->cap_deg, 0);
		WayPoint *wp3 = new WayPoint(seg->pt2->x, seg->pt2->y, rad2deg(convertirAngle(seg->getAngle() + M_PI)), 0);
		WayPoint *wp4 = new WayPoint(seg->pt1->x, seg->pt1->y, wp3->cap_deg, 0);
	
		this->AjouterWayPoint(wp1);
		this->AjouterWayPoint(wp2);
		this->AjouterWayPoint(wp3);
		this->AjouterWayPoint(wp4);
	
		this->matrice[wp1->id][wp2->id] = seg->getLongueur();
		this->matrice[wp3->id][wp4->id] = seg->getLongueur();
		
		obs1->waypoints.push_back(wp1);
		obs1->waypoints.push_back(wp4);
		obs2->waypoints.push_back(wp2);
		obs2->waypoints.push_back(wp3);
		
		return 1; // insertion OK
	} else {
		return 0; // insertion non réalisée car collision
	}
}

void AlgoPlanifTrajectoire::AjouterWayPoint( WayPoint *wp ) {
	wp->id = this->listeWayPoints.size();
	this->listeWayPoints.push_back(wp);
}

int AlgoPlanifTrajectoire::CalculerWaypointObstacle ( WayPoint *wp1, double rayon1, Obstacle *obs, vectorObstacles &tableDesObstacles, Segment2D *segLL, Segment2D *segRR, Segment2D *segLR, Segment2D *segRL ) {

  Arc2D *arc1L = new Arc2D(wp1,  1, rayon1);
  Arc2D *arc1R = new Arc2D(wp1, -1, rayon1);
  Arc2D *arc2  = new Arc2D(obs->centreObstacle, obs->rayonObstacle_m + this->rayonRobot_m + DIST_PASSAGE_OBSTACLE, 0, 2*M_PI);
  
  // Initilialisation
  Segment2D *segVide  = new Segment2D(new Point2D(-1,-1), new Point2D(-1,-1));
  Segment2D *segUtile = new Segment2D(new Point2D(-1,-1), new Point2D(-1,-1));
  
  // Calcul des segments d'intersection
  // SegmentLL
  if ( 2 <= arc1L->CalculerSegmentsTangents( arc2, segUtile, segVide, segVide, segVide) ) {
	if ( this->AddLegCL(wp1, arc1L, 1, segUtile, tableDesObstacles, obs) == 0 ) {
		segLL->copySegment(segUtile);
	}
  }
  
  // SegmentRR
  if ( 2 <= arc1R->CalculerSegmentsTangents( arc2, segVide, segUtile, segVide, segVide) ) {
	if ( this->AddLegCL(wp1, arc1R, -1, segUtile, tableDesObstacles, obs) == 0 ) {
		segRR->copySegment(segUtile);
	}
  }
  
  // SegmentLR
  if ( 4 <= arc1L->CalculerSegmentsTangents( arc2, segVide, segVide, segUtile, segVide) ) {
	if ( this->AddLegCL(wp1, arc1L, 1, segUtile, tableDesObstacles, obs) == 0 ) {
		segLR->copySegment(segUtile);
	}
  }
  
  // SegmentRL
  if ( 4 <= arc1R->CalculerSegmentsTangents( arc2, segVide, segVide, segVide, segUtile) ) {
	if ( this->AddLegCL(wp1, arc1R, -1, segUtile, tableDesObstacles, obs) == 0 ) {
		segRL->copySegment(segUtile);
	}
  }
  
  delete(arc1L);
  delete(arc1R);
  delete(arc2);
  delete(segVide);
  delete(segUtile);
  
  return 0;
  
}

int AlgoPlanifTrajectoire::CalculerObstacleWaypoint ( Obstacle *obs, WayPoint *wp2, double rayon2, vectorObstacles &tableDesObstacles, Segment2D *segLL, Segment2D *segRR, Segment2D *segLR, Segment2D *segRL ) {

  Arc2D *arc1 = new Arc2D(obs->centreObstacle, obs->rayonObstacle_m + this->rayonRobot_m + DIST_PASSAGE_OBSTACLE, 0, 2*M_PI);
  Arc2D *arc2L = new Arc2D(wp2,  1, rayon2);
  Arc2D *arc2R = new Arc2D(wp2, -1, rayon2);
  
  // Initilialisation
  Segment2D *segVide  = new Segment2D(new Point2D(-1,-1), new Point2D(-1,-1));
  Segment2D *segUtile = new Segment2D(new Point2D(-1,-1), new Point2D(-1,-1));
  
  // Calcul des segments d'intersection
  // SegmentLL
  if ( 2 <= arc1->CalculerSegmentsTangents( arc2L, segUtile, segVide, segVide, segVide) ) {
	if ( this->AddLegLC(wp2, arc2L, 1, segUtile, tableDesObstacles, obs) == 0 ) {
		segLL->copySegment(segUtile);
	}
  }
  
  // SegmentRR
  if ( 2 <= arc1->CalculerSegmentsTangents( arc2R, segVide, segUtile, segVide, segVide) ) {
	if ( this->AddLegLC(wp2, arc2R, -1, segUtile, tableDesObstacles, obs) == 0 ) {
		segRR->copySegment(segUtile);
	}
  }
  
  // SegmentLR
  if ( 4 <= arc1->CalculerSegmentsTangents( arc2R, segVide, segVide, segUtile, segVide) ) {
	if ( this->AddLegLC(wp2, arc2R, -1, segUtile, tableDesObstacles, obs) == 0 ) {
		segLR->copySegment(segUtile);
	}
  }
  
  // SegmentRL
  if ( 4 <= arc1->CalculerSegmentsTangents( arc2L, segVide, segVide, segVide, segUtile) ) {
	if ( this->AddLegLC(wp2, arc2L, 1, segUtile, tableDesObstacles, obs) == 0 ) {
		segRL->copySegment(segUtile);
	}
  }
  
  delete(arc1);
  delete(arc2L);
  delete(arc2R);
  delete(segVide);
  delete(segUtile);
  
  return 0;
  
}

// Teste la collision entre un arc et les contours du terrain et les obstacles
int AlgoPlanifTrajectoire::CollisionArc ( Arc2D *arc, vectorObstacles &tableDesObstacles ) {
	
	printf("AntiCol Arc [%f %f] %f [%f %f]deg\n", arc->pt->x, arc->pt->y, arc->rayon, arc->angleA*180/M_PI, arc->angleB*180/M_PI);
	
	Point2D *pt1 = new Point2D(0,0);
	Point2D *pt2 = new Point2D(0,0);
		
	// Collision avec les bordures du terrain
	for ( unsigned int i = 0; i < this->segBorduresTerrain.size() ; i++ ) {
		if ( arc->TestIntersectionSegment(this->segBorduresTerrain[i], pt1, pt2) > 0 ) {
			delete(pt1);
			delete(pt2);
			return 1;
		} 
	}
	
	// Collision avec les obstacles
	for ( unsigned int i = 0 ; i < tableDesObstacles.size() ; i++ ) {
		Arc2D *arc2 = new Arc2D(tableDesObstacles[i]->centreObstacle, tableDesObstacles[i]->rayonObstacle_m + this->rayonRobot_m, 0, 2*M_PI);
		printf("- anticol avec obstacle %d [%f %f %f]\n", i, arc2->pt->x, arc2->pt->y, arc2->rayon );
		if ( arc->TestIntersectionArc(arc2, pt1, pt2) > 0 ) {
			printf("COLLISION de l'arc avec un obstacle\n");
			delete(pt1);
			delete(pt2);
			delete(arc2);
			return 1;
		}
		delete(arc2);
	}
	
	delete(pt1);
	delete(pt2);
	return 0; // Pas de collision
	
}

// Teste la collision entre un segment et les contours du terrain et les obstacles
int AlgoPlanifTrajectoire::CollisionSegment ( Segment2D *seg, vectorObstacles &tableDesObstacles ) {

	Point2D *pt = new Point2D(0,0);

	printf("AntiCol du segment [%f %f] [%f %f]\n", seg->pt1->x, seg->pt1->y, seg->pt2->x, seg->pt2->y);
	// Collision avec les bordures du terrain
	for ( unsigned int i = 0; i < this->segBorduresTerrain.size() ; i++ ) {
		printf("- avec segment [%f %f] [%f %f] ", this->segBorduresTerrain[i]->pt1->x, this->segBorduresTerrain[i]->pt1->y, this->segBorduresTerrain[i]->pt2->x, this->segBorduresTerrain[i]->pt2->y);
		if ( seg->TestIntersection(this->segBorduresTerrain[i], 0, pt) == 1 ) {
			printf("COL !\n");
			delete(pt);
			return 1;
		} else {
			printf("nocol\n");
		}
	}
	
	Point2D *pt2 = new Point2D(0,0);
	
	// Collision avec les obstacles
	for ( unsigned int i = 0 ; i < tableDesObstacles.size() ; i++ ) {
		
		Arc2D *arc = new Arc2D(tableDesObstacles[i]->centreObstacle, tableDesObstacles[i]->rayonObstacle_m + this->rayonRobot_m, 0, 2*M_PI);
		printf("- anticol avec obstacle %d [%f %f %f]\n", i, arc->pt->x, arc->pt->y, arc->rayon );
		if ( arc->TestIntersectionSegment(seg, pt, pt2) > 0 ) {
			delete(pt);
			delete(pt2);
			delete(arc);
			printf("COL du segment avec un obstacle\n");
			return 1;
		}
		delete(arc);
		
	}
	
	delete(pt);
	delete(pt2);
	printf("Segment Anticol : aucune collision\n");
	return 0; // Pas de collision
	
	
}

// On va relier entre eux les vertex situés autour du même obstacle
// Pour éviter d'avoir des trajectoires ayant trop de waypoint, on ajoutera une petite garde supplémentaire GARDE_RELIER_WP_OBSTACLE 
// de qqs cm pour forcer la réduction du nombre de WayPoints.
int AlgoPlanifTrajectoire::relierVertexObstacles ( vectorObstacles &tableDesObstacles ) {
	for ( unsigned int iObstacle = 0 ; iObstacle < tableDesObstacles.size() ; iObstacle++ ) {
		printf("Obstacle %d\n", iObstacle);
		Obstacle *obs = tableDesObstacles[iObstacle];
		for ( unsigned int iWP1 = 0 ; iWP1 < obs->waypoints.size() ; iWP1++ ) {
			WayPoint *WP1 = obs->waypoints[iWP1];
			int sensWP1 = WP1->cotePoint(obs->centreObstacle);
			double angleWP1 = convertirAngle(deg2rad(WP1->cap_deg));
			double distWP1 = calculerDistance(obs->centreObstacle->x, obs->centreObstacle->y, WP1->pt->x, WP1->pt->y);
			for ( unsigned int iWP2 = iWP1+1 ; iWP2 < obs->waypoints.size() ; iWP2++ ) {
				// On a deux WP distincts, on regarde s'ils ont le même sens
				WayPoint *WP2 = obs->waypoints[iWP2];
				int sensWP2 = WP2->cotePoint(obs->centreObstacle);
				double angleWP2 = convertirAngle(deg2rad(WP2->cap_deg));
				double distWP2 = calculerDistance(obs->centreObstacle->x, obs->centreObstacle->y, WP2->pt->x, WP2->pt->y);
				printf("Relier %d [%f] à %d [%f] ? ", WP1->id, distWP1, WP2->id, distWP2);
				if ( sensWP1 == sensWP2 ) {
					// Ils sont dans le même sens, donc on calcule leurs écarts
					// Si les deux sont sur le côté doit
					printf("OUI\n");
					if ( sensWP1 == COTE_DROIT ) {
						this->matrice[WP1->id][WP2->id] = DistanceAngulaire(angleWP2, angleWP1) * obs->rayonObstacle_m + GARDE_RELIER_WP_OBSTACLE;
						this->matrice[WP2->id][WP1->id] = DistanceAngulaire(angleWP1, angleWP2) * obs->rayonObstacle_m + GARDE_RELIER_WP_OBSTACLE;
					} else {
						this->matrice[WP1->id][WP2->id] = DistanceAngulaire(angleWP1, angleWP2) * obs->rayonObstacle_m + GARDE_RELIER_WP_OBSTACLE;
						this->matrice[WP2->id][WP1->id] = DistanceAngulaire(angleWP2, angleWP1) * obs->rayonObstacle_m + GARDE_RELIER_WP_OBSTACLE;
					}	
				} else {
					// on ne fait rien, ils ne sont pas dans le même sens
					printf("non\n");
				}
			}
		}
	}
	return 0;
}

int AlgoPlanifTrajectoire::Dijkstra ( vectorWayPoint &routeOptimale ) {
	//int idWPDepart = 0;
	//int idWPArrivee = 1;
	int nVertex = this->listeWayPoints.size();
	
	// déclarations
	int predecesseur[N_VERTEX]; // meilleur prédécesseur du vertex
	char relaxe[N_VERTEX]; // le vertex a été relaxé
	double distances[N_VERTEX];
	
	// initialisation
	for ( int i = 0 ; i < nVertex ; i++ ) {
		predecesseur[i] = -1;
		relaxe[i] = 0;
		distances[i] = 1e12; // infini
	}
	
	distances[0] = 0;
	
	// Construction de l'arbre
	printf("Construction de l'arbre\n");
	while( 1 == 1) {
	
		// Recherche de l'élément ayant la plus petite distance et n'étant pas relaxé
		int iVertex = -1;
		double iVertexDistance = -1;
		for ( int i = 0 ; i < nVertex ; i++ ) {
			if ( relaxe[i] == 0 && (iVertexDistance < 0 || distances[i] < iVertexDistance) ) {
				iVertex = i; // on a trouvé un nouveau minimum
				iVertexDistance = distances[i];
			}
		}
		
		// Si aucun élément n'est trouvé, alors la relaxe est terminée
		if ( iVertex == -1 ) {
			break;
		}
		
		printf("Le plus petit est %d avec une distance de %f\n", iVertex, iVertexDistance);
		
		// On s'occupe du point trouvé
		relaxe[iVertex] = 1;
		
		// On relaxe chacun de ses voisins
		for ( int i = 0 ; i < nVertex ; i++ ) {
			if ( this->matrice[iVertex][i] > 0 && distances[i] > distances[iVertex] + this->matrice[iVertex][i] ) {
				distances[i] = distances[iVertex] + this->matrice[iVertex][i];
				predecesseur[i] = iVertex;
				printf("%d est le nouveau predecesseur de %d avec une distance de %f\n", iVertex, i, distances[i]);
			}
		}
		
	}
	
	// Extraction de la route optimale
	printf("Extraction de la route optimale\n");
	vectorWayPoint routeInversee;
	int iVertex = 1;
	printf("La route optimale mesure %f\n", distances[1]);
	while ( iVertex != 0 ) {
		printf("Vertex %d\n", iVertex);
		routeInversee.push_back(this->listeWayPoints[iVertex]);
		iVertex = predecesseur[iVertex];
		if ( iVertex == -1 ) {
			printf("OOoops, solution non trouvable\n");
			return -1;
		}
	}
	
	// Inversion du résultat
	for ( int i = routeInversee.size()-1 ; i >= 0 ; i-- ) {
		routeOptimale.push_back(routeInversee[i]);
	}
	
	printf("Dijkstra fini %d\n", routeOptimale.size());
	
	return 0;
	
}
