// Qt things
#include <QTextStream>
#include <QVector>

// std things
#include <cassert>
#include <complex>

//global IA things
#include "common.h"

// input/output/state of IA 
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"

// helper for manipulating mecanics of the robot
#include "barillet.h"
#include "deplacement.h"

#include "comportement_strategie.h"
#include "geom2D.h"

class WayPoint;

 comportement_strategie_t::comportement_strategie_t(int _name) : comportement_t(_name) 
{
    QTextStream qout(stdout);
    output.name("strategie comportement");
    qout << "init comportement_strategie:" << _name << endl;
    
    currentWaypointId = 0;
    nextWaypointId    = 0;
    
    listeWaypoints.clear();
    
    // Chargement de la liste des waypoints
    
    FILE * f = NULL;
    f = fopen("waypoints.csv","r");
    
    if ( f == NULL ) {
    	qout << "Fichier waypoints.csv introuvable." << _name << endl;
    	return;
    }
    
    float Wp_x, Wp_y, Wp_cap, Wp_vit;
    
    while ( fscanf(f, "%f;%f;%f;%f\n", &Wp_x, &Wp_y, &Wp_cap, &Wp_vit) > 0 ) {
    	printf("Chargement d'un WP [%f %f] %f [%f m/s]\n", Wp_x, Wp_y, Wp_cap, Wp_vit);
    	WayPoint *wp = new WayPoint(Wp_x, Wp_y, Wp_cap, Wp_vit);
    	listeWaypoints.push_back(wp);
    	
    }
    printf("Chargement de %d WP effectue\n", listeWaypoints.size());
    
    printf("Affichage des Waypoints : \n");
    for ( int i = 1 ; i <= listeWaypoints.size() ; i++ ) {
    	WayPoint *wp = listeWaypoints[i-1];
    	printf("WP%d : [%f %f] %f [%f m/s]\n", i, wp->pt->x, wp->pt->y, wp->cap_deg, wp->vitesse_m_par_s);
    }
    printf("OK\n");
    
    fclose(f);
    objetOk = false;
    
}

 comportement_result_t 
comportement_strategie_t::execute(const trame_in_t & tin,  
			     internal_state_t & state) 
{
  for(int n=0;n<REFEREE_NUMBER;n++){
    output.isActive[n]=false;
  }
  output.isActive[REFEREE_DEPLACEMENT] = true;
  
  // Quel est le WP courant du PIC ?
  unsigned int currentPICWaypointId = -1;
  
  deplacement_t d;
  //int currentPICWaypointId;
  d.getWaypointCibleId(0, currentPICWaypointId, tin);
  
  printf("Le WP cible du PIC est %d (%d)\n", currentPICWaypointId, tin.current_wp_number);
  
  // Quel est le WP courant de la stratégie ?
  // C'est currentWaypointId
  printf("Le WP cible de la strategie est %d\n", currentWaypointId);
  
  WayPoint *wpCurr;
  WayPoint *wpNext;
  
  switch ( currentPICWaypointId ) {
  
  	case 0 :
  	case 1 :

  		d.setWaypoint(0,0,listeWaypoints[1],WP_FORCE,EN_AVANT,DEPLACEMENT,0,output);
  		d.setWaypoint(1,1,listeWaypoints[2],WP_WAIT,EN_AVANT,DEPLACEMENT,0,output);
  		break;	
  		
  	case 2 : 
  	
  		d.setWaypoint(0,1,listeWaypoints[2],WP_WAIT,EN_AVANT,DEPLACEMENT,0,output);
  		d.setWaypoint(1,2,listeWaypoints[3],WP_WAIT,EN_AVANT,ROTATION,HORAIRE,output);
  		break;
  		
  	case 3 :
  	
  		break;
  		
  	case 4 : 
  	
  		servo.fermerPelle(tin, output);
  		d.setWaypoint(0,1,listeWaypoints[2],WP_WAIT,EN_AVANT,DEPLACEMENT,0,output);
  		d.setWaypoint(1,2,listeWaypoints[3],WP_WAIT,EN_AVANT,DEPLACEMENT,0,output);
  		
  	
  }
  
  // Si le WP courant du PIC > au WP courant de la stratégie, update du PIC
  if ( currentWaypointId < currentPICWaypointId && currentWaypointId <= 2 ) {
  	
  	printf("On passe au PIC un nouveau waypoint\n");
  	currentWaypointId = currentWaypointId + 1;
  	nextWaypointId    = currentWaypointId + 1;
  	
  	WayPoint *wpCurr = listeWaypoints[currentWaypointId];
  	WayPoint *wpNext = listeWaypoints[nextWaypointId];
  	
  	printf("Enregistrement waypoint %d vers le PIC...\n", currentWaypointId);
  	// On enregistre le prochain waypoint courant en position 0
  	//d.setWaypoint(0, currentWaypointId, wpCurr->pt->x, wpCurr->pt->y, wpCurr->cap_deg, wpCurr->vitesse_m_par_s, , output);
  	printf("Enregistrement OK\n");
  	
  	printf("Enregistrement waypoint %d vers le PIC...\n", nextWaypointId);
  	// On enregistre le futur waypoint en position 1
  	//d.setWaypoint(1, nextWaypointId, wpNext->pt->x, wpNext->pt->y, wpNext->cap_deg, wpNext->vitesse_m_par_s, output);
  	
  } else {
  	printf("La trame out est laisee intacte\n");
  }

  if ( currentPICWaypointId = 4 ) {
  
  	servo.fermerPelle(tin, output);
  	servo.fermerDepose(tin, output);
  	objetOk = true;
  
  }

	printf("Contenu de la trame : \n");
	printf("WP0id : %d\n", output.waypoint_number[0] ) ;
	printf("WP1id : %d\n", output.waypoint_number[1] );
	
  return output;
};
