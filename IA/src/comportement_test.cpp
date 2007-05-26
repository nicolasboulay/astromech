#include "comportement.h"
#include <QTextStream>
#include <assert.h>
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement_test.h"
#include <QVector>
#include "common.h"
#include <complex>
#include "barillet.h"
#include "servo.h"
#include "lecteur.h"
#include "geom2D.h"
#include "deplacement.h"

 comportement_test_t::comportement_test_t(int _name) : comportement_t(_name) , step (0)
{
    QTextStream qout(stdout);
    output.name("test comportement");
    qout << "init comportement_test:" << _name << endl;
}

comportement_result_t 
comportement_test_t::execute(const trame_in_t & tin,  
			     internal_state_t & state) 
{
  for(int n=0;n<REFEREE_NUMBER;n++){
    output.isActive[n]=true;
  }

  output.led1_jaune  ^= 1;
  output.led1_vert   ^= 1;
  output.led1_orange ^= 1;

  output.led2_rouge  ^= 1;
  output.led2_orange ^= 1; //
  output.led2_jaune  ^= 1; //
  
  output.led3_vert   ^= 1;
  output.led3_jaune  ^= 1;
  output.led3_orange ^= 1;
  output.led3_rouge  ^= 1;

  printf("STEP : %d (%f / %f)\n", step, state.current_time_s, starttime);
   
  deplacement_t d;
  lecteur_t l_pwm_bar("PWM_BARILLET");
  barillet_t barillet;
  short pwm_bar_value;
   
  switch ( step ) {
  	// initialisation
  	case 0 : 	      
	  d.initAsserv(output);
	  //init_test_barillet();
  	
  	  //output.pic1_reset = 1;
	  //output.pic2_reset = 1;
	  //output.pic3_reset = 1;
  	
	  starttime = state.current_time_s;
    	step++;
    	break;
    
  	case 1 :
	  output.pic1_reset = 0;
	  output.pic2_reset = 0;
	  output.pic3_reset = 0;
	  
	  //bar.todefault(tin,output);
	  
	  starttime = state.current_time_s;
	  step++;
	  break;
	  
  	case 2 :
  		
  		step++;break;
  		printf("Initialisation du barillet\n");
  		output.ctrl_bar_sens_rot = 1;
  		output.ctrl_bar_pwm = 0;
  		output.ctrl_bar_libre = 0;
  		output.ctrl_bar_init = 1;
  		
  		printf("Etat barrillet : %d\n", tin.bar_status_init);
  		
  		if ( tin.bar_status_init == 1 ) {
  			step++;
  			break;
  		}
  		
  		break;
  		
  		pwm_bar_value = l_pwm_bar.get();
  		printf("PWM BARILLET : %d\n", pwm_bar_value);
  		
		barillet.mode_pwm(pwm_bar_value,1,tin, output);
		
		printf("POS BARILLET : %d\n", tin.bar_pos_courante);
  	  
  	  	if ( state.current_time_s - starttime > 120 ) {
  	  		starttime = state.current_time_s;
  	  		step++;
  	  	}
  	  
	  /*if ( test_barillet(tin,output,state)) {
	    init_test_barillet();
	    starttime = state.current_time_s;
	    step++;
	    }*/
	  break;
  	
  	case 3 : 
  	  
  	  step++;break;
  	  
  	  printf("Fin calibration");
  	  output.ctrl_bar_init = 0;
  	  
  	  break;
  	  
      /*step++;break;
  	  if ( test_calibration_barillet(tin, output, state) ) {
  	  	starttime = state.current_time_s;
  	  	step++;
  	  }
  	  break;*/
  	
  	
	case 4 :
		
		init_test_moteurs_vitesse();
		starttime = state.current_time_s;
		step++;
		break;    
    
    case 5 :
    
    	if ( test_moteurs_vitesse(tin, output, state) ) {
    		starttime = state.current_time_s;
    		step++;
    	}
    	break;
    
	default : 
	
		printf("Test terminé !\n");
	
    
  } 

  return output;
};


void comportement_test_t::init_test_barillet()
{ 
   step_bar=0;
}

