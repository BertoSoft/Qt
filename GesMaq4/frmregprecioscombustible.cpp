#include "frmregprecioscombustible.h"
#include "ui_frmregprecioscombustible.h"

#include "funcaux.h"
#include "frmcalendario.h"

#include <QDate>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QMessageBox>
#include <QScreen>


frmRegPreciosCombustible::frmRegPreciosCombustible(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmRegPreciosCombustible){
    ui->setupUi(this);

    this->installEventFilter(this);


}

frmRegPreciosCombustible::~frmRegPreciosCombustible(){
    delete ui;
}

bool frmRegPreciosCombustible::eventFilter(QObject *obj, QEvent *event){

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

void frmRegPreciosCombustible::funInicio(){
    QString strRutaDb, strSql, str, strCodEmpresa;
    QDate   datFechaInicial, datFechaFinal;
    QLocale qlTraductor;
    int     ancho_src, alto_src;
    int     ancho_form, alto_form;
    int     ix0, iy0;

    //
    // Dimensionamos la pantalla al 70% del screen
    //
    /*
    QScreen *src = QGuiApplication::screens().at(0);

    ancho_src = src->availableGeometry().width();
    alto_src  = src->availableGeometry().height();

    ancho_form  = (int) ancho_src * 0.85;
    alto_form   = (int) alto_src * 0.50;

    ix0         = (int) ancho_src * 0.075;
    iy0         = (int) alto_src * 0.25;

    this->setGeometry(ix0, iy0, ancho_form, alto_form);
    */

    //
    // Colocamos las columnas del tablelist
    //
    ui->tlCombustible->setColumnCount(6);

    QStringList headers;
    headers += tr("Empresa");
    headers += tr("Carburante");
    headers += tr("Fecha Inicial");
    headers += tr("Fecha Final");
    headers += tr("Precio");
    headers += tr("Codigo");


    ui->tlCombustible->setHorizontalHeaderLabels(headers);

    ui->tlCombustible->setColumnWidth(0, (int) ( ( this->width() * 2 ) / 6 ) );
    ui->tlCombustible->setColumnWidth(1, (int) ( ( this->width() * 1.3 ) / 6 ) );
    ui->tlCombustible->setColumnWidth(2, (int) ( ( this->width() * 0.75 ) / 6 ) );
    ui->tlCombustible->setColumnWidth(3, (int) ( ( this->width() * 0.75 ) / 6 ) );
    ui->tlCombustible->setColumnWidth(4, (int) ( ( this->width() * 0.5 ) / 6 ) );
    ui->tlCombustible->setColumnWidth(5, (int) ( ( this->width() * 0.5 ) / 6 ) );

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    //
    // Rellenamos el cmbEmpresas
    //
    FuncAux* pFuncAux = new FuncAux;

    if(ui->cmbCentroTrabajo->count()>0){
        ui->cmbCentroTrabajo->removeItem(0);
    }
    strSql = "SELECT *FROM Empresas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {
        //
        // A Partir del codigo Cod nos da el txt empresas
        //
        strCodEmpresa = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());

        str = pFuncAux->funTxtToEmpresas(strCodEmpresa);

        ui->cmbCentroTrabajo->addItem(str);
        sql.next();
    }

    delete pFuncAux;

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
    // Colocamos la fecha Inicial y final en los txtFecha
    //
    datFechaInicial = QDate::currentDate();
    datFechaFinal   = QDate::currentDate();

    datFechaInicial.setDate(datFechaInicial.year(), datFechaInicial.month(), 01);

    datFechaFinal = datFechaFinal.addMonths(1);
    datFechaFinal.setDate(datFechaFinal.year(), datFechaFinal.month(), 01);
    datFechaFinal = datFechaFinal.addDays(-1);

    ui->txtFecha0->setText(qlTraductor.toString(datFechaInicial,"dd/MM/yyyy"));
    ui->txtFecha1->setText(qlTraductor.toString(datFechaFinal,"dd/MM/yyyy"));

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    //
    // Rellenamos el listado
    //
    funRellenaListado();

    //
    // Desactivamos Controles, tbEliminar y cmdGuardar
    //
    funDesHabilitaControles();

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    ui->cmdGuardar->setEnabled(false);
    ui->cmdGuardar->setToolTip("");

    ui->tbAgregar->setFocus();
    ui->tbAgregar->setToolTip("Añadir un nuevo Precio...");

    ui->cmdVolver->setToolTip("Volver al la pantalla pricipal...");

}

