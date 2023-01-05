#include "frmgespartesdiarios.h"
#include "ui_frmgespartesdiarios.h"

#include "frmcalendario.h"
#include "funcaux.h"

#include <QDate>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QMessageBox>
#include <QImageReader>


frmGesPartesDiarios::frmGesPartesDiarios(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmGesPartesDiarios){
    ui->setupUi(this);

    this->installEventFilter(this);

}

frmGesPartesDiarios::~frmGesPartesDiarios(){
    delete ui;
}

bool frmGesPartesDiarios::eventFilter(QObject *obj, QEvent *event){
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

void frmGesPartesDiarios::funInicio(){
    QString strRutaDb, strSql, str;
    QString strCodigo, strEmpresa, strMaquina, strEmpleado;
    QString strRutaFoto, strRutaFotoCod;
    QDate   qdFecha;
    QLocale qlTraductor;

    FuncAux* pFuncAux = new FuncAux();

    /*
    //
    // Dimensionamos la pantalla al 90% del screen
    //
    QScreen *src = QGuiApplication::screens().at(0);

    ancho_src = src->availableGeometry().width();
    alto_src  = src->availableGeometry().height();

    ancho_form  = (int) ancho_src * 0.90;
    alto_form   = (int) alto_src * 0.75;

    ix0         = (int) ancho_src * 0.05;
    iy0         = (int) alto_src * 0.12;

    this->setGeometry(ix0, iy0, ancho_form, alto_form);

    */

    //
    // Colocamos las columnas del tablelist
    //
    ui->tlPartes->setColumnCount(5);

    QStringList headers;
    headers += tr("Empresa");
    headers += tr("Maquina");
    headers += tr("Descripción del Trabajo...");
    headers += tr("Duración (Horas)");
    headers += tr("Facturable ?");

    ui->tlPartes->setHorizontalHeaderLabels(headers);

    ui->tlPartes->setColumnWidth(0, (int) ( ( this->width() * 0.9 ) / 5 ) );
    ui->tlPartes->setColumnWidth(1, (int) ( ( this->width() * 0.9 ) / 5 ) );
    ui->tlPartes->setColumnWidth(2, (int) ( ( this->width() * 2.2 ) / 5 ) );
    ui->tlPartes->setColumnWidth(3, (int) ( ( this->width() * 0.4 ) / 5 ) );
    ui->tlPartes->setColumnWidth(4, (int) ( ( this->width() * 0.4 ) / 5 ) );

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    //
    // Colocamos la fecha en txtFecha
    //
    qdFecha = QDate::currentDate();
    ui->txtFecha->setText(qlTraductor.toString(qdFecha,"dd/MM/yyyy"));
    GstrUltimaFecha = ui->txtFecha->text();
    ui->lblCalendario->setSelectedDate(qdFecha);

    //
    //Rellenamos el cmbEmpresas
    //
    while(ui->cmbEmpresa->count()>0){
        ui->cmbEmpresa->removeItem(0);
    }
    strSql = "SELECT *FROM Empresas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodigo   = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());
        strEmpresa  = pFuncAux->funTxtToEmpresas(strCodigo);

        ui->cmbEmpresa->addItem(strEmpresa);
        sql.next();
    }

    //
    // Rellenamos el cmbEmpleados
    //
    while(ui->cmbEmpleado->count()>0){
        ui->cmbEmpleado->removeItem(0);
    }

    strSql = "SELECT *FROM Empleados";
    sql.exec(strSql);
    sql.first();

    //
    // Mostramos la foto del primer empleado, solo si existe
    //
    strRutaFotoCod  = sql.record().value("Foto").toString();
    if(strRutaFotoCod != ""){
        strRutaFoto = pFuncAux->funDesCifrar_bis(strRutaFotoCod);
    }
    else{
        strRutaFoto = "";
    }
    pFuncAux->funMostrarFoto(ui->lblFotoEmpleado, strRutaFoto);

    while (sql.isValid()) {

        strCodigo   = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());
        strEmpleado = pFuncAux->funTxtToEmpleados(strCodigo);

        ui->cmbEmpleado->addItem(strEmpleado);
        sql.next();
    }
    GstrUltimoEmpleado = ui->cmbEmpleado->currentText();

    //
    // Rellenamos el cmbMaquinas
    //
    while (ui->cmbMaquina->count()>0) {
        ui->cmbMaquina->removeItem(0);
    }
    strSql = "SELECT *FROM Maquinas";
    sql.exec(strSql);
    sql.first();

    //
    // Mostramos la foto de la primera maquina, solo si existe
    //
    strRutaFotoCod  = sql.record().value("Foto").toString();
    if(strRutaFotoCod != ""){
        strRutaFoto = pFuncAux->funDesCifrar_bis(strRutaFotoCod);
    }
    else{
        strRutaFoto = "";
    }
    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, strRutaFoto);

    while (sql.isValid()) {

        strCodigo   = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());
        strMaquina  = pFuncAux->funTxtToMaquinas(strCodigo);

        ui->cmbMaquina->addItem(strMaquina);
        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    //
    // Rellenamos el listado si hay parte diario
    //
    funRellenaListado(ui->txtFecha->text(), ui->cmbEmpleado->currentText());

    //
    // Selecionamos el checkbox de facturable y desactivamos cmdsuprimir
    //
    ui->chkFacturable->setChecked(true);
    ui->chkFacturable->setToolTip("Selecciona si el trabajo es Facturable...");

    ui->cmdSuprimir->setEnabled(false);
    ui->cmdSuprimir->setToolTip("");

    ui->cmdGuardar->setEnabled(false);
    ui->cmdGuardar->setToolTip("");

    ui->txtDescripcion->setToolTip("Describe el trabajo realizado...");
    ui->txtDuracion->setToolTip("Indica la duración del trabajo en Horas...");

    //
    // Si hay listado en la tabla cmdGuardar enabled
    if(ui->tlPartes->rowCount()>0){
        ui->cmdGuardar->setEnabled(true);
    }

    //
    // Establecemos como ultimo empleado a ui.cmb Text
    //
    GstrUltimoEmpleado = ui->cmbEmpleado->currentText();

    //
    // Colocamos el titulo de la tabla
    //
    str = "Parte diario de :\t ";
    str.append(pFuncAux->funNombreToCodEmpleado(pFuncAux->funCodigoToEmpleado(GstrUltimoEmpleado)));
    str.append("\t\t Fecha:\t");

    qdFecha =  pFuncAux->funStrToDate(ui->txtFecha->text());
    str.append(qlTraductor.toString(qdFecha,"dddd, dd MMMM yyyy"));

    ui->lblTitulo->setText(str);

    ui->txtDescripcion->setFocus();

    delete pFuncAux;


}

