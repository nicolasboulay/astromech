#ifndef COMPORTEMENT_TEST_H
#define COMPORTEMENT_TEST_H
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement.h"
#include <QVector>

// class de test
class comportement_test_t : public comportement_t 
{
 public:
  comportement_test_t(int _name);
  virtual comportement_result_t  execute(trame_in_t tin, 
					 internal_state_t & state);
};

#endif