void frmRegPreciosCombustible::funRellenaListado(){
    QString strRutaDb, strSql, str;
    QString strCarburante, strFecha0, strFecha1, strPrecio, strCodigo;
    int i;

    FuncAux* pFuncAux = new FuncAux;

    //
    // Si el listado no esta vacio lo vaciamos
    //
    while (ui->tlCombustible->rowCount()>0) {
        ui->tlCombustible->removeRow(0);
    }

    //
    // Ahora miramos los registros de la base de datos
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM PrecioCombustible";
    sql.exec(strSql);
    sql.first();
    i=0;

    while (sql.isValid()) {

        ui->tlCombustible->setRowCount(i + 1);

        str = pFuncAux->funTxtToEmpresas(pFuncAux->funDesCifrar_bis(sql.record().value("Empresa").toString()));

        strCarburante   = pFuncAux->funDesCifrar_bis(sql.record().value("Combustible").toString());
        strFecha0       = pFuncAux->funDesCifrar_bis(sql.record().value("FechaInicial").toString());
        strFecha1       = pFuncAux->funDesCifrar_bis(sql.record().value("FechaFinal").toString());
        strPrecio       = pFuncAux->funDesCifrar_bis(sql.record().value("Precio").toString());
        strCodigo       = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());


        QTableWidgetItem *item_empresa = new QTableWidgetItem(str);
        QTableWidgetItem *item_carburante = new QTableWidgetItem(strCarburante);
        QTableWidgetItem *item_fecha_inicial = new QTableWidgetItem(strFecha0);
        QTableWidgetItem *item_fecha_final = new QTableWidgetItem(strFecha1);

        str = QLocale(QLocale::Spanish, QLocale::Spain).toCurrencyString(strPrecio.toDouble());
        QTableWidgetItem *item_precio = new QTableWidgetItem(str);

        QTableWidgetItem *item_codigo = new QTableWidgetItem(strCodigo);

        item_empresa->setTextAlignment(Qt::AlignCenter);
        item_carburante->setTextAlignment(Qt::AlignCenter);
        item_fecha_inicial->setTextAlignment(Qt::AlignCenter);
        item_fecha_final->setTextAlignment(Qt::AlignCenter);
        item_precio->setTextAlignment(Qt::AlignCenter);
        item_codigo->setTextAlignment(Qt::AlignCenter);

        ui->tlCombustible->setItem(i, 0, item_empresa);
        ui->tlCombustible->setItem(i, 1, item_carburante);
        ui->tlCombustible->setItem(i, 2, item_fecha_inicial);
        ui->tlCombustible->setItem(i, 3, item_fecha_final);
        ui->tlCombustible->setItem(i, 4, item_precio);
        ui->tlCombustible->setItem(i, 5, item_codigo);

        i++;
        sql.next();
    }

    delete pFuncAux;

    dbSql.close();
    dbSql.removeDatabase("conSql");


}

void frmRegPreciosCombustible::funLimpiaControles(){

    ui->cmbCentroTrabajo->setCurrentIndex(0);
    ui->cmbCombustible->setCurrentIndex(0);
    ui->txtCodigo->setText("");

    ui->txtPrecio->setText("");

    if(ui->txtPrecio->isEnabled()){
        ui->txtPrecio->setStyleSheet("color: black; background-color: white");
    }
    else{
        ui->txtPrecio->setStyleSheet("color: gray; background-color: white");
    }

}

