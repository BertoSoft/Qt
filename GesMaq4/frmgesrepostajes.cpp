#include "frmgesrepostajes.h"
#include "ui_frmgesrepostajes.h"

#include "frmcalendario.h"
#include "funcaux.h"


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QMessageBox>
#include <QDate>
#include <QScreen>
#include <QImageReader>

frmGesRepostajes::frmGesRepostajes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmGesRepostajes){

    ui->setupUi(this);

    this->installEventFilter(this);
}

frmGesRepostajes::~frmGesRepostajes(){
    delete ui;
}

bool frmGesRepostajes::eventFilter(QObject *obj, QEvent *event){

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

void frmGesRepostajes::funInicio(){
    QString strRutaDb, strSql, str;
    QLocale qlTraductor;
    QString strCodigo, strRutaFoto;
    int ancho_src, alto_src;
    int ancho_form, alto_form;
    int ix0, iy0;

    FuncAux* pFuncAux = new FuncAux();

    /*
    //
    // Dimensionamos la pantalla al 70% del screen
    //
    QScreen *src = QGuiApplication::screens().at(0);

    ancho_src = src->availableGeometry().width();
    alto_src  = src->availableGeometry().height();

    ancho_form  = (int) ancho_src * 0.90;
    alto_form   = (int) alto_src * 0.50;

    ix0         = (int) ancho_src * 0.45;
    iy0         = (int) alto_src * 0.25;

    this->setGeometry(ix0, iy0, ancho_form, alto_form);

    */

    //
    // Colocamos las columnas del tablelist
    //
    ui->tlRepostajes->setColumnCount(6);

    QStringList headers;
    headers += tr("Fecha");
    headers += tr("Empresa");
    headers += tr("Empleado");
    headers += tr("Máquina");
    headers += tr("Carburante");
    headers += tr("Litros");


    ui->tlRepostajes->setHorizontalHeaderLabels(headers);

    ui->tlRepostajes->setColumnWidth(0, (int) ( ( this->width() * 0.6) / 6 ) );
    ui->tlRepostajes->setColumnWidth(1, (int) ( ( this->width() * 1.2) / 6 ) );
    ui->tlRepostajes->setColumnWidth(2, (int) ( ( this->width() * 1.2) / 6 ) );
    ui->tlRepostajes->setColumnWidth(3, (int) ( ( this->width() * 1.2) / 6 ) );
    ui->tlRepostajes->setColumnWidth(4, (int) ( ( this->width() * 1) / 6 ) );
    ui->tlRepostajes->setColumnWidth(5, (int) ( ( this->width() * 0.6) / 6 ) );


    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql22");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    //
    //Rellenamos el cmbEmpresas
    //
    while(ui->cmbEmpresas->count()>0){
        ui->cmbEmpresas->removeItem(0);
    }
    strSql = "SELECT *FROM Empresas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodigo = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());

        str = pFuncAux->funTxtToEmpresas(strCodigo);
        ui->cmbEmpresas->addItem(str);

        sql.next();
    }

    //
    // Rellenamos el tipo carburante
    //
    while (ui->cmbCombustible->count()>0) {
        ui->cmbCombustible->removeItem(0);
    }
    ui->cmbCombustible->addItem("Gas_Oil B");
    ui->cmbCombustible->addItem("Gas_Oil A");
    ui->cmbCombustible->addItem("Gasolina 95");
    ui->cmbCombustible->addItem("Gasolina 98");

    //
    // Rellenamos el cmbEmpleados
    //
    while(ui->cmbEmpleados->count()>0){
        ui->cmbEmpleados->removeItem(0);
    }
    strSql = "SELECT *FROM Empleados";
    sql.exec(strSql);
    sql.first();

    //
    // Colocamos la foto del primer registro
    //
    strRutaFoto = pFuncAux->funDesCifrar_bis(sql.record().value("Foto").toString());
    pFuncAux->funMostrarFoto(ui->lblFotoEmpleado, strRutaFoto);

    while (sql.isValid()) {

        strCodigo = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());

        str = pFuncAux->funTxtToEmpleados(strCodigo);
        ui->cmbEmpleados->addItem(str);

        sql.next();
    }

    //
    // Rellenamos el cmbMaquinas
    //
    while(ui->cmbMaquinas->count()>0){
        ui->cmbMaquinas->removeItem(0);
    }
    strSql = "SELECT *FROM Maquinas";
    sql.exec(strSql);
    sql.first();

    //
    // Colocamos la foto del primer registro
    //
    strRutaFoto   = pFuncAux->funDesCifrar_bis(sql.record().value("Foto").toString());
    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, strRutaFoto);

    while (sql.isValid()) {

        strCodigo   = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());

        str = pFuncAux->funTxtToMaquinas(strCodigo);
        ui->cmbMaquinas->addItem(str);
        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql22");

    delete pFuncAux;

    //
    // Rellenamos el listado
    //
    funRellenaListado();

    //
    // Deshabilitamos los controles cmdGuardar y tbEliminar
    //
    ui->cmdGuardar->setEnabled(false);
    ui->cmdGuardar->setToolTip("");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    //
    // Colocamos el tooltip de tbAgregar y cmdVolver
    //
    ui->tbAgregar->setToolTip("Añadir un nuevo repostaje...");
    ui->cmdVolver->setToolTip("Volver a la pantalla principal...");

    //
    // Colocamos la fecha Actual
    //
    ui->txtFecha->setText(qlTraductor.toString(QDate::currentDate() ,"dd/MM/yyyy"));

    //
    // Desactivamos los controles
    //
    ui->txtFecha->setEnabled(false);

    ui->tbFecha->setEnabled(false);
    ui->tbFecha->setToolTip("");

    ui->cmbEmpresas->setEnabled(false);
    ui->cmbEmpleados->setEnabled(false);
    ui->cmbMaquinas->setEnabled(false);
    ui->cmbCombustible->setEnabled(false);
    ui->lblFotoEmpleado->setEnabled(false);
    ui->lblFotoMaquina->setEnabled(false);

    ui->txtCantidad->setEnabled(false);
    ui->txtCantidad->setStyleSheet("color: blue; background-color: white");



}