bool comportement_test_t::test_calibration_barillet(const trame_in_t & tin, trame_out_t & out, internal_state_t & state) {

    double angle;
	lecteur_t l("BARILLET");
	
	bool ret = false;
	switch ( step_bar ) {
	
	case 0 :
      starttime_bar = state.current_time_s;
      if(bar.todefault(tin,out)) {
        ++step_bar;
      }
      break;
      
	case 1 :
	  
	  step_bar++;break;
	  
	  printf("*** MODE PWM ZERO ***\n");
      bar.mode_pwm(0,1,tin,out);
      if ( state.current_time_s - starttime_bar > 60 ) {
        starttime_bar = state.current_time_s;
        step_bar++;
      }
      break;
      
    case 2 : 
      
	  angle = l.get();
	  
	  printf("*** MODE MANUEL %f deg ***\n", angle);
	  
	  bar.print_info(tin, out);
	  bar.mode_angle(angle,1,tin,out);
	  
	  if ( state.current_time_s - starttime_bar > 1000 ) {
        starttime_bar = state.current_time_s;
        step_bar++;
      }
	  break;
	  
	default:
	
	  step_bar = 0;
	  ret = true;
	  
	}

	return ret;

}

void comportement_test_t::init_test_moteurs_vitesse()
{ 
   step_mot_vit=0;
}

bool comportement_test_t::test_moteurs_vitesse ( const trame_in_t & tin, trame_out_t & out,
					internal_state_t & state)
					
{

	bool ret = false;
	printf("TEST Moteurs Vitesse, step %i\n", step_mot_vit);

	deplacement_t d;
	servo_t servo;

	switch ( step_mot_vit ) {
		case 0 :
			starttime_mot_vit = state.current_time_s;
			d.initAsserv(out);
			//step_mot_vit++;
			//break;
			
			out.pic1_spare[1] = 1+2+4;
			printf("**** ATTENTE JACK ****\n");
			if ( tin.start == 0xff || true ) {
				out.pic1_spare[1] = 0;
				step_mot_vit++;
				servo.fermerDepose(tin, out);
				servo.prefermerPelle(tin, out);
			}	
			
			break;
		
		case 1 :
			
			//d.mode_wp(out);
			d.initNav(1,1,0,out);
			//d.mode_vit(out);
			
			step_mot_vit++;
			
			/*d.mode_vitesse(10,10,out);
			if ( state.current_time_s - starttime_mot_vit > 5 ) {
    			starttime_mot_vit = state.current_time_s;
      			step_mot_vit++;
    		}*/
			break;
			
		case 2 :
			
			d.finInitNav(out);
			
			//d.setWaypoint(0,1,2,3,45,0.3,WP_FORCE,EN_AVANT,DEPLACEMENT,0,out);
			//d.setWaypoint(1,2,3,4,45,0  ,WP_WAIT ,EN_AVANT,DEPLACEMENT,0,out);
			
			step_mot_vit++;
			
			d.mode_vitesse(5,5,out);
			/*if ( state.current_time_s - starttime_mot_vit > 5 ) {
    			starttime_mot_vit = state.current_time_s;
      			step_mot_vit++;
    		}*/
			break;
			
		case 3 :
			
			printf("WP transmis\n");
			printf("WP courant %d\n", tin.current_wp_number);
			out.pic1_spare[1] = tin.current_wp_number;
			
			/*d.mode_vitesse(0,0,out);
			if ( state.current_time_s - starttime_mot_vit > 5 ) {
    			starttime_mot_vit = state.current_time_s;
      			step_mot_vit++;
    		}*/
			break;
			
		default:
			ret = true;
		
	}

	return ret;

}