void frmRegPreciosCombustible::funHabilitaControles(){

    ui->cmbCentroTrabajo->setEnabled(true);
    ui->cmbCentroTrabajo->setToolTip("Elige un centro de trabajo...");

    ui->cmbCombustible->setEnabled(true);
    ui->cmbCombustible->setToolTip("Elige un combustible...");

    ui->txtFecha0->setEnabled(true);
    ui->txtFecha0->setToolTip("Elige la fecha inicial del periodo...");

    ui->txtFecha1->setEnabled(true);
    ui->txtFecha1->setToolTip("Elige la fecha final del periodo...");

    ui->tbFecha0->setEnabled(true);
    ui->tbFecha0->setToolTip("Establecer la fecha Inicial del Periodo...");

    ui->tbFecha1->setEnabled(true);
    ui->tbFecha1->setToolTip("Establecer la fecha final del Periodo...");

    ui->txtPrecio->setEnabled(true);
    ui->txtPrecio->setToolTip("Indica el precio de este periodo...");


}

void frmRegPreciosCombustible::funDesHabilitaControles(){
    ui->cmbCentroTrabajo->setEnabled(false);
    ui->cmbCentroTrabajo->setToolTip("");

    ui->cmbCombustible->setEnabled(false);
    ui->cmbCombustible->setToolTip("");

    ui->txtFecha0->setEnabled(false);
    ui->txtFecha0->setToolTip("");

    ui->txtFecha1->setEnabled(false);
    ui->txtFecha1->setToolTip("");

    ui->tbFecha0->setEnabled(false);
    ui->tbFecha0->setToolTip("");

    ui->tbFecha1->setEnabled(false);
    ui->tbFecha1->setToolTip("");

    ui->txtPrecio->setEnabled(false);
    ui->txtPrecio->setToolTip("");

    ui->txtCodigo->setEnabled(false);
}

bool frmRegPreciosCombustible::funExisteRegistro(QString strCodigo){
    QString strRutaDb, strSql, strCodigoDb;
    bool existe = false;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Abro la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql25");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM PrecioCombustible";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodigoDb = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());

        if(strCodigoDb == strCodigo){
            existe = true;
            sql.finish();
        }
        sql.next();
    }

    //
    // Cierro la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql25");

    delete pFuncAux;

    return existe;
}

QString frmRegPreciosCombustible::funExistePeriodo(QString strFechaInicial, QString strFechaFinal){
    QString strRutaDb, strSql, strEmpresa, strCombustible;
    QString strCodigo = "-1";
    QString strFecha0, strFecha1;
    QDate   datFechaInicial, datFechaFinal, datFechaInicialBD, datFechaFinalDB;

    FuncAux* pFuncAux = new FuncAux;


    datFechaInicial = pFuncAux->funStrToDate(strFechaInicial);
    datFechaFinal   = pFuncAux->funStrToDate(strFechaFinal);

    //
    // Abro la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql24");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM PrecioCombustible";
    sql.exec(strSql);

    //
    // Buscamos en todos los registros si existe este periodo
    //
    sql.first();
    while (sql.isValid()) {

        strFecha0 = pFuncAux->funDesCifrar_bis(sql.record().value("FechaInicial").toString());
        strFecha1 = pFuncAux->funDesCifrar_bis(sql.record().value("FechaFinal").toString());




        datFechaInicialBD   = pFuncAux->funStrToDate(strFecha0);
        datFechaFinalDB     = pFuncAux->funStrToDate(strFecha1);
        strEmpresa          = pFuncAux->funTxtToEmpresas(pFuncAux->funDesCifrar_bis(sql.record().value("Empresa").toString()));
        strCombustible      = pFuncAux->funDesCifrar_bis(sql.record().value("Combustible").toString());

        //
        // Si la empresa y el combustible coinciden
        //
        if(strEmpresa == ui->cmbCentroTrabajo->currentText() && strCombustible == ui->cmbCombustible->currentText()){
            //
            // Si FechaInicial esta en el periodo y ademas la empresa coincide, asi como el combustible
            //
            if(datFechaInicial >= datFechaInicialBD && datFechaInicial <= datFechaFinalDB){
                strCodigo = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());
            }

            //
            // Si la fecha final esta en el periodo
            //
            if(datFechaFinal > datFechaInicialBD && datFechaFinal <= datFechaFinalDB && strCodigo !="-1"){
                strCodigo = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());
            }
        }

        sql.next();
    }

    delete pFuncAux;

    //
    // Cierro la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql24");

    return strCodigo;
}

