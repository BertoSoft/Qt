#include "listhorasempleados.h"
#include "ui_listhorasempleados.h"

listHorasEmpleados::listHorasEmpleados(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::listHorasEmpleados){

    ui->setupUi(this);

    this->installEventFilter(this);
}

listHorasEmpleados::~listHorasEmpleados(){

    delete ui;
}

void listHorasEmpleados::on_cmdVolver_clicked(){

    this->close();

}

