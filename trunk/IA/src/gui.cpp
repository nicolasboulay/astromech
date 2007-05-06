
#include <QApplication>
#include <QTableWidget>
#include <QString>
#include <QVBoxLayout>
#include "gui.h"
#include <QSize>
#include<QTreeWidget>
#include "trame_out.h"
#include "trame_in.h"
#include <QString>
#include <QStringList>
gui_t::gui_t(task_rt_t * mt,QWidget *parent)
  : QWidget(parent), in_j(0), out_j(0)
{
  //
  // table view of the trame_out
  //
  QWidget *window_out = new QWidget;
  table_trame_out = new QTableWidget(12, 3, 0);

  table_trame_out->setRowCount(32);
  table_trame_out->setColumnCount(128);
  table_trame_out->show();
  QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1"));
  table_trame_out->setItem(0, 0, newItem);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(table_trame_out);
  window_out->setLayout(layout);
  window_out->show();
  
  table_trame_out->window()->setWindowTitle("Transmitted by the Linux PC/IA[*]");

  //
  // table view of the trame_in
  //
  QWidget *window_in = new QWidget;
  table_trame_in = new QTableWidget(12, 3, 0);
  table_trame_in->setRowCount(32);
  table_trame_in->setColumnCount(128);
  table_trame_in->show();
  QTableWidgetItem *newItem_in = new QTableWidgetItem(tr("%1"));
  table_trame_in->setItem(0, 0, newItem_in);

  QVBoxLayout *in_layout = new QVBoxLayout;
  in_layout->addWidget(table_trame_in);
  window_in->setLayout(in_layout);
  table_trame_in->window()->setWindowTitle("Received from the pic µC [*]");
  window_in->show();


  //
  // tree view of the symbolic data
  //
  QWidget *window_tree = new QWidget;
  QTreeWidget *treeWidget = new QTreeWidget();
  treeWidget->setColumnCount(2);


  QTreeWidgetItem *tree_out_root =new QTreeWidgetItem(treeWidget, QStringList(QString("out")) );

  tree_out.append(new QTreeWidgetItem(tree_out_root, QStringList(tr("pic1"))));
  tree_out.append(new QTreeWidgetItem(tree_out_root, QStringList(tr("pic2"))));
  tree_out.append(new QTreeWidgetItem(tree_out_root, QStringList(tr("pic3"))));

  QTreeWidgetItem *tree_in_root =new QTreeWidgetItem(treeWidget, QStringList(QString("in")) );

  tree_in.append(new QTreeWidgetItem(tree_in_root, QStringList(tr("pic1"))));
  tree_in.append(new QTreeWidgetItem(tree_in_root, QStringList(tr("pic2"))));
  tree_in.append(new QTreeWidgetItem(tree_in_root, QStringList(tr("pic3"))));

//   tree_in = new QTreeWidgetItem(treeWidget);
//   tree_in->setText(0, tr("in"));


  create_tree_out();
  create_tree_in();
//   QTreeWidgetItem *osloItem = new QTreeWidgetItem(tree_in);
//   osloItem->setText(0, tr("Oslo"));
//   osloItem->setText(1, tr("Yes"));
 
  // QTreeWidgetItem *planets = new QTreeWidgetItem(treeWidget, tree_in);
//  planets->setText(0, tr("out"));

  QVBoxLayout *symbolic_layout = new QVBoxLayout;
  symbolic_layout->addWidget(treeWidget);
  window_tree->setLayout(symbolic_layout);
  treeWidget->window()->setWindowTitle("Tree");

  window_tree->show();

  printf("plop\n");;
}


void gui_t::create_tree_in()
{
#undef INSERT
#define INSERT(n,a) tree_in_data.append(new QTreeWidgetItem(tree_in[n], QStringList(QString(a))));

INSERT(0,"position_pelle");
INSERT(0,"mesure_us_1");  
INSERT(0,"mesure_us_2");
INSERT(0,"mesure_us_3");
INSERT(0,"bo_basse");
INSERT(0,"bo_haute");
INSERT(0,"pic1_spare");

INSERT(1,"x");
INSERT(1,"y");
INSERT(1,"cap");
INSERT(1,"speed");
INSERT(1,"current_wp_number");
INSERT(1,"next_wp_number");

INSERT(1,"left_nb_pulse_com");
INSERT(1,"left_nb_pulse_pid");
INSERT(1,"left_error");
INSERT(1,"left_sum_integ");
INSERT(1,"left_pwm_motor");

INSERT(1,"right_nb_pulse_com");
INSERT(1,"right_nb_pulse_pid");
INSERT(1,"right_error");
INSERT(1,"right_sum_integ");
INSERT(1,"right_pwm_motor");
INSERT(1,"pic2_spare");

INSERT(2,"start");
INSERT(2,"contact");
INSERT(2,"bar_status_init ");
INSERT(2,"bar_pos_courante");
INSERT(2,"bar_nb_pulse_com");
INSERT(2,"bar_nb_pulse_pid");
INSERT(2,"bar_error");
INSERT(2,"bar_sum_integ");
INSERT(2,"bar_pwm");
INSERT(2,"pic3_spare");
}