void frmRegPreciosCombustible::funProcesaFecha0(){
    QString str;
    QDate   datPosterior, datFecha;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Comprobamos que la fecha no sea mayor que la del intervalo superior
    //
    datPosterior    = pFuncAux->funStrToDate(ui->txtFecha1->text());
    datFecha        = pFuncAux->funStrToDate(ui->txtFecha0->text());

    //
    // Fecha mayor que la posterior
    //
    if(datFecha > datPosterior){
        str = "La Fecha no puede ser posterior al ";
        str.append(ui->txtFecha1->text());
        QMessageBox::warning(this, GstrTitulo, str);
        ui->txtFecha0->setFocus();
        ui->txtFecha0->selectAll();
    }
    //
    // Todo correcto listamos
    //
    else{
        ui->txtFecha1->setFocus();
        ui->txtFecha1->selectAll();
    }
    delete pFuncAux;
}

void frmRegPreciosCombustible::funProcesaFecha1(){
    QString str;
    QDate   datActual, datFecha, datFecha0;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Comprobamos que la fecha no sea menor que Fecha0
    //
    datActual   = QDate::currentDate();
    datFecha0   = pFuncAux->funStrToDate(ui->txtFecha0->text());
    datFecha    = pFuncAux->funStrToDate(ui->txtFecha1->text());
    //
    // Si es menor que la fecha 0
    if(datFecha < datFecha0 ){
        str = "La Fecha no puede ser anterior al ";
        str.append(ui->txtFecha0->text());
        QMessageBox::warning(this, GstrTitulo, str);
        ui->txtFecha1->setFocus();
        ui->txtFecha1->selectAll();
    }
    //
    // Todo correcto listamos
    //
    else{
        ui->txtPrecio->setFocus();
        ui->txtPrecio->selectAll();
    }

    delete pFuncAux;
}

void frmRegPreciosCombustible::on_cmdVolver_clicked(){
    this->close();
}

void frmRegPreciosCombustible::on_tbAgregar_clicked(){
    QString strRutaDb, strSql, strCodigo;
    QDate   datFechaInicial, datFechaFinal;
    QLocale qlTraductor;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Primero limpio y habilito los controles
    //
    funHabilitaControles();
    funLimpiaControles();

    //
    // Colocamos la fecha Inicial y final en los txtFecha
    //
    datFechaInicial = QDate::currentDate();
    datFechaFinal   = QDate::currentDate();

    datFechaInicial.setDate(datFechaInicial.year(), datFechaInicial.month(), 01);

    datFechaFinal = datFechaFinal.addMonths(1);
    datFechaFinal.setDate(datFechaFinal.year(), datFechaFinal.month(), 01);
    datFechaFinal = datFechaFinal.addDays(-1);

    ui->txtFecha0->setText(qlTraductor.toString(datFechaInicial,"dd/MM/yyyy"));
    ui->txtFecha1->setText(qlTraductor.toString(datFechaFinal,"dd/MM/yyyy"));

    //
    // Obtengo el codigo de Registro
    //
    strCodigo = pFuncAux->funNuevoCodigo("PrecioCombustible");
    ui->txtCodigo->setText(strCodigo);

    ui->cmdGuardar->setEnabled(true);
    ui->cmdGuardar->setToolTip("Guardar el precio de un periodo...");

    ui->tlCombustible->clearSelection();

    ui->txtPrecio->setFocus();

    //
    // Deshabilito el boton de + y el de -
    //
    ui->tbAgregar->setEnabled(false);
    ui->tbAgregar->setToolTip("");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    delete pFuncAux;

}

void frmRegPreciosCombustible::on_tbEliminar_clicked(){
    QString strRutaDb, strSql;
    QString strCodigoDbCod, strCodigoDb, strCodigo;
    int     codigo = 1;

    FuncAux *pFuncAux = new FuncAux();

    strCodigo = ui->tlCombustible->item(fila_seleccionada, 5)->text();

    //
    // Abro la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    //
    // Recorremos toda la tabla en busca del codigo cifrado
    //
    strSql = "SELECT *FROM PrecioCombustible";
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

    strSql = "DELETE FROM PrecioCombustible WHERE Codigo='"+ strCodigoDbCod +"';";
    sql.exec(strSql);

    //
    // limpio y Deshabilito los controles
    //
    funDesHabilitaControles();
    funLimpiaControles();
    funRellenaListado();

    //
    // desactivo boton - y activo el +
    //
    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Añadir un nuevo Precio...");


    //
    // Cierro la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    delete pFuncAux;
}

