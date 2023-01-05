#include "frmgesaverias.h"
#include "ui_frmgesaverias.h"


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


frmGesAverias::frmGesAverias(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmGesAverias){

    ui->setupUi(this);

    this->installEventFilter(this);
}

frmGesAverias::~frmGesAverias(){
    delete ui;
}

bool frmGesAverias::eventFilter(QObject *obj, QEvent *event){

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

void frmGesAverias::funInicio(){
    QString strRutaDb, strSql;
    QDate   qdFecha;
    QLocale qlTraductor;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Colocamos las columnas del tablelist
    //
    ui->tlAverias->setColumnCount(6);

    QStringList headers;
    headers += tr("Fecha");
    headers += tr("Máquina");
    headers += tr("Horas");
    headers += tr("Descripción de la Avería");
    headers += tr("Precio");
    headers += tr("Código");

    ui->tlAverias->setHorizontalHeaderLabels(headers);

    ui->tlAverias->setColumnWidth(0, (int) ( ( this->width() * 0.5 ) / 6 ) );
    ui->tlAverias->setColumnWidth(1, (int) ( ( this->width() * 1.1 ) / 6 ) );
    ui->tlAverias->setColumnWidth(2, (int) ( ( this->width() * 0.5 ) / 6 ) );
    ui->tlAverias->setColumnWidth(3, (int) ( ( this->width() * 2.8 ) / 6 ) );
    ui->tlAverias->setColumnWidth(4, (int) ( ( this->width() * 0.5 ) / 6 ) );
    ui->tlAverias->setColumnWidth(5, (int) ( ( this->width() * 0.5 ) / 6 ) );

    //
    // Colocamos la fecha en txtFecha
    //
    qdFecha = QDate::currentDate();
    ui->txtFecha->setText(qlTraductor.toString(qdFecha,"dd/MM/yyyy"));

    ui->tbFecha->setToolTip("Escoger una fecha en el Calendario...");

    //
    // Rellenamos los combos y el listado
    //
    funRellenaCombos();
    funRellenaListado();

    //
    // tbEliminar y cmdGuardar disabled
    //
    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    ui->cmdGuardar->setEnabled(false);
    ui->cmdGuardar->setToolTip("");

    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Añadir una Nueva Avería a la Base de Datos...");

    ui->cmdVolver->setToolTip("Volver a la pantalla principal...");

    //
    // Limpiamos los controles
    //
    ui->cmbMaquina->setCurrentIndex(0);
    ui->txtDescripcion->setText("");
    ui->txtHoras->setText("");
    ui->txtPrecio->setText("");
    ui->txtCodigo->setText("");

    ui->txtHoras->setStyleSheet("color: gray; background-color: white");
    ui->txtPrecio->setStyleSheet("color: gray; background-color: white");
    ui->txtDescripcion->setStyleSheet("color: gray; background-color: white");

    ui->txtCodigo->setEnabled(false);
    ui->txtCodigo->setStyleSheet("color: gray; background-color: white");

    funDeshabilitaControles();

    ui->lblFotoMaquina->setFocus();

    delete pFuncAux;

}

void frmGesAverias::funRellenaCombos(){
    QString strRutaDb, strSql, str;
    QString strCodMaquina, strRutaFoto;

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
    //Rellenamos el cmbMaquinas
    //
    while(ui->cmbMaquina->count()>0){
        ui->cmbMaquina->removeItem(0);
    }

    ui->cmbMaquina->addItem("Escoje una Máquina...");

    strSql = "SELECT *FROM Maquinas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodMaquina       = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());
        str                 = pFuncAux->funTxtToMaquinas(strCodMaquina);

        ui->cmbMaquina->addItem(str);
        sql.next();
    }

    //
    // Mostramos la foto
    //
    strRutaFoto = pFuncAux->funRutaFotoMaquinaToCodigo(pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText()));
    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, strRutaFoto);

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    delete pFuncAux;

}