void frmGesRepostajes::funRellenaListado(){
    QString strRutaDb, strSql, str;
    QString strEmpleado, strEmpresa, strMaquina;
    QString strCodigo, strFecha, strCombustible, strCantidad;
    int     i;

    FuncAux* pFuncAux = new FuncAux();

    //
    // si la tabla no esta vacia la borramos
    //
    while(ui->tlRepostajes->rowCount()>0){
        ui->tlRepostajes->removeRow(0);
    }

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Repostajes";
    sql.exec(strSql);
    sql.first();
    i=0;

    while (sql.isValid()) {

        ui->tlRepostajes->setRowCount(i + 1);

        strCodigo       = pFuncAux->funDesCifrar_bis(sql.record().value("Empresa").toString());
        strEmpresa      = pFuncAux->funTxtToEmpresas(strCodigo);
        strCodigo       = pFuncAux->funDesCifrar_bis(sql.record().value("Empleado").toString());
        strEmpleado     = pFuncAux->funTxtToEmpleados(strCodigo);
        strCodigo       = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());
        strMaquina      = pFuncAux->funTxtToMaquinas(strCodigo);
        strFecha        = pFuncAux->funDesCifrar_bis(sql.record().value("Fecha").toString());
        strCombustible  = pFuncAux->funDesCifrar_bis(sql.record().value("Carburante").toString());
        strCantidad     = pFuncAux->funDesCifrar_bis(sql.record().value("Litros").toString());

        QTableWidgetItem *item_fecha = new QTableWidgetItem(strFecha);
        QTableWidgetItem *item_empresa = new QTableWidgetItem(strEmpresa);
        QTableWidgetItem *item_empleado = new QTableWidgetItem(strEmpleado);
        QTableWidgetItem *item_maquina = new QTableWidgetItem(strMaquina);
        QTableWidgetItem *item_carburante = new QTableWidgetItem(strCombustible);
        QTableWidgetItem *item_litros = new QTableWidgetItem(strCantidad);

        item_fecha->setTextAlignment(Qt::AlignCenter);
        item_empresa->setTextAlignment(Qt::AlignCenter);
        item_empleado->setTextAlignment(Qt::AlignCenter);
        item_maquina->setTextAlignment(Qt::AlignCenter);
        item_carburante->setTextAlignment(Qt::AlignCenter);
        item_litros->setTextAlignment(Qt::AlignCenter);

        ui->tlRepostajes->setItem(i, 0, item_fecha);
        ui->tlRepostajes->setItem(i, 1, item_empresa);
        ui->tlRepostajes->setItem(i, 2, item_empleado);
        ui->tlRepostajes->setItem(i, 3, item_maquina);
        ui->tlRepostajes->setItem(i, 4, item_carburante);
        ui->tlRepostajes->setItem(i, 5, item_litros);

        i++;
        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    delete pFuncAux;

}