void frmRegPreciosCombustible::on_cmdGuardar_clicked(){
    QString strRutaDb, strSql, str;
    QString strCodigoNuevo, strCodigoPeriodo, strCodigoEmpresa, strCodigoDb, strCodigoDbCod, strEmpresaCod;
    QDate   datFechaInicial, datFechaFinal;
    QLocale qlTraductor;
    int     fila;

    //
    // Si esta vacio avisamos
    //
    if(ui->txtPrecio->text() == ""){
        QMessageBox::information(this, GstrTitulo, "\t El Campo del precio tiene que estar cubierto...");

        ui->txtPrecio->setStyleSheet("color: black; background-color: yellow");
        ui->txtPrecio->setFocus();
    }
    //
    // Todo Correcto, guardamos
    //
    else{

        //
        // Abrimos la BD
        //
        strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
        QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql26");
        dbSql.setDatabaseName(strRutaDb);
        dbSql.open();
        QSqlQuery sql = QSqlQuery(dbSql);

        //
        // Comprobamos si existe el Periodo
        //
        strCodigoPeriodo = funExistePeriodo(ui->txtFecha0->text(), ui->txtFecha1->text());

        //
        // Si strCodigoPeriodo > 0, ya existe un precio en este periodo
        //
        if(strCodigoPeriodo != "-1"){

            //
            // Seleccionamos el periodo en tlPrecios
            //
            fila=0;
            while (fila < ui->tlCombustible->rowCount()) {
                if(ui->tlCombustible->item(fila, 5)->text() == strCodigoPeriodo){

                    //
                    // Si no existe ese codigo en la BD, colocamos en los txt el codigo existente
                    //
                    if(!funExisteRegistro(ui->txtCodigo->text())){

                        //
                        // Avisamos que ese periodo ya existe, aunque no el codigo
                        //
                        str = "\n\tYa existe un precio para el ";
                        str.append(ui->cmbCombustible->currentText());
                        str.append("\t\n\n\t en la empresa ");
                        str.append(ui->cmbCentroTrabajo->currentText());
                        str.append("\t\n\n\tDel ");
                        str.append(ui->txtFecha0->text());
                        str.append(" al ");
                        str.append(ui->txtFecha1->text());

                        QMessageBox::information(this, GstrTitulo, str);



                        //
                        // Seleccionamos la fila
                        //
                        ui->tlCombustible->selectRow(fila);
                        fila_seleccionada = fila;

                        //
                        // Relleno los campos con los valores de la fila seleccionada
                        //
                        ui->cmbCentroTrabajo->setCurrentText(ui->tlCombustible->item(fila, 0)->text());
                        ui->cmbCombustible->setCurrentText(ui->tlCombustible->item(fila, 1)->text());
                        ui->txtFecha0->setText(ui->tlCombustible->item(fila, 2)->text());
                        ui->txtFecha1->setText(ui->tlCombustible->item(fila, 3)->text());
                        ui->txtPrecio->setText(ui->tlCombustible->item(fila, 4)->text());
                        ui->txtCodigo->setText(ui->tlCombustible->item(fila, 5)->text());

                        fila = ui->tlCombustible->rowCount();

                        //
                        // Activamos el tbELiminar y el tbagregar
                        //
                        ui->tbAgregar->setEnabled(true);
                        ui->tbEliminar->setEnabled(true);

                        //
                        // Ponemos el foco en txtPrecio
                        //
                        ui->txtPrecio->setFocus();
                        ui->txtPrecio->selectAll();

                    }
                    //
                    // Si existe lo editamos
                    //
                    else{

                        //
                        // Avisamos que ese Codigo ya existe
                        //
                        str = "\n\tYa existe un Registro para el ";
                        str.append(ui->tlCombustible->item(fila, 1)->text());
                        str.append("\t\n\n\t en la empresa ");
                        str.append(ui->cmbCentroTrabajo->currentText());
                        str.append("\t\n\n\tDel ");
                        str.append(ui->txtFecha0->text());
                        str.append(" al ");
                        str.append(ui->txtFecha1->text());
                        str.append("\t\n\n\t Se procede a cambiarlo...");

                        QMessageBox::information(this, GstrTitulo, str);

                        FuncAux* pFuncAux = new FuncAux;
                        strCodigoEmpresa = pFuncAux->funCodigoToEmpresa(ui->cmbCentroTrabajo->currentText());
                        delete pFuncAux;

                        //
                        // Tenemos que recorrer la BD para encontrar el codigo y borarlo
                        //
                        strSql = "SELECT *FROM PrecioCombustible";
                        sql.exec(strSql);
                        sql.first();
                        while (sql.isValid()) {

                            strCodigoDb = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());

                            if(strCodigoDb == ui->txtCodigo->text()){
                                strEmpresaCod   = sql.record().value("Empresa").toString();
                                strCodigoDbCod  = sql.record().value("Codigo").toString();
                                sql.finish();
                            }
                            sql.next();
                        }
                        strSql = "DELETE FROM PrecioCombustible WHERE Codigo='" + strCodigoDbCod + "'AND Empresa='" + strEmpresaCod + "';";
                        sql.exec(strSql);

                        strSql = " INSERT INTO PrecioCombustible(Empresa,"
                                 "                               Combustible,"
                                 "                               FechaInicial,"
                                 "                               FechaFinal,"
                                 "                               Precio,"
                                 "                               Codigo) VALUES ('" + pFuncAux->funCifrar_bis(strCodigoEmpresa) + "',"
                                 "                                               '" + pFuncAux->funCifrar_bis(ui->cmbCombustible->currentText()) + "',"
                                 "                                               '" + pFuncAux->funCifrar_bis(ui->txtFecha0->text()) + "',"
                                 "                                               '" + pFuncAux->funCifrar_bis(ui->txtFecha1->text()) + "',"
                                 "                                               '" + pFuncAux->funCifrar_bis(ui->txtPrecio->text()) + "',"
                                 "                                               '" + pFuncAux->funCifrar_bis(ui->txtCodigo->text()) + "');";

                        sql.exec(strSql);

                        fila = ui->tlCombustible->rowCount();

                        funRellenaListado();

                        //
                        // Limpiamos los controles
                        //
                        ui->cmbCentroTrabajo->setCurrentIndex(0);
                        ui->cmbCombustible->setCurrentIndex(0);
                        ui->txtPrecio->setText("");
                        ui->txtCodigo->setText("");

                        //
                        // Colocamos las fechas
                        //
                        datFechaInicial = QDate::currentDate();
                        datFechaFinal   = QDate::currentDate();

                        datFechaInicial.setDate(datFechaInicial.year(), datFechaInicial.month(), 01);

                        datFechaFinal.setDate(datFechaInicial.year(), datFechaInicial.month() + 1, 01);
                        datFechaFinal = datFechaFinal.addDays(-1);

                        ui->txtFecha0->setText(qlTraductor.toString(datFechaInicial,"dd/MM/yyyy"));
                        ui->txtFecha1->setText(qlTraductor.toString(datFechaFinal,"dd/MM/yyyy"));

                        //
                        // Deshabilitamos los controles
                        //
                        funDesHabilitaControles();

                        //
                        // Activamos el tbAgregar y desactivamos el cmd guardar y el tbEliminar
                        //
                        ui->tbAgregar->setEnabled(true);
                        ui->tbAgregar->setToolTip("Añadir un nuevo Precio...");

                        ui->cmdGuardar->setEnabled(false);
                        ui->cmdGuardar->setToolTip("");

                        ui->tbEliminar->setEnabled(false);
                        ui->tbEliminar->setToolTip("");

                        ui->label_10->setFocus();
                    }
                }
                fila++;
            }
        }
        //
        // No existe el periodo lo añadimos
        //
        else{

            FuncAux* pFuncAux = new FuncAux;

            QString tmp = ui->cmbCentroTrabajo->currentText();

            strCodigoEmpresa = pFuncAux->funCodigoToEmpresa(ui->cmbCentroTrabajo->currentText());

            delete pFuncAux;


            strSql = " INSERT INTO PrecioCombustible(Empresa,"
                     "                               Combustible,"
                     "                               FechaInicial,"
                     "                               FechaFinal,"
                     "                               Precio,"
                     "                               Codigo) VALUES ('" + pFuncAux->funCifrar_bis(strCodigoEmpresa) + "',"
                     "                                               '" + pFuncAux->funCifrar_bis(ui->cmbCombustible->currentText()) + "',"
                     "                                               '" + pFuncAux->funCifrar_bis(ui->txtFecha0->text()) + "',"
                     "                                               '" + pFuncAux->funCifrar_bis(ui->txtFecha1->text()) + "',"
                     "                                               '" + pFuncAux->funCifrar_bis(ui->txtPrecio->text()) + "',"
                     "                                               '" + pFuncAux->funCifrar_bis(ui->txtCodigo->text()) + "');";

            sql.exec(strSql);

            funRellenaListado();

            //
            // Limpiamos los controles
            //
            ui->cmbCentroTrabajo->setCurrentIndex(0);
            ui->cmbCombustible->setCurrentIndex(0);
            ui->txtPrecio->setText("");
            ui->txtCodigo->setText("");

            //
            // Colocamos las fechas
            //
            datFechaInicial = QDate::currentDate();
            datFechaFinal   = QDate::currentDate();

            datFechaInicial.setDate(datFechaInicial.year(), datFechaInicial.month(), 01);

            datFechaFinal.setDate(datFechaInicial.year(), datFechaInicial.month() + 1, 01);
            datFechaFinal = datFechaFinal.addDays(-1);

            ui->txtFecha0->setText(qlTraductor.toString(datFechaInicial,"dd/MM/yyyy"));
            ui->txtFecha1->setText(qlTraductor.toString(datFechaFinal,"dd/MM/yyyy"));

            //
            // Deshabilitamos los controles
            //
            funDesHabilitaControles();

            //
            // Activamos el tbAgregar y desactivamos el cmd guardar
            //
            ui->tbAgregar->setEnabled(true);
            ui->tbAgregar->setToolTip("Añadir un nuevo Precio...");

            ui->cmdGuardar->setEnabled(false);
            ui->cmdGuardar->setToolTip("");

        }


        //
        // Cierro la BD
        //
        dbSql.close();
        dbSql.removeDatabase("conSql26");
    }


}

