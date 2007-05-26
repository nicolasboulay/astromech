#include "servo.h"
#include <cmath>
#include "lecteur.h"

#define POSITION_DEPOSE_OUVERT 2
#define POSITION_DEPOSE_FERME 150
#define POSITION_PELLE_OUVERT 245
#define POSITION_PELLE_FERME 70

bool servo_t::setPositionDepose(unsigned short position, trame_out_t & out) {
	out.servo_ouverture = position;
	return true;
}

bool servo_t::ouvrirDepose(const trame_in_t & in, trame_out_t & out) {
	
	lecteur_t l("DEP_OUVERT");
	int position = l.get();
	printf("*** POSITION_DEPOSE_OUVERT : %d ***\n", position);
	
	//return setPositionDepose(POSITION_DEPOSE_OUVERT, out); // CALIBRAGE
	return setPositionDepose(position, out); // CALIBRAGE
	
}

bool servo_t::fermerDepose(const trame_in_t & in, trame_out_t & out) {

	lecteur_t l("DEP_FERME");
	int position = l.get();
	printf("*** POSITION_DEPOSE_FERME : %d ***\n", position);

	//return setPositionDepose(POSITION_DEPOSE_FERME, out); // CALIBRAGE
	return setPositionDepose(position, out); // CALIBRAGE

}

bool servo_t::pointMortDepose(trame_out_t & out ) {
	return setPositionDepose(0, out);
}

short calculerEcartPositionPelle ( short pos1, short pos2 ) {
	if ( pos1 > pos2 ) {
		return pos1 - pos2;
	} else {
		return pos2 - pos1;
	}
}

bool servo_t::setPositionPelle(unsigned short position, trame_out_t & out) {
	out.servo_pelle = position;
	return true;
}

bool servo_t::ecartPositionPelle(short position_out, short position_in, short ecart) {
	if ( calculerEcartPositionPelle(position_out, position_in) < ecart ) {
		return true;
	} else {
		return false;
	}
}

bool servo_t::ouvrirPelle(const trame_in_t & in, trame_out_t & out) {
	
	setPositionPelle(POSITION_PELLE_OUVERT, out); // CALIBRAGE position pelle ouverte
	return ecartPositionPelle(out.servo_pelle, in.position_pelle, 0); // CALIBRAGE tolérance position pelle ouverture
		
}

bool servo_t::fermerPelle(const trame_in_t & in, trame_out_t & out) {

	lecteur_t l("PELLE_FERME");
	int position = l.get();
	printf("*** POSITION_PELLE_FERME : %d ***\n", position);
//	setPositionPelle(POSITION_PELLE_FERME, out); // CALIBRAGE position pelle fermée
	setPositionPelle(position, out); // CALIBRAGE position pelle fermée
	return ecartPositionPelle(out.servo_pelle, in.position_pelle, 0); // CALIBRAGE tolérance position pelle fermeture
	
	
}

bool servo_t::pointMortPelle(trame_out_t & out) {
	return setPositionPelle(0, out);
}

bool servo_t::prefermerPelle(const trame_in_t & in, trame_out_t & out) {

	setPositionPelle(140, out); // CALIBRAGE position pelle quasiment fermée
	return ecartPositionPelle(out.servo_pelle, in.position_pelle, 0); // CALIBRAGE tolérance position pelle fermeture
	

}

bool servo_t::oscillerPelle(const trame_in_t & in, trame_out_t & out) {
	
	if ( in.position_pelle > 62 ) {
		setPositionPelle(140, out);
	} else {
		setPositionPelle(180, out);
	}
	
}

bool servo_t::ouvrirPelleHauteurCannette(const trame_in_t & in, trame_out_t & out) {

	setPositionPelle(0, out); // CALIBRAGE position pelle quasiment ouverte
	return ecartPositionPelle(out.servo_pelle, in.position_pelle, 0); // CALIBRAGE tolérance position pelle ouverture

}

