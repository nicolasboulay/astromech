#ifndef AlgoH
#define AlgoH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Module;
class Trace;
class Systeme;

class Algo : public Module
{
  protected: //accessible uniquement par héritage
    
  public : // accessible partout
    Algo(Systeme *s,Trace * t);
    ~Algo(void);
    void execute(int tempsCourant_ms);
    void chargerXML(TiXmlElement* pModuleXML);
};
#endif