void frmRegPreciosCombustible::on_txtPrecio_returnPressed(){

    ui->txtPrecio->setStyleSheet("color: black; background-color: white");
    on_cmdGuardar_clicked();
}

void frmRegPreciosCombustible::on_txtPrecio_textChanged(const QString &arg1){
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
                ui->txtPrecio->setText(str);
            }
        }

    }

    //
    // Si su longitud es dos solo permito str == 0.
    if(str.length() == 2){
        if(str != "0."){
            if(numero == 0){
                str.remove(str.length() - 1, 1);
                ui->txtPrecio->setText(str);
            }
        }
    }

    //
    // S i su longitud es mayor de 2 solo si es numero
    //
    if(str.length() > 2 && numero == 0){
        str.remove(str.length() - 1, 1);
        ui->txtPrecio->setText(str);
    }


}

void frmRegPreciosCombustible::on_cmbCentroTrabajo_activated(int index){

    ui->txtPrecio->setFocus();
    ui->txtPrecio->selectAll();
}

void frmRegPreciosCombustible::on_cmbCombustible_activated(int index){

    ui->txtPrecio->setFocus();
    ui->txtPrecio->selectAll();

}

void frmRegPreciosCombustible::on_txtFecha0_returnPressed(){
    QDate datFecha;

    FuncAux* pFuncAux = new FuncAux();

    datFecha = pFuncAux->funStrToDate(ui->txtFecha0->text());

    //
    // Si la fecha es correcta
    //
    if(datFecha.isValid()){
        funProcesaFecha0();
    }
    //
    // Si no es correcta avisamos
    //
    else{

        QMessageBox::warning(this, GstrTitulo,"Formato de Fecha No reconocido...");
        ui->txtFecha0->setFocus();
        ui->txtFecha0->selectAll();
    }

    delete pFuncAux;

}

