#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "funcaux.h"
#include "frmlogin0.h"
#include "frmlogin.h"
#include "frmregempleados.h"
#include "frmregcentrostrabajo.h"
#include "frmregmaquinas.h"
#include "frmregmantenimiento.h"
#include "frmregprecioscombustible.h"
#include "frmregtarifasmaquinas.h"
#include "frmgespartesdiarios.h"
#include "frmgesrepostajes.h"
#include "frmgesmantenimiento.h"
#include "frmgesaverias.h"
#include "frmlisthorasempleados.h"
#include "frmlisthorasmaquinas.h"
#include "frmlisthorascentrotrabajo.h"
#include "frmlistgastosmaquina.h"
#include "frmtareaspendientes.h"


#include <cryptopp/cryptlib.h>
#include <cryptopp/rijndael.h>
#include <cryptopp/modes.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/rdrand.h>

#include <QTimer>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){
    ui->setupUi(this);

    //
    // Instalamos un eventfiltro en la app
    //
    qApp->installEventFilter(this);
    this->installEventFilter(this);

    //
    // Hacemos que cada segundo refresque la hora
    //
    QTimer *qtReloj = new QTimer();
    connect( qtReloj, &QTimer::timeout, [this](){funRefrescaReloj();} );
    qtReloj->start(1000);
}

MainWindow::~MainWindow(){

    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event){

    //
    // Si se redimensiona la ventana, y esta cargada la imagen, debemos resize
    //
    if(obj == ui->centralwidget && event->type() == QEvent::Resize){
       this->setFixedSize(this->width(), this->height());
    }

    //
    // Si se pulsa x cerramos con el boton salir
    //
    if(obj == this && event->type() == QEvent::Close ){
        on_actionSalir_triggered();
    }

    return QObject::eventFilter(obj, event);

}

void MainWindow::funInicio(){
    QString strRutaData, strRutaDb;
    QString strError, str;
    QDir    dirData;
    QFile   fileDB;
    bool    bExisteUsuario;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Establezco la barra de estado
    //
    lblTexto->setStyleSheet("color: black; background-color: lightgray");
    lblFecha->setStyleSheet("color: black; background-color: lightgray");
    lblHora->setStyleSheet("color: black; background-color: lightgray");

    lblHora->setFrameShape(QFrame::Shape::WinPanel);
    lblFecha->setFrameShape(QFrame::Shape::WinPanel);
    lblTexto->setFrameShape(QFrame::Shape::WinPanel);

    lblHora->setFrameShadow(QFrame::Shadow::Sunken);
    lblFecha->setFrameShadow(QFrame::Shadow::Sunken);
    lblTexto->setFrameShadow(QFrame::Shadow::Sunken);

    ui->statusbar->addWidget(lblTexto, 10);
    ui->statusbar->addWidget(lblFecha, 3);
    ui->statusbar->addWidget(lblHora, 1);

    lblTexto->setText(strTitulo);

    funRefrescaReloj();

    //
    // Obtenemos la ruta del directorio Data y si no existe lo creamos
    //
    strRutaData = qApp->applicationDirPath() + "/Data";
    dirData.setPath(strRutaData);
    if(!dirData.exists()){
        dirData.mkdir(strRutaData);
    }

    //
    // Si no existe la base de datos la Creamos
    //
    strRutaDb = strRutaData + "/GesMaq.db";
    fileDB.setFileName(strRutaDb);
    if(!fileDB.exists()){
        strError = funCrearBd(strRutaDb);
    }

    //
    // Si se produjo algun error al crear la BD, cerramos el programa
    //
    if(strError != ""){
        QMessageBox::critical(0, strTitulo, "Se ha producido un error... \t\t\tImposible acceder a los datos del programa.");
        exit(1);
    }

    //
    // Ahora toca comprobar si existen usuarios
    //
    bExisteUsuario = pFuncAux->funExisteUsuario();

    //
    // Si existe usuario frmLogin
    //
    if(bExisteUsuario){

        frmLogin* pLogin = new frmLogin(this);

        pLogin->setWindowModality(Qt::ApplicationModal);
        pLogin->setWindowFlag(Qt::FramelessWindowHint);
        pLogin->setWindowTitle(strTitulo);
        pLogin->strTitulo = strTitulo;
        pLogin->exec();
        bEstaCargado = true;
        strTexto = strTitulo;

        //
        // Establezco el usuario activo y la hora de inicio de la sesion
        //
        GstrUsuarioActivo       = pLogin->GstrUsuario;
        GstrHoraInicioSesion    = pLogin->GstrHoraInicio;
        GstrFechaInicioSesion   = pLogin->GstrFechaInicio;


        delete pLogin;

    }
    //
    // Si no existe usuario frmLogin0
    //
    else{

        QMessageBox::information(this, strTitulo, "No existen Usuarios registrados...\t\t\t"
                                                  "Debes establecer un primer Usuario\t\t\t"
                                                  "(Sera el Administrador del Programa)");

        frmLogin0* pLogin0 = new frmLogin0(this);

        pLogin0->setWindowModality(Qt::ApplicationModal);
        pLogin0->setWindowFlag(Qt::FramelessWindowHint);
        pLogin0->setWindowTitle(strTitulo);
        pLogin0->strTitulo = strTitulo;
        pLogin0->exec();
        bEstaCargado = true;
        strTexto = strTitulo;

        //
        // Establezco el usuario activo y la hora de inicio de la sesion
        //
        GstrUsuarioActivo       = pLogin0->GstrUsuario;
        GstrHoraInicioSesion    = pLogin0->GstrHoraInicio;
        GstrFechaInicioSesion   = pLogin0->GstrFechaInicio;


        delete pLogin0;

    }

    //
    // Activo solo los actions que se pueden usar
    //
    funActivaActions();

    delete pFuncAux;


}

