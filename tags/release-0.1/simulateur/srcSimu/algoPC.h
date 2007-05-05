#ifndef AlgoPCH
#define AlgoPCH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

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
};
#endif
