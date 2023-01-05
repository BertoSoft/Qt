#include "frmregtarifasmaquinas.h"
#include "ui_frmregtarifasmaquinas.h"

#include "funcaux.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QMessageBox>
#include <QImageReader>

frmRegTarifasMaquinas::frmRegTarifasMaquinas(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmRegTarifasMaquinas){
    ui->setupUi(this);

    this->installEventFilter(this);

}

frmRegTarifasMaquinas::~frmRegTarifasMaquinas(){
    delete ui;
}

bool frmRegTarifasMaquinas::eventFilter(QObject *obj, QEvent *event){

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

void frmRegTarifasMaquinas::funInicio(){
    QString strRutaDb, strSql, str;
    QString strCodigo, strCentroTrabajo, strRutaFoto, strRutaFotoCod, strMaquina;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Colocamos las columnas del tablelist
    //
    ui->tlTarifas->setColumnCount(3);

    QStringList headers;
    headers += tr("Empresa");
    headers += tr("Máquina");
    headers += tr("Tarifa");

    ui->tlTarifas->setHorizontalHeaderLabels(headers);

    ui->tlTarifas->setColumnWidth(0, (int) ( ( this->width() * 1.1) / 3 ) );
    ui->tlTarifas->setColumnWidth(1, (int) ( ( this->width() * 1.1 ) / 3 ) );
    ui->tlTarifas->setColumnWidth(2, (int) ( ( this->width() * 0.75) / 3 ) );

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    //
    //Rellenamos el cmbEmpresas
    //
    while(ui->cmbCentroTrabajo->count()>0){
        ui->cmbCentroTrabajo->removeItem(0);
    }
    strSql = "SELECT *FROM Empresas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodigo           = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());
        strCentroTrabajo = pFuncAux->funTxtToEmpresas(strCodigo);

        ui->cmbCentroTrabajo->addItem(strCentroTrabajo);
        sql.next();
    }

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
    pFuncAux->funMostrarFoto(ui->lblFoto, strRutaFoto);

    while (sql.isValid()) {

        strCodigo = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());
        strMaquina = pFuncAux->funTxtToMaquinas(strCodigo);

        ui->cmbMaquina->addItem(strMaquina);
        sql.next();
    }

    delete pFuncAux;

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    funRellenaListado();

    //
    // Deseleccionamos la tabla
    //
    on_tbAgregar_clicked();

    //
    // Deshabilitamos todo menos el tb anadir
    //
    ui->cmbCentroTrabajo->setEnabled(false);
    ui->cmbMaquina->setEnabled(false);

    ui->txtPrecio->setEnabled(false);
    ui->txtPrecio->setStyleSheet("color: gray; background-color: white");

    ui->lblFoto->setEnabled(false);

    ui->cmdGuardar->setEnabled(false);
    ui->cmdGuardar->setToolTip("");

    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Añadir un nuevo precio por hora...");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    ui->cmdVolver->setToolTip("Volver al la pantalla principal...");

    //
    // Ponemos el foco en txtPrecio
    //
    ui->txtPrecio->setFocus();

}

void frmRegTarifasMaquinas::funRellenaListado(){
    QString str, strRutaDb, strSql;
    QString strEmpresa, strMaquina, strCodigoEmpresa, strCodigoMaquina, strPrecio;
    int i;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Si el listado no esta vacio lo vaciamos
    //
    while (ui->tlTarifas->rowCount()>0) {
        ui->tlTarifas->removeRow(0);
    }

    //
    // Ahora miramos los registros de la base de datos
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Tarifas";
    sql.exec(strSql);
    sql.first();
    i=0;

    while (sql.isValid()) {

        ui->tlTarifas->setRowCount(i + 1);

        strCodigoEmpresa    = pFuncAux->funDesCifrar_bis(sql.record().value("Empresa").toString());
        strCodigoMaquina    = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());
        strPrecio           = pFuncAux->funDesCifrar_bis(sql.record().value("Precio").toString());

        strEmpresa = pFuncAux->funTxtToEmpresas(strCodigoEmpresa);
        strMaquina = pFuncAux->funTxtToMaquinas(strCodigoMaquina);

        QTableWidgetItem *item_empresa = new QTableWidgetItem(strEmpresa);
        QTableWidgetItem *item_maquina = new QTableWidgetItem(strMaquina);

        str = QLocale(QLocale::Spanish, QLocale::Spain).toCurrencyString(strPrecio.toDouble());
        QTableWidgetItem *item_precio = new QTableWidgetItem(str);

        item_empresa->setTextAlignment(Qt::AlignCenter);
        item_maquina->setTextAlignment(Qt::AlignCenter);
        item_precio->setTextAlignment(Qt::AlignCenter);

        ui->tlTarifas->setItem(i, 0, item_empresa);
        ui->tlTarifas->setItem(i, 1, item_maquina);
        ui->tlTarifas->setItem(i, 2, item_precio);

        i++;
        sql.next();
    }

    //
    // Cierro la base de Datos
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    delete pFuncAux;


}