void MainWindow::on_actionSalir_triggered(){

    FuncAux* pFuncAux = new FuncAux();

    //
    // Antes de Salir debemos grabar el cierre en el registro de sesiones
    //
    pFuncAux->funCierreSesion();

    delete pFuncAux;

    exit(0);
}

void MainWindow::funRefrescaReloj(){

    QTime   qtHora;
    QDate   qdFecha;
    QLocale qlTraductor;

    qdFecha =   QDate::currentDate();
    qtHora  =   QTime::currentTime();

    //
    // Rellenamos la fecha y la hora
    //
    lblFecha->setText(qlTraductor.toString(qdFecha,"dddd ',' dd 'de' MMMM 'de' yyyy"));
    lblHora->setText(qtHora.toString("hh:mm:ss"));

    //
    // Rellenamos el texto de la barra de tareas
    //
    if( bEstaCargado == true ){
        funSetlblTexto();
    }

    //
    // si bPrimer paso == true , comprobamos los avisos
    //
    if(bEstaCargado && bPrimerPaso){

        FuncAux* pFuncAux = new FuncAux();

        if(pFuncAux->funHayAvisos()){
            QMessageBox::warning(this, strTitulo, "Tienes Avisos en el Buzón de Tareas Pendientes...\t\n\n\t Hay máquinas pasadas de horas");
        }

        delete pFuncAux;
        bPrimerPaso = false;
    }
}