void frmRegPreciosCombustible::on_txtFecha1_returnPressed(){
    QDate datFecha;

    FuncAux* pFuncAux = new FuncAux();

    datFecha = pFuncAux->funStrToDate(ui->txtFecha1->text());

    //
    // Si la fecha es correcta
    //
    if(datFecha.isValid()){
        funProcesaFecha1();
    }
    //
    // Si no es correcta avisamos
    //
    else{

        QMessageBox::warning(this, GstrTitulo,"Formato de Fecha No reconocido...");
        ui->txtFecha1->setFocus();
        ui->txtFecha1->selectAll();
    }

    delete pFuncAux;
}

void frmRegPreciosCombustible::on_tbFecha0_clicked(){
    frmCalendario *pCalendario = new frmCalendario(this);

    pCalendario->setWindowTitle(GstrTitulo);
    pCalendario->setWindowFlag(Qt::CustomizeWindowHint);
    pCalendario->setWindowModality(Qt::WindowModal);

    pCalendario->exec();

    if(pCalendario->strFecha != ""){
        ui->txtFecha0->setText(pCalendario->strFecha);
    }

    delete pCalendario;

    funProcesaFecha0();

}

void frmRegPreciosCombustible::on_tbFecha1_clicked(){
    frmCalendario *pCalendario = new frmCalendario(this);

    pCalendario->setWindowTitle(GstrTitulo);
    pCalendario->setWindowFlag(Qt::CustomizeWindowHint);
    pCalendario->setWindowModality(Qt::WindowModal);

    pCalendario->exec();

    if(pCalendario->strFecha != ""){
        ui->txtFecha1->setText(pCalendario->strFecha);
    }

    delete pCalendario;

    funProcesaFecha1();

}

