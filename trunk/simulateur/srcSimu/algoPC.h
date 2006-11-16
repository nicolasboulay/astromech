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
typedef vector<WayPoint *> vectorWayPoint;

class AlgoPC : public Algo
{
  protected: //accessible uniquement par héritage
    ComSeriePC_PIC * comSerie;
    double frequenceExecution_Hz;
    double tempsDerniereExecution_ms;
    int numeroSequenceWP;
    vectorWayPoint listeWP;
    
  public : // accessible partout
    AlgoPC(Systeme *s,Trace * t);
    ~AlgoPC(void);
    void execute(int tempsCourant_ms);
    void chargerXML(TiXmlElement* pModuleXML);
    void connectModules(void);
};
#endif
