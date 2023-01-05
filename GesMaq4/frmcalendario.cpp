#include "frmcalendario.h"
#include "ui_frmcalendario.h"


frmCalendario::frmCalendario(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmCalendario){
    ui->setupUi(this);



}

frmCalendario::~frmCalendario(){
    delete ui;
}

void frmCalendario::on_cmdVolver_clicked(){
    this->close();
}

void frmCalendario::on_cmdGuardar_clicked(){
    QDate   datFecha;
    QLocale qlTraductor;

    datFecha = ui->calCalendario->selectedDate();
    strFecha = qlTraductor.toString(datFecha,"dd/MM/yyyy");

    this->close();

}


void frmCalendario::on_calCalendario_activated(const QDate &date){
    on_cmdGuardar_clicked();
}

