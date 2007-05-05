#ifndef CameraH
#define CameraH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Senseur;
class Systeme;
class Trace;

class Camera : public Senseur
{
  protected: //accessible uniquement par héritage

  public : // accessible partout
    Camera(Systeme *s,Trace * t);
    ~Camera(void);
    void execute(int tempsCourant_ms);
    void chargerXML(TiXmlElement* pModuleXML);
    void connectModules(void);
};
#endif