void frmGesRepostajes::funProcesaFecha(){
    QDate datFecha;

    FuncAux* pFuncAux = new FuncAux();

    datFecha = pFuncAux->funStrToDate(ui->txtFecha->text());

    //
    // Si la fecha es correcta
    //
    if(datFecha.isValid()){
        //
        // Si la fecha es posterior a la actual, avisamos
        //
        if(datFecha > QDate::currentDate()){

            QMessageBox::warning(this, GstrTitulo,"La Fecha No puede ser posterior a la actual...");
            ui->txtFecha->setFocus();
            ui->txtFecha->selectAll();

        }
        //
        // Todo Correcto
        //
        else{
            funRellenaListado();
            ui->txtCantidad->setFocus();
            ui->txtCantidad->selectAll();
        }
    }
    //
    // Si no es correcta avisamos
    //
    else{

        QMessageBox::warning(this, GstrTitulo,"Formato de Fecha No reconocido...");
        ui->txtFecha->setFocus();
        ui->txtFecha->selectAll();
    }

    delete pFuncAux;

}

void frmGesRepostajes::on_cmdVolver_clicked(){

    this->close();
}

void frmGesRepostajes::on_tbFecha_clicked(){

    frmCalendario* pCalendario  = new frmCalendario(this);

    pCalendario->setWindowTitle(GstrTitulo);
    pCalendario->setWindowFlag(Qt::CustomizeWindowHint);
    pCalendario->setWindowModality(Qt::WindowModal);

    pCalendario->exec();

    if(pCalendario->strFecha != ""){
        ui->txtFecha->setText(pCalendario->strFecha);
        funProcesaFecha();
    }

    delete pCalendario;

}

void frmGesRepostajes::on_txtFecha_returnPressed(){

    funProcesaFecha();
}

void frmGesRepostajes::on_cmbEmpresas_activated(int index){

    ui->txtCantidad->setFocus();
    ui->txtCantidad->selectAll();
}

void frmGesRepostajes::on_cmbCombustible_activated(int index){

    ui->txtCantidad->setFocus();
    ui->txtCantidad->selectAll();
}

void frmGesRepostajes::on_cmbEmpleados_activated(int index){
    QString         strCodigo, strRuta;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Obtenemos el codigo de la cmbEmpleado
    //
    strCodigo = pFuncAux->funCodigoToEmpleado(ui->cmbEmpleados->currentText());

    //
    // Obtenemos la ruta de la foto
    //
    strRuta = pFuncAux->funRutaFotoEmpleadoToCodigo(strCodigo);

    //
    // Mostramos la foto
    //
    pFuncAux->funMostrarFoto(ui->lblFotoEmpleado, strRuta);

    ui->txtCantidad->setFocus();
    ui->txtCantidad->selectAll();

    delete pFuncAux;

}

void frmGesRepostajes::on_cmbMaquinas_activated(int index){
    QString         strCodigo, strRuta;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Obtenemos el codigo de la cmbEmpleado
    //
    strCodigo = pFuncAux->funCodigoToMaquina(ui->cmbMaquinas->currentText());

    //
    // Obtenemos la ruta de la foto
    //
    strRuta = pFuncAux->funRutaFotoMaquinaToCodigo(strCodigo);

    //
    // Mostramos la foto
    //
    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, strRuta);

    ui->txtCantidad->setFocus();
    ui->txtCantidad->selectAll();

    delete pFuncAux;
}

void frmGesRepostajes::on_txtCantidad_textChanged(const QString &arg1){
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
                ui->txtCantidad->setText(str);
            }
        }

    }

    //
    // Si su longitud es dos solo permito str == 0.
    if(str.length() == 2){
        if(str != "0."){
            if(numero == 0){
                str.remove(str.length() - 1, 1);
                ui->txtCantidad->setText(str);
            }
        }
    }

    //
    // S i su longitud es mayor de 2 solo si es numero
    //
    if(str.length() > 2 && numero == 0){
        str.remove(str.length() - 1, 1);
        ui->txtCantidad->setText(str);
    }


}