QString MainWindow::funCrearBd(QString strRuta){
    QString         strError = "";
    QString         strSql;
    QSqlDatabase    dbSql;

    //
    // Creo la conexion con la BD
    //
    dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");

    //
    // Establezco la ruta de la BD
    //
    dbSql.setDatabaseName(strRuta);

    //
    // Si se abre y no da error, creamos la Base de Datos
    //
    if(dbSql.open()){

        strSql = "CREATE TABLE if not exists RegistroSesiones(_ID INTEGER PRIMARY KEY AUTOINCREMENT,Usuario TEXT, "
                 "                                                                                 FechaInicio TEXT,"
                 "                                                                                 HoraInicio TEXT,"
                 "                                                                                 FechaCierre TEXT,"
                 "                                                                                 HoraCierre TEXT);";
        dbSql.exec(strSql);

        strSql = "CREATE TABLE if not exists Usuarios(_ID INTEGER PRIMARY KEY AUTOINCREMENT,Usuario TEXT, "
                 "                                                                          Passwd TEXT);";
        dbSql.exec(strSql);

        strSql = "CREATE TABLE if not exists Empleados(_ID INTEGER PRIMARY KEY AUTOINCREMENT,Nombre TEXT, "
                 "                                                                           Dni TEXT,"
                 "                                                                           Apellidos TEXT,"
                 "                                                                           Direccion TEXT,"
                 "                                                                           Telefono TEXT,"
                 "                                                                           Codigo TEXT,"
                 "                                                                           Foto TEXT);";
        dbSql.exec(strSql);

        strSql = "CREATE TABLE if not exists Empresas(_ID INTEGER PRIMARY KEY AUTOINCREMENT,Nombre TEXT, "
                 "                                                                          Cif TEXT,"
                 "                                                                          Seccion TEXT,"
                 "                                                                          RazonSocial TEXT,"
                 "                                                                          Telefono TEXT,"
                 "                                                                          Codigo TEXT);";
        dbSql.exec(strSql);

        strSql = "CREATE TABLE if not exists Maquinas(_ID INTEGER PRIMARY KEY AUTOINCREMENT,Marca TEXT, "
                 "                                                                          Modelo TEXT,"
                 "                                                                          Apodo TEXT,"
                 "                                                                          Bastidor TEXT,"
                 "                                                                          Horas TEXT,"
                 "                                                                          Matricula TEXT,"
                 "                                                                          Codigo TEXT,"
                 "                                                                          Foto TEXT);";
        dbSql.exec(strSql);

        strSql = "CREATE TABLE if not exists Mantenimiento(_ID INTEGER PRIMARY KEY AUTOINCREMENT,Trabajo TEXT, "
                 "                                                                               Descripcion TEXT,"
                 "                                                                               Periodico TEXT,"
                 "                                                                               MaxHoras TEXT,"
                 "                                                                               Correctivo TEXT,"
                 "                                                                               Preventivo TEXT,"
                 "                                                                               Precio TEXT,"
                 "                                                                               Codigo TEXT);";
        dbSql.exec(strSql);

        strSql = "CREATE TABLE if not exists PrecioCombustible(_ID INTEGER PRIMARY KEY AUTOINCREMENT,Empresa TEXT, "
                 "                                                                                   Combustible TEXT,"
                 "                                                                                   FechaInicial TEXT,"
                 "                                                                                   FechaFinal TEXT,"
                 "                                                                                   Precio TEXT,"
                 "                                                                                   Codigo TEXT);";
        dbSql.exec(strSql);

        strSql = "CREATE TABLE if not exists Tarifas(_ID INTEGER PRIMARY KEY AUTOINCREMENT,Empresa TEXT, "
                 "                                                                         Maquina TEXT,"
                 "                                                                         Precio TEXT);";
        dbSql.exec(strSql);

        strSql = "CREATE TABLE if not exists PartesDiarios(_ID INTEGER PRIMARY KEY AUTOINCREMENT,Fecha TEXT, "
                 "                                                                               Empresa TEXT,"
                 "                                                                               Empleado TEXT,"
                 "                                                                               Maquina TEXT,"
                 "                                                                               Trabajo TEXT,"
                 "                                                                               Tiempo TEXT,"
                 "                                                                               Facturable TEXT);";
        dbSql.exec(strSql);

        strSql = "CREATE TABLE if not exists Repostajes(_ID INTEGER PRIMARY KEY AUTOINCREMENT,Fecha TEXT, "
                 "                                                                            Empresa TEXT,"
                 "                                                                            Empleado TEXT,"
                 "                                                                            Maquina TEXT,"
                 "                                                                            Carburante TEXT,"
                 "                                                                            Litros TEXT);";
        dbSql.exec(strSql);

        strSql = "CREATE TABLE if not exists MantenimientoProgramado(_ID INTEGER PRIMARY KEY AUTOINCREMENT,Trabajo TEXT, "
                 "                                                                                         Maquina TEXT,"
                 "                                                                                         MaxHoras TEXT,"
                 "                                                                                         Estado TEXT,"
                 "                                                                                         Codigo TEXT);";
        dbSql.exec(strSql);

        strSql = "CREATE TABLE if not exists Averias(_ID INTEGER PRIMARY KEY AUTOINCREMENT,Fecha TEXT, "
                 "                                                                         Maquina TEXT,"
                 "                                                                         Horas TEXT,"
                 "                                                                         Descripcion TEXT,"
                 "                                                                         Precio TEXT,"
                 "                                                                         Codigo TEXT);";
        dbSql.exec(strSql);

        //
        // Cerramos la BD y la Conexion
        //
        dbSql.close();
        dbSql.removeDatabase("conSql");

    }
    //
    // La Base de Datos no se abrió, establecemos el error
    //
    else{
        strError = "Imposible Acceder a los Datos del Programa...";
    }


    return strError;
}