void frmGesAverias::funRellenaListado(){
    QString strRutaDb, strSql, strMaquina, str;
    QString strCodMaquina, strFecha, strDescripcion, strHoras, strPrecio, strCodigo;
    int fila;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Si el listado no esta vacio lo vaciamos
    //
    while (ui->tlAverias->rowCount()>0) {
        ui->tlAverias->removeRow(0);
    }

    //
    // Ahora miramos los registros de la base de datos
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Averias";
    sql.exec(strSql);
    sql.first();
    fila=0;

    while (sql.isValid()) {

        ui->tlAverias->setRowCount(fila + 1);

        strFecha            = pFuncAux->funDesCifrar_bis(sql.record().value("Fecha").toString());
        strCodMaquina       = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());
        strMaquina          = pFuncAux->funTxtToMaquinas(strCodMaquina);
        strHoras            = pFuncAux->funDesCifrar_bis(sql.record().value("Horas").toString());
        strDescripcion      = pFuncAux->funDesCifrar_bis(sql.record().value("Descripcion").toString());
        strPrecio           = pFuncAux->funDesCifrar_bis(sql.record().value("Precio").toString());
        strCodigo           = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());

        QTableWidgetItem *item_fecha = new QTableWidgetItem(strFecha);
        QTableWidgetItem *item_maquina = new QTableWidgetItem(strMaquina);
        QTableWidgetItem *item_horas = new QTableWidgetItem(strHoras);
        QTableWidgetItem *item_descripcion = new QTableWidgetItem(strDescripcion);

        str = QLocale(QLocale::Spanish, QLocale::Spain).toCurrencyString(strPrecio.toDouble());
        QTableWidgetItem *item_precio = new QTableWidgetItem(str);

        QTableWidgetItem *item_codigo = new QTableWidgetItem(strCodigo);

        item_fecha->setTextAlignment(Qt::AlignCenter);
        item_maquina->setTextAlignment(Qt::AlignCenter);
        item_horas->setTextAlignment(Qt::AlignCenter);
        item_descripcion->setTextAlignment(Qt::AlignCenter);
        item_precio->setTextAlignment(Qt::AlignCenter);
        item_codigo->setTextAlignment(Qt::AlignCenter);

        ui->tlAverias->setItem(fila, 0, item_fecha);
        ui->tlAverias->setItem(fila, 1, item_maquina);
        ui->tlAverias->setItem(fila, 2, item_horas);
        ui->tlAverias->setItem(fila, 3, item_descripcion);
        ui->tlAverias->setItem(fila, 4, item_precio);
        ui->tlAverias->setItem(fila, 5, item_codigo);

        fila++;
        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    //
    // Ordenamos la tabla
    //
    ui->tlAverias->sortItems(1, Qt::AscendingOrder);

    delete pFuncAux;


}

void frmGesAverias::funHabilitaControles(){

    ui->txtFecha->setEnabled(true);

    ui->tbFecha->setEnabled(true);
    ui->tbFecha->setToolTip("Escoger una fecha en el Calendario...");

    ui->txtDescripcion->setEnabled(true);
    ui->txtDescripcion->setStyleSheet("color: black; background-color: white");

    ui->cmbMaquina->setEnabled(true
                               );
    ui->txtHoras->setEnabled(true);
    ui->txtHoras->setStyleSheet("color: black; background-color: white");

    ui->txtPrecio->setEnabled(true);
    ui->txtPrecio->setStyleSheet("color: black; background-color: white");


}

void frmGesAverias::funDeshabilitaControles(){

    ui->txtFecha->setEnabled(false);

    ui->tbFecha->setEnabled(false);
    ui->tbFecha->setToolTip("");

    ui->txtDescripcion->setEnabled(false);
    ui->cmbMaquina->setEnabled(false);
    ui->txtHoras->setEnabled(false);
    ui->txtPrecio->setEnabled(false);

}

void frmGesAverias::on_cmdVolver_clicked(){

    this->close();
}

void frmGesAverias::on_cmbMaquina_activated(int index){
    QString strCodMaquina, strRutaFoto;
    FuncAux* pFuncAux = new FuncAux();

    //
    // Colocamos las horas actuales de la funHorasMaquina
    //
    strCodMaquina = pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText());
    ui->txtHoras->setText(pFuncAux->funHorasToMaquina(strCodMaquina));

    //
    // Mostramos la foto
    //
    strRutaFoto = pFuncAux->funRutaFotoMaquinaToCodigo(strCodMaquina);
    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, strRutaFoto);

    ui->txtDescripcion->setFocus();
    ui->txtDescripcion->selectAll();

    delete pFuncAux;
}