bool frmGesPartesDiarios::funParteGuardado(QString strFecha, QString strCodEmpleado){
    QString strRutaDb, strSql;
    QString strCodEmpresaTl, strCodMaquinaTl, strDescripcionTl, strDuracionTl, strFacturableTl;
    QString strCodEmpresaDb, strCodMaquinaDb, strDescripcionDb, strDuracionDb, strFacturableDb;
    QString strCodEmpleadoDb, strFechaDb;
    bool    bGuardado = false;
    int     iFila;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql21");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);


    //
    // Por cada linea de la tabla, Comprobamos si estan guardados todos los campos
    //
    iFila= 0;
    while (iFila < ui->tlPartes->rowCount()) {

        strCodEmpresaTl     = pFuncAux->funCodigoToEmpresa(ui->tlPartes->item(iFila, 0)->text());
        strCodMaquinaTl     = pFuncAux->funCodigoToMaquina(ui->tlPartes->item(iFila, 1)->text());
        strDescripcionTl    = ui->tlPartes->item(iFila, 2)->text();
        strDuracionTl       = ui->tlPartes->item(iFila, 3)->text();
        strFacturableTl     = ui->tlPartes->item(iFila, 4)->text();

        //
        // Recorremos la BD, para saber si esta linea esta guardada
        //
        strSql = "SELECT *FROM PartesDiarios";
        sql.exec(strSql);
        sql.first();
        while (sql.isValid()) {

            strFechaDb          = pFuncAux->funDesCifrar_bis(sql.record().value("Fecha").toString());
            strCodEmpleadoDb    = pFuncAux->funDesCifrar_bis(sql.record().value("Empleado").toString());

            if(strFecha == strFechaDb && strCodEmpleado == strCodEmpleadoDb){

                //
                // Obtenemos los valores de la BD
                //
                strCodEmpresaDb     = pFuncAux->funDesCifrar_bis(sql.record().value("Empresa").toString());
                strCodMaquinaDb     = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());
                strDescripcionDb    = pFuncAux->funDesCifrar_bis(sql.record().value("Trabajo").toString());
                strDuracionDb       = pFuncAux->funDesCifrar_bis(sql.record().value("Tiempo").toString());
                strFacturableDb     = pFuncAux->funDesCifrar_bis(sql.record().value("Facturable").toString());

                //
                // Comprobamos si coinciden con los Valores de la tabla, si es asi, bGuardado = true
                //
                if(strCodEmpresaTl == strCodEmpresaDb &&
                        strCodMaquinaTl == strCodMaquinaDb &&
                        strDescripcionTl == strDescripcionDb &&
                        strDuracionTl == strDuracionDb &&
                        strFacturableTl == strFacturableDb){

                    bGuardado = true;
                    sql.finish();
                }
                //
                // Si los valores no coinciden, bGuardado = false
                //
                else{
                    bGuardado = false;
                }
            }

            sql.next();
        }

        //
        // Si bGuardado = true , seguimos
        //
        if(bGuardado == false){
            iFila = ui->tlPartes->rowCount();
        }
        //
        // Si bGuardado == false, salimos
        //
        else{
            iFila++;
        }
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql21");

    delete pFuncAux;

    return bGuardado;
}

void frmGesPartesDiarios::funRellenaListado(QString strFecha, QString strEmpleado){
    QString strRutaDb, strSql;
    QString strCodEmpleado, strCodEmpresa, strCodMaquina, strCodigoEmpleadoDb, strFechaDb;
    int fila = 0;

    FuncAux* pFuncAux = new FuncAux();

    //
    // si la tabla no esta vacia la borramos
    //
    while(ui->tlPartes->rowCount()>0){
        ui->tlPartes->removeRow(0);
    }

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    //
    // Comprobamos si existe parte de esa fecha con ese empleado
    //
    strCodEmpleado = pFuncAux->funCodigoToEmpleado(ui->cmbEmpleado->currentText());

    strSql = "SELECT *FROM PartesDiarios";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {
        //
        // Si coinciden fecha y empleado creamos fila nueva
        //

        strCodigoEmpleadoDb = pFuncAux->funDesCifrar_bis(sql.record().value("Empleado").toString());
        strFechaDb          = pFuncAux->funDesCifrar_bis(sql.record().value("Fecha").toString());

        if(strFechaDb == strFecha
        && strCodigoEmpleadoDb == strCodEmpleado){

            QString strEmpresa, strMaquina, strTrabajo, strDuracion, strFacturable;

            ui->tlPartes->setRowCount(fila +1);

            strCodEmpresa   = pFuncAux->funDesCifrar_bis(sql.record().value("Empresa").toString());
            strCodMaquina   = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());

            strEmpresa      = pFuncAux->funTxtToEmpresas(strCodEmpresa);
            strMaquina      = pFuncAux->funTxtToMaquinas(strCodMaquina);
            strTrabajo      = pFuncAux->funDesCifrar_bis(sql.record().value("Trabajo").toString());
            strDuracion     = pFuncAux->funDesCifrar_bis(sql.record().value("Tiempo").toString());
            strFacturable   = pFuncAux->funDesCifrar_bis(sql.record().value("Facturable").toString());

            QTableWidgetItem *item_empresa = new QTableWidgetItem(strEmpresa);
            QTableWidgetItem *item_maquina = new QTableWidgetItem(strMaquina);
            QTableWidgetItem *item_trabajo = new QTableWidgetItem(strTrabajo);
            QTableWidgetItem *item_duracion = new QTableWidgetItem(strDuracion);
            QTableWidgetItem *item_facturable = new QTableWidgetItem(strFacturable);

            item_empresa->setTextAlignment(Qt::AlignCenter);
            item_maquina->setTextAlignment(Qt::AlignCenter);
            item_trabajo->setTextAlignment(Qt::AlignCenter);
            item_duracion->setTextAlignment(Qt::AlignCenter);
            item_facturable->setTextAlignment(Qt::AlignCenter);

            ui->tlPartes->setItem(fila, 0, item_empresa);
            ui->tlPartes->setItem(fila, 1, item_maquina);
            ui->tlPartes->setItem(fila, 2, item_trabajo);
            ui->tlPartes->setItem(fila, 3, item_duracion);
            ui->tlPartes->setItem(fila, 4, item_facturable);

            fila++;
        }

        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    //
    // Selecionamos el checkbox de facturable y desactivamos cmdsuprimir
    //
    ui->chkFacturable->setChecked(true);
    ui->cmdSuprimir->setEnabled(false);
    if(ui->tlPartes->rowCount()>0){
        ui->cmdGuardar->setEnabled(true);
    }
    else{
        ui->cmdGuardar->setEnabled(false);
    }

    ui->txtDescripcion->setFocus();

    delete pFuncAux;

}