bool
comportement_test_t::test_barillet(const trame_in_t & tin,  trame_out_t & out,
				   internal_state_t & state) 
{
  bool ret=false;
  bar.print_info(tin,out);
  printf("test_bar: step %i ",step_bar);
  printf("(%f / %f)\n", state.current_time_s, starttime_bar);
  switch ( step_bar ) {
  	// initialisation	
  case 0 :
    starttime_bar = state.current_time_s;
    if(bar.todefault(tin,out))
      ++step_bar;
    break;
  case 1 :
    //bar.mode_pwm(80,1,tin,out);
    if ( state.current_time_s - starttime_bar > 3 ) {
      starttime_bar = state.current_time_s;
      step_bar++;
    }
    break;
  case 2 :
    //bar.mode_pwm(160,1,tin,out);
    if ( state.current_time_s - starttime_bar > 3 ) {
      starttime_bar = state.current_time_s;
      step_bar++;
    }
    break;
  case 3 :
    //bar.mode_pwm(0,1,tin,out);
    if ( state.current_time_s - starttime_bar > 3 ) {
      starttime_bar = state.current_time_s;
      step_bar++;
    }
    break;
  case 4 :
    bar.mode_angle(0.0,1,tin,out);
    if ( state.current_time_s - starttime_bar > 7 ) {
      starttime_bar = state.current_time_s;
      step_bar++;
    }
    break;

  case 5 :
    bar.mode_angle(90.0,1,tin,out);
    if ( state.current_time_s - starttime_bar > 7 ) {
      starttime_bar = state.current_time_s;
      step_bar++;
    }
    break;

  case 6 :
    bar.mode_angle(2*90.0,1,tin,out);
    if ( state.current_time_s - starttime_bar > 7 ) {
      starttime_bar = state.current_time_s;
      step_bar++;
    }
    break;

  case 7 :
    bar.mode_angle(3*90.0,1,tin,out);
    if ( state.current_time_s - starttime_bar > 7 ) {
      starttime_bar = state.current_time_s;
      step_bar++;
    }
    break;

  case 8 :
    bar.mode_angle(4*90.0,1,tin,out);
    if ( state.current_time_s - starttime_bar > 10 ) {
      starttime_bar = state.current_time_s;
      step_bar++;
    }
    break;
  case 9 :
    bar.mode_angle(3*90.0,1,tin,out);
    if ( state.current_time_s - starttime_bar > 10 ) {
      starttime_bar = state.current_time_s;
      step_bar++;
    }
    break;
  case 10 :
    bar.mode_angle(2*90.0,1,tin,out);
    if ( state.current_time_s - starttime_bar > 10 ) {
      starttime_bar = state.current_time_s;
      step_bar++;
    }
    break;
    
    
  default:
    step_bar=0;
    ret=true;
  }

  return ret;
}


// comportement_result_t 
// comportement_test_t::execute(const trame_in_t & tin,  
// 			     internal_state_t & state) 
// {
//   for(int n=0;n<REFEREE_NUMBER;n++){
//     output.isActive[n]=true;
//   }

//   /*if(barillet.test(tin,output)){
//     barillet.test_init();
//   }*/

//   output.led1_jaune  ^= 1;
//   output.led1_vert   ^= 1;
//   output.led1_orange ^= 1;

//   output.led2_rouge  ^= 1;
//   output.led2_orange ^= 1; //
//   output.led2_jaune  ^= 1; //
  
//   output.led3_vert   ^= 1;
//   output.led3_jaune  ^= 1;
//   output.led3_orange ^= 1;
//   output.led3_rouge  ^= 1;

//   // Commande PWM route droite
//   printf("STEP : %d (%f / %f)\n", step, state.current_time_s, starttime);
   
//   deplacement_t d;
   
//   switch ( step ) {
//   	// initialisation
//   	case 0 :
 		
//  		//deplacement_t d;
//  		d.initialiserMoteurs(output);
 		
//  		/*output.mode_pwm = 0;
//  		output.mode_vitesse = 0;
//  		output.mode_wp = 1;
//  		output.nav_ctrl_update = 1;
//  		output.new_position_x = 0.95 * 1e6;
//  		output.new_position_y = 0.90 * 1e6;
//  		output.new_cap = 0;
//  		output.quat_w = 10000;
//  		output.quat_z = 0;*/
  	
//   		//output.pic1_reset = 1;
//   		//output.pic2_reset = 1;
//   		//output.pic3_reset = 1;
  	
//     	starttime = state.current_time_s;
//     	step++;
//     	break;
    