void MainWindow::funActivaActions(){
    QString         strBd, strSql, str;
    QSqlDatabase    dbGesMaq;
    QSqlQuery       sql;
    bool            hay_empleados, hay_empresas, hay_maquinas, hay_mantenimiento;

    hay_empleados       = false;
    hay_maquinas        = false;
    hay_empresas        = false;
    hay_mantenimiento   = false;

    //
    // Abrimos la BD
    //
    strBd = qApp->applicationDirPath() + "/Data/GesMaq.db";
    dbGesMaq =QSqlDatabase::addDatabase("QSQLITE","conSql");
    dbGesMaq.setDatabaseName(strBd);
    dbGesMaq.open();

    //
    // Establecemos el sql query
    //
    sql = QSqlQuery(dbGesMaq);

    //
    // Comprobamos empleados
    //
    strSql = "SELECT *FROM Empleados";
    sql.exec(strSql);
    sql.first();
    if(sql.isValid()){
        hay_empleados = true;
    }

    //
    // Comprobamos Empresas
    //
    strSql = "SELECT *FROM Empresas";
    sql.exec(strSql);
    sql.first();
    if(sql.isValid()){
        hay_empresas = true;
    }

    //
    // Comprobamos Maquinas
    //
    strSql = "SELECT *FROM Maquinas";
    sql.exec(strSql);
    sql.first();
    if(sql.isValid()){
        hay_maquinas = true;
    }

    //
    // Comprobamos Mantenimiento
    //
    strSql = "SELECT *FROM Mantenimiento";
    sql.exec(strSql);
    sql.first();
    if(sql.isValid()){
        hay_mantenimiento = true;
    }

    //
    // Colocamos lo action que no estan activos
    //
    ui->actionRegistro_de_Precios_Combustible->setEnabled(false);
    ui->actionRegistro_de_Tarifas_de_las_M_quina->setEnabled(false);
    ui->actionGesti_n_Diaria_de_Partes_de_Trabajo->setEnabled(false);
    ui->actionRepostajes->setEnabled(false);
    ui->actionMantenimiento->setEnabled(false);

    //
    // Si solo hay empresas
    //
    if(hay_empresas == true && hay_empleados == false && hay_maquinas == false){
        ui->actionRegistro_de_Precios_Combustible->setEnabled(true);
    }


    //
    // Si hay Empresas y Maquinas, pero no empleados
    //
    if(hay_empresas == true && hay_empleados == false && hay_maquinas == true){
        ui->actionRegistro_de_Tarifas_de_las_M_quina->setEnabled(true);
        ui->actionRegistro_de_Precios_Combustible->setEnabled(true);

    }

    //
    // Si hay Empresas , Maquinas y empleados
    //
    if(hay_empresas == true && hay_empleados == true && hay_maquinas == true){
        ui->actionGesti_n_Diaria_de_Partes_de_Trabajo->setEnabled(true);
        ui->actionRegistro_de_Precios_Combustible->setEnabled(true);
        ui->actionRegistro_de_Tarifas_de_las_M_quina->setEnabled(true);
        ui->actionRepostajes->setEnabled(true);

    }

    //
    // Si Hay Registro de Operaciones de Mantenimiento
    //
    if(hay_mantenimiento == true && hay_maquinas == true){
        ui->actionMantenimiento->setEnabled(true);
    }

    //
    // Cerramos la base de Datos y la conexion
    //
    dbGesMaq.close();
    dbGesMaq.removeDatabase("conSql");

}

