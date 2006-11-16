#ifndef ComSeriePC_PICH
#define ComSeriePC_PICH
#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
class Algo;
class Trace;
class Systeme;
class WayPoint;

typedef vector<WayPoint *> vectorWayPoint;

class TramePIC_PC
{
  protected: //accessible uniquement par héritage
  
  public : // accessible partout
  WayPoint * pos_att_vit;
  TramePIC_PC(void);
  ~TramePIC_PC(void);
};

class TramePC_PIC
{
  protected: //accessible uniquement par héritage
   
  public : // accessible partout
  int numeroSequenceWP;
  vectorWayPoint tabWayPoint;
  
  TramePC_PIC(void);
  ~TramePC_PIC(void);
};

class ComSeriePC_PIC : public Algo
{
  protected: //accessible uniquement par héritage
    TramePC_PIC *tramePC_PIC;
    TramePIC_PC *tramePIC_PC;
    bool flagTramePC_PIC_recue;
    bool flagTramePIC_PC_recue;
    bool flagSynchroTramePC_PIC_recue;
    bool flagSynchroTramePIC_PC_recue;
    
  public : // accessible partout
    ComSeriePC_PIC(Systeme *s,Trace * t);
    ~ComSeriePC_PIC(void);
    void execute(int tempsCourant_ms);
    void sendToPIC(TramePC_PIC * trame);
    void sendToPC(TramePIC_PC * trame);
    bool tramePIC_PC_recue(void);
    bool tramePC_PIC_recue(void);
    TramePC_PIC * getTramePC_PIC(void);
    TramePIC_PC * getTramePIC_PC(void);
    void chargerXML(TiXmlElement* pModuleXML);
    void connectModules(void);
};
#endif



