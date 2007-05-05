#ifndef SimulationH
#define SimulationH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

class Module;
class Systeme;
class TiXmlDocument;
class Trace;
class Obstacle;

typedef vector<Systeme*> vectorSysteme;
typedef vector<Module*> vectorModule;

class Simulation
{
  protected: //accessible uniquement par héritage
    int pasTemps_ms;
    int dureeSimu_s;
    Trace * trace;
    string src;
    string cl;
    string message;
    vectorSysteme tabSysteme;
    vectorModule tabSenseurs;
    vectorModule tabAlgos;
    vectorModule tabEffecteurs;
    Obstacle * obstacle;
    
    int chargerXML(TiXmlDocument & doc);
    
  public : // accessible partout
    Simulation(string fichierSimulation, Trace * t);
    ~Simulation(void);
    int getPasTemps(void);
    vectorSysteme getTabSystemes(void);
    void classerModules(void);
    void run(void);
};
#endif