void MainWindow::funSetlblTexto(){
    QTime           tHoraInicio, tHoraActual, tTiempoConexion;
    QString         strHoras, strMin, strSeg, str;
    QString         strHoraInicioCod, strHoraInicio, strUusarioCod;

    FuncAux     *pFuncAux = new FuncAux();

    //
    // Obtenemos la Hora actual
    //
    tHoraActual = QTime::currentTime();

    //
    // Obtenemos la hora de inicio de la variable global
    //
    tHoraInicio = pFuncAux->funStrToTime(GstrHoraInicioSesion);

    int iMSeg = tHoraActual.msecsSinceStartOfDay() - tHoraInicio.msecsSinceStartOfDay();

    tTiempoConexion.setHMS(0,0,0,0);
    tTiempoConexion = tTiempoConexion.addMSecs(iMSeg);

    //
    // Obtenemos los tiempos
    //

    strHoras    = QString::number(tTiempoConexion.hour());
    strMin      = QString::number(tTiempoConexion.minute());
    strSeg      = QString::number(tTiempoConexion.second());

    str = strTexto;
    str.append("\t\t\t\t\t\t Usuario : ");
    str.append(GstrUsuarioActivo);
    str.append("\t\t\t\t Tiempo Sesión : ");

    //
    // Mas de una hora
    //
    if(tTiempoConexion.hour() > 0){
        str.append(strHoras);
        str.append(" Horas. ");
        str.append(strMin);
        str.append(" Minutos. y ");
        str.append(strSeg);
        str.append(" Segundos.");
    }
    //
    // Mas de 1 minuto , pero menos de una hora
    //
    else if(tTiempoConexion.hour() == 0 && tTiempoConexion.minute() > 0){
        str.append(strMin);
        str.append(" Minutos. y ");
        str.append(strSeg);
        str.append(" Segundos.");
    }
    //
    // Menos de un minuto
    //
    else{
        str.append(strSeg);
        str.append(" Segundos.");
    }

    lblTexto->setText(str);

    delete pFuncAux;

}

void MainWindow::on_actionImportar_Datos_triggered(){
    QString strRutaArchivoDestino, strRutaArchivoOrigen;
    bool    bCopiado = false;

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Restaurar una antigua copia de seguridad de los datos del Programa...";
    funSetlblTexto();

    //
    // El archivo de origen sera el que nos devuelva el openfilename
    //
    strRutaArchivoOrigen = QFileDialog::getOpenFileName(this,
                                                        tr("Restaurar Copia Seguridad de los Datos del Programa"),
                                                        qApp->applicationDirPath() ,
                                                        tr("Archivo de Datos GesMaq (*.*)"));


    //
    // El archivo de destino sera /Data/GesMaq.db
    //
    strRutaArchivoDestino =  qApp->applicationDirPath();
    strRutaArchivoDestino.append("/Data/GesMaq.db");

    //
    // Si el nombre del archivo no esta vacio, comprobamos si este ya existe
    //
    if(!strRutaArchivoOrigen.isEmpty()){
        //
        // Ahora compruebo si el archivo ya existe, si es asi lo borro
        //
        if(QFile(strRutaArchivoDestino).exists()){
            QFile::remove(strRutaArchivoDestino);
        }

        //
        // ahora hago la copia
        //
        bCopiado = QFile::copy(strRutaArchivoOrigen, strRutaArchivoDestino);
    }

    //
    // Si bCopiado es cierto, hemos hecho bien la coia de seguridad
    //
    if(bCopiado){
        QMessageBox::information(this, strTitulo, "Restauracion de la copia de seguridad :\n\n " + strRutaArchivoOrigen + " \n\nRealizada con exito...");
    }

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;
    funSetlblTexto();

}

void MainWindow::on_actionExportar_Datos_triggered(){
    QString strRutaArchivoDestino, strRutaArchivoOrigen;
    bool    bCopiado = false;

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Guardando una copia de seguridad de los Datos del Programa...";
    funSetlblTexto();

    //
    // El archivo de origen sera la base de datos GesMaq.db
    //
    strRutaArchivoOrigen = qApp->applicationDirPath();
    strRutaArchivoOrigen.append("/Data/GesMaq.db");

    //
    // Pedimos el archivo de destino
    //
    strRutaArchivoDestino = QFileDialog::getSaveFileName(this,
                                                  tr("Copia Seguridad de los Datos del Programa"),
                                                  qApp->applicationDirPath() + "/Copia de Seguridad.old",
                                                  tr("Archivo de Datos GesMaq (*.*)"));

    //
    // Si el nombre del archivo no esta vacio, comprobamos si este ya existe
    //
    if(!strRutaArchivoDestino.isEmpty()){
        //
        // Ahora compruebo si el archivo ya existe, si es asi lo borro
        //
        if(QFile(strRutaArchivoDestino).exists()){
            QFile::remove(strRutaArchivoDestino);
        }

        //
        // ahora hago la copia
        //
        bCopiado = QFile::copy(strRutaArchivoOrigen, strRutaArchivoDestino);
    }

    //
    // Si bCopiado es cierto, hemos hecho bien la coia de seguridad
    //
    if(bCopiado){
        QMessageBox::information(this, strTitulo, "Copia de seguridad :\n\n " + strRutaArchivoDestino + " \n\nRealizada con exito...");
    }

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;
    funSetlblTexto();

}

