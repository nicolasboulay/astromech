#include "lecteur.h"
#include <cmath>

lecteur_t::lecteur_t ( const char * filename ) {
	f = fopen(filename, "r");
}

int lecteur_t::get () {
	int value;
	if ( f != NULL && fscanf(f, "%d", &value) == 1 ) {
		return value;
	} else {
		return 0;
	}
}

lecteur_t::~lecteur_t () {
	if ( f != NULL ) {
		fclose(f);
	}
}

