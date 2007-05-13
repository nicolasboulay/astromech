#include "simulation.h"
#include "systeme.h"
#include "module.h"
#include "math.h"
#include "geom2D.h"
#include "obstacles.h"
#include "tinyxml.h"
#include "trace.h"
#include "algoPlanifTraj.h"

using namespace std;


Simulation::Simulation(string fichierSimulation, Trace * t)
{
  trace = t;
  src = "simulation.cpp";
  cl = "Simulation";
  message = "debut";
  trace->print(src,cl,"Simulation",message);
  TiXmlDocument doc( fichierSimulation );
  if(!doc.LoadFile())
  {
    message = "Problème doc.LoadFile() "+ fichierSimulation;
    trace->print(src,cl,"Simulation",message);
    exit(1);  
  }
  else
  {
    if(!chargerXML(doc))
    {
      message = "Problème après appel chargerXML ";
      trace->print(src,cl,"Simulation",message);
      exit(1);
    }
    else
    {
      //rangement des modules dans les 3 tableaux
      classerModules();
    }
  }
  message = "fin";
  trace->print(src,cl,"Simulation",message);
}

Simulation::~Simulation(void)
{
  message = "debut";
  trace->print(src,cl,"~Simulation",message);
  vectorSysteme::iterator it;
  for (it = tabSysteme.begin();it!=tabSysteme.end();++it)
  {
    delete(*it);
  }
  
  delete (obstacle);
  
  message = "fin";
  tabSysteme.clear();
  trace->print(src,cl,"~Simulation",message);
}

int Simulation::getPasTemps(void)
{
  return pasTemps_ms;
}
vectorSysteme Simulation::getTabSystemes(void)
{
  return tabSysteme;
}
int Simulation::chargerXML(TiXmlDocument & doc)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  TiXmlHandle docHandle( &doc );
  
  //chargement du nombre de modules
  TiXmlElement* pElt = docHandle.FirstChild( "Simulation" ).FirstChildElement("pasTemps_ms").Element();
  if (pElt)
  {
    istringstream iss( pElt->GetText() );
    iss >> pasTemps_ms;
    message = "lecture pasTemps_ms : " + iss.str();
    trace->print(src,cl,"chargerXML",message);
  }
  else	{return 0;}
  
  pElt = docHandle.FirstChild( "Simulation" ).FirstChildElement("duree_s").Element();
  if (pElt)
  {
    istringstream iss( pElt->GetText() );
    iss >> dureeSimu_s;
    message = "lecture dureeSimu_s : " + iss.str();
    trace->print(src,cl,"chargerXML",message);
  }
  else	{return 0;}
  
  //chargement des systemes
  pElt = docHandle.FirstChild( "Simulation" ).FirstChildElement("fichierSysteme").Element();
  for( pElt; pElt; pElt=pElt->NextSiblingElement("fichierSysteme"))
  {
    string nomFichierSysteme;
    Systeme *sys;
    nomFichierSysteme = pElt->GetText();
    sys = new Systeme(nomFichierSysteme,this,trace);
    tabSysteme.push_back(sys);
  }
  
/*  pElt = docHandle.FirstChild( "Simulation" ).FirstChildElement("fichierObstacle").Element();
  string nomFichierObstacle;
  nomFichierObstacle = pElt->GetText();
  obstacles = new Obstacles(nomFichierObstacle,trace);*/
  
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
  
  return 1;
}