void frmGesRepostajes::on_tbAgregar_clicked(){
    QLocale qlTraductor;

    //
    // Limpiamos los controles y colocamos la fecha actual
    //
    ui->txtCantidad->setText("");
    ui->txtFecha->setText(qlTraductor.toString(QDate::currentDate() ,"dd/MM/yyyy"));

    //
    // Activamos los controles
    //
    ui->txtFecha->setEnabled(true);
    ui->cmbEmpresas->setEnabled(true);

    ui->tbFecha->setEnabled(true);
    ui->tbFecha->setToolTip("Seleccionar Fecha en un Calendario...");


    ui->cmbEmpleados->setEnabled(true);
    ui->cmbMaquinas->setEnabled(true);
    ui->cmbCombustible->setEnabled(true);
    ui->lblFotoEmpleado->setEnabled(true);
    ui->lblFotoMaquina->setEnabled(true);


    ui->txtCantidad->setEnabled(true);
    ui->txtCantidad->setStyleSheet("color: blue; background-color: white");


    //
    // Deselecciono la tabla
    //
    ui->tlRepostajes->clearSelection();

    //
    // Deshabilito el boton de + y el de -
    //
    ui->tbAgregar->setEnabled(false);
    ui->tbAgregar->setToolTip("");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    //
    // Habilitamos el cmdGuardar
    //
    ui->cmdGuardar->setEnabled(true);
    ui->cmdGuardar->setToolTip("Guardar los datos de este repostaje...");

    ui->txtCantidad->setFocus();

}

