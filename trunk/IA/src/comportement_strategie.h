#ifndef COMPORTEMENT_STRATEGIE_H
#define COMPORTEMENT_STRATEGIE_H
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement.h"
#include <QVector>
#include "barillet.h"

// class de strategie
class comportement_strategie_t : public comportement_t 
{
  barillet_t barillet;
 public:
  comportement_strategie_t(int _name);
  virtual comportement_result_t  execute(const trame_in_t & tin, 
					 internal_state_t & state);
};

#endif