void MainWindow::on_actionRegistro_de_Empleados_triggered(){

    frmRegEmpleados* pRegEmpleados = new frmRegEmpleados(this);

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Gestionando el Registro de Empleados...";
    funSetlblTexto();

    //
    // Llamamos a frmEmpleados
    //
    pRegEmpleados->setWindowFlag(Qt::CustomizeWindowHint);
    pRegEmpleados->setWindowModality(Qt::WindowModal);
    pRegEmpleados->setWindowTitle("Registro de empleados...");
    pRegEmpleados->GstrTitulo = strTitulo;
    pRegEmpleados->funInicio();
    pRegEmpleados->exec();

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;

    //
    //Comprobamos si podemos activar actions
    //
    funActivaActions();

    delete pRegEmpleados;


}

void MainWindow::on_actionRegistro_de_Centros_de_Trabajo_triggered(){

    frmregCentrosTrabajo *pCentroTrabajo = new frmregCentrosTrabajo(this);

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Gestionando el Registro de Centros de Trabajo...";
    funSetlblTexto();

    //
    // Llamamos a frmEmpleados
    //
    pCentroTrabajo->setWindowFlag(Qt::CustomizeWindowHint);
    pCentroTrabajo->setWindowModality(Qt::WindowModal);
    pCentroTrabajo->setWindowTitle("Registro de Centros de Trabajo...");
    pCentroTrabajo->GstrTitulo = strTitulo;
    pCentroTrabajo->funInicio();
    pCentroTrabajo->exec();

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;

    //
    //Comprobamos si podemos activar actions
    //
    funActivaActions();

    delete pCentroTrabajo;


}

void MainWindow::on_actionRegistro_de_M_quinas_triggered(){

    frmRegMaquinas *pMaquinas = new frmRegMaquinas(this);

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Gestionando el Registro de Maquinaria...";
    funSetlblTexto();

    //
    // Llamamos a frmEmpleados
    //
    pMaquinas->setWindowFlag(Qt::CustomizeWindowHint);
    pMaquinas->setWindowModality(Qt::WindowModal);
    pMaquinas->setWindowTitle("Registro de Máquinas...");
    pMaquinas->GstrTitulo = strTitulo;
    pMaquinas->funInicio();
    pMaquinas->exec();

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;

    //
    //Comprobamos si podemos activar actions
    //
    funActivaActions();

    delete pMaquinas;


}

void MainWindow::on_actionRegistro_de_Operaciones_de_Mantenimiento_triggered(){

    frmRegMantenimiento *pMantenimiento = new frmRegMantenimiento(this);

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Gestionando el Registro de OPeraciones de Mantenimiento...";
    funSetlblTexto();

    //
    // Llamamos a frmEmpleados
    //
    pMantenimiento->setWindowFlag(Qt::CustomizeWindowHint);
    pMantenimiento->setWindowModality(Qt::WindowModal);
    pMantenimiento->setWindowTitle("Registro de OPeraciones de Mantenimiento...");
    pMantenimiento->GstrTitulo = strTitulo;
    pMantenimiento->funInicio();
    pMantenimiento->exec();

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;

    //
    //Comprobamos si podemos activar actions
    //
    funActivaActions();

    delete pMantenimiento;


}

void MainWindow::on_actionRegistro_de_Precios_Combustible_triggered(){

    frmRegPreciosCombustible *pPreciosCombustible = new frmRegPreciosCombustible(this);

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Gestionando el Registro de los precios del combustible...";
    funSetlblTexto();

    //
    // Llamamos a frmEmpleados
    //
    pPreciosCombustible->setWindowFlag(Qt::CustomizeWindowHint);
    pPreciosCombustible->setWindowModality(Qt::WindowModal);
    pPreciosCombustible->setWindowTitle("Registro de Precios Combustible...");
    pPreciosCombustible->GstrTitulo = strTitulo;
    pPreciosCombustible->funInicio();
    pPreciosCombustible->exec();

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;

    //
    //Comprobamos si podemos activar actions
    //
    funActivaActions();

    delete pPreciosCombustible;


}