//   	case 1 :
  		
//   		output.pic1_reset = 0;
//   		output.pic2_reset = 0;
//   		output.pic3_reset = 0;
  		
//   		/*output.nav_ctrl_update = 0;
  		
//   		servo.pointMortPelle(output);
//   		servo.pointMortDepose(output);
  		
//   		WayPoint *wp1 = new WayPoint(0.95,1.10,0,0.1);
//   		WayPoint *wp2 = new WayPoint(0.95,1.2,0,0);
  		
//   		//deplacement_t d;
//   		d.setWaypoint(0,0, wp1->pt->x,wp1->pt->y,wp1->cap_deg,wp1->vitesse_m_par_s, output);
//   		d.setWaypoint(1,1, wp2->pt->x,wp2->pt->y,wp2->cap_deg,wp2->vitesse_m_par_s, output);*/
  		
//   		starttime = state.current_time_s;
//   		step++;
//   		break;
  	
//   	case 2 :
  	
//   		step++;break;
//   		output.mode_pwm = 1;
//   		output.right_pwm_motor = -20;
//   		output.left_pwm_motor = 20;
  		
//   		if ( state.current_time_s - starttime > 5 ) { // test 5 s
	    	
//   			output.right_pwm_motor = 0;
// 	  		output.left_pwm_motor = 0;
// 	    	starttime = state.current_time_s;
//     		step++;
// 	    }
//   		break;
  	
//   	// test depose : ouverture
//   	case 3 : 
  	
//   		//output.mode_pwm = 0;
//   		step++;
//   		break;
  	
  	
//   	case 4 : 
  	
  		
//   		output.ctrl_bar_sens_rot = 1;
//   		output.ctrl_bar_pwm = 0;
//         output.ctrl_bar_libre = 0;
//         output.ctrl_bar_init = 0;
  		
//   		step++;
//   		break;
    

//     case 5 : 
    	
//     	step++;break;
    	
//     	servo.ouvrirPelle(tin, output);
//     	if ( state.current_time_s - starttime > 3 ) { // test 5 s
// 	    	starttime = state.current_time_s;
//     		step++;
// 	    }
//   		break;
  	

//   	case 6 : 
  	
//   		step++;break;
  		
//     	servo.fermerPelle(tin, output);
    	
//     	printf("*** POTENTIOMETRE PELLE : %d\n", tin.position_pelle);
    	
//     	if ( state.current_time_s - starttime > 1000 ) { // test 5 s
// 	    	starttime = state.current_time_s;
	 		
// 	    	servo.pointMortPelle(output);
	    	
// 	    	bar.test_init();
	    	
//     		step++;
// 	    }
//   		break;
  	
//   	// test barillet  	
//     case 7 : 
    	
//     	step++;break;
    	
//     	output.ctrl_bar_sens_rot = 1;
//     	output.ctrl_bar_pwm = 1;
//     	output.bar_pwm = 50;
    	
//     	//barillet.test(tin, output);
    	
//     	printf("*** POS COURANTE BARILLET : %d\n", tin.bar_pos_courante);
    	
//     	if ( state.current_time_s - starttime > 10 ) { // test 5 s
// 	    	starttime = state.current_time_s;
// 	    	output.bar_pwm = 0;
// 	    	output.ctrl_bar_pwm = 0;
//     		step++;
// 	    }
// 	    break;
    	
//     // test potentiomètre
//     case 8 : 
    
//     	step++;break;
//     	output.ctrl_bar_sens_rot = 1;
//     	output.ctrl_bar_pwm = 1;
//     	output.bar_pwm = 50;
    	
//     	//barillet.test(tin, output);
    	
//     	printf("*** POS COURANTE BARILLET : %d\n", tin.bar_pos_courante);
    	
//     	if ( state.current_time_s - starttime > 10 ) { // test 5 s
// 	    	starttime = state.current_time_s;
// 	    	output.bar_pwm = 0;
// 	    	output.ctrl_bar_pwm = 0;
//     		step++;
// 	    }
// 	    break;
    	
//     	break;
    
//     case 9 : 
    
//     	// shunt
//     	step++;break;
    
