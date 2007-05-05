#ifndef EffecteurH
#define EffecteurH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Module;
class Trace;
class Systeme;

class Effecteur : public Module
{
  protected: //accessible uniquement par héritage
    
  public : // accessible partout
    Effecteur(Systeme *s,Trace * t);
    ~Effecteur(void);
    void execute(int tempsCourant_ms);
    void chargerXML(TiXmlElement* pModuleXML);
};
#endif
