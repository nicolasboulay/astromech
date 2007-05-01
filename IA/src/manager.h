#ifndef MANAGER_H
#define MANAGER_H
#include "trame_in.h"
#include "trame_out.h"
#include "comportement.h"
#include "internal_state.h"

// Comportement nick name
#define COMPO_DUMMY1 0
#define COMPO_DUMMY2 1
#define COMPO_NUMBER 2

// Referee
#define REFEREE_DEPLACEMENT 0
#define REFEREE_TOOLS       1
#define REFEREE_DISPLAY     2 
#define REFEREE_NAVIGATION  3 //recenter the pic navigation
#define REFEREE_GESTION     4
#define REFEREE_NUMBER      5

class manager_t {
 public:
  manager_t();
  trame_out_t execute(trame_in_t in);
 private:
  void setPriority(int compo, int referee, int rank);
  void generateTrameout();
  void copyTrame(QVector<int> compo);

  QVector<QVector<int> > priority_rank;
  QVector<comportement_t> compo;
  internal_state_t state;
  trame_out_t out;
  QVector<comportement_result_t> res;
};




#endif
