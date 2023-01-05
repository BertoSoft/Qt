#include "frmexportardatos.h"
#include "ui_frmexportardatos.h"

frmExportarDatos::frmExportarDatos(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmExportarDatos){
    ui->setupUi(this);

    //
    // instalamos en filtro de eventos
    //
    this->installEventFilter(this);

}

frmExportarDatos::~frmExportarDatos(){
    delete ui;
}

bool frmExportarDatos::eventFilter(QObject *obj, QEvent *event){

    //
    // Si se redimensiona la ventana, y esta cargada la imagen, debemos resize
    //
    if(obj == this && event->type() == QEvent::Resize){
       this->setFixedSize(this->width(), this->height());
    }

    //
    // Si se pulsa x cerramos con el boton salir
    //
    if(obj == this && event->type() == QEvent::Close){
        on_cmdVolver_clicked();
    }

    return QObject::eventFilter(obj, event);

}

void frmExportarDatos::funInicio(){

}

void frmExportarDatos::on_cmdVolver_clicked(){
    this->close();
}