void frmRegTarifasMaquinas::on_cmdGuardar_clicked(){
    QString strRutaDb, strSql;
    QString strCodEmpresa, strCodMaquina;
    QString strCodEmpresaDb, strCodEmpresaDbCod, strCodMaquinaDb, strCodMaquinaDbCod;
    QString strCodEmpresaCod, strCodMaquinaCod, strPrecioCod;
    bool    existe = false;

    FuncAux* pFuncAux = new FuncAux();

    //
    // si txtPrecio esta vacio lo delvemos al txtPrecio
    //
    if(ui->txtPrecio->text() == ""){
        ui->txtPrecio->setStyleSheet("color: black; background-color: yellow");
        QMessageBox::information(this, GstrTitulo, "\t El Campo del Precio es obligatorio...");
        ui->txtPrecio->setFocus();
    }
    //
    // Si no esta vacio Guardamos y añadimos a la tabla
    //
    else{
        //
        // Ponemos el txtPrecio en blanco
        //
        ui->txtPrecio->setStyleSheet("color: gray; background-color: white");

        //
        // Guardo valores de empresa y maquina
        //
        strCodEmpresa = pFuncAux->funCodigoToEmpresa(ui->cmbCentroTrabajo->currentText());
        strCodMaquina = pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText());

        //
        // Abro la BD
        //
        strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
        QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
        dbSql.setDatabaseName(strRutaDb);
        dbSql.open();
        QSqlQuery sql = QSqlQuery(dbSql);

        strSql = "SELECT *FROM Tarifas";
        sql.exec(strSql);
        sql.first();
        while (sql.isValid()) {
            //
            // Miramos si existe ese registro
            //
            strCodEmpresaDbCod = sql.record().value("Empresa").toString();
            strCodMaquinaDbCod = sql.record().value("Maquina").toString();

            strCodEmpresaDb = pFuncAux->funDesCifrar_bis(strCodEmpresaDbCod);
            strCodMaquinaDb = pFuncAux->funDesCifrar_bis(strCodMaquinaDbCod);

            if( strCodEmpresaDb == strCodEmpresa &&
                strCodMaquinaDb == strCodMaquina){
                existe = true;
                sql.finish();
            }
            sql.next();
        }

        //
        // Si existe avisamos y Borramos el registro antiguo
        //
        if(existe){

            QMessageBox::information(this, GstrTitulo, "Este Registro, ya existe\nSe procede a modificarlo...");

            strSql = "DELETE FROM Tarifas WHERE Empresa='" + strCodEmpresaDbCod + "' And Maquina='" + strCodMaquinaDbCod + "';";
            sql.exec(strSql);

        }

        //
        // Colocamos los nuevos valores Codificados
        //
        strCodEmpresaCod = pFuncAux->funCifrar_bis(strCodEmpresa);
        strCodMaquinaCod = pFuncAux->funCifrar_bis(strCodMaquina);
        strPrecioCod     = pFuncAux->funCifrar_bis(ui->txtPrecio->text());

        strSql = " INSERT INTO Tarifas(Empresa,"
                 "                     Maquina,"
                 "                     Precio) VALUES ('" + strCodEmpresaCod + "',"
                 "                                     '" + strCodMaquinaCod + "',"
                 "                                     '" + strPrecioCod + "');";
        sql.exec(strSql);

        //
        // Cierro la BD
        //
        dbSql.close();
        dbSql.removeDatabase("conSql");

        //
        // Refresco la tabla
        //
        funRellenaListado();

        ui->txtPrecio->setText("");
        ui->cmbCentroTrabajo->setCurrentIndex(0);
        ui->cmbMaquina->setCurrentIndex(0);
        strCodMaquina = pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText());
        pFuncAux->funMostrarFoto(ui->lblFoto, pFuncAux->funRutaFotoMaquinaToCodigo(strCodMaquina));


        ui->cmdGuardar->setEnabled(false);
        ui->cmdGuardar->setToolTip("");

        ui->cmbCentroTrabajo->setEnabled(false);
        ui->cmbMaquina->setEnabled(false);
        ui->lblFoto->setEnabled(false);

        ui->txtPrecio->setEnabled(false);
        ui->txtPrecio->setStyleSheet("color: gray; background-color: white");

        ui->tbAgregar->setEnabled(true);
        ui->tbAgregar->setToolTip("Añadir un nuevo precio por hora...");

    }

    delete pFuncAux;

}