void frmGesAverias::on_txtFecha_returnPressed(){
    QDate   datFecha;
    QLocale qlTraductor;

    FuncAux* pFuncAux = new FuncAux();

    datFecha = pFuncAux->funStrToDate(ui->txtFecha->text());

    //
    // Si la fecha es valida
    //
    if(datFecha.isValid()){
        ui->txtFecha->deselect();
        ui->txtDescripcion->setFocus();
        ui->txtDescripcion->selectAll();

    }
    //
    // Si la fecha no es valida
    //
    else{
        QMessageBox::information(this, GstrTitulo, "\t Formato de fecha no reconocido...");

        datFecha = QDate::currentDate();
        ui->txtFecha->setText(qlTraductor.toString(datFecha,"dd/MM/yyyy"));

        ui->txtFecha->setFocus();
        ui->txtFecha->selectAll();
    }


    delete pFuncAux;

}

void frmGesAverias::on_txtDescripcion_returnPressed(){

    ui->txtPrecio->setFocus();
    ui->txtPrecio->selectAll();
}

void frmGesAverias::on_txtPrecio_returnPressed(){

    ui->txtHoras->setFocus();
    ui->txtHoras->selectAll();
}

void frmGesAverias::on_txtHoras_returnPressed(){

   on_cmdGuardar_clicked();
}

void frmGesAverias::on_cmdGuardar_clicked(){
    QString strRutaDb, strSql;
    QString strCodigo, strCodigoCod;
    QString strFechaCod, strCodMaquinaCod, strDescripcionCod, strHorasCod, strPrecioCod;
    QString strCodMaquina;
    bool    bExiste;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Si cmbMaquina esta vacio le damos el foco a txtFecha
    //
    if(ui->cmbMaquina->currentIndex() == 0){
        QMessageBox::information(this, GstrTitulo, "\t Debes escojer una máquina...");

        //ui->txtFecha->setStyleSheet("color: black; background-color: yellow");
        ui->txtFecha->setFocus();
    }
    //
    // Si hay maquina pero no txtAverias
    //
    else if(ui->txtDescripcion->text() == ""){
        QMessageBox::information(this, GstrTitulo, "\t Debes poner una descripción de la avería...");

        ui->txtDescripcion->setStyleSheet("color: black; background-color: yellow");
        ui->txtDescripcion->setFocus();
    }
    //
    // Hay Máquina, txtAveria pero no txtPrecio
    else if(ui->txtPrecio->text() == ""){
        QMessageBox::information(this, GstrTitulo, "\t Debes poner un precio de la avería...");

        ui->txtPrecio->setStyleSheet("color: black; background-color: yellow");
        ui->txtPrecio->setFocus();
    }

    //
    // Tenemos los tres txt llenos todo correcto, Guardamos registro
    //
    if( ui->cmbMaquina->currentIndex() > 0 && ui->txtDescripcion->text() != "" && ui->txtPrecio->text() != "" ){
        //
        // Abro la BD
        //
        strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
        QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
        dbSql.setDatabaseName(strRutaDb);
        dbSql.open();
        QSqlQuery sql = QSqlQuery(dbSql);

        //
        // Comprobamos si existe, y si existe borramos el registro
        //
        strSql = "SELECT *FROM Averias";
        sql.exec(strSql);
        sql.first();
        while (sql.isValid()) {

            strCodigoCod    = sql.record().value("Codigo").toString();
            strCodigo       = pFuncAux->funDesCifrar_bis(strCodigoCod);

            if(strCodigo == ui->txtCodigo->text()){
                bExiste = true;
                sql.finish();
            }

            sql.next();
        }

        if(bExiste){
            //
            // Borramos el registro
            //
            strSql = "DELETE FROM Averias WHERE Codigo ='" + strCodigoCod + "' ;";
            sql.exec(strSql);

        }


        //
        // Preparo los valores
        //
        strCodMaquina   = pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText());

        //
        // Codifico los valores
        //
        strFechaCod         = pFuncAux->funCifrar_bis(ui->txtFecha->text());
        strCodMaquinaCod    = pFuncAux->funCifrar_bis(strCodMaquina);
        strHorasCod         = pFuncAux->funCifrar_bis(ui->txtHoras->text());
        strDescripcionCod   = pFuncAux->funCifrar_bis(ui->txtDescripcion->text());
        strPrecioCod        = pFuncAux->funCifrar_bis(ui->txtPrecio->text());
        strCodigoCod        = pFuncAux->funCifrar_bis(ui->txtCodigo->text());


        strSql = " INSERT INTO Averias(Fecha,"
                 "                     Maquina,"
                 "                     Horas,"
                 "                     Descripcion,"
                 "                     Precio,"
                 "                     Codigo) VALUES ('" + strFechaCod + "',"
                 "                                     '" + strCodMaquinaCod + "',"
                 "                                     '" + strHorasCod + "',"
                 "                                     '" + strDescripcionCod + "',"
                 "                                     '" + strPrecioCod + "',"
                 "                                     '" + strCodigoCod + "');";
        sql.exec(strSql);

        //
        // Cierro la BD
        //
        dbSql.close();
        dbSql.removeDatabase("conSql");

        funInicio();


    }
}

