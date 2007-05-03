#include <math.h>

// Mise au carr� de a
double square(double a) {
	return a*a;
}

// Calcul de la valeur absolue de a
double ValeurAbsolue(double a) {
	return ( a > 0 ) ? a : -a;
}

// Calcule la distance (norme euclidienne) entre les points A (x1,y1) et B (x2, y2)
double calculerDistance ( double x1, double y1, double x2, double y2 ) {
	return sqrt(square(x2-x1) + square(y2-y1));
}

// Retourne la valeur minimum entre a et b
double min (double a, double b ) {
	return (a < b) ? a : b;
}

// Calcul l'angle trigonom�trique du vecteur AB dans le rep�re (O, x, y) et normalisation entre 0 et 2*M_PI
double calculerAngle(double xA, double yA, double xB, double yB) {
	
	double angle = atan2(yB-yA, xB-xA);
	if ( angle < 0 ) {
		return angle+2*M_PI;
	} else {
		return angle;
	}
}

// Calcule l'�cart angulaire entre A et B, dans le sens trigonom�trique et normalis� entre 0 et 2*M_PI
double ecartAngulaire(double thetaA, double thetaB) {
	double angle = thetaB - thetaA;
	if ( angle > 2*M_PI ) {
		return angle-2*M_PI;
	} else if ( angle < 0 ) {
		return angle+2*M_PI;
	} else {
		return angle;
	}
}

// Calcule le produit vectoriel entre les vecteurs V1 (x1, y1) et V2 (x2, y2) projet� sur z
double produitVectoriel ( double x1, double y1, double x2, double y2 ) {
	return x1*y2 - x2*y1;
}

// Calculer le cercle passant par les points A (x1,y1) et B (x2,y2) et le point o� se trouve le robot, position depuis laquelle il voit l'�cart angulaire entre A et B �gal � dtheta.
// Le rayon du cercle est sauvegard� dans *R, la position de son centre en *x0 et *y0.
void calculerCercle ( double dtheta, double x1, double y1, double x2, double y2, double *x0, double *y0, double *R) {
	
	// Distance entre A et B
	double longueur = sqrt(square(x2-x1)+square(y2-y1));
	
	// Calcul du rayon du cercle
	*R = ValeurAbsolue(longueur/(2*sin(dtheta)));
	
	// Saturation du rayon de virage � 1000 km (�viter les divisions par z�ro)
	if ( *R > 1e9 ) {
		printf("Saturation de R de %f � %f\n", *R, 1e9);
		*R = 1e9;
	}
	
	// Les deux �quations du cercle passant par A et B
	// (xA-x)^2 + (yA-y)^2 = R^2  (1)
	// (xB-x)^2 + (yB-y)^2 = R^2  (2)
	// (1)-(2) sous la forme y = A * x + B  (3)
	double A = - (x2-x1)/(y2-y1);
	double B = ( square(y2) - square(y1) + square(x2) - square(x1) ) / ( 2*(y2-y1) );
	
	printf("A:%f B:%f\n", A, B);
	
	// On remplace y (3) dans l'�quation (1)
	// (1) avec (3) sous la forme Cx^2 + Dx + E = 0
	double C = 1+square(A);
	double D = -2*x1 - 2*A*(y1-B);
	double E = square(x1) + square(y1-B) - square(*R);
	
	printf("C:%f D:%f E:%f\n", C, D, E);
	
	// R�solution de l'�quation en x^2 : discriminant et racines du polynomes
	double delta = square(D) - 4*C*E;
	printf("delta:%f\n", delta);
	
	// On obtient deux centres possibles pour le cercle
	double xr1 = (-D + sqrt(delta))/(2*C);
	double xr2 = (-D - sqrt(delta))/(2*C);
	
	double yr1 = A * xr1 + B;
	double yr2 = A * xr2 + B;
	
	printf("C1 : %f %f\n", xr1, yr1);
	printf("C2 : %f %f\n", xr2, yr2);
	
	// Calcul du produit vectoriel AB x AO1 (premier centre)
	double ProdVec = produitVectoriel(x2-x1, y2-y1, xr1-x1, yr1-y1);
	// De quel signe devrait �tre le produit vectoriel (c�t� du cercle par rapport � la position du robot)
	double signePrevu = ( dtheta < M_PI && dtheta > M_PI/2 ) ? -1 : 1;
	
	printf("PV:%f PVprev:%f\n", ProdVec, signePrevu);
	
	// S�lection du c�t� du cercle en fonction du signe du produit vectoriel
	if ( ProdVec * signePrevu > 0 ) { // M�me signe, donc on prend le premier point
		*x0 = xr1;
		*y0 = yr1;
	} else { // C'est le deuxi�me point qui est le bon
		*x0 = xr2;
		*y0 = yr2;
	}
	
}

