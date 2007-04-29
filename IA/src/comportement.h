#ifndef COMPORTEMENT_H
#define COMPORTEMENT_H
#include "trame_in.h"
#include "trame_out.h"

class comportement_result_t : public trame_out_t {

  QVector<bool> isActive;
};

class comportement_t {
  public :
    comportement_t(int _name) : name(_name) { }
    virtual comportement_result_t & execute(trame_in_t tin, 
					    internal_state_t state);
 private:
    int name;
  comportement_result_t ouput;
};

// class de test
class dummy_comportement_t : public comportement_t {
 public:
  virtual comportement_result_t & execute(trame_in_t tin, 
					  internal_state_t state) {return output;};
};

#endif
