#include "module.h"
#include "algo.h"
#include "comSeriePC_PIC.h"
#include "systeme.h"
#include "trace.h"
#include "tinyxml.h"
#include "geom2D.h"
#include "bit.h"

using namespace std;
TramePIC_PC::TramePIC_PC(void)
{
  s16_POS_X_mm = 0;
  s16_POS_Y_mm = 0;
  u16_CAP_deg = 0;
  u8_VITESSE_cm_par_s = 0;
  u8_NUM_WP_CRT = 0;
  u8_MUM_NEXT_WP = 0;

}
TramePIC_PC::~TramePIC_PC(void)
{
  //delete pos_att_vit;
}
void TramePIC_PC::copieTramePIC_PC(TramePIC_PC * trameARecopier)
{
  s16_POS_X_mm        = trameARecopier->s16_POS_X_mm;
  s16_POS_Y_mm        = trameARecopier->s16_POS_Y_mm;
  u16_CAP_deg         = trameARecopier->u16_CAP_deg;
  u8_VITESSE_cm_par_s = trameARecopier->u8_VITESSE_cm_par_s;
  u8_NUM_WP_CRT       = trameARecopier->u8_NUM_WP_CRT;
  u8_MUM_NEXT_WP      = trameARecopier->u8_MUM_NEXT_WP;
}
string TramePIC_PC::tramePIC_PCToString(void)
{
  string str;
  str ="******** DEBUT TRAME PIC => PC ************\n";
  str+="\nposX";
  str+=intToString(s16_POS_X_mm);
  str+="\nposY";
  str+=intToString(s16_POS_Y_mm);
  str+="\ncap";
  str+=intToString(u16_CAP_deg);
  str+="\nvitesse";
  str+=intToString(u8_VITESSE_cm_par_s);
  str+="\nn°WP courant";
  str+=intToString(u8_NUM_WP_CRT);
  str+="\nn° prochain WP";
  str+=intToString(u8_MUM_NEXT_WP);
  str+="\n******** FIN TRAME PIC => PC ************\n";
  return str;
}
TramePC_PIC::TramePC_PIC(void)
{
  numeroSequenceWP = -1;
  u8_NAV_CTRL = 0;
  s16_NEW_POS_X_mm = 0;
  s16_NEW_POS_Y_mm = 0;
  u16_NEW_CAP_deg = 0;
  s16_QUAT_W = 0;
  s16_QUAT_Z = 0;
  
  //waypoints
  u8_NUM_WP1 = 0;
  s16_POS_X_WP1_mm = 0;
  s16_POS_Y_WP1_mm = 0;
  u16_CAP_WP1_deg = 0;
  s16_XCAP_WP1_mm = 0;
  s16_YCAP_WP1_mm = 0;
  u8_VIT_WP1_cm_par_s = 0;
  u8_CTRL_WP1 = 0;
  
  u8_NUM_WP2 = 0;
  s16_POS_X_WP2_mm = 0;
  s16_POS_Y_WP2_mm = 0;
  u16_CAP_WP2_deg = 0;
  s16_XCAP_WP2_mm = 0;
  s16_YCAP_WP2_mm = 0;
  u8_VIT_WP2_cm_par_s = 0;
  u8_CTRL_WP2 = 0;

  tabWayPoint.clear();
}
TramePC_PIC::~TramePC_PIC(void)
{
  //on efface la liste des wp
  vectorWayPoint::iterator it;
  cout << tabWayPoint.size() <<endl;
  for (it = tabWayPoint.begin();it!=tabWayPoint.end();++it)
  {
    delete(*it);
  }
}
void TramePC_PIC::copieTramePC_PIC(TramePC_PIC * trameARecopier)
{
  u8_NAV_CTRL    = trameARecopier->u8_NAV_CTRL;
  s16_NEW_POS_X_mm  = trameARecopier->s16_NEW_POS_X_mm;
  s16_NEW_POS_Y_mm  = trameARecopier->s16_NEW_POS_Y_mm;
  u16_NEW_CAP_deg    = trameARecopier->u16_NEW_CAP_deg;
  s16_QUAT_W     = trameARecopier->s16_QUAT_W;
  s16_QUAT_Z     = trameARecopier->s16_QUAT_Z;
  
  u8_NUM_WP1     = trameARecopier->u8_NUM_WP1;
  s16_POS_X_WP1_mm  = trameARecopier->s16_POS_X_WP1_mm;
  s16_POS_Y_WP1_mm  = trameARecopier->s16_POS_Y_WP1_mm;
  u16_CAP_WP1_deg   = trameARecopier->u16_CAP_WP1_deg;
  s16_XCAP_WP1_mm   = trameARecopier->s16_XCAP_WP1_mm; 
  s16_YCAP_WP1_mm   = trameARecopier->s16_YCAP_WP1_mm;
  u8_VIT_WP1_cm_par_s     = trameARecopier->u8_VIT_WP1_cm_par_s;
  u8_CTRL_WP1       = trameARecopier->u8_CTRL_WP1;
  
  u8_NUM_WP2     = trameARecopier->u8_NUM_WP2;
  s16_POS_X_WP2_mm  = trameARecopier->s16_POS_X_WP2_mm;
  s16_POS_Y_WP2_mm  = trameARecopier->s16_POS_Y_WP2_mm;
  u16_CAP_WP2_deg    = trameARecopier->u16_CAP_WP2_deg;
  s16_XCAP_WP2_mm   = trameARecopier->s16_XCAP_WP2_mm; 
  s16_YCAP_WP2_mm   = trameARecopier->s16_YCAP_WP2_mm;
  u8_VIT_WP2_cm_par_s     = trameARecopier->u8_VIT_WP2_cm_par_s;
  u8_CTRL_WP2    = trameARecopier->u8_CTRL_WP2;
}
string TramePC_PIC::tramePC_PICToString(void)
{
  string str;
  str ="******** DEBUT TRAME PC => PIC ************\n";
  str+= "******RECALAGE NAV******\n";
  str+="octet controle recalage navigation:";
  str+=byteToString(u8_NAV_CTRL);
  str+="\nposX";
  str+=intToString(s16_NEW_POS_X_mm);
  str+="\nposY";
  str+=intToString(s16_NEW_POS_Y_mm);
  str+="\ncap";
  str+=intToString(u16_NEW_CAP_deg);
  str+="\nqW";
  str+=intToString(s16_QUAT_W);
  str+="\nqZ";
  str+=intToString(s16_QUAT_Z);
  str+="\n******WP1******\n";
  str+="n° WP";
  str+=intToString(u8_NUM_WP1);
  str+="\nposX";
  str+=intToString(s16_POS_X_WP1_mm);
  str+="\nposY";
  str+=intToString(s16_POS_Y_WP1_mm);
  str+="\ncap";
  str+=intToString(u16_CAP_WP1_deg);
  str+="\nXcap";
  str+=intToString(s16_XCAP_WP1_mm);
  str+="\nYcap";
  str+=intToString(s16_YCAP_WP1_mm);
  str+="\nvitesse";
  str+=intToString(u8_VIT_WP1_cm_par_s);
  str+="\noctet de controle";
  str+=byteToString(u8_CTRL_WP1);
  str+="\n******WP2******\n";
  str+="n° WP";
  str+=intToString(u8_NUM_WP2);
  str+="\nposX";
  str+=intToString(s16_POS_X_WP2_mm);
  str+="\nposY";
  str+=intToString(s16_POS_Y_WP2_mm);
  str+="\ncap";
  str+=intToString(u16_CAP_WP2_deg);
  str+="\nXcap";
  str+=intToString(s16_XCAP_WP2_mm);
  str+="\nYcap";
  str+=intToString(s16_YCAP_WP2_mm);
  str+="\nvitesse";
  str+=intToString(u8_VIT_WP2_cm_par_s);
  str+="\noctet de controle";
  str+=byteToString(u8_CTRL_WP2);
  str+="\n******** FIN TRAME PC => PIC ************\n";

  return str;
}

  
ComSeriePC_PIC::ComSeriePC_PIC(Systeme *s,Trace * t):Algo(s,t)
{
  trace = t;
  src = "comSeriePC_PIC.cpp";
  cl = "ComSeriePC_PIC";
  message = "debut";
  trace->print(src,cl,"ComSeriePC_PIC",message);
  
  tramePC_PIC = new TramePC_PIC();
  tramePIC_PC = new TramePIC_PC();
  
  flagTramePC_PIC_recue = false;
  flagTramePIC_PC_recue = false;
  flagSynchroTramePC_PIC_recue = false;
  flagSynchroTramePIC_PC_recue = false;
  
    
  message = "fin";
  trace->print(src,cl,"ComSeriePC_PIC",message);
}