void frmRegTarifasMaquinas::on_cmdVolver_clicked(){

    this->close();

}

void frmRegTarifasMaquinas::on_tbAgregar_clicked(){
    QString strCodMaquina;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Deseleccionamos la tabla
    //
    ui->tlTarifas->clearSelection();

    //
    // Activamos los cmb el txt y cmdGuardar
    //
    ui->cmbCentroTrabajo->setEnabled(true);
    ui->cmbMaquina->setEnabled(true);

    ui->txtPrecio->setEnabled(true);
    ui->txtPrecio->setStyleSheet("color: black; background-color: white");

    ui->txtPrecio->setText("");

    ui->lblFoto->setEnabled(true);
    strCodMaquina = pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText());
    pFuncAux->funMostrarFoto(ui->lblFoto, pFuncAux->funRutaFotoMaquinaToCodigo(strCodMaquina));

    ui->cmdGuardar->setEnabled(true);
    ui->cmdGuardar->setToolTip("Guardar este precio por hora...");

    //
    // Desactivamos el tbanadir
    //
    ui->tbAgregar->setEnabled(false);
    ui->tbAgregar->setToolTip("");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    //
    // Ponemos el foco en txtPrecio
    //
    ui->txtPrecio->setFocus();

    delete pFuncAux;

}

void frmRegTarifasMaquinas::on_tbEliminar_clicked(){
    QString strRutaDb, strSql;
    QString strCodEmpresa, strCodMaquina;
    QString strCodEmpresaDbCod, strCodMaquinaDbCod, strCodEmpresaDb, strCodMaquinaDb;
    QString strCodigoMaquina;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Abro la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strCodEmpresa = pFuncAux->funCodigoToEmpresa(ui->cmbCentroTrabajo->currentText());
    strCodMaquina = pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText());

    //
    // Tengo que encontrar en la BD los codigos
    //
    strSql = "SELECT *FROM Tarifas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodEmpresaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Empresa").toString());
        strCodMaquinaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());

        if(strCodEmpresaDb == strCodEmpresa &&
                strCodMaquinaDb == strCodMaquina){

            strCodEmpresaDbCod = sql.record().value("Empresa").toString();
            strCodMaquinaDbCod = sql.record().value("Maquina").toString();
            sql.finish();
        }

        sql.next();
    }

    strSql = "DELETE FROM Tarifas WHERE Empresa='" + strCodEmpresaDbCod + "'AND Maquina='" + strCodMaquinaDbCod + "';";
    sql.exec(strSql);

    //
    // Limpiamos Txt, y desactivamos los controles
    //
    ui->cmbCentroTrabajo->setCurrentIndex(0);
    ui->cmbMaquina->setCurrentIndex(0);
    strCodMaquina = pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText());
    pFuncAux->funMostrarFoto(ui->lblFoto, pFuncAux->funRutaFotoMaquinaToCodigo(strCodMaquina));

    ui->txtPrecio->setText("");

    ui->cmbCentroTrabajo->setEnabled(false);
    ui->cmbMaquina->setEnabled(false);

    ui->txtPrecio->setEnabled(false);
    ui->txtPrecio->setStyleSheet("color: gray; background-color: white");

    //
    // Rellenamos la nueva tabla
    //
    funRellenaListado();

    //
    // desactivo boton - y activo el +
    //
    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Añadir un nuevo precio por hora...");

    //
    // Cierro la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    delete pFuncAux;

}