// Calcule de l'intersection entre les deux cercles C1 de centre (x1, y1) et de rayon R1 et C2 de centre (x2, y2) et de rayon R2
// Les deux intersections seront not�es (xi1, yi1) et (xi2, yi2)
void calculerIntersectionCercles( double x1, double y1, double R1, double x2, double y2, double R2, double *xi1, double *yi1, double *xi2, double *yi2) {

	printf("Cercles : %f %f %f ; %f %f %f\n", x1, y1, R1, x2, y2, R2);

	// S�lection de la m�thode, soit on prend y=Ax+B, soit x=Ay+B, permet de g�rer les points align�s sur l'axe X ou l'axe Y
	if ( ValeurAbsolue(y2-y1) > ValeurAbsolue(x2-x1) ) {
		
		// �quation du cercle 1 - �quation du cercle 2 sous la forme yi = A xi + B (3)
		double A = - (x1-x2)/(y1-y2);
		double B = (square(x1) + square(y1) - square(x2) - square(y2) - square(R1) + square(R2)) / ( 2*(y1-y2));
		
		// (3) report�e dans (1) exprim� sous la forme C xi^2 + D xi + E = 0
		double C = 1 + square(A);
		double D = -2*x1 + 2*A*(B-y1);
		double E = square(B-y1) - square(R1) + square(x1);
		
		double delta = square(D) - 4*C*E;
		
		// Solutions
		*xi1 = ( -D - sqrt(delta) ) / (2*C);
		*xi2 = ( -D + sqrt(delta) ) / (2*C);
		
		*yi1 = A * (*xi1) + B;
		*yi2 = A * (*xi2) + B;
		
	} else {
	
		// �quation du cercle 1 - �quation du cercle 2 sous la forme xi = A yi + B
		double A = - (y1-y2)/(x1-x2);
		double B = (square(x1) + square(y1) - square(x2) - square(y2) - square(R1) + square(R2)) / ( 2*(x1-x2));
		
		// (3) report�e dans (1) exprim� sous la forme C xi^2 + D xi + E = 0
		double C = 1 + square(A);
		double D = -2*y1 + 2*A*(B-x1);
		double E = square(B-x1) - square(R1) + square(y1);
		
		double delta = square(D) - 4*C*E;
		
		// Solutions
		*yi1 = ( -D - sqrt(delta) ) / (2*C);
		*yi2 = ( -D + sqrt(delta) ) / (2*C);
		
		*xi1 = A * (*yi1) + B;
		*xi2 = A * (*yi2) + B;
		
	}
	
}