// 		break;
    
//     // test des barrières optiques, contacteurs, top départ, bo_basse, bo_haute
//     case 10 : 
  		  	
//   		step++;break;
//     	printf("*** BO_BASSE : %d\n", tin.bo_basse);
//     	printf("*** BO_HAUTE : %d\n", tin.bo_haute);
//     	printf("*** START    : %x\n", tin.start);
//     	printf("*** CONTACT  : %x\n", tin.contact);
//     	printf("*** BAR_POS  : %d\n", tin.bar_pos_courante);
//     	printf("*** PELLE_POS: %d\n", tin.position_pelle);
    	
//     	if ( state.current_time_s - starttime > 120 ) { // test 5 s
// 	    	starttime = state.current_time_s;
//     		step++;
// 	    }
// 	    break;
    	
//     // test des contacteurs dans le barrillet
    
//     // test du marqueur noir/blanc du barillet
    
//     case 11 : 
    
//     	//step++;break;
//     	output.bar_consigne_pos = 10000;
// 		output.bar_vitesse_rot_max = 100;
//   		output.bar_accel_max = 20;
  		
//   		output.mode_vitesse=1;
//    		output.mode_pwm=0;		
//   		output.consigne_speed_right=30;
//  		  		 		output.consigne_speed_left=30;
//   		printf("*** BAR_POS  : %d\n", tin.bar_pos_courante);
  		
//     	if ( state.current_time_s - starttime > 10 ) { // test 5 s
// 	    	starttime = state.current_time_s;
//     		step++;
// 	    }
// 	    break;
    
//     case 12 : 
    
//     	//step++;break;
//     	output.bar_consigne_pos = 20000;
    	
//     	printf("*** BAR_POS  : %d\n", tin.bar_pos_courante);
		  		
//     	if ( state.current_time_s - starttime > 10 ) { // test 5 s
// 	    	starttime = state.current_time_s;
//     		//step++;
// 	    }
// 	    break;
	    
// 	case 13 :
// 		printf("*** Recherche d'un objet\n");
// 		servo.ouvrirPelle(tin, output);
	
// 		if ( tin.bo_basse < 50 || tin.bo_haute < 50 ) {
// 			// présence d'un objet
// 			if ( tin.bo_haute < 50 ) {
// 				printf("*** OBJET : Cannette ou bouteille\n");
// 			} else {
// 				printf("*** OBJET : Pile\n");
// 			}
// 			starttime = state.current_time_s;
// 			step++;
// 		}
	
// 		break;
		
// 	case 14 :
	
// 		printf("FERMER LA PELLE\n");
// 		servo.prefermerPelle(tin, output);
// 		printf("*** POS_PELLE : %d\n", tin.position_pelle);
// 		if ( state.current_time_s - starttime > 1 ) {
// 			starttime = state.current_time_s;
// 			step++;
// 		}
// 		break;
	
// 	case 15 :
		
// 		printf("*** POS_PELLE : %d\n", tin.position_pelle);
// 		if ( tin.position_pelle < 40 ) {
// 			printf("FERMER\n");
// 			servo.fermerPelle(tin, output);
// 			starttime = state.current_time_s;
// 			step++;
// 		} else if ( tin.position_pelle > 40 && state.current_time_s - starttime > 1 ) {
// 			printf("***** BLOCAGE *****\n");
// 			servo.oscillerPelle(tin, output);
// 			//servo.ouvrirPelle(tin, output);
// 		} else if ( tin.position_pelle > 40 && state.current_time_s - starttime > 3 ) { 
// 			printf("**** Relache ****\n");
// 			servo.ouvrirPelle(tin, output);
// 		}
		
// 		break;
    
//     case 16 :
    
    	
//     	if ( state.current_time_s - starttime > 2 && state.current_time_s - starttime < 4 ) { // test 5 s
//     		servo.ouvrirPelle(tin, output);
//     	} else if ( state.current_time_s - starttime > 4 ) {
//     		step = 11;
// 	    	starttime = state.current_time_s;
//     		step++;
// 	    }
	    
    
//     	break;
    
//   } 

//   return output;
// };