void frmGesRepostajes::on_cmdGuardar_clicked(){
    QString strRutaDb, strSql, strCodigo;
    QString strCodEmpresa, strCodEmpleado, strCodMaquina;
    QString strCodEmpresaDb, strCodMaquinaDb, strCodEmpleadoDb, strFechaDb, strCombustibleDb;
    QString strFechaCod, strEmpresaCod, strEmpleadoCod, strMaquinaCod, strCombustibleCod, strCantidadCod;

    //
    // Si el campo del nombre esta cubierto seguimos y guardamos los valores
    //
    if(ui->txtCantidad->text() != ""){

        //
        // Abro la BD
        //
        strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
        QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
        dbSql.setDatabaseName(strRutaDb);
        dbSql.open();
        QSqlQuery sql = QSqlQuery(dbSql);

        //
        // Obtenemos los codigos de empresa , empleado y maquina
        //
        FuncAux* pFuncAux = new FuncAux();
        strCodEmpresa   = pFuncAux->funCodigoToEmpresa(ui->cmbEmpresas->currentText());
        strCodEmpleado  = pFuncAux->funCodigoToEmpleado(ui->cmbEmpleados->currentText());
        strCodMaquina   = pFuncAux->funCodigoToMaquina(ui->cmbMaquinas->currentText());
        delete pFuncAux;

        //
        // Comprobamos si existe ese registro, si es asi lo borramos
        //
        strSql = "SELECT *FROM Repostajes";
        sql.exec(strSql);
        sql.first();
        while (sql.isValid()) {

            strFechaCod         = sql.record().value("Fecha").toString();
            strEmpleadoCod      = sql.record().value("Empleado").toString();
            strEmpresaCod       = sql.record().value("Empresa").toString();
            strMaquinaCod       = sql.record().value("Maquina").toString();
            strCombustibleCod   = sql.record().value("Carburante").toString();

            strFechaDb          = pFuncAux->funDesCifrar_bis(strFechaCod);
            strCodEmpleadoDb    = pFuncAux->funDesCifrar_bis(strEmpleadoCod);
            strCodEmpresaDb     = pFuncAux->funDesCifrar_bis(strEmpresaCod);
            strCodMaquinaDb     = pFuncAux->funDesCifrar_bis(strMaquinaCod);
            strCombustibleDb    = pFuncAux->funDesCifrar_bis(strCombustibleCod);

            if(strFechaDb == ui->txtFecha->text() &&
               strCodEmpresaDb == strCodEmpresa &&
               strCodEmpleadoDb == strCodEmpleado &&
               strCodMaquinaDb == strCodMaquina &&
               strCombustibleDb == ui->cmbCombustible->currentText()){

                strSql = "DELETE FROM Repostajes WHERE Fecha='" + strFechaCod + "'AND "
                                                      "Empresa='" + strEmpresaCod + "' AND "
                                                      "Empleado='" + strEmpleadoCod + "' AND "
                                                      "Maquina='" + strMaquinaCod + "' AND "
                                                      "Carburante='" + strCombustibleCod + "';";
                sql.exec(strSql);

                sql.finish();
            }

            sql.next();
        }


        //
        // Guardamos el nuevo registro
        //

        strFechaCod         = pFuncAux->funCifrar_bis(ui->txtFecha->text());
        strEmpleadoCod      = pFuncAux->funCifrar_bis(strCodEmpleado);
        strEmpresaCod       = pFuncAux->funCifrar_bis(strCodEmpresa);
        strMaquinaCod       = pFuncAux->funCifrar_bis(strCodMaquina);
        strCombustibleCod   = pFuncAux->funCifrar_bis(ui->cmbCombustible->currentText());
        strCantidadCod      = pFuncAux->funCifrar_bis(ui->txtCantidad->text());

        strSql = " INSERT INTO Repostajes(Fecha,"
                                          "Empresa,"
                                          "Empleado,"
                                          "Maquina,"
                                          "Carburante,"
                                          "Litros) VALUES ('" + strFechaCod + "',"
                                                          "'" + strEmpresaCod + "',"
                                                          "'" + strEmpleadoCod + "',"
                                                          "'" + strMaquinaCod + "',"
                                                          "'" + strCombustibleCod + "',"
                                                          "'" + strCantidadCod + "');";
        sql.exec(strSql);

        //
        // Cierro la BD
        //
        dbSql.close();
        dbSql.removeDatabase("conSql");

        //
        // Limpio los controles
        //
        ui->txtCantidad->setStyleSheet("color: blue; background-color: white");
        ui->txtCantidad->setText("");

        funRellenaListado();

        //
        // Activo el boton de +
        //
        ui->tbAgregar->setEnabled(true);
        ui->tbAgregar->setToolTip("Añadir un nuevo repostaje...");

        ui->cmdGuardar->setEnabled(false);
        ui->cmdGuardar->setToolTip("");

        //
        // Desactivamos los controles
        //
        ui->txtFecha->setEnabled(false);

        ui->tbFecha->setEnabled(false);
        ui->tbFecha->setToolTip("");

        ui->cmbEmpresas->setEnabled(false);
        ui->cmbEmpleados->setEnabled(false);
        ui->cmbMaquinas->setEnabled(false);
        ui->cmbCombustible->setEnabled(false);
        ui->lblFotoEmpleado->setEnabled(false);
        ui->lblFotoMaquina->setEnabled(false);


        ui->txtCantidad->setEnabled(false);


    }
    //
    // Si el campo de lo litros esta vacio avisamos y lo situamos ahi
    //
    else{
        QMessageBox::information(this, GstrTitulo, "\t El Campo de los Litros es obligatorio...");

        ui->txtCantidad->setStyleSheet("color: blue; background-color: yellow");
        ui->txtCantidad->setFocus();
    }


}

void frmGesRepostajes::on_txtCantidad_returnPressed(){

    on_cmdGuardar_clicked();
}

