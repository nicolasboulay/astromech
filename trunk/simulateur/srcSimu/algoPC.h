#ifndef AlgoPCH
#define AlgoPCH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#define POS_X_BALISE_1 1.05
#define POS_Y_BALISE_1 0
#define POS_X_BALISE_2 2.1
#define POS_Y_BALISE_2 3
#define POS_X_BALISE_3 0
#define POS_Y_BALISE_3 3

using namespace std;
class Algo;
class Trace;
class Systeme;
class ComSeriePC_PIC;
class WayPoint;
class TramePC_PIC;
class TramePIC_PC;
class AlgoPlanifTrajectoire;

typedef vector<WayPoint *> vectorWayPoint;

class AlgoPC : public Algo
{
  protected: //accessible uniquement par héritage
    ComSeriePC_PIC * comSerie;
    AlgoPlanifTrajectoire * algoPlanifTraj;
    double frequenceExecution_Hz;
    double tempsDerniereExecution_ms;
    vectorWayPoint listeWP;
    
    TramePC_PIC * trameEnvoyee;
    TramePIC_PC * trameRecue;
    
    int indiceListeEnCours;
    unsigned char u8_numeroAbsoluWP1;
    unsigned char u8_numeroWP1_dansListe;
    
  public : // accessible partout
    AlgoPC(Systeme *s,Trace * t);
    ~AlgoPC(void);
    void execute(int tempsCourant_ms);
    void gereFluxWayPoints(vectorWayPoint listeWPCourante, int indiceListe);
    bool listeWayPointsTerminee(vectorWayPoint listeWPCourante);
    void chargerXML(TiXmlElement* pModuleXML);
    void connectModules(void);
    void calculerPosition( double & angleGisement1, double & angleGisement2, double & angleGisement3, double & capRobot, double & posX, double & posY );
    void calculerGisements( double & angleGisement1, double & angleGisement2, double & angleGisement3, double & capRobot, double & posX, double & posY );
};
#endif