void frmGesPartesDiarios::funGrabarHorasMaquina(QString strCodigo, QString strHoras){
    QString strRutaDb, strSql, strCodigoDb, strCodigoDbCod, strHorasCod;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Abro la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql15");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    //
    // Hago una busqueda de la maquina por el codigo
    //
    strSql = "SELECT *FROM Maquinas";
    sql.exec(strSql);

    sql.first();
    while (sql.isValid()) {

        strCodigoDbCod  = sql.record().value("Codigo").toString();
        strCodigoDb     = pFuncAux->funDesCifrar_bis(strCodigoDbCod);

        if(strCodigoDb == strCodigo){
            sql.finish();
        }

        sql.next();
    }

    strHorasCod = pFuncAux->funCifrar_bis(strHoras);

    strSql= "UPDATE Maquinas SET Horas ='" + strHorasCod + "' WHERE  Codigo ='" + strCodigoDbCod + "';";
    sql.exec(strSql);

    //
    // Cierro la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql15");

    delete pFuncAux;

}

void frmGesPartesDiarios::funProcesaFecha(){
    QString strRutaDb, strSql, strFacturable, str;
    QString strCodEmpresa, strCodMaquina, strCodEmpleado;
    QString strCodEmpresaCod, strCodMaquinaCod, strCodEmpleadoCod, strFechaCod;
    QString strTrabajoCod, strTiempoCod, strFacturableCod, strCodEmpleadoDbCod, strFechaDbCod;
    QString strFecha, strHoras;
    QString strCodEmpleadoDb, strFechaDb, strCodMaquinaDb, strTiempoDb, strFacturableDb;
    QLocale qlTraductor;
    QDate   qdFecha;
    int     iRegDb, iRegTabla, i, iFila;
    bool    bParteGuardado;
    double  dHoras;

    FuncAux* pFuncAux = new FuncAux();

    //
    // En primer lugar miramos si existe parte en la tabla, y cuantas lineas tiene
    //
    iRegTabla = 0;
    iRegTabla = ui->tlPartes->rowCount();

    //
    // Colocamos Fecha y Empleado, que marcan la exclusividad
    //
    strFecha        = GstrUltimaFecha;
    strCodEmpleado  = pFuncAux->funCodigoToEmpleado(GstrUltimoEmpleado);

    //
    //Ahora Comprobamos si existe parte en la BD, y cuantos Registros Tiene
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql12");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM PartesDiarios";
    sql.exec(strSql);
    sql.first();
    iRegDb = 0;
    while (sql.isValid()) {

        strCodEmpleadoDb    = pFuncAux->funDesCifrar_bis(sql.record().value("Empleado").toString());
        strFechaDb          = pFuncAux->funDesCifrar_bis(sql.record().value("Fecha").toString());

        if(strFecha == strFechaDb && strCodEmpleado == strCodEmpleadoDb){

            iRegDb++;
        }

        sql.next();
    }

    //
    // Si iRegTabla != 0 o bien iRegBd !=, tenemos que comprobar si la tabla esta guardada en la BD
    //
    bParteGuardado = true;
    if(iRegTabla > 0 || iRegDb > 0){
        if(iRegDb == iRegTabla){
            bParteGuardado = funParteGuardado(strFecha, strCodEmpleado);
        }
        else{
            bParteGuardado = false;
        }

    }

    s_HorasMaquina hRestar[iRegDb];

    //
    // Si bParteGuardado es false, avisamos
    //
    if(!bParteGuardado){

        //
        // Avisamos que el parte, aunque guardado esta modificado
        //
        str = " Este Parte, Ha sido Modificado...\n\n¿Quieres Guardar el Nuevo Parte de :\n\n";
        str.append(GstrUltimoEmpleado);
        str.append("\n\n Con Fecha : \n\n");
        str.append(GstrUltimaFecha);
        str.append(" ?");

        if(QMessageBox::question(this, GstrTitulo, str, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes){

            //
            // Guardamos el parte nuevo, Rellenamos la estructura s_horasMquina, para luego restar las horas
            //
            strSql = "SELECT *FROM PartesDiarios";
            sql.exec(strSql);
            sql.first();
            i=0;
            while (sql.isValid()) {

                strCodEmpleadoDbCod = sql.record().value("Empleado").toString();
                strFechaDbCod       = sql.record().value("Fecha").toString();

                strCodEmpleadoDb    = pFuncAux->funDesCifrar_bis(strCodEmpleadoDbCod);
                strFechaDb          = pFuncAux->funDesCifrar_bis(strFechaDbCod);
                strFacturableDb     = pFuncAux->funDesCifrar_bis(sql.record().value("Facturable").toString());

                if(strFecha == strFechaDb && strCodEmpleado == strCodEmpleadoDb){

                    hRestar[i].strCodEmpleadoCod   = strCodEmpleadoDbCod;
                    hRestar[i].strFechaCod         = strFechaDbCod;

                    if(strFacturableDb == "Si"){

                        strCodMaquinaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());
                        strTiempoDb     = pFuncAux->funDesCifrar_bis(sql.record().value("Tiempo").toString());

                        hRestar[i].strCodMaquina   = strCodMaquinaDb;
                        hRestar[i].dHoras          = strTiempoDb.toDouble();
                    }
                    else{

                        hRestar[i].strCodMaquina   = "";
                        hRestar[i].dHoras          = 0.0;
                    }
                    i++;
                }
                sql.next();
            }

            //
            // Tenemos en horasRestar todos los valores, restamos las horas y borramos el registro
            //
            i=0;
            while(i<iRegDb){

                //
                // Restamos las Horas, solo si tienen cod de Maquina
                //
                if(hRestar[i].strCodMaquina != ""){
                    strHoras = pFuncAux->funHorasToMaquina(hRestar[i].strCodMaquina);
                    dHoras   = strHoras.toDouble();
                    dHoras   = dHoras - hRestar[i].dHoras;
                    strHoras = QString::number(dHoras);

                    funGrabarHorasMaquina(hRestar[i].strCodMaquina, strHoras);
                }

                //
                // Borramos el registro
                //
                strSql = "DELETE FROM PartesDiarios WHERE Fecha='" + hRestar[i].strFechaCod + "' And Empleado='" + hRestar[i].strCodEmpleadoCod + "';";
                sql.exec(strSql);

                i++;
            }

            //
            // Ahora tenemos que guardar cada linea del tlPartes en la BD, y ademas sumar las horas
            //
            iFila=0;
            while (iFila < ui->tlPartes->rowCount()) {

                //
                // Comprobamos si es facturable
                //
                if(ui->tlPartes->item(iFila, 4)->text() == "Si"){
                    strFacturable = "Si";
                }
                else{
                    strFacturable = "No";
                }
                //
                // Guardamos cada fila, fecha y empleado fijos los demas pueden variar, tanto empresa como maquina
                //
                strCodEmpresa       = pFuncAux->funCodigoToEmpresa(ui->tlPartes->item(iFila,0)->text());
                strCodMaquina       = pFuncAux->funCodigoToMaquina(ui->tlPartes->item(iFila,1)->text());
                strFecha            = GstrUltimaFecha;
                strCodEmpleado      = pFuncAux->funCodigoToEmpleado(GstrUltimoEmpleado);

                //
                // Codificamos los valores
                //
                strFechaCod         = pFuncAux->funCifrar_bis(strFecha);
                strCodEmpresaCod    = pFuncAux->funCifrar_bis(strCodEmpresa);
                strCodEmpleadoCod   = pFuncAux->funCifrar_bis(strCodEmpleado);
                strCodMaquinaCod    = pFuncAux->funCifrar_bis(strCodMaquina);
                strTrabajoCod       = pFuncAux->funCifrar_bis(ui->tlPartes->item(iFila, 2)->text());
                strTiempoCod        = pFuncAux->funCifrar_bis(ui->tlPartes->item(iFila, 3)->text());
                strFacturableCod    = pFuncAux->funCifrar_bis(strFacturable);

                strSql = " INSERT INTO PartesDiarios(Fecha,"
                         "                           Empresa,"
                         "                           Empleado,"
                         "                           Maquina,"
                         "                           Trabajo,"
                         "                           Tiempo,"
                         "                           Facturable) VALUES ('" + strFechaCod + "',"
                         "                                               '" + strCodEmpresaCod + "',"
                         "                                               '" + strCodEmpleadoCod + "',"
                         "                                               '" + strCodMaquinaCod + "',"
                         "                                               '" + strTrabajoCod + "',"
                         "                                               '" + strTiempoCod + "',"
                         "                                               '" + strFacturableCod + "');";
                sql.exec(strSql);

                //
                // Sumamos las horas
                //
                if(strFacturable == "Si"){

                    strHoras    = pFuncAux->funHorasToMaquina(strCodMaquina);
                    dHoras      = strHoras.toDouble();
                    dHoras      = dHoras + ui->tlPartes->item(iFila, 3)->text().toDouble();
                    strHoras    = QString::number(dHoras);
                    funGrabarHorasMaquina(strCodMaquina, strHoras);

                }


                iFila++;
            }
        }
        //
        // Se aviso de que el parte esta modificado , pero no lo quiere grabar, No hacemos nada
        //
        else{

        }
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql12");

    //
    // Cambiamos la fecha de lblCalendario
    //
    ui->lblCalendario->setSelectedDate(pFuncAux->funStrToDate(ui->txtFecha->text()));

    //
    // Rellenamos el listado con la nueva fecha
    //
    funRellenaListado(ui->txtFecha->text(), ui->cmbEmpleado->currentText());

    //
    // Colocamos el titulo de la tabla
    //
    str = "Parte Diario de :\t ";
    str.append(pFuncAux->funNombreToCodEmpleado(pFuncAux->funCodigoToEmpleado(GstrUltimoEmpleado)));
    str.append("\t\t Fecha:\t");
    qdFecha =  pFuncAux->funStrToDate(ui->txtFecha->text());
    str.append(qlTraductor.toString(qdFecha,"dddd, dd MMMM yyyy"));

    ui->lblTitulo->setText(str);

    delete pFuncAux;

}

void frmGesPartesDiarios::on_cmdVolver_clicked(){
    QString strRutaDb, strSql;
    QString strCodEmpleado, strCodEmpleadoDb, strFechaDb;
    int     registros, filas;

    FuncAux* pFuncAux = new FuncAux();

    filas = ui->tlPartes->rowCount();

    //
    // Comprobamos si las filas de la tabla estan guardadas
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql15");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    //
    //Comprobamos cuantos registros hay en la base de datos
    //
    strSql = "SELECT *FROM PartesDiarios;";
    sql.exec(strSql);
    sql.first();
    registros = 0;
    strCodEmpleado = pFuncAux->funCodigoToEmpleado(ui->cmbEmpleado->currentText());

    while (sql.isValid()) {

        strCodEmpleadoDb    = pFuncAux->funDesCifrar_bis(sql.record().value("Empleado").toString());
        strFechaDb          = pFuncAux->funDesCifrar_bis(sql.record().value("Fecha").toString());

        if(strCodEmpleadoDb == strCodEmpleado &&
           strFechaDb == ui->txtFecha->text() ){

            registros++;
        }
        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql15");

    if(Gprimer_paso){

        //
        // Hago esto por que la ejecucion pasa dos veces por aqui
        //
        Gprimer_paso = false;

        //
        // Si los registros no coinciden con las filas del tablet avisamos
        //
        if(filas != registros){

            //
            // Si responde si, llamamos a cmdGuardar, si dice no, no hacemos nada, borramos la tabla y salimos
            //
            QString str = "Este parte esta sin guardar...\n\n";
            str.append("quieres guardar este parte ???");
            if(QMessageBox::Yes == QMessageBox::question(this, GstrTitulo, str, QMessageBox::Yes | QMessageBox::No)){
                on_cmdGuardar_clicked();
            }
        }

    }

    //
    //Borramos la tabla
    //
    while (ui->tlPartes->rowCount()>0) {
        ui->tlPartes->removeRow(0);
    }

    //
    // cerramos
    //
    this->close();

    delete pFuncAux;

}

void frmGesPartesDiarios::on_cmdGuardar_clicked(){
    QString     strRutaDb, strSql;
    QString     strCodEmpleado, strFecha, strCodMaquina, strCodEmpresa, strFacturable;
    QString     strCodEmpleadoDb, strFechaDb, strFacturableDb, strTiempoDb, strCodMaquinaDb;
    QString     strCodEmpleadoDbCod, strFechaDbCod;
    QString     strFechaCod, strCodEmpleadoCod, strCodMaquinaCod, strCodEmpresaCod;
    QString     strTrabajoCod, strTiempoCod, strFacturableCod;
    QString     strHoras;
    QDate       datFecha;
    QLocale     qlTraductor;
    int         iRegDb, i, iFila;
    double      dHoras;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    //
    // Obtenemos fecha y empleado que marcan la exclusividad
    //
    strCodEmpleado  = pFuncAux->funCodigoToEmpleado(ui->cmbEmpleado->currentText());
    strFecha        = ui->txtFecha->text();

    //
    // Comprobamos si existe en la BD parte de esa fecha y ese empleado y obtenemos los registros de ese parte
    //
    iRegDb = 0;

    strSql = "SELECT *FROM PartesDiarios";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodEmpleadoDb    = pFuncAux->funDesCifrar_bis(sql.record().value("Empleado").toString());
        strFechaDb          = pFuncAux->funDesCifrar_bis(sql.record().value("Fecha").toString());

        if(strFecha == strFechaDb && strCodEmpleado == strCodEmpleadoDb){

            iRegDb++;
        }

        sql.next();
    }

    //
    // Inicializo la estructura s_horasMaquina
    //
    s_HorasMaquina horasMaquinaRestar[iRegDb];

    //
    // Si existe parte, en la BD, Guardamos en horasMaquinaRestar los valores de cada linea, para luego borrarla
    //
    i=0;
    if(iRegDb > 0){

        sql.first();
        while (sql.isValid()) {

            strCodEmpleadoDbCod = sql.record().value("Empleado").toString();
            strFechaDbCod       = sql.record().value("Fecha").toString();

            strCodEmpleadoDb    = pFuncAux->funDesCifrar_bis(strCodEmpleadoDbCod);
            strFechaDb          = pFuncAux->funDesCifrar_bis(strFechaDbCod);
            strFacturableDb     = pFuncAux->funDesCifrar_bis(sql.record().value("Facturable").toString());

            if(strFecha == strFechaDb && strCodEmpleado == strCodEmpleadoDb){

                horasMaquinaRestar[i].strCodEmpleadoCod   = strCodEmpleadoDbCod;
                horasMaquinaRestar[i].strFechaCod         = strFechaDbCod;

                if(strFacturableDb == "Si"){

                    strCodMaquinaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());
                    strTiempoDb     = pFuncAux->funDesCifrar_bis(sql.record().value("Tiempo").toString());

                    horasMaquinaRestar[i].strCodMaquina   = strCodMaquinaDb;
                    horasMaquinaRestar[i].dHoras          = strTiempoDb.toDouble();
                }
                else{

                    horasMaquinaRestar[i].strCodMaquina   = "";
                    horasMaquinaRestar[i].dHoras          = 0.0;
                }
                i++;
            }

            sql.next();
        }
    }

    //
    // Tenemos en horasMaquina todos los valores, restamos las horas y borramos el registro
    //
    i=0;
    while(i<iRegDb){

        //
        // Restamos las Horas, solo si tienen cod de Maquina
        //
        if(horasMaquinaRestar[i].strCodMaquina != ""){
            strHoras = pFuncAux->funHorasToMaquina(horasMaquinaRestar[i].strCodMaquina);
            dHoras   = strHoras.toDouble();
            dHoras   = dHoras - horasMaquinaRestar[i].dHoras;
            strHoras = QString::number(dHoras);

            funGrabarHorasMaquina(horasMaquinaRestar[i].strCodMaquina, strHoras);
        }

        //
        // Borramos el registro
        //
        strSql = "DELETE FROM PartesDiarios WHERE Fecha='" + horasMaquinaRestar[i].strFechaCod + "' And Empleado='" + horasMaquinaRestar[i].strCodEmpleadoCod + "';";
        sql.exec(strSql);

        i++;
    }

    //
    // Ahora tenemos que guardar cada linea del tlPartes en la BD, y ademas sumar las horas
    //
    iFila=0;
    while (iFila < ui->tlPartes->rowCount()) {

        //
        // Comprobamos si es facturable
        //
        if(ui->tlPartes->item(iFila, 4)->text() == "Si"){
            strFacturable = "Si";
        }
        else{
            strFacturable = "No";
        }
        //
        // Guardamos cada fila, fecha y empleado fijos los demas pueden variar, tanto empresa como maquina
        //
        strCodEmpresa       = pFuncAux->funCodigoToEmpresa(ui->tlPartes->item(iFila,0)->text());
        strCodMaquina       = pFuncAux->funCodigoToMaquina(ui->tlPartes->item(iFila,1)->text());
        strFecha            = ui->txtFecha->text();
        strCodEmpleado      = pFuncAux->funCodigoToEmpleado(ui->cmbEmpleado->currentText());

        //
        // Codificamos los valores
        //
        strFechaCod         = pFuncAux->funCifrar_bis(strFecha);
        strCodEmpresaCod    = pFuncAux->funCifrar_bis(strCodEmpresa);
        strCodEmpleadoCod   = pFuncAux->funCifrar_bis(strCodEmpleado);
        strCodMaquinaCod    = pFuncAux->funCifrar_bis(strCodMaquina);
        strTrabajoCod       = pFuncAux->funCifrar_bis(ui->tlPartes->item(iFila, 2)->text());
        strTiempoCod        = pFuncAux->funCifrar_bis(ui->tlPartes->item(iFila, 3)->text());
        strFacturableCod    = pFuncAux->funCifrar_bis(strFacturable);

        strSql = " INSERT INTO PartesDiarios(Fecha,"
                 "                           Empresa,"
                 "                           Empleado,"
                 "                           Maquina,"
                 "                           Trabajo,"
                 "                           Tiempo,"
                 "                           Facturable) VALUES ('" + strFechaCod + "',"
                 "                                               '" + strCodEmpresaCod + "',"
                 "                                               '" + strCodEmpleadoCod + "',"
                 "                                               '" + strCodMaquinaCod + "',"
                 "                                               '" + strTrabajoCod + "',"
                 "                                               '" + strTiempoCod + "',"
                 "                                               '" + strFacturableCod + "');";
        sql.exec(strSql);

        //
        // Sumamos las horas
        //
        if(strFacturable == "Si"){

            strHoras    = pFuncAux->funHorasToMaquina(strCodMaquina);
            dHoras      = strHoras.toDouble();
            dHoras      = dHoras + ui->tlPartes->item(iFila, 3)->text().toDouble();
            strHoras    = QString::number(dHoras);
            funGrabarHorasMaquina(strCodMaquina, strHoras);

        }


        iFila++;
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");


    //
    // Borro toda la tabla y los txt
    //
    while(ui->tlPartes->rowCount() > 0){
        ui->tlPartes->removeRow(0);
    }

    //
    // Colocamos la fecha actual en txtFecha
    //
    datFecha = QDate::currentDate();
    ui->txtFecha->setText(qlTraductor.toString(datFecha,"dd/MM/yyyy"));

    //
    // Relleno el listado
    //
    funRellenaListado(ui->txtFecha->text(), ui->cmbEmpleado->currentText());

    ui->txtDescripcion->setText("");
    ui->txtDuracion->setText("");
    ui->txtDescripcion->setFocus();

    //
    // Selecciono la fecha en lblCalendario
    //
    ui->lblCalendario->setSelectedDate(pFuncAux->funStrToDate(ui->txtFecha->text()));

    delete pFuncAux;

}

void frmGesPartesDiarios::on_cmdAnadir_clicked(){
    QString strFacturable, strEmpresa, strMaquina;
    int     fila = 0;
    bool    insertar = true;

    //
    // Si los dos campos estan cubiertos miramos si editamos o si insertamos
    //
    if( ui->txtDescripcion->text() != "" && ui->txtDuracion->text() != ""){

        if(Gfila_seleccionada == -1){
            insertar = true;
        }
        else{
            insertar = false;
        }

        //
        // Si insertar = true añadimos fila, sino editamos fila
        //
        if(insertar){
            fila = ui->tlPartes->rowCount();
            ui->tlPartes->setRowCount(fila + 1);
        }
        //
        // Editamos la fila seleccionada
        //
        else{
            fila = Gfila_seleccionada;
        }

        //
        // Si chkFacturable esta seleccionado si sino No
        //
        if(ui->chkFacturable->isChecked()){
            strFacturable = "Si";
        }
        else{
            strFacturable = "No";
        }

        QTableWidgetItem *item_empresa = new QTableWidgetItem(ui->cmbEmpresa->currentText());
        QTableWidgetItem *item_maquina = new QTableWidgetItem(ui->cmbMaquina->currentText());
        QTableWidgetItem *item_trabajo = new QTableWidgetItem(ui->txtDescripcion->text());
        QTableWidgetItem *item_duracion = new QTableWidgetItem(ui->txtDuracion->text());
        QTableWidgetItem *item_facturable = new QTableWidgetItem(strFacturable);

        item_empresa->setTextAlignment(Qt::AlignCenter);
        item_maquina->setTextAlignment(Qt::AlignCenter);
        item_trabajo->setTextAlignment(Qt::AlignCenter);
        item_duracion->setTextAlignment(Qt::AlignCenter);
        item_facturable->setTextAlignment(Qt::AlignCenter);

        ui->tlPartes->setItem(fila, 0, item_empresa);
        ui->tlPartes->setItem(fila, 1, item_maquina);
        ui->tlPartes->setItem(fila, 2, item_trabajo);
        ui->tlPartes->setItem(fila, 3, item_duracion);
        ui->tlPartes->setItem(fila, 4, item_facturable);

        ui->txtDescripcion->setText("");
        ui->txtDuracion->setText("");
        ui->chkFacturable->setChecked(true);
        ui->cmdAnadir->setText("Añadir");

        //
        // si rowcout > 0 cmdGuardar.setenabled true
        //
        if(ui->tlPartes->rowCount() > 0){
            ui->cmdGuardar->setEnabled(true);
        }

        //
        // Deseleccionamos la fila seleccionada y marcamos filaseleccionada = -1
        //
        ui->tlPartes->clearSelection();
        Gfila_seleccionada = -1;
    }

    //
    // Si alguno de los campos esta vacío lo enviamos a ese campo
    //
    if(ui->txtDescripcion->text() == ""){
        ui->txtDescripcion->setFocus();
    }
    if(ui->txtDescripcion->text() != "" && ui->txtDuracion->text() == ""){
        ui->txtDuracion->setFocus();
    }


}

void frmGesPartesDiarios::on_cmdSuprimir_clicked(){

    if(Gfila_seleccionada >= 0){
        ui->tlPartes->removeRow(Gfila_seleccionada);
        ui->cmdSuprimir->setEnabled(false);
        ui->cmdAnadir->setEnabled(true);
        Gfila_seleccionada = -1;
        ui->tlPartes->clearSelection();
        ui->txtDescripcion->setFocus();
    }

}

void frmGesPartesDiarios::on_tbFecha_clicked(){
    frmCalendario *pCalendario = new frmCalendario(this);

    pCalendario->setWindowTitle(GstrTitulo);
    pCalendario->setWindowFlag(Qt::CustomizeWindowHint);
    pCalendario->setWindowModality(Qt::WindowModal);

    pCalendario->exec();

    if(pCalendario->strFecha != ""){
        GstrUltimaFecha = ui->txtFecha->text();
        ui->txtFecha->setText(pCalendario->strFecha);
        funProcesaFecha();
    }

    delete pCalendario;
}

void frmGesPartesDiarios::on_cmbEmpleado_currentTextChanged(const QString &arg1){

    ui->txtDescripcion->setFocus();
    ui->txtDescripcion->selectAll();
}

void frmGesPartesDiarios::on_cmbMaquina_currentTextChanged(const QString &arg1){

    ui->txtDescripcion->setFocus();
    ui->txtDescripcion->selectAll();
}

void frmGesPartesDiarios::on_cmbEmpresa_currentTextChanged(const QString &arg1){

    ui->txtDescripcion->setFocus();
    ui->txtDescripcion->selectAll();
}

void frmGesPartesDiarios::on_txtDescripcion_returnPressed(){

    ui->txtDuracion->setFocus();
    ui->txtDuracion->selectAll();
}

void frmGesPartesDiarios::on_txtDuracion_returnPressed(){

    on_cmdAnadir_clicked();
}

void frmGesPartesDiarios::on_txtDuracion_textChanged(const QString &arg1){
    QString str = "";
    double  numero = 0;

    //
    // Lo pasamos a double
    //
    str     = arg1;
    numero  = str.toDouble();

    //
    // Si su longitud es 1, solo puede ser str == 0
    //
    if(str.length() == 1){
        if(str != "0"){
            if(numero == 0){
                str.remove(str.length() - 1, 1);
                ui->txtDuracion->setText(str);
            }
        }

    }

    //
    // Si su longitud es dos solo permito str == 0.
    if(str.length() == 2){
        if(str != "0."){
            if(numero == 0){
                str.remove(str.length() - 1, 1);
                ui->txtDuracion->setText(str);
            }
        }
    }

    //
    // S i su longitud es mayor de 2 solo si es numero
    //
    if(str.length() > 2 && numero == 0){
        str.remove(str.length() - 1, 1);
        ui->txtDuracion->setText(str);
    }

}

void frmGesPartesDiarios::on_chkFacturable_stateChanged(int arg1){

    ui->txtDescripcion->setFocus();
    ui->txtDescripcion->selectAll();
}

void frmGesPartesDiarios::on_tlPartes_itemClicked(QTableWidgetItem *item){

    ui->cmdSuprimir->setEnabled(true);
    ui->cmdAnadir->setEnabled(false);

    Gfila_seleccionada = item->row();
}

void frmGesPartesDiarios::on_tlPartes_itemDoubleClicked(QTableWidgetItem *item){
    QString strRutaFoto;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Debo editar la fila seleccionada
    //
    Gfila_seleccionada = item->row();

    ui->txtDescripcion->setText(ui->tlPartes->item(Gfila_seleccionada, 2)->text());
    ui->txtDuracion->setText(ui->tlPartes->item(Gfila_seleccionada, 3)->text());
    if(ui->tlPartes->item(Gfila_seleccionada, 4)->text() == "Si"){
        ui->chkFacturable->setChecked(true);
    }
    else{
        ui->chkFacturable->setChecked(false);
    }

    ui->cmbMaquina->setCurrentText(ui->tlPartes->item(Gfila_seleccionada, 1)->text());
    strRutaFoto = pFuncAux->funRutaFotoMaquinaToCodigo(pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText()));
    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, strRutaFoto);

    ui->cmbEmpresa->setCurrentText(ui->tlPartes->item(Gfila_seleccionada, 0)->text());

    ui->cmdSuprimir->setEnabled(false);
    ui->cmdAnadir->setEnabled(true);
    ui->cmdAnadir->setText("Modificar");

    ui->txtDescripcion->setFocus();
    ui->txtDescripcion->selectAll();

    delete pFuncAux;
}

