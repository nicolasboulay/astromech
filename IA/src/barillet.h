#ifndef BARILLET_H
#define BARILLET_H
#include "trame_in.h"
#include "trame_out.h"
#include "common.h"
#include <vector>

using namespace std;
typedef vector<int> vectorInt;

class barillet_t {
 public:
  enum sensRot_t { CW=0,CCW, KEEP};
  barillet_t() : test_step(0) {}
  bool todefault(const trame_in_t & in, trame_out_t & out);
  bool initPosition(const trame_in_t & in, trame_out_t & out);
  bool set(const double & degres, sensRot_t, 
	   const trame_in_t & in, trame_out_t & out);
  bool positionneEntreeCase(const int num_case, enum sensRot_t sensrot, 
			      const trame_in_t & in, trame_out_t & out);// {return true;}
  bool positionneSortieCase(const int num_case, enum sensRot_t sensrot, 
			      const trame_in_t & in, trame_out_t & out);// {return true;}
  bool test(const trame_in_t & in, trame_out_t & out);
  bool test_init();
  bool initialiserPositions();
  void print_info(const trame_in_t & in, trame_out_t & out);
  bool mode_pwm(const trame_in_t & in, trame_out_t & out);
  bool mode_pwm(short pwm, int sens, const trame_in_t & in, trame_out_t & out);
  bool mode_angle(unsigned short pos, int sens, const trame_in_t & in, trame_out_t & out);
  bool mode_angle(double pos_degree, int sens, const trame_in_t & in, trame_out_t & out);
 private:
  int test_step;
  vectorInt positionsEntree;
  vectorInt positionsSortie;
};

#endif
