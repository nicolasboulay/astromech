#include "module.h"
#include "algo.h"
#include "comSeriePC_PIC.h"
#include "systeme.h"
#include "trace.h"
#include "tinyxml.h"
#include "geom2D.h"

using namespace std;
TramePIC_PC::TramePIC_PC(void)
{

}
TramePIC_PC::~TramePIC_PC(void)
{
  //delete pos_att_vit;
}
TramePC_PIC::TramePC_PIC(void)
{
  numeroSequenceWP = -1;
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

  
ComSeriePC_PIC::ComSeriePC_PIC(Systeme *s,Trace * t):Algo(s,t)
{
  trace = t;
  src = "comSeriePC_PIC.cpp";
  cl = "ComSeriePC_PIC";
  message = "debut";
  trace->print(src,cl,"ComSeriePC_PIC",message);
  
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
    flagSynchroTramePC_PIC_recue = true;
    flagTramePC_PIC_recue = false;
    log << tempsCourant_ms <<"\t"<<"flagTramePC_PIC_recue"<<endl; 
  }
  
  if (flagTramePIC_PC_recue)
  {
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
  tramePC_PIC = trame;
  flagTramePC_PIC_recue = true; 
  message = "fin";
  trace->print(src,cl,"sendToPIC",message);
}
void ComSeriePC_PIC::sendToPC(TramePIC_PC * trame){
  message = "debut";
  trace->print(src,cl,"sendToPC",message);
  tramePIC_PC = trame;
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
TramePC_PIC * ComSeriePC_PIC::getTramePC_PIC(void)
{
  flagSynchroTramePC_PIC_recue = false;
  return tramePC_PIC;
}
TramePIC_PC * ComSeriePC_PIC::getTramePIC_PC(void)
{
  flagSynchroTramePIC_PC_recue = false;
  return tramePIC_PC;
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