void gui_t::create_tree_out()
{
#undef INSERT
#define INSERT(n,a) tree_out_data.append(new QTreeWidgetItem(tree_out[n], QStringList(QString(a))));
INSERT(0,"pic1_reset");
INSERT(0,"led1_jaune");
INSERT(0,"led1_vert");
INSERT(0,"led1_orange");
INSERT(0,"pelle_feedback");
INSERT(0,"servo_pelle ");
INSERT(0,"servo_ouverture");
INSERT(0,"servo_3");
INSERT(0,"pic1_spare");

INSERT(1,"pic2_reset");
INSERT(1,"led2_rouge");
INSERT(1,"led2_orange");
INSERT(1,"led2_jaune");
INSERT(1,"nav_ctrl_update");
INSERT(1,"new_position_x");
INSERT(1,"new_position_y");
INSERT(1,"new_cap");
INSERT(1,"quat_w");
INSERT(1,"quat_z");

INSERT(1,"waypoint_number");
INSERT(1,"waypoint_pos_x");
INSERT(1,"waypoint_pos_y");
INSERT(1,"waypoint_cap");
INSERT(1,"waypoint_cap_x");
INSERT(1,"waypoint_cap_y");
INSERT(1,"waypoint_speed");
INSERT(1,"waypoint_ctrl_wp_nul");
INSERT(1,"waypoint_ctrl_wp_next");
INSERT(1,"waypoint_ctrl_sens_wp");
INSERT(1,"waypoint_ctrl_rot");
INSERT(1,"waypoint_ctrl_sens_rot");
INSERT(1,"waypoint_ctrl_pwm");

INSERT(1,"right_pwm_motor");
INSERT(1,"left_pwm_motor");

INSERT(1,"left_gain_prop");
INSERT(1,"left_gain_integ");
INSERT(1,"left_gain_deriv");
INSERT(1,"left_satur_sum_integ");
INSERT(1,"left_thres_prop_only");

INSERT(1,"right_gain_prop");
INSERT(1,"right_gain_integ");
INSERT(1,"right_gain_deriv");
INSERT(1,"right_satur_sum_integ");
INSERT(1,"right_thres_prop_only");

INSERT(1,"pic2_spare"); 

INSERT(2,"pic3_reset");
INSERT(2,"led3_vert");
INSERT(2,"led3_jaune");
INSERT(2,"led3_orange");
INSERT(2,"led3_rouge");

INSERT(2,"ctrl_bar_sens_rot");
INSERT(2,"ctrl_bar_pwm");
INSERT(2,"ctrl_bar_libre");
INSERT(2,"ctrl_bar_init");

INSERT(2,"bar_consigne_pos");
INSERT(2,"bar_vitesse_rot_max");
INSERT(2,"bar_accel_max");
INSERT(2,"bar_pwm");

INSERT(2,"bar_gain_prop");
INSERT(2,"bar_gain_integ");
INSERT(2,"bar_gain_deriv");
INSERT(2,"bar_satur_sum_integ");
INSERT(2,"bar_thres_prop_only");

INSERT(2,"pic3_spare"); 

}

template <typename T_t>
QString qvector_to_qstring(QVector<T_t> x)
{
  QString s=""; 
  for (int i=0;i<x.size();i++){
    s +=QObject::tr("%1 ").arg(x.at(i));
  }
return s; // QString is implicitly shared.
}

void gui_t::update_tree_in(const trame_in_t & in)
{
  int i=0;
#undef UPDATE_S
#undef UPDATE
#define UPDATE_S(a)  tree_in_data[i++]->setText(1, (a));
#define UPDATE(a)  UPDATE_S(tr("%1").arg(in.a));

UPDATE(position_pelle);
UPDATE(mesure_us_1);  
UPDATE(mesure_us_2);
UPDATE(mesure_us_3);
UPDATE(bo_basse);
UPDATE(bo_haute);
UPDATE_S(qvector_to_qstring(in.pic1_spare));

UPDATE(x);
UPDATE(y);
UPDATE(cap);
UPDATE(speed);
UPDATE(current_wp_number);
UPDATE(next_wp_number);

UPDATE(left_nb_pulse_com);
UPDATE(left_nb_pulse_pid);
UPDATE(left_error);
UPDATE(left_sum_integ);
UPDATE(left_pwm_motor);

UPDATE(right_nb_pulse_com);
UPDATE(right_nb_pulse_pid);
UPDATE(right_error);
UPDATE(right_sum_integ);
UPDATE(right_pwm_motor);
UPDATE_S(qvector_to_qstring(in.pic2_spare));

UPDATE(start);
UPDATE(contact);
UPDATE(bar_status_init );
UPDATE(bar_pos_courante);
UPDATE(bar_nb_pulse_com);
UPDATE(bar_nb_pulse_pid);
UPDATE(bar_error);
UPDATE(bar_sum_integ);
UPDATE(bar_pwm);
UPDATE_S(qvector_to_qstring(in.pic3_spare));

}

