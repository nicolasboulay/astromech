#ifndef PANIER_H
#define PANIER_H
#include <QtGlobal>
#include <complex>
using namespace std;
// 
// C'est la class qui contient toutes les infos que l'on a sur les paniers
//

// panier0 : centre du jeu
// panier1 
// panier2 : le plus externe

class panier_t {
 public:
  complex<double> position[3];
  float proba_presence[3];
  
  panier_t();

  void  change_proba_presence(int index, float value);
};

#endif