void frmRegTarifasMaquinas::on_cmbCentroTrabajo_activated(int index){

    ui->txtPrecio->setFocus();
    ui->txtPrecio->selectAll();
}

void frmRegTarifasMaquinas::on_cmbMaquina_activated(int index){
    QString strRutaDb, strSql;
    QString strRutaFoto, strCodMaquinaDb;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Abro la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Maquinas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodMaquinaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());

        if(strCodMaquinaDb == pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText())){
            strRutaFoto = pFuncAux->funDesCifrar_bis(sql.record().value("Foto").toString());
            sql.finish();
        }
        sql.next();
    }

    //
    // Cierro la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    //
    // Muestro la nueva foto de la maquina
    //
    pFuncAux->funMostrarFoto(ui->lblFoto, strRutaFoto);

    delete pFuncAux;

    ui->txtPrecio->setFocus();

}

void frmRegTarifasMaquinas::on_txtPrecio_returnPressed(){

    on_cmdGuardar_clicked();
}

void frmRegTarifasMaquinas::on_txtPrecio_textChanged(const QString &arg1){
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

void frmRegTarifasMaquinas::on_tlTarifas_itemClicked(QTableWidgetItem *item){
    QString strRutaDb, strSql;
    QString strEmpresa, strMaquina, strPrecio;
    QString strCodEmpresa, strCodMaquina, str, strRutaFoto;
    int fila;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Abro la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Tarifas";
    sql.exec(strSql);
    sql.first();
    fila = 0;
    while (sql.isValid()) {
        if(fila == item->row()){

            strCodEmpresa   = pFuncAux->funDesCifrar_bis(sql.record().value("Empresa").toString());
            strCodMaquina   = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());
            strPrecio       = pFuncAux->funDesCifrar_bis(sql.record().value("Precio").toString());

            strEmpresa  = pFuncAux->funTxtToEmpresas(strCodEmpresa);
            strMaquina  = pFuncAux->funTxtToMaquinas(strCodMaquina);
            str         = QLocale(QLocale::Spanish, QLocale::Spain).toCurrencyString(strPrecio.toDouble());

            ui->cmbCentroTrabajo->setCurrentText(strEmpresa);
            ui->cmbMaquina->setCurrentText(strMaquina);
            ui->txtPrecio->setText(str);

            strRutaFoto = pFuncAux->funRutaFotoMaquinaToCodigo(strCodMaquina);
            pFuncAux->funMostrarFoto(ui->lblFoto, strRutaFoto);

            sql.finish();
        }

        fila++;
        sql.next();
    }

    //
    // Cierro la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    delete pFuncAux;

    //
    // Desactivo los controles
    //
    ui->cmbCentroTrabajo->setEnabled(false);
    ui->cmbMaquina->setEnabled(false);

    ui->txtPrecio->setEnabled(false);
    ui->txtPrecio->setStyleSheet("color: gray; background-color: white");

    //
    // Activo el boton anadir y desactivo el boton guardar
    //
    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Añadir un nuevo precio por hora...");

    ui->cmdGuardar->setEnabled(false);
    ui->cmdGuardar->setToolTip("");

    //
    // Activo el boton de suprimir
    //
    if(item->text() != ""){
        ui->tbEliminar->setEnabled(true);
        ui->tbEliminar->setToolTip("Eliminar el registro seleccionado...");
    }

}

void frmRegTarifasMaquinas::on_tlTarifas_itemDoubleClicked(QTableWidgetItem *item){

    //
    // Activo los controles
    //
    ui->cmbCentroTrabajo->setEnabled(true);
    ui->cmbMaquina->setEnabled(true);

    ui->txtPrecio->setEnabled(true);
    ui->txtPrecio->setStyleSheet("color: black; background-color: white");

    ui->lblFoto->setEnabled(true);

    //
    // Activo el boton Guardar y desactivo los botones +`y -
    //
    ui->cmdGuardar->setEnabled(true);
    ui->cmdGuardar->setToolTip("Guardar este precio por hora...");

    ui->tbAgregar->setEnabled(false);
    ui->tbAgregar->setToolTip("");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    //
    // situo el foco en txtNombre
    //
    ui->txtPrecio->setFocus();
    ui->txtPrecio->selectAll();
}


