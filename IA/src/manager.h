#ifndef MANAGER_H
#define MANAGER_H
#include "trame_in.h"
#include "trame_out.h"
#include "comportement.h"
#include "internal_state.h"
#include "common.h"

class manager_t {
 public:
  manager_t();
  trame_out_t & execute(trame_in_t in, internal_state_t & state);
  QString refereetostring(int r);
 private:
  void setPriority(int compo, int referee, int rank);
  void referee();
  void copyTrame(QVector<int> compo);

  QVector<QVector<int> > priority_rank;
  QVector<comportement_t *> compo;
  
  trame_out_t out;
  QVector<comportement_result_t> res;
};




#endif
