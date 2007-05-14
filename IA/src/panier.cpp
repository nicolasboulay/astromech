#include"panier.h"

panier_t::panier_t()  {
  complex<double> I(0.,1.0);

  position[0]=3*0.35+I*1.5;
  position[1]=4*0.35+I*1.5;
  position[2]=5*0.35+I*1.5;
  
  proba_presence[0]=1.0/3;
  proba_presence[1]=1.0/3;
  proba_presence[2]=1.0/3;
}


// en cas de modification de probabilité la somme des probas restent
// 1.0
void panier_t::change_proba_presence(int index, float value)
{
  float old= proba_presence[index];
  proba_presence[index]=value;

  float diff=old-value;

  proba_presence[(index+1)%3]+=diff/2;
  proba_presence[(index+2)%3]+=diff/2;
}
