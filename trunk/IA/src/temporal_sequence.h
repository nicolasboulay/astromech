#ifndef TEMPORAL_SEQUENCE_H
#define TEMPORAL_SEQUENCE_H
#include "common.h"
#include "internal_state.h"
#include "trame_out.h"

class temporal_sequence_t {
  double start_time;
 public:
  temporal_sequence_t();

  bool testing(double current_time, trame_out_t & out);
  void start_sequence(double _start_time);
};

#endif