ComSeriePC_PIC::~ComSeriePC_PIC(void)
{
  message = "debut";
  trace->print(src,cl,"~ComSeriePC_PIC",message);
  message = "fin";
  trace->print(src,cl,"~ComSeriePC_PIC",message);
}

void ComSeriePC_PIC::execute(int tempsCourant_ms)
{
  message = "debut";
  trace->print(src,cl,"execute",message);
  
  if (flagTramePC_PIC_recue)
  {
    cout <<"flagTramePC_PIC_recue"<<endl;
    flagSynchroTramePC_PIC_recue = true;
    flagTramePC_PIC_recue = false;
    log << tempsCourant_ms <<"\t"<<"flagTramePC_PIC_recue"<<endl; 
  }
  
  if (flagTramePIC_PC_recue)
  {
    cout <<"flagTramePIC_PC_recue"<<endl;
    flagSynchroTramePIC_PC_recue = true;
    flagTramePIC_PC_recue = false;
    log << tempsCourant_ms <<"\t"<<"flagTramePIC_PC_recue"<<endl;
  }

  message = "fin";
  trace->print(src,cl,"execute",message);
}
void ComSeriePC_PIC::sendToPIC(TramePC_PIC * trame)
{
  message = "debut";
  trace->print(src,cl,"sendToPIC",message);
  tramePC_PIC->copieTramePC_PIC(trame);
  flagTramePC_PIC_recue = true; 
  message = "fin";
  trace->print(src,cl,"sendToPIC",message);
}
void ComSeriePC_PIC::sendToPC(TramePIC_PC * trame){
  message = "debut";
  trace->print(src,cl,"sendToPC",message);
  tramePIC_PC->copieTramePIC_PC(trame);
  flagTramePIC_PC_recue = true;
  message = "fin";
  trace->print(src,cl,"sendToPC",message);
}
bool ComSeriePC_PIC::tramePIC_PC_recue(void)
{
  return flagSynchroTramePIC_PC_recue;
}
bool ComSeriePC_PIC::tramePC_PIC_recue(void)
{
  return flagSynchroTramePC_PIC_recue;
}
void ComSeriePC_PIC::getTramePC_PIC(TramePC_PIC * trameARemplir)
{
  trameARemplir->copieTramePC_PIC(tramePC_PIC);   
  flagSynchroTramePC_PIC_recue = false;
}
void ComSeriePC_PIC::getTramePIC_PC(TramePIC_PC * trameARemplir)
{
  trameARemplir->copieTramePIC_PC(tramePIC_PC);
  flagSynchroTramePIC_PC_recue = false;
}
void ComSeriePC_PIC::chargerXML(TiXmlElement* pModuleXML)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  Algo::chargerXML(pModuleXML);
      
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
}

void ComSeriePC_PIC::connectModules(void)
{
  message = "debut";
  trace->print(src,cl,"connectModules",message);
    
  message = "fin";
  trace->print(src,cl,"connectModules",message);
}


