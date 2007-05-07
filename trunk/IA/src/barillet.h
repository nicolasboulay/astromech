#ifndef BARILLET_H
#define BARILLET_H
#include "trame_in.h"
#include "trame_out.h"
#include "common.h"

class barillet_t {
 public:
  enum sensRot_t { CW=0,CCW, KEEP};
  barillet_t() : test_step(0) {}
  bool todefault(const trame_in_t & in, trame_out_t & out);
  bool initPosition(const trame_in_t & in, trame_out_t & out);
  bool set(const double & degres, sensRot_t, 
	   const trame_in_t & in, trame_out_t & out);
  bool positionneEntreeCase(const int num_case, enum sensRot_t sensrot, 
			      const trame_in_t & in, trame_out_t & out) {return true;}
  bool positionneSortieCase(const int num_case, enum sensRot_t sensrot, 
			      const trame_in_t & in, trame_out_t & out) {return true;}
  bool test(const trame_in_t & in, trame_out_t & out);
  bool test_init();
 private:
  int test_step;
};

#endif