void gui_t::update_tree_out(const trame_out_t & out)
{
  int i=0;

#undef UPDATE_S
#undef UPDATE
#define UPDATE_S(a)  tree_out_data[i++]->setText(1, (a));
#define UPDATE(a)  UPDATE_S(tr("%1").arg(out.a));

UPDATE(pic1_reset);
UPDATE(led1_jaune);
UPDATE(led1_vert);
UPDATE(led1_orange);
UPDATE(pelle_feedback);
UPDATE(servo_pelle );
UPDATE(servo_ouverture);
UPDATE(servo_3);
UPDATE_S( qvector_to_qstring(out.pic1_spare));

UPDATE(pic2_reset);
UPDATE(led2_rouge);
UPDATE(led2_orange);
UPDATE(led2_jaune);
UPDATE(nav_ctrl_update);
UPDATE(new_position_x);
UPDATE(new_position_y);
UPDATE(new_cap);
UPDATE(quat_w);
UPDATE(quat_z);

UPDATE_S(qvector_to_qstring(out.waypoint_number));
UPDATE_S(qvector_to_qstring(out.waypoint_pos_x));
UPDATE_S(qvector_to_qstring(out.waypoint_pos_y));
UPDATE_S(qvector_to_qstring(out.waypoint_cap));
UPDATE_S(qvector_to_qstring(out.waypoint_cap_x));
UPDATE_S(qvector_to_qstring(out.waypoint_cap_y));
UPDATE_S(qvector_to_qstring(out.waypoint_speed));
UPDATE_S(qvector_to_qstring(out.waypoint_ctrl_wp_nul));
UPDATE_S(qvector_to_qstring(out.waypoint_ctrl_wp_next));
UPDATE_S(qvector_to_qstring(out.waypoint_ctrl_sens_wp));
UPDATE_S(qvector_to_qstring(out.waypoint_ctrl_rot));
UPDATE_S(qvector_to_qstring(out.waypoint_ctrl_sens_rot));
UPDATE_S(qvector_to_qstring(out.waypoint_ctrl_pwm));

UPDATE(right_pwm_motor);
UPDATE(left_pwm_motor);

UPDATE(left_gain_prop);
UPDATE(left_gain_integ);
UPDATE(left_gain_deriv);
UPDATE(left_satur_sum_integ);
UPDATE(left_thres_prop_only);

UPDATE(right_gain_prop);
UPDATE(right_gain_integ);
UPDATE(right_gain_deriv);
UPDATE(right_satur_sum_integ);
UPDATE(right_thres_prop_only);

UPDATE_S(qvector_to_qstring(out.pic2_spare)); 

UPDATE(pic3_reset);
UPDATE(led3_vert);
UPDATE(led3_jaune);
UPDATE(led3_orange);
UPDATE(led3_rouge);

UPDATE(ctrl_bar_sens_rot);
UPDATE(ctrl_bar_pwm);
UPDATE(ctrl_bar_libre);
UPDATE(ctrl_bar_init);

UPDATE(bar_consigne_pos);
UPDATE(bar_vitesse_rot_max);
UPDATE(bar_accel_max);
UPDATE(bar_pwm);

UPDATE(bar_gain_prop);
UPDATE(bar_gain_integ);
UPDATE(bar_gain_deriv);
UPDATE(bar_satur_sum_integ);
UPDATE(bar_thres_prop_only);

UPDATE_S(qvector_to_qstring(out.pic3_spare)); 


}

void gui_t::update_trame_out(QVector<unsigned char> tab)
{
  //  table_trame_out->setColumnCount(tab.count());
  //table_trame_out->setRowCount(out_j+1);
  
  for(int i=0;i<tab.count();i++){
    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(tab[i],2,16));
    table_trame_out->setItem(out_j, i, newItem);
  }
  //printf("out_j:%i\n",out_j);
  out_j++;
  table_trame_out->resizeColumnsToContents();
  table_trame_out->resizeRowsToContents();
}

void gui_t::update_trame_in(QVector<unsigned char> tab)
{
  for(int i=0;i<tab.count();i++){
    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(tab[i],2,16));
    table_trame_in->setItem(in_j, i, newItem);
  }
  //  printf("in_j:%i\n",in_j);
  in_j++;
  table_trame_in->resizeColumnsToContents();
  table_trame_in->resizeRowsToContents();
}

void gui_t::updateC(int i, int j,int val)
{


  QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(val));
  table_trame_out->setItem(j, i, newItem);
  //printf("j:%i\n",j);
  // update();
}