void frmGesPartesDiarios::on_cmbEmpleado_activated(int index){
    QString strRutaDb, strSql, strFacturable, str;
    QString strCodEmpresa, strCodMaquina, strCodEmpleado, strRutaFoto;
    QString strCodEmpleadoDb, strFechaDb, strFacturableDb, strEmpleadoDb, strCodMaquinaDb, strTiempoDb;
    QString strCodEmpresaCod, strCodMaquinaCod, strCodEmpleadoCod, strFechaCod;
    QString strTrabajoCod, strTiempoCod, strFacturableCod, strFechaDbCod, strCodEmpleadoDbCod;
    QString strFecha, strHoras;
    QLocale qlTraductor;
    QDate   qdFecha;
    int     iRegTabla, iRegDb, iFila, i;
    double  dHoras;
    bool    bParteGuardado;

    FuncAux* pFuncAux = new FuncAux();


    if(index >= 0 && ui->txtFecha->text() != ""){

        //
        // En primer lugar miramos si existe parte en la tabla, y cuantas lineas tiene
        //
        iRegTabla = 0;
        iRegTabla = ui->tlPartes->rowCount();

        //
        // Colocamos Fecha y Empleado, que marcan la exclusividad
        //
        strFecha        = ui->txtFecha->text();
        strCodEmpleado  = pFuncAux->funCodigoToEmpleado(GstrUltimoEmpleado);

        //
        //Ahora Comprobamos si existe parte de ese empleado y ese dia en la BD, y cuantos Registros Tiene
        //
        strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
        QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql12");
        dbSql.setDatabaseName(strRutaDb);
        dbSql.open();
        QSqlQuery sql = QSqlQuery(dbSql);

        strSql = "SELECT *FROM PartesDiarios";
        sql.exec(strSql);
        sql.first();
        iRegDb = 0;
        while (sql.isValid()) {

            strCodEmpleadoDb    = pFuncAux->funDesCifrar_bis(sql.record().value("Empleado").toString());
            strFechaDb          = pFuncAux->funDesCifrar_bis(sql.record().value("Fecha").toString());

            if(strFecha == strFechaDb && strCodEmpleado == strCodEmpleadoDb){

                iRegDb++;
            }

            sql.next();
        }

        //
        // Si iRegTabla != 0 o bien iRegBd !=, tenemos que comprobar si la tabla esta guardada en la BD
        //
        bParteGuardado = true;
        if(iRegTabla > 0 || iRegDb > 0){
            if(iRegDb == iRegTabla){
                bParteGuardado = funParteGuardado(strFecha, strCodEmpleado);
            }
            else{
                bParteGuardado = false;
            }

        }

        s_HorasMaquina hRestar[iRegDb];

        //
        // Si bParteGuardado es false, avisamos
        //
        if(!bParteGuardado){

            //
            // Avisamos que el parte, aunque guardado esta modificado
            //
            str = " Este Parte, Ha sido Modificado...\n\n¿Quieres Guardar el Nuevo Parte de :\n\n";
            str.append(GstrUltimoEmpleado);
            str.append("\n\n Con Fecha : \n\n");
            str.append(strFecha);
            str.append(" ?");

            if(QMessageBox::question(this, GstrTitulo, str, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes){

                //
                // Guardamos el parte nuevo, Rellenamos la estructura s_horasMquina, para luego restar las horas
                //
                strSql = "SELECT *FROM PartesDiarios";
                sql.exec(strSql);
                sql.first();
                i=0;
                while (sql.isValid()) {

                    strCodEmpleadoDbCod = sql.record().value("Empleado").toString();
                    strFechaDbCod       = sql.record().value("Fecha").toString();

                    strCodEmpleadoDb    = pFuncAux->funDesCifrar_bis(strCodEmpleadoDbCod);
                    strFechaDb          = pFuncAux->funDesCifrar_bis(strFechaDbCod);
                    strFacturableDb     = pFuncAux->funDesCifrar_bis(sql.record().value("Facturable").toString());

                    if(strFecha == strFechaDb && strCodEmpleado == strCodEmpleadoDb){

                        hRestar[i].strCodEmpleadoCod   = strCodEmpleadoDbCod;
                        hRestar[i].strFechaCod         = strFechaDbCod;

                        if(strFacturableDb == "Si"){

                            strCodMaquinaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());
                            strTiempoDb     = pFuncAux->funDesCifrar_bis(sql.record().value("Tiempo").toString());

                            hRestar[i].strCodMaquina   = strCodMaquinaDb;
                            hRestar[i].dHoras          = strTiempoDb.toDouble();
                        }
                        else{

                            hRestar[i].strCodMaquina   = "";
                            hRestar[i].dHoras          = 0.0;
                        }
                        i++;
                    }
                    sql.next();
                }

                //
                // Tenemos en horasRestar todos los valores, restamos las horas y borramos el registro
                //
                i=0;
                while(i<iRegDb){

                    //
                    // Restamos las Horas, solo si tienen cod de Maquina
                    //
                    if(hRestar[i].strCodMaquina != ""){
                        strHoras = pFuncAux->funHorasToMaquina(hRestar[i].strCodMaquina);
                        dHoras   = strHoras.toDouble();
                        dHoras   = dHoras - hRestar[i].dHoras;
                        strHoras = QString::number(dHoras);

                        funGrabarHorasMaquina(hRestar[i].strCodMaquina, strHoras);
                    }

                    //
                    // Borramos el registro
                    //
                    strSql = "DELETE FROM PartesDiarios WHERE Fecha='" + hRestar[i].strFechaCod + "' And Empleado='" + hRestar[i].strCodEmpleadoCod + "';";
                    sql.exec(strSql);

                    i++;
                }

                //
                // Ahora tenemos que guardar cada linea del tlPartes en la BD, y ademas sumar las horas
                //
                iFila=0;
                while (iFila < ui->tlPartes->rowCount()) {

                    //
                    // Comprobamos si es facturable
                    //
                    if(ui->tlPartes->item(iFila, 4)->text() == "Si"){
                        strFacturable = "Si";
                    }
                    else{
                        strFacturable = "No";
                    }
                    //
                    // Guardamos cada fila, fecha y empleado fijos los demas pueden variar, tanto empresa como maquina
                    //
                    strCodEmpresa       = pFuncAux->funCodigoToEmpresa(ui->tlPartes->item(iFila,0)->text());
                    strCodMaquina       = pFuncAux->funCodigoToMaquina(ui->tlPartes->item(iFila,1)->text());
                    strCodEmpleado      = pFuncAux->funCodigoToEmpleado(GstrUltimoEmpleado);

                    //
                    // Codificamos los valores
                    //
                    strFechaCod         = pFuncAux->funCifrar_bis(strFecha);
                    strCodEmpresaCod    = pFuncAux->funCifrar_bis(strCodEmpresa);
                    strCodEmpleadoCod   = pFuncAux->funCifrar_bis(strCodEmpleado);
                    strCodMaquinaCod    = pFuncAux->funCifrar_bis(strCodMaquina);
                    strTrabajoCod       = pFuncAux->funCifrar_bis(ui->tlPartes->item(iFila, 2)->text());
                    strTiempoCod        = pFuncAux->funCifrar_bis(ui->tlPartes->item(iFila, 3)->text());
                    strFacturableCod    = pFuncAux->funCifrar_bis(strFacturable);

                    strSql = " INSERT INTO PartesDiarios(Fecha,"
                             "                           Empresa,"
                             "                           Empleado,"
                             "                           Maquina,"
                             "                           Trabajo,"
                             "                           Tiempo,"
                             "                           Facturable) VALUES ('" + strFechaCod + "',"
                             "                                               '" + strCodEmpresaCod + "',"
                             "                                               '" + strCodEmpleadoCod + "',"
                             "                                               '" + strCodMaquinaCod + "',"
                             "                                               '" + strTrabajoCod + "',"
                             "                                               '" + strTiempoCod + "',"
                             "                                               '" + strFacturableCod + "');";
                    sql.exec(strSql);

                    //
                    // Sumamos las horas
                    //
                    if(strFacturable == "Si"){

                        strHoras    = pFuncAux->funHorasToMaquina(strCodMaquina);
                        dHoras      = strHoras.toDouble();
                        dHoras      = dHoras + ui->tlPartes->item(iFila, 3)->text().toDouble();
                        strHoras    = QString::number(dHoras);
                        funGrabarHorasMaquina(strCodMaquina, strHoras);

                    }


                    iFila++;
                }
            }
            //
            // Se aviso de que el parte esta modificado , pero no lo quiere grabar, No hacemos nada
            //
            else{

            }
        }

        //
        // Cerramos la BD
        //
        dbSql.close();
        dbSql.removeDatabase("conSql12");

        //
        // Obtenemos la ruta de la foto del empleado actual
        //
        strRutaFoto = pFuncAux->funRutaFotoEmpleadoToCodigo(pFuncAux->funCodigoToEmpleado(ui->cmbEmpleado->currentText()));

        //
        // Establecemos ui.cmbEmpleado.currenteText como ultimo empleado y rellenamos el tablet con el nuevo empleado
        //
        GstrUltimoEmpleado = ui->cmbEmpleado->currentText();
        funRellenaListado(ui->txtFecha->text(), ui->cmbEmpleado->currentText());

        //
        // Colocamos el titulo de la tabla
        //
        str = "Parte Diario de :\t ";
        str.append(pFuncAux->funNombreToCodEmpleado(pFuncAux->funCodigoToEmpleado(GstrUltimoEmpleado)));
        str.append("\t\t Fecha:\t");
        qdFecha =  pFuncAux->funStrToDate(ui->txtFecha->text());
        str.append(qlTraductor.toString(qdFecha,"dddd, dd MMMM yyyy"));

        ui->lblTitulo->setText(str);

        //
        // Actualizamos la foto del empleado
        //
        pFuncAux->funMostrarFoto(ui->lblFotoEmpleado, strRutaFoto);

    }

    delete pFuncAux;

}