void frmGesRepostajes::on_tlRepostajes_itemClicked(QTableWidgetItem *item){
    QString strEmpresa, strEmpleado, strMaquina;
    int     fila;

    fila = item->row();

    //
    // Desactivamos los controles
    //
    ui->txtFecha->setEnabled(false);

    ui->tbFecha->setEnabled(false);
    ui->tbFecha->setToolTip("");

    ui->cmbEmpresas->setEnabled(false);
    ui->cmbEmpleados->setEnabled(false);
    ui->cmbMaquinas->setEnabled(false);
    ui->cmbCombustible->setEnabled(false);
    ui->lblFotoEmpleado->setEnabled(false);
    ui->lblFotoMaquina->setEnabled(false);


    ui->txtCantidad->setStyleSheet("color: lightblue; background-color: white");
    ui->txtCantidad->setEnabled(false);


    //
    // Rellenamos los txt
    //
    ui->txtFecha->setText(ui->tlRepostajes->item(fila, 0)->text());
    ui->cmbEmpresas->setCurrentText(ui->tlRepostajes->item(fila, 1)->text());
    ui->cmbEmpleados->setCurrentText(ui->tlRepostajes->item(fila, 2)->text());
    ui->cmbMaquinas->setCurrentText(ui->tlRepostajes->item(fila, 3)->text());
    ui->cmbCombustible->setCurrentText(ui->tlRepostajes->item(fila, 4)->text());
    ui->txtCantidad->setText(ui->tlRepostajes->item(fila, 5)->text());

    //
    // Colocamos las fotos
    //
    FuncAux* pFuncAux = new FuncAux();

    pFuncAux->funMostrarFoto(ui->lblFotoEmpleado, pFuncAux->funRutaFotoEmpleadoToCodigo(pFuncAux->funCodigoToEmpleado(ui->cmbEmpleados->currentText())));
    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, pFuncAux->funRutaFotoMaquinaToCodigo(pFuncAux->funCodigoToMaquina(ui->cmbMaquinas->currentText())));

    delete pFuncAux;

    //
    // Activamos suprimir y agregar
    //
    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Añadir un nuevo repostaje...");

    if(item->text() != ""){
        ui->tbEliminar->setEnabled(true);
        ui->tbEliminar->setToolTip("Eliminar un Repostaje...");
    }


}

void frmGesRepostajes::on_tlRepostajes_itemDoubleClicked(QTableWidgetItem *item){
    QLocale qlTraductor;

    //
    // Activamos los controles
    //
    ui->txtFecha->setEnabled(true);

    ui->cmbEmpresas->setEnabled(true);

    ui->tbFecha->setEnabled(true);
    ui->tbFecha->setToolTip("Seleccionar Fecha en un Calendario...");


    ui->cmbEmpleados->setEnabled(true);
    ui->cmbMaquinas->setEnabled(true);
    ui->cmbCombustible->setEnabled(true);
    ui->lblFotoEmpleado->setEnabled(true);
    ui->lblFotoMaquina->setEnabled(true);


    ui->txtCantidad->setEnabled(true);
    ui->txtCantidad->setStyleSheet("color: blue; background-color: white");


    //
    // Deselecciono la tabla
    //
    ui->tlRepostajes->clearSelection();

    //
    // Deshabilito el boton de + y el de -
    //
    ui->tbAgregar->setEnabled(false);
    ui->tbAgregar->setToolTip("");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    //
    // Habilitamos el cmdGuardar
    //
    ui->cmdGuardar->setEnabled(true);
    ui->cmdGuardar->setToolTip("Guardar los datos de este repostaje...");

    ui->txtCantidad->setFocus();
    ui->txtCantidad->selectAll();


}

void frmGesRepostajes::on_tbEliminar_clicked(){
    QString strRutaDb, strSql;
    QString strEmpresa, strEmpleado, strMaquina;

    //
    // Abro la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    FuncAux* pFuncAux = new FuncAux();
    strEmpresa  = pFuncAux->funCodigoToEmpresa(ui->cmbEmpresas->currentText());
    strEmpleado = pFuncAux->funCodigoToEmpleado(ui->cmbEmpleados->currentText());
    strMaquina  = pFuncAux->funCodigoToMaquina(ui->cmbMaquinas->currentText());
    delete pFuncAux;

    strSql = "DELETE FROM Repostajes WHERE Fecha='"+ui->txtFecha->text()+"'AND "
                                          "Empresa='"+strEmpresa+"' AND "
                                          "Empleado='"+strEmpleado+"' AND "
                                          "Maquina='"+strMaquina+"' AND "
                                          "Carburante='"+ui->cmbCombustible->currentText()+"' AND "
                                          "Litros='"+ui->txtCantidad->text()+"';";
    sql.exec(strSql);

    //
    // Limpio los controles
    //
    ui->txtCantidad->setStyleSheet("color: black; background-color: white");
    ui->txtCantidad->setText("");

    funRellenaListado();

    //
    // desactivo boton - y activo el +
    //
    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Añadir un nuevo repostaje...");

    //
    // Cierro la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    ui->txtCantidad->setFocus();

}