void frmGesAverias::on_tbAgregar_clicked(){
    QDate   qdFecha;
    QLocale qlTraductor;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Limpiamos los controles y los habilitamos
    //
    funHabilitaControles();

    //
    // Colocamos el txtHoras y el txtPrecio en Negro
    //
    ui->txtDescripcion->setStyleSheet("color: black; background-color: white");
    ui->txtPrecio->setStyleSheet("color: black; background-color: white");

    //
    // Colocamos la fecha en txtFecha
    //
    qdFecha = QDate::currentDate();
    ui->txtFecha->setText(qlTraductor.toString(qdFecha,"dd/MM/yyyy"));

    ui->txtDescripcion->setText("");
    ui->txtHoras->setText("");
    ui->txtPrecio->setText("");

    ui->cmbMaquina->setCurrentIndex(0);
    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, "");

    //
    // Colocamos el codigo
    //
    ui->txtCodigo->setText(pFuncAux->funNuevoCodigo("Averias"));

    //
    // Deseleccionamos la tabla
    //
    ui->tlAverias->clearSelection();

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
    ui->cmdGuardar->setToolTip("Guardar esta tarea de mantenimiento...");

    ui->txtDescripcion->setFocus();

    delete pFuncAux;
}

void frmGesAverias::on_tbEliminar_clicked(){
    QString strRutaDb, strSql;
    QString strFechaDbCod, strMaquinaDbCod, strHorasDbCod, strDescripcionDbCod, strPrecioDbCod, strCodigoDbCod;
    QString strFechaDb, strMaquinaDb, strCodMaquinaDb, strHorasDb, strDescripcionDb, strPrecioDb, strCodigoDb;
    QDate   qdFecha;
    QLocale qlTraductor;
    bool bExiste = false;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Borramos la fila seleccionada
    //
    if(Gfila_seleccionada >= 0){
        //
        // Abrimos la BD
        //
        strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
        QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
        dbSql.setDatabaseName(strRutaDb);
        dbSql.open();
        QSqlQuery sql = QSqlQuery(dbSql);

        //
        // Buscamos el registro
        //
        strSql = "SELECT *FROM Averias";
        sql.exec(strSql);
        sql.first();
        while (sql.isValid()) {

            strCodigoDbCod      = sql.record().value("Codigo").toString();
            strCodigoDb         = pFuncAux->funDesCifrar_bis(strCodigoDbCod);

            if(strCodigoDb == ui->tlAverias->item(Gfila_seleccionada, 5)->text() ){
                bExiste = true;
                sql.finish();
            }

            sql.next();
        }

        //
        // Borramos el registro
        //
        if(bExiste){

            strSql = "DELETE FROM Averias WHERE Codigo ='" + strCodigoDbCod + "';";
            sql.exec(strSql);

        }

        //
        // Cerramos la BD
        //
        dbSql.close();
        dbSql.removeDatabase("conSql");


        //
        //Desactivamos tbEliminar
        //
        ui->tbEliminar->setEnabled(false);
        ui->tbEliminar->setToolTip("");

        //
        // Colocamos la fecha en txtFecha
        //
        qdFecha = QDate::currentDate();
        ui->txtFecha->setText(qlTraductor.toString(qdFecha,"dd/MM/yyyy"));

        ui->txtDescripcion->setText("");
        ui->txtHoras->setText("");
        ui->txtPrecio->setText("");
        ui->cmbMaquina->setCurrentIndex(0);
        pFuncAux->funMostrarFoto(ui->lblFotoMaquina, "");

        //
        // Deseleccionamos la tabla y desactivamos el boton realizado
        //
        ui->tlAverias->clearSelection();

        funRellenaListado();

        Gfila_seleccionada = -1;

        ui->txtDescripcion->setFocus();
        ui->txtDescripcion->selectAll();
    }

    delete pFuncAux;

}

