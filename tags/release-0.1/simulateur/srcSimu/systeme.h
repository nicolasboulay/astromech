#ifndef SystemeH
#define SystemeH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

class Module;
class TiXmlDocument;
class Trace;
class Simulation;
class Point2D;
class Systeme;
class Contour;

typedef vector<Module*> vectorModule;
typedef vector<Systeme*> vectorSysteme;

class Systeme
  {
  protected: //accessible uniquement par héritage
    string nom;
    Simulation *simulationPere;
    int nbModules;
    string src;
    string cl;
    string message;
    vectorModule tabModule;
    Trace * trace;
    Point2D * positionCG;
    double attitude_rad;
    Contour * contour;
    
  public : // accessible partout
    Systeme(string fichierSysteme,Simulation *simulationPere,Trace * trace);
    ~Systeme(void);
    int chargerXML(TiXmlDocument & doc);
    void run(void);
    Module * creerModule(string nom_classe);
    Module * getPtrModule(string nom_module);
    string getNomSysteme(void);
    Simulation * getSimuPere(void);
    Contour * getPt_Contour(void);
    Point2D * getPositionCourante(void);
    double * getAttitudeCourante_rad(void);
    void connectModules(void);
    void classerModules(vectorModule & tabSenseurs,
                        vectorModule & tabAlgos,
			vectorModule & tabEffecteurs);
    void initPositionModules(void);
  };
#endif