void Simulation::classerModules(void)
{
  message = "debut";
  trace->print(src,cl,"classerModules",message);
  
  //demande de classement aux différents systèmes
  vectorSysteme::iterator it;
  Systeme *s;
  for (it = tabSysteme.begin();it!=tabSysteme.end();++it)
  {
    s = *it;
    s->classerModules(tabSenseurs,tabAlgos,tabEffecteurs);
  }
  message = "fin";
  trace->print(src,cl,"classerModules",message);
}
void Simulation::run(void)
{
  message = "debut";
  trace->print(src,cl,"run",message);
  vectorModule::iterator it;
  Module *m;
  int finSimu_ms = dureeSimu_s*1000;
  int compteurTemps_ms;
  cout <<"**********************************************************************\n";
  cout <<"*                                                                    *\n";
  cout <<"*                        DEBUT SIMULATION                            *\n";
  cout <<"*                                                                    *\n";
  cout <<"**********************************************************************\n";
  
  // Création d'un point
  Point2D *pt = new Point2D(2, 3);
  printf("Affichage x,y [%f , %f]\n", pt->x, pt->y);
  printf("Affichage x,y [%f , %f]\n", pt->getX(), pt->getY());
  printf("Changement coordonnees\n");
  pt->setXY(3,4);
  printf("Affichage x,y [%f , %f]\n", pt->getX(), pt->getY());
  
  Point2D *pt2 = new Point2D(7,2);
  printf("Affichage pt2 x,y [%f , %f]\n", pt2->getX(), pt2->getY());
  double Dist_PT_PT2 = pt->distanceAuPoint(pt2);
  printf("Distance au point 2 : %f\n", Dist_PT_PT2);
    
  // Distance angulaire
  printf("Distance angulaire : \n");
  printf("0-30 : %f\n", DistanceAngulaire(0, 1.0001*M_PI*2)*180/M_PI);
  
  // Segment
  Segment2D *seg = new Segment2D(pt, pt2);
  printf("Segment : \n");
  Point2D *pt3 = new Point2D(7,7);
  printf("Affichage pt3 x,y [%f , %f]\n", pt3->getX(), pt3->getY());
  printf("Affichage seg x,y [%f , %f]\n", seg->pt1->getX(), seg->pt1->getY());
  printf("Affichage seg x,y [%f , %f]\n", seg->pt2->getX(), seg->pt2->getY());
  printf("Distance : %f\n", seg->DistancePoint(pt3));
  
  Point2D *pt4 = new Point2D(1,0);
  Point2D *pt5 = new Point2D(9,1);
  Segment2D *seg34 = new Segment2D(pt3, pt4);
  Segment2D *seg35 = new Segment2D(pt3, pt5);
  
  Point2D *pt6 = new Point2D(0,0);
  
  int Inter34 = seg->TestIntersection(seg34, true, pt6);
  printf("Intersection 34 : %d PtInter : %f , %f\n", Inter34, pt6->x, pt6->y);
  
  int Inter35 = seg->TestIntersection(seg35, true, pt6);
  printf("Intersection 35 : %d PtInter : %f , %f\n", Inter35, pt6->x, pt6->y);
  
  // Vecteur
  Vecteur2D *vec1 = new Vecteur2D(-5,-1);
  printf("Vec1 [%f , %f]\n", vec1->x, vec1->y);
  Vecteur2D *vec2 = new Vecteur2D(seg);
  printf("Vec2 [%f , %f]\n", vec2->x, vec2->y);
  Vecteur2D *vec3 = new Vecteur2D(pt, pt3);
  printf("Vec3 [%f , %f]\n", vec3->x, vec3->y);
  
  printf("Vec2.Vec3 : %f\n", vec2->ProduitScalaire(vec3));
  printf("Vec2xVec3 : %f\n", vec2->ProduitVectoriel(vec3));
  
  printf("Angle vec1 : %f\n", vec1->angle()*180/M_PI);
  printf("Angle vec2 : %f\n", vec2->angle()*180/M_PI);
  printf("Angle vec3 : %f\n", vec3->angle()*180/M_PI);
  
  printf("Distance Angulaire V1->V2 : %f\n", DistanceAngulaire(vec1->angle(), vec2->angle())*180/M_PI);
  printf("Distance Angulaire V2->V3 : %f\n", DistanceAngulaire(vec2->angle(), vec3->angle())*180/M_PI);
  printf("Distance Angulaire V3->V1 : %f\n", DistanceAngulaire(vec3->angle(), vec1->angle())*180/M_PI);
  printf("Distance Angulaire V3->V2 : %f\n", DistanceAngulaire(vec3->angle(), vec2->angle())*180/M_PI);
  
  printf("|Vec1| %f\n", vec1->norme());
  printf("|Vec2| %f\n", vec2->norme());
  printf("|Vec3| %f\n", vec3->norme());
  
  // Arc
  Arc2D *arc = new Arc2D(pt, 3, M_PI/2, M_PI);
  printf("Arc de centre [%f,%f] de rayon %f entre les angles %f et %f, long %f\n", arc->pt->x, arc->pt->y, arc->rayon, arc->angleA*180/M_PI, arc->angleB*180/M_PI, arc->longueur);
  printf("Segment [%f,%f] [%f,%f]\n", seg34->pt1->x, seg34->pt1->y, seg34->pt2->x, seg34->pt2->y);
  Point2D *pt7 = new Point2D(0,0);
  Point2D *pt8 = new Point2D(0,0);
  int RepInterSeg = arc->TestIntersectionSegment(seg34, pt7, pt8);
  
  printf("Inter Arc Seg34 %d aux points [%f,%f] et [%f,%f]\n", RepInterSeg, pt7->x, pt7->y, pt8->x, pt8->y);
  
  // Deux cercles
  Point2D *pt10 = new Point2D(2.5, 3);
  Point2D *pt11 = new Point2D(7.5, 1.5);
  
  Arc2D *arc10 = new Arc2D(pt10, 5, 0, 0);
  Arc2D *arc11 = new Arc2D(pt11, 1, 0, 0);
  
  Segment2D *segLL = new Segment2D(new Point2D(0,0), new Point2D(0,0));
  Segment2D *segLR = new Segment2D(new Point2D(0,0), new Point2D(0,0));
  Segment2D *segRL = new Segment2D(new Point2D(0,0), new Point2D(0,0));
  Segment2D *segRR = new Segment2D(new Point2D(0,0), new Point2D(0,0));
  
  int Rep4Seg = arc10->CalculerSegmentsTangents( arc11, segLL, segRR, segLR, segRL);
  
  printf("SegmentsTangents : %d\n", Rep4Seg);
  //Segment2D *seg = void;
  seg = segLL; printf("[%f %f] [%f %f]\n", seg->pt1->x, seg->pt1->y, seg->pt2->x, seg->pt2->y);
  seg = segRR; printf("[%f %f] [%f %f]\n", seg->pt1->x, seg->pt1->y, seg->pt2->x, seg->pt2->y);
  seg = segLR; printf("[%f %f] [%f %f]\n", seg->pt1->x, seg->pt1->y, seg->pt2->x, seg->pt2->y);
  seg = segRL; printf("[%f %f] [%f %f]\n", seg->pt1->x, seg->pt1->y, seg->pt2->x, seg->pt2->y);
  
  /*AlgoPlanifTrajectoire *apt = new AlgoPlanifTrajectoire(1,3,2,0.15,trace);
  
  vectorObstacles vo;
  WayPoint *wp1 = new WayPoint(0.5, 1.2, 270, 0);
  WayPoint *wp2 = new WayPoint(2.4, 0.3, 0, 0);
  vectorWayPoint vwp;
  
  apt->calculeTrajectoire(vo, *wp1, *wp2, vwp);
  */
  
  printf("Calculer Segments Intermediaires\n");
  
  AlgoPlanifTrajectoire *apt = new AlgoPlanifTrajectoire(1,3,2,0.15,trace);
  WayPoint *wp1 = new WayPoint(5, 14, 270, 0);
  WayPoint *wp2 = new WayPoint(24, 7, 0, 0);
  segLL = new Segment2D(new Point2D(-1,-1), new Point2D(-1,-1));
  segRR = new Segment2D(new Point2D(-1,-1), new Point2D(-1,-1));
  segLR = new Segment2D(new Point2D(-1,-1), new Point2D(-1,-1));
  segRL = new Segment2D(new Point2D(-1,-1), new Point2D(-1,-1));
  
  //apt->CalculerWaypointWaypoint(wp1, 2, wp2, 2, segLL, segRR, segLR, segRL );
  //seg = segLL; printf("[%f %f] [%f %f]\n", seg->pt1->x, seg->pt1->y, seg->pt2->x, seg->pt2->y);
  //seg = segRR; printf("[%f %f] [%f %f]\n", seg->pt1->x, seg->pt1->y, seg->pt2->x, seg->pt2->y);
  //seg = segLR; printf("[%f %f] [%f %f]\n", seg->pt1->x, seg->pt1->y, seg->pt2->x, seg->pt2->y);
  //seg = segRL; printf("[%f %f] [%f %f]\n", seg->pt1->x, seg->pt1->y, seg->pt2->x, seg->pt2->y);  
  
  printf("Navigation ----------------------------------\n");
  
  /*double xPos = 0.2;
  double yPos = 0.4;
  double capRobot = 0;
  double angle1 = -M_PI/180;
  double angle2 = -M_PI/180;
  double angle3 = -M_PI/180;
  */
  //AlgoPC->calculerGisements(angle1, angle2, angle3, capRobot, xPos, yPos);
  
  /*Obstacle *obs1 = new Obstacle(1,2,0.1);
  wp1->pt->x = 0.5;
  wp1->pt->y = 0.5;
  wp1->cap_deg = 90;
  
  Obstacle *obs2 = new Obstacle(1,3,0.2);
  
  //apt->CalculerWaypointObstacle(wp1, 0.2, obs1, segLL, segRR, segLR, segRL);
  
  if ( obs1 == obs1 ) {
	printf("1==1 OK\n");
  } else {
	printf("1!=1 PERDU\n");
  }
  
  if ( obs1 == obs2 ) {
	printf("1==2 PERDU\n");
  } else {
	printf("1!=2 OK\n");
  }
  
  if ( obs2 == obs2 ) {
	printf("2==2 OK\n");
  } else {
	printf("1!=2 PERDU\n");
  }*/
  
  vectorObstacles tdo;
  Obstacle *obs3 = new Obstacle(8.5,7,1); // 4 11 1 pour tester, 8.5 7 1 pr collisionner
  tdo.push_back(obs3);
  Obstacle *obs4 = new Obstacle(9.5,9,1); // 10 3 1 pour tester, 9 9 1 pour collisionner
  tdo.push_back(obs4);
  
  vectorWayPoint vWP;
  
  AlgoPlanifTrajectoire *apt2 = new AlgoPlanifTrajectoire(1,15,15,0.25,trace);
  WayPoint *wp10 = new WayPoint(4, 2, 0, 0);
  WayPoint *wp11 = new WayPoint(11, 11, 90, 0);
  apt2->calculeTrajectoire(tdo, wp10, wp11, vWP);
  
  printf("WayPoints\n");
  for ( unsigned int i = 0 ; i < apt2->listeWayPoints.size() ; i++ ) {
	printf("WP [%d] X:%.2f Y:%.2f %f deg\n", apt2->listeWayPoints[i]->id, apt2->listeWayPoints[i]->pt->x, apt2->listeWayPoints[i]->pt->y, apt2->listeWayPoints[i]->cap_deg );
  }
  
  printf("Matrice\n");
  printf("     ");
  for ( int i = 0 ; i <  apt2->listeWayPoints.size(); i++ ) {
	printf(" %2d  ", i);
  }
  printf("\n");
  printf("-----");
  for ( int i = 0 ; i <  apt2->listeWayPoints.size(); i++ ) {
	printf("-----", i);
  }
  printf("\n");
  
  for ( int i = 0 ; i <  apt2->listeWayPoints.size(); i++ ) {
    printf("%2d | ", i);
	for ( int j = 0 ; j <  apt2->listeWayPoints.size() ; j++ ) {
		if ( apt2->matrice[i][j] == -1 ) {
			printf("  .  ");
		} else {
			printf("%4.1f ", apt2->matrice[i][j]);
		}
	}
	printf("\n");
  }
  
  printf("Route optimale\n");
  for ( int i = 0 ; i < vWP.size() ; i++ ) {
	printf("WP [%d] %f %f %f\n", vWP[i]->id, vWP[i]->pt->x, vWP[i]->pt->y, vWP[i]->cap_deg);
  }
  printf("Route finie\n");
  
  /*Code normal (commenté pr les tests)
  for (compteurTemps_ms = 0;compteurTemps_ms<=finSimu_ms;compteurTemps_ms+=pasTemps_ms)
  {
    ostringstream oss;
    oss << compteurTemps_ms;
    message = "t = " + oss.str();
    trace->print(src,cl,"run",message);
    oss.clear();
    for (it = tabSenseurs.begin();it!=tabSenseurs.end();++it)
    {
      m = *it;
      m->execute(compteurTemps_ms);
    }
    for (it = tabAlgos.begin();it!=tabAlgos.end();++it)
    {
      m = *it;
      m->execute(compteurTemps_ms);
    }
    for (it = tabEffecteurs.begin();it!=tabEffecteurs.end();++it)
    {
      m = *it;
      m->execute(compteurTemps_ms);
    }
  
  }*/
    
  cout <<"**********************************************************************\n";
  cout <<"*                                                                    *\n";
  cout <<"*                        FIN SIMULATION                              *\n";
  cout <<"*                                                                    *\n";
  cout <<"**********************************************************************\n";
  message = "fin";
  trace->print(src,cl,"run",message);
  
}