void MainWindow::on_actionRegistro_de_Tarifas_de_las_M_quina_triggered(){

    frmRegTarifasMaquinas *pTarifasMaquinas = new frmRegTarifasMaquinas(this);

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Gestionando el Registro de las tarifas de las distintas máquinas...";
    funSetlblTexto();

    //
    // Llamamos a frmEmpleados
    //
    pTarifasMaquinas->setWindowFlag(Qt::CustomizeWindowHint);
    pTarifasMaquinas->setWindowModality(Qt::WindowModal);
    pTarifasMaquinas->setWindowTitle("Registro de Tarifas Máquinas...");
    pTarifasMaquinas->GstrTitulo = strTitulo;
    pTarifasMaquinas->funInicio();
    pTarifasMaquinas->exec();

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;

    //
    //Comprobamos si podemos activar actions
    //
    funActivaActions();

    delete pTarifasMaquinas;


}

void MainWindow::on_actionGesti_n_Diaria_de_Partes_de_Trabajo_triggered(){

    frmGesPartesDiarios *pPartesDiarios = new frmGesPartesDiarios(this);

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Guardando Partes de Trabajo de Empleados...";
    funSetlblTexto();

    //
    // Llamamos a frmEmpleados
    //
    pPartesDiarios->setWindowFlag(Qt::CustomizeWindowHint);
    pPartesDiarios->setWindowModality(Qt::WindowModal);
    pPartesDiarios->setWindowTitle("Guardando Partes de Trabajo...");
    pPartesDiarios->GstrTitulo = strTitulo;
    pPartesDiarios->funInicio();
    pPartesDiarios->exec();

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;

    //
    //Comprobamos si podemos activar actions
    //
    funActivaActions();

    delete pPartesDiarios;


}

void MainWindow::on_actionRepostajes_triggered(){

    frmGesRepostajes *pRepostajes = new frmGesRepostajes(this);

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Gestionando Repostajes Diarios...";
    funSetlblTexto();

    //
    // Llamamos a frmEmpleados
    //
    pRepostajes->setWindowFlag(Qt::CustomizeWindowHint);
    pRepostajes->setWindowModality(Qt::WindowModal);
    pRepostajes->setWindowTitle("Repostajes Diarios...");
    pRepostajes->GstrTitulo = strTitulo;
    pRepostajes->funInicio();
    pRepostajes->exec();

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;

    //
    //Comprobamos si podemos activar actions
    //
    funActivaActions();

    delete pRepostajes;


}

void MainWindow::on_actionMantenimiento_triggered(){

    frmGesMantenimiento *pMantenimiento = new frmGesMantenimiento(this);

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Gestionando el Mantenimiento Diario...";
    funSetlblTexto();

    //
    // Llamamos a frmEmpleados
    //
    pMantenimiento->setWindowFlag(Qt::CustomizeWindowHint);
    pMantenimiento->setWindowModality(Qt::WindowModal);
    pMantenimiento->setWindowTitle("Mantenimiento Diario...");
    pMantenimiento->GstrTitulo = strTitulo;
    pMantenimiento->funInicio();
    pMantenimiento->exec();

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;

    //
    //Comprobamos si podemos activar actions
    //
    funActivaActions();

    delete pMantenimiento;


}

void MainWindow::on_actionAver_as_triggered(){

    frmGesAverias *pAverias = new frmGesAverias(this);

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Gestionando el Registro de Averías...";
    funSetlblTexto();

    //
    // Llamamos a frmEmpleados
    //
    pAverias->setWindowFlag(Qt::CustomizeWindowHint);
    pAverias->setWindowModality(Qt::WindowModal);
    pAverias->setWindowTitle("Averías...");
    pAverias->GstrTitulo = strTitulo;
    pAverias->funInicio();
    pAverias->exec();

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;

    //
    //Comprobamos si podemos activar actions
    //
    funActivaActions();

    delete pAverias;

}

