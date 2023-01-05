#include "frmlogin0.h"
#include "ui_frmlogin0.h"

#include "funcaux.h"

#include <QMessageBox>

frmLogin0::frmLogin0(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmLogin0){
    ui->setupUi(this);

    //
    // instalamos en filtro de eventos
    //
    this->installEventFilter(this);

    ui->txtUsuario->setFocus();
}

frmLogin0::~frmLogin0(){
    delete ui;
}

bool frmLogin0::eventFilter(QObject *obj, QEvent *event){

    //
    // Si se redimensiona la ventana, y esta cargada la imagen, debemos resize
    //
    if(obj == this && event->type() == QEvent::Resize){
       this->setFixedSize(this->width(), this->height());
    }

    return QObject::eventFilter(obj, event);

}

void frmLogin0::on_cmdSalir_clicked(){

    exit(0);
}

void frmLogin0::on_cmdEntrar_clicked(){
    QString strError = "";

    FuncAux*    pFuncAux = new FuncAux();

    //
    // Si los tres campos no estan vacios
    //
    if( ui->txtUsuario->text()!="" && ui->txtPasswd->text()!="" && ui->txtPasswd1->text()!="" ){
        //
        // si ademas passwd == passwd1, todo correcto
        //
        if(ui->txtPasswd->text() == ui->txtPasswd1->text()){

            //
            // Todo Correcto grabamos usuario y lo ponemos de usuario activo
            //
            strError=pFuncAux->funAddUsuario(ui->txtUsuario->text(), ui->txtPasswd->text());
            pFuncAux->funSetUsuarioActivo(ui->txtUsuario->text());

            //
            // Establezco el usuario activo y la hora de inicio de la sesion
            //
            GstrUsuario       = pFuncAux->GstrUsuario;
            GstrHoraInicio    = pFuncAux->GstrHoaraInicio;
            GstrFechaInicio   = pFuncAux->GstrFechaInicio;


            this->close();
        }
        //
        // el Campo passwd no es igual al passwd1
        //
        else{
            QMessageBox::information(0, strTitulo, "Las contraseñas no coinciden...");

            ui->txtPasswd->setFocus();
            ui->txtPasswd->selectAll();
        }
    }
    //
    // Alguno de los campos esta vacio
    //
    else{
        QMessageBox::information(0, strTitulo, "Debes rellenar todos los campos...");

        if(ui->txtUsuario->text()==""){
            ui->txtUsuario->setFocus();
        }
        if(ui->txtPasswd->text()=="" && ui->txtUsuario->text()!=""){
            ui->txtPasswd->setFocus();
        }
        if(ui->txtPasswd1->text()=="" && ui->txtPasswd->text()!="" && ui->txtUsuario->text()!=""){
            ui->txtPasswd1->setFocus();
        }
    }

    delete pFuncAux;
}

void frmLogin0::on_txtUsuario_returnPressed(){

    ui->txtPasswd->setFocus();
    ui->txtPasswd->selectAll();
}

void frmLogin0::on_txtPasswd_returnPressed(){

    ui->txtPasswd1->setFocus();
    ui->txtPasswd1->selectAll();
}

void frmLogin0::on_txtPasswd1_returnPressed(){

    on_cmdEntrar_clicked();
}