void frmGesAverias::on_tbFecha_clicked(){

    frmCalendario *pCalendario = new frmCalendario(this);

    pCalendario->setWindowTitle(GstrTitulo);
    pCalendario->setWindowFlag(Qt::CustomizeWindowHint);
    pCalendario->setWindowModality(Qt::WindowModal);

    pCalendario->exec();

    if(pCalendario->strFecha != ""){
        ui->txtFecha->setText(pCalendario->strFecha);
    }

    delete pCalendario;
}

void frmGesAverias::on_tlAverias_itemClicked(QTableWidgetItem *item){
    QString strPrecio, strRutaFoto, strCodMaquina;
    int     i;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Habilitamnos el cmdEliminar
    //
    ui->tbEliminar->setEnabled(true);
    ui->tbEliminar->setToolTip("Eliminar este registro...");

    Gfila_seleccionada = item->row();

    //
    // Rellenamos los txt y lo cmb y la foto de la maquina
    //
    ui->txtFecha->setText(ui->tlAverias->item(Gfila_seleccionada, 0)->text());
    ui->cmbMaquina->setCurrentText(ui->tlAverias->item(Gfila_seleccionada, 1)->text());
    ui->txtHoras->setText(ui->tlAverias->item(Gfila_seleccionada, 2)->text());
    ui->txtDescripcion->setText(ui->tlAverias->item(Gfila_seleccionada, 3)->text());

    //
    // Obtenemos el precio, le sacamosla $ y cambiamos las comas por puntos
    //
    strPrecio = ui->tlAverias->item(item->row(), 4)->text();
    strPrecio.remove(strPrecio.length() - 1, 1);
    i=0;
    while (i < strPrecio.length()) {
        if(strPrecio[i] == ','){
            strPrecio[i] = '.';
        }
        i++;
    }

    ui->txtPrecio->setText(strPrecio);    ui->txtCodigo->setText(ui->tlAverias->item(Gfila_seleccionada, 5)->text());

    strCodMaquina   = pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText());
    strRutaFoto     = pFuncAux->funRutaFotoMaquinaToCodigo(strCodMaquina);
    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, strRutaFoto);

    delete pFuncAux;
}

void frmGesAverias::on_txtPrecio_textChanged(const QString &arg1){
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

void frmGesAverias::on_txtHoras_textChanged(const QString &arg1){
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
                ui->txtHoras->setText(str);
            }
        }

    }

    //
    // Si su longitud es dos solo permito str == 0.
    if(str.length() == 2){
        if(str != "0."){
            if(numero == 0){
                str.remove(str.length() - 1, 1);
                ui->txtHoras->setText(str);
            }
        }
    }

    //
    // S i su longitud es mayor de 2 solo si es numero
    //
    if(str.length() > 2 && numero == 0){
        str.remove(str.length() - 1, 1);
        ui->txtHoras->setText(str);
    }


}

void frmGesAverias::on_tlAverias_itemDoubleClicked(QTableWidgetItem *item){

    //
    // Activamos los controles
    //
    funHabilitaControles();

    //
    // Deseleccionamos la tabla
    //
    ui->tlAverias->clearSelection();

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
    ui->cmdGuardar->setToolTip("Guardar esta tarea de mantenimiento...");

    ui->txtDescripcion->setFocus();
    ui->txtDescripcion->selectAll();


}

