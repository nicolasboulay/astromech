#ifndef ObstaclesH
#define ObstaclesH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

class TiXmlDocument;
class Trace;
class Segment2D;

typedef vector<Segment2D *> vectorSegment2D;

class Obstacles
{
  protected: //accessible uniquement par héritage
    Trace * trace;
    string src;
    string cl;
    string message;
    vectorSegment2D tabSegments;
    
  public : // accessible partout
    Obstacles(string fichierObstacles, Trace * t);
    ~Obstacles(void);
    int chargerXML(TiXmlDocument & doc);
};
#endif