// Calcule la position d'intersection valable sur les deux calcul�es : une des deux est la position de la balise pr�sente sur les deux cercles, l'autre est celle du robot
void calculerPosition(double xBalise, double yBalise, double xC1, double yC1, double R1, double xC2, double yC2, double R2, double *xP, double *yP) {
	
	double xi1, yi1, xi2, yi2;
	calculerIntersectionCercles(xC1, yC1, R1, xC2, yC2, R2, &xi1, &yi1, &xi2, &yi2);

	printf("Intersections: %f %f ; %f %f\n", xi1, yi1, xi2, yi2);
	
	// Calcule les deux distances par rapport � la balise
	double distBaliseI1 = calculerDistance(xBalise, yBalise, xi1, yi1);
	double distBaliseI2 = calculerDistance(xBalise, yBalise, xi2, yi2);
	
	// Le point qui a la distance la plus faible par rapport � la balise correspond � l'intersection qui ne nous int�resse pas
	if ( distBaliseI1 < distBaliseI2 ) {
		*xP = xi2;
		*yP = yi2;
	} else {
		*xP = xi1;
		*yP = yi1;
	}

}

// Prototype d'utilisation
int main() {

  // position originelle du robot, not�e en mm
  double xReel = 1500;
  double yReel = 1000;
  
  // position des balises, attention les yi doivent �tre diff�rents !
  double x1 = 0;
  double y1 = 2000;
  double x2 = 0;
  double y2 = 0;
  double x3 = 3000;
  double y3 = 1000;
  
  // conversion en angles
  double theta1 = calculerAngle(x1, y1, xReel, yReel);
  double theta2 = calculerAngle(x2, y2, xReel, yReel);
  double theta3 = calculerAngle(x3, y3, xReel, yReel);
  
  printf("Theta1 : %f\n", theta1 * 180 / M_PI);
  printf("Theta2 : %f\n", theta2 * 180 / M_PI);
  printf("Theta3 : %f\n", theta3 * 180 / M_PI);
  
  // conversion en �cart d'angles, et ajouts d'�ventuelles erreurs de mesure
  double dt12 = ecartAngulaire(theta1, theta2) + 1*M_PI/180;
  double dt23 = ecartAngulaire(theta2, theta3) + 0*M_PI/180;
  //double dt31 = ecartAngulaire(theta3, theta1) + 0/180*M_PI;
  double dt31 = 2*M_PI - dt12 - dt23;
  
  printf("DTheta12 : %f\n", dt12 * 180 / M_PI);
  printf("DTheta23 : %f\n", dt23 * 180 / M_PI);
  printf("DTheta31 : %f\n", dt31 * 180 / M_PI);
  
  // calcul des cercles
  double xC1, yC1, R1;
  double xC2, yC2, R2;
  double xC3, yC3, R3;
  
  calculerCercle(dt12, x1, y1, x2, y2, &xC1, &yC1, &R1);
  printf("Rayon : %f\n", R1);
  printf("Centre: %f %f\n\n", xC1, yC1);
  
  calculerCercle(dt23, x2, y2, x3, y3, &xC2, &yC2, &R2);
  printf("Rayon : %f\n", R2);
  printf("Centre: %f %f\n\n", xC2, yC2);
  
  calculerCercle(dt31, x3, y3, x1, y1, &xC3, &yC3, &R3);
  printf("Rayon : %f\n", R3);
  printf("Centre: %f %f\n\n", xC3, yC3);
  
  // calcul des intersections des cercles
  
  double xP1, yP1, xP2, yP2, xP3, yP3;
  calculerPosition(x1, y1, xC3, yC3, R3, xC1, yC1, R1, &xP1, &yP1);
  calculerPosition(x2, y2, xC1, yC1, R1, xC2, yC2, R2, &xP2, &yP2);
  calculerPosition(x3, y3, xC2, yC2, R2, xC3, yC3, R3, &xP3, &yP3);

  printf("Position : %f %f [%f]\n", xP1, yP1, calculerDistance(xReel, yReel, xP1, yP1));
  printf("Position : %f %f [%f]\n", xP2, yP2, calculerDistance(xReel, yReel, xP2, yP2));
  printf("Position : %f %f [%f]\n", xP3, yP3, calculerDistance(xReel, yReel, xP3, yP3));
  
  return 0;

}

