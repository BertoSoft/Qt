#include "frmlogin.h"
#include "ui_frmlogin.h"

#include "funcaux.h"

#include <QMessageBox>


frmLogin::frmLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmLogin){
    ui->setupUi(this);

    //
    // instalamos en filtro de eventos
    //
    this->installEventFilter(this);


    //
    // Coloco el foco en txtUsuario
    //
    ui->txtUsuario->setFocus();
}

frmLogin::~frmLogin(){
    delete ui;
}

bool frmLogin::eventFilter(QObject *obj, QEvent *event){

    //
    // Si se redimensiona la ventana, y esta cargada la imagen, debemos resize
    //
    if(obj == this && event->type() == QEvent::Resize){
       this->setFixedSize(this->width(), this->height());
    }

    return QObject::eventFilter(obj, event);

}


void frmLogin::on_txtUsuario_returnPressed(){
    ui->txtPasswd->setFocus();
    ui->txtPasswd->selectAll();
}

void frmLogin::on_txtPasswd_returnPressed(){
    on_cmdEntrar_clicked();
}

void frmLogin::on_cmdSalir_clicked(){

    exit(0);
}

void frmLogin::on_cmdEntrar_clicked(){
    QString     strError = "";
    bool        acceso = false;

    FuncAux*    pFuncAux = new FuncAux();

    //
    // Los dos campos no son vacios
    //
    if(ui->txtUsuario->text() != "" && ui->txtPasswd->text() != ""){
        acceso = pFuncAux->funComprobarUsuarioPasswd(ui->txtUsuario->text(), ui->txtPasswd->text());
        //
        // Usuario y contraseña estan bien , entro
        //
        if(acceso){
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
        // Usuario o contraseña estan mal
        //
        else{
            QMessageBox::information(0,strTitulo, "\t\nUsuario o Contraseña incorrecto...");
            ui->txtUsuario->setText("");
            ui->txtPasswd->setText("");
            ui->txtUsuario->setFocus();
        }
    }
    //
    // alguno de los campos esta vacio
    //
    else{
        QMessageBox::information(0,strTitulo, "\t\nDebes rellenar los dos campos");

        if(ui->txtUsuario->text()==""){
            ui->txtUsuario->setFocus();
        }
        else{
            ui->txtPasswd->setFocus();
        }
    }

    delete pFuncAux;
}