void frmRegPreciosCombustible::on_tlCombustible_itemClicked(QTableWidgetItem *item){
    QString strPrecio;
    int     i;

    ui->cmbCentroTrabajo->setCurrentText(ui->tlCombustible->item(item->row(), 0)->text());
    ui->cmbCombustible->setCurrentText(ui->tlCombustible->item(item->row(), 1)->text());
    ui->txtFecha0->setText(ui->tlCombustible->item(item->row(), 2)->text());
    ui->txtFecha1->setText(ui->tlCombustible->item(item->row(), 3)->text());

    //
    // Obtenemos el precio, le sacamosla $ y cambiamos las comas por puntos
    //
    strPrecio = ui->tlCombustible->item(item->row(), 4)->text();
    strPrecio.remove(strPrecio.length() - 1, 1);
    i=0;
    while (i < strPrecio.length()) {
        if(strPrecio[i] == ','){
            strPrecio[i] = '.';
        }
        i++;
    }

    ui->txtPrecio->setText(strPrecio);
    ui->txtCodigo->setText(ui->tlCombustible->item(item->row(), 5)->text());


    ui->tbEliminar->setEnabled(true);
    ui->tbEliminar->setToolTip("Eliminar precio de un periodo...");

    ui->tbAgregar->setEnabled(false);
    ui->tbAgregar->setToolTip("");

    fila_seleccionada = item->row();
}

void frmRegPreciosCombustible::on_tlCombustible_itemDoubleClicked(QTableWidgetItem *item){

    //
    // Activo los controles
    //
    funHabilitaControles();

    //
    // Activo el boton Guardar y desactivo los botones +`y -
    //
    ui->cmdGuardar->setEnabled(true);
    ui->cmdGuardar->setToolTip("Guardar el precio de un periodo...");

    ui->tbAgregar->setEnabled(false);
    ui->tbAgregar->setToolTip("");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");
    //
    // situo el foco en txtPrecio
    //
    ui->txtPrecio->setFocus();
    ui->txtPrecio->selectAll();
}


