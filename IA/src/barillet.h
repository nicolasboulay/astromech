#ifndef BARILLET_H
#define BARILLET_H
#include "trame_in.h"
#include "trame_out.h"
#include "common.h"

class barillet_t {
 public:
  barillet_t();
  bool init(const trame_in_t & in, trame_out_t & out);
};

#endif