void frmGesPartesDiarios::on_txtFecha_returnPressed(){
    QDate   datFecha, datFechaSeleccionada;
    QLocale qlTraductor;

    FuncAux* pFuncAux = new FuncAux();

    datFecha = pFuncAux->funStrToDate(ui->txtFecha->text());

    //
    // Si la fecha es valida
    //
    if(datFecha.isValid()){

        datFechaSeleccionada    = ui->lblCalendario->selectedDate();
        GstrUltimaFecha         = qlTraductor.toString(datFechaSeleccionada,"dd/MM/yyyy");

        funProcesaFecha();

    }
    //
    // La fecha no es valida
    //
    else{
        QMessageBox::information(this, GstrTitulo, "\t Formato de fecha no reconocido...");

        datFecha = QDate::currentDate();
        ui->txtFecha->setText(qlTraductor.toString(datFecha,"dd/MM/yyyy"));
        ui->lblCalendario->setSelectedDate(datFecha);

        funRellenaListado(ui->txtFecha->text(), ui->cmbEmpleado->currentText());

        ui->txtFecha->setFocus();
        ui->txtFecha->selectAll();
    }


    delete pFuncAux;
}

void frmGesPartesDiarios::on_lblCalendario_clicked(const QDate &date){
    QString strFecha;
    QLocale qlTraductor;

    strFecha = qlTraductor.toString(date,"dd/MM/yyyy");


    GstrUltimaFecha = ui->txtFecha->text();
    ui->txtFecha->setText(strFecha);
    funProcesaFecha();
}

void frmGesPartesDiarios::on_cmbMaquina_activated(int index){
    QString strRutaDb, strSql;
    QString strRutaFoto, strCodMaquina;

    FuncAux* pFuncAux = new FuncAux();

    strCodMaquina   = pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText());
    strRutaFoto     = pFuncAux->funRutaFotoMaquinaToCodigo(strCodMaquina);

    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, strRutaFoto);

    delete pFuncAux;

}

