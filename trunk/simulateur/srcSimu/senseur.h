#ifndef SenseurH
#define SenseurH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Module;
class Trace;
class Systeme;
typedef vector<Systeme*> vectorSysteme;

class Senseur : public Module
{
  protected: //accessible uniquement par héritage
    
  public : // accessible partout
    Senseur(Systeme *s,Trace * t);
    ~Senseur(void);
    void execute(int tempsCourant_ms);
    void chargerXML(TiXmlElement* pModuleXML);
};
#endif
