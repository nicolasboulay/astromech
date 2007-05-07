#ifndef COMPORTEMENT_ENDOFMATCH_H
#define COMPORTEMENT_ENDOFMATCH_H
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement.h"
#include <QVector>

//
//  Class de sécurité de fin de match
//   - s'assure que tous les effecteurs du robot se coupent après 1 minutes 30
//
class comportement_endofmatch_t : public comportement_t 
{
  bool first_execute;
 public:
  comportement_endofmatch_t(int _name);
  virtual comportement_result_t  execute(trame_in_t tin, 
					 internal_state_t & state);
};

#endif
