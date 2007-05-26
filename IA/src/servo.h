#ifndef SERVO_H
#define SERVO_H
#include "trame_in.h"
#include "trame_out.h"
#include "common.h"

class servo_t {
 public:
	bool ouvrirDepose(const trame_in_t & in, trame_out_t & out);
	bool fermerDepose(const trame_in_t & in, trame_out_t & out);
	bool pointMortDepose(trame_out_t & out);
	
	bool ouvrirPelle(const trame_in_t & in, trame_out_t & out);
	bool fermerPelle(const trame_in_t & in, trame_out_t & out);
	bool pointMortPelle(trame_out_t & out);
	bool prefermerPelle(const trame_in_t & in, trame_out_t & out);
	bool ouvrirPelleHauteurCannette(const trame_in_t & in, trame_out_t & out); // pour percuter une cannette
	bool oscillerPelle(const trame_in_t & in, trame_out_t & out);
	
 private:
  	bool ecartPositionPelle(short position_out, short position_in, short ecart);
  	bool setPositionPelle(unsigned short position, trame_out_t & out);
  	bool setPositionDepose(unsigned short position, trame_out_t & out);
};

#endif
