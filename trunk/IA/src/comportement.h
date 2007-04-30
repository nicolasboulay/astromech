#ifndef COMPORTEMENT_H
#define COMPORTEMENT_H
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include <QVector>

class comportement_result_t : public trame_out_t {
 public:
  QVector<bool> isActive;
};

class comportement_t {
  public :
    virtual ~comportement_t() {}
    comportement_t(int _name=-1) : name(_name) { }
    virtual comportement_result_t & execute(trame_in_t tin, 
					    internal_state_t state) {return output;};
 protected:
  int name;
  comportement_result_t output;
};

// class de test
class dummy_comportement_t : public comportement_t {
 public:
  dummy_comportement_t(int _name) : comportement_t(_name) { }
  virtual comportement_result_t & execute(trame_in_t tin, 
					  internal_state_t state) {return output;};
};

#endif
