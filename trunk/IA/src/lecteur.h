#ifndef LECTEUR_H
#define LECTEUR_H
#include "common.h"
#include "stdio.h"

class lecteur_t {
 public:
  lecteur_t(const char * filename);
  int get();
  ~lecteur_t();
 private:
  FILE *f;
};

#endif