void MainWindow::on_actionListado_de_Horas_Por_Empleado_triggered(){

    frmListHorasEmpleados *pHorasEmpleados = new frmListHorasEmpleados(this);

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Listado de Horas por Empleado...";
    funSetlblTexto();

    //
    // Llamamos a frmEmpleados
    //
    pHorasEmpleados->setWindowFlag(Qt::CustomizeWindowHint);
    pHorasEmpleados->setWindowModality(Qt::WindowModal);
    pHorasEmpleados->setWindowTitle("Listado de Horas Por Empleado...");
    pHorasEmpleados->GstrTitulo = strTitulo;
    pHorasEmpleados->funInicio();
    pHorasEmpleados->exec();

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;

    //
    //Comprobamos si podemos activar actions
    //
    funActivaActions();

    delete pHorasEmpleados;


}

void MainWindow::on_actionListado_de_Horas_Por_Maquina_triggered(){

    frmListHorasMaquinas *pHorasMaquinas = new frmListHorasMaquinas(this);

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Listado de Horas por Máquina...";
    funSetlblTexto();

    //
    // Llamamos a frmEmpleados
    //
    pHorasMaquinas->setWindowFlag(Qt::CustomizeWindowHint);
    pHorasMaquinas->setWindowModality(Qt::WindowModal);
    pHorasMaquinas->setWindowTitle("Listado de Horas por Máquina...");
    pHorasMaquinas->GstrTitulo = strTitulo;
    pHorasMaquinas->funInicio();
    pHorasMaquinas->exec();

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;

    //
    //Comprobamos si podemos activar actions
    //
    funActivaActions();

    delete pHorasMaquinas;


}


void MainWindow::on_actionListado_Horas_Por_Centro_de_Trabajo_triggered(){

    frmListHorasCentroTrabajo *pHorasCentrosTrabajo = new frmListHorasCentroTrabajo(this);

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Listado de Horas por Centro de Trabajo...";
    funSetlblTexto();

    //
    // Llamamos a frmEmpleados
    //
    pHorasCentrosTrabajo->setWindowFlag(Qt::CustomizeWindowHint);
    pHorasCentrosTrabajo->setWindowModality(Qt::WindowModal);
    pHorasCentrosTrabajo->setWindowTitle("Listado de Horas por Centro de Trabajo...");
    pHorasCentrosTrabajo->GstrTitulo = strTitulo;
    pHorasCentrosTrabajo->funInicio();
    pHorasCentrosTrabajo->exec();

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;

    //
    //Comprobamos si podemos activar actions
    //
    funActivaActions();

    delete pHorasCentrosTrabajo;


}

void MainWindow::on_actionListado_de_Gastos_Por_Maquina_triggered(){

    frmListGastosMaquina *pGastosMaquina = new frmListGastosMaquina(this);

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Listado de Gastos por Máquina...";
    funSetlblTexto();

    //
    // Llamamos a frmEmpleados
    //
    pGastosMaquina->setWindowFlag(Qt::CustomizeWindowHint);
    pGastosMaquina->setWindowModality(Qt::WindowModal);
    pGastosMaquina->setWindowTitle("Listado de Gastos por Máquina...");
    pGastosMaquina->GstrTitulo = strTitulo;
    pGastosMaquina->funInicio();
    pGastosMaquina->exec();

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;

    //
    //Comprobamos si podemos activar actions
    //
    funActivaActions();

    delete pGastosMaquina;


}

void MainWindow::on_actionBuzn_de_Tareas_Pendientes_triggered(){

    frmTareasPendientes *pTareasPendientes = new frmTareasPendientes(this);

    //
    // Colocamos la informacion en la barra de tareas
    //
    strTexto = "Buzón de Avisos de Tareas Pendientes...";
    funSetlblTexto();

    //
    // Llamamos a frmEmpleados
    //
    pTareasPendientes->setWindowFlag(Qt::CustomizeWindowHint);
    pTareasPendientes->setWindowModality(Qt::WindowModal);
    pTareasPendientes->setWindowTitle("Buzón de Avisos...");
    pTareasPendientes->GstrTitulo = strTitulo;
    pTareasPendientes->funInicio();
    pTareasPendientes->exec();

    //
    // Volvemos a MainWindow
    //
    strTexto = strTitulo;

    //
    //Comprobamos si podemos activar actions
    //
    funActivaActions();

    delete pTareasPendientes;


}

