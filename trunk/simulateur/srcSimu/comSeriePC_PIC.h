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
//bits pour u8_CTRL_WPx
#define BIT_WP_NUL 7
#define BIT_WP_NEXT 6
#define BIT_SENS_WP 5
#define BIT_ROT 4
#define BIT_SENS_ROT 3
#define BIT_PWM 2

//bits pour u8_NAV_CTRL
#define BIT_UPDATE 7

class TramePIC_PC
{
  protected: //accessible uniquement par héritage
  
  public : // accessible partout

  signed short   s16_POS_X_mm;
  signed short   s16_POS_Y_mm;
  unsigned short u16_CAP_deg;
  unsigned char  u8_VITESSE_cm_par_s;
  unsigned char  u8_NUM_WP_CRT;
  unsigned char  u8_MUM_NEXT_WP;

  WayPoint * pos_att_vit;
  TramePIC_PC(void);
  ~TramePIC_PC(void);
  void copieTramePIC_PC(TramePIC_PC * trameARecopier);
  string tramePIC_PCToString(void);
};

class TramePC_PIC
{
  protected: //accessible uniquement par héritage
   
  public : // accessible partout
  int numeroSequenceWP;
  vectorWayPoint tabWayPoint;
  
  //Recalage de navigation
  unsigned char  u8_NAV_CTRL;
  signed short   s16_NEW_POS_X_mm;
  signed short   s16_NEW_POS_Y_mm;
  unsigned short u16_NEW_CAP_deg;
  signed short   s16_QUAT_W;
  signed short   s16_QUAT_Z;
  
  //waypoints
  unsigned char  u8_NUM_WP1;
  signed short   s16_POS_X_WP1_mm;
  signed short   s16_POS_Y_WP1_mm;
  unsigned short u16_CAP_WP1_deg;
  unsigned char  u8_VIT_WP1_cm_par_s;
  unsigned char  u8_CTRL_WP1;
  
  unsigned char  u8_NUM_WP2;
  signed short   s16_POS_X_WP2_mm;
  signed short   s16_POS_Y_WP2_mm;
  unsigned short u16_CAP_WP2_deg;
  unsigned char  u8_VIT_WP2_cm_par_s;
  unsigned char  u8_CTRL_WP2;
   
  TramePC_PIC(void);
  ~TramePC_PIC(void);
  void copieTramePC_PIC(TramePC_PIC * trameARecopier);
  string tramePC_PICToString(void);
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
    void getTramePC_PIC(TramePC_PIC * trameARemplir);
    void getTramePIC_PC(TramePIC_PC * trameARemplir);
    void chargerXML(TiXmlElement* pModuleXML);
    void connectModules(void);
};
#endif



