#include "frmregmantenimiento.h"
#include "ui_frmregmantenimiento.h"

#include "funcaux.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QMessageBox>
#include <QScreen>
#include <QTableWidgetItem>



frmRegMantenimiento::frmRegMantenimiento(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmRegMantenimiento){
    ui->setupUi(this);

    this->installEventFilter(this);
}

frmRegMantenimiento::~frmRegMantenimiento(){
    delete ui;
}

bool frmRegMantenimiento::eventFilter(QObject *obj, QEvent *event){
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

void frmRegMantenimiento::funInicio(){
    int ancho_src, alto_src;
    int ancho_form, alto_form;
    int ix0, iy0;

    //
    // Esto no daria el tamaño de la ventana recpecto a la pantalla
    //
    /*
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
    ui->tlMantenimiento->setColumnCount(8);

    QStringList headers;
    headers += tr("Nombre Trabajo");
    headers += tr("Descripción");
    headers += tr("Periodicidad");
    headers += tr("Max. Horas");
    headers += tr("Op. Correctiva");
    headers += tr("Op. Preventiva");
    headers += tr("Precio");
    headers += tr("Código");

    ui->tlMantenimiento->setHorizontalHeaderLabels(headers);

    ui->tlMantenimiento->setColumnWidth(0, (int) ( ( this->width() * 1.40 ) / 8 ) );
    ui->tlMantenimiento->setColumnWidth(1, (int) ( ( this->width() * 2.60 ) / 8 ) );
    ui->tlMantenimiento->setColumnWidth(2, (int) ( ( this->width() * 1.2 ) / 8 ) );
    ui->tlMantenimiento->setColumnWidth(3, (int) ( ( this->width() * 0.5 ) / 8 ) );
    ui->tlMantenimiento->setColumnWidth(4, (int) ( ( this->width() * 0.6 ) / 8 ) );
    ui->tlMantenimiento->setColumnWidth(5, (int) ( ( this->width() * 0.6 ) / 8 ) );
    ui->tlMantenimiento->setColumnWidth(6, (int) ( ( this->width() * 0.4 ) / 8 ) );
    ui->tlMantenimiento->setColumnWidth(7, (int) ( ( this->width() * 0.4 ) / 8 ) );

    funRellenaListado();
    funDesHabilitaControles();

    ui->cmdGuardar->setEnabled(false);
    ui->cmdGuardar->setToolTip("");

    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Agregar una nueva OP. de Mantenimiento...");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    ui->cmdVolver->setToolTip("Volver a la pantalla Principal...");

    ui->txtCodigo->setEnabled(false);

    //
    // Para unificar intensidad cuando los controles estan inhabilitados
    //
    /*
    ui->txtNombre->setStyleSheet("color: black; background-color: white");
    ui->txtDescripcion->setStyleSheet("color: black; background-color: white");
    ui->txtLimiteHoras->setStyleSheet("color: black; background-color: white");
    ui->txtPrecio->setStyleSheet("color: black; background-color: white");
    ui->cmbPeriocidad->setStyleSheet("color: black; background-color: white");
    ui->cmbTipoMantenimiento->setStyleSheet("color: black; background-color: white");
    ui->txtCodigo->setStyleSheet("color: black; background-color: white");


    ui->label_2->setStyleSheet("color: black; background-color: white");
    ui->label_3->setStyleSheet("color: black; background-color: white");
    ui->label_4->setStyleSheet("color: black; background-color: white");
    ui->label_5->setStyleSheet("color: black; background-color: white");
    ui->label_6->setStyleSheet("color: black; background-color: white");
    ui->label_7->setStyleSheet("color: black; background-color: white");
    ui->label_8->setStyleSheet("color: black; background-color: white");
    */

}

void frmRegMantenimiento::funRellenaListado(){
    QString strRutaDb, strSql, str;
    QString strNombre, strDescripcion, strPeriodicidad, strLimiteHoras;
    QString strCorrectivo, strPreventivo, strPrecio, strCodigo;
    int     i, iPrecio;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Si el listado no esta vacio lo vaciamos
    //
    while (ui->tlMantenimiento->rowCount()>0) {
        ui->tlMantenimiento->removeRow(0);
    }

    //
    // Ahora miramos los registros de la base de datos
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Mantenimiento";
    sql.exec(strSql);
    sql.first();
    i=0;

    while (sql.isValid()) {

        ui->tlMantenimiento->setRowCount(i + 1);

        //
        //Decodificamos los valores
        //
        strNombre           = pFuncAux->funDesCifrar_bis(sql.record().value("Trabajo").toString());
        strDescripcion      = pFuncAux->funDesCifrar_bis(sql.record().value("Descripcion").toString());
        strPeriodicidad     = pFuncAux->funDesCifrar_bis(sql.record().value("Periodico").toString());
        strLimiteHoras      = pFuncAux->funDesCifrar_bis(sql.record().value("MaxHoras").toString());
        strCorrectivo       = pFuncAux->funDesCifrar_bis(sql.record().value("Correctivo").toString());
        strPreventivo       = pFuncAux->funDesCifrar_bis(sql.record().value("Preventivo").toString());
        strPrecio           = pFuncAux->funDesCifrar_bis(sql.record().value("Precio").toString());
        strCodigo           = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());


        QTableWidgetItem *item_nombre = new QTableWidgetItem(strNombre);
        QTableWidgetItem *item_descripcion = new QTableWidgetItem(strDescripcion);
        QTableWidgetItem *item_periodicidad = new QTableWidgetItem(strPeriodicidad);
        QTableWidgetItem *item_max_horas = new QTableWidgetItem(strLimiteHoras);
        QTableWidgetItem *item_tipo_op_correctivo = new QTableWidgetItem(strCorrectivo);
        QTableWidgetItem *item_tipo_op_preventivo = new QTableWidgetItem(strPreventivo);

        str = QLocale(QLocale::Spanish, QLocale::Spain).toCurrencyString(strPrecio.toDouble());
        QTableWidgetItem *item_precio = new QTableWidgetItem(str);

        QTableWidgetItem *item_codigo = new QTableWidgetItem(strCodigo);

        item_nombre->setTextAlignment(Qt::AlignCenter);
        item_descripcion->setTextAlignment(Qt::AlignCenter);
        item_periodicidad->setTextAlignment(Qt::AlignCenter);
        item_max_horas->setTextAlignment(Qt::AlignCenter);
        item_tipo_op_correctivo->setTextAlignment(Qt::AlignCenter);
        item_tipo_op_preventivo->setTextAlignment(Qt::AlignCenter);
        item_precio->setTextAlignment(Qt::AlignCenter);
        item_codigo->setTextAlignment(Qt::AlignCenter);

        ui->tlMantenimiento->setItem(i, 0, item_nombre);
        ui->tlMantenimiento->setItem(i, 1, item_descripcion);
        ui->tlMantenimiento->setItem(i, 2, item_periodicidad);
        ui->tlMantenimiento->setItem(i, 3, item_max_horas);
        ui->tlMantenimiento->setItem(i, 4, item_tipo_op_correctivo);
        ui->tlMantenimiento->setItem(i, 5, item_tipo_op_preventivo);
        ui->tlMantenimiento->setItem(i, 6, item_precio);
        ui->tlMantenimiento->setItem(i, 7, item_codigo);

        i++;
        sql.next();
    }

    dbSql.close();
    dbSql.removeDatabase("conSql");

    delete pFuncAux;
}

void frmRegMantenimiento::funDesHabilitaControles(){

    ui->txtNombre->setEnabled(false);
    ui->txtDescripcion->setEnabled(false);
    ui->txtLimiteHoras->setEnabled(false);
    ui->txtPrecio->setEnabled(false);
    ui->cmbPeriocidad->setEnabled(false);
    ui->cmbTipoMantenimiento->setEnabled(false);

}

void frmRegMantenimiento::funHabilitaControles(){

    ui->txtNombre->setEnabled(true);
    ui->txtDescripcion->setEnabled(true);

    if(ui->cmbTipoMantenimiento->currentIndex() == 1 ){
        ui->txtLimiteHoras->setEnabled(false);
    }
    else{
        ui->txtLimiteHoras->setEnabled(true);
    }

    ui->txtPrecio->setEnabled(true);
    ui->cmbPeriocidad->setEnabled(true);
    ui->cmbTipoMantenimiento->setEnabled(true);

}

void frmRegMantenimiento::funLimpiaControles(){

    ui->txtNombre->setText("");
    ui->txtDescripcion->setText("");
    ui->txtLimiteHoras->setText("");
    ui->txtPrecio->setText("");
    ui->txtCodigo->setText("");

    ui->txtNombre->setStyleSheet("color: black; background-color: white");
    ui->txtDescripcion->setStyleSheet("color: black; background-color: white");
    ui->txtLimiteHoras->setStyleSheet("color: black; background-color: white");
    ui->txtPrecio->setStyleSheet("color: black; background-color: white");
    ui->txtCodigo->setStyleSheet("color: black; background-color: white");

}

void frmRegMantenimiento::on_cmdVolver_clicked(){

    this->close();
}

void frmRegMantenimiento::on_cmdGuardar_clicked(){
    QString strRutaDb, strSql, strCorrectivo, strPreventivo;
    QString strCodigo, strCodigoViejoCod, strCodigoCod;
    QString strNombreCod, strDescripcionCod, strPeriodicidadCod;
    QString strLimiteHorasCod, strPreventivoCod, strCorrectivoCod, strPrecioCod;
    bool    existe = false;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Si el campo del nombre esta cubierto seguimos y guardamos los valores
    //
    if(ui->txtNombre->text() != ""){

        //
        // Abro la BD
        //
        strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
        QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
        dbSql.setDatabaseName(strRutaDb);
        dbSql.open();
        QSqlQuery sql = QSqlQuery(dbSql);

        //
        // comprobamos si existe este registro
        //
        strSql = "SELECT *FROM Mantenimiento";
        sql.exec(strSql);
        sql.first();
        while (sql.isValid()) {

            strCodigoCod    = sql.record().value("Codigo").toString();
            strCodigo       = pFuncAux->funDesCifrar_bis(strCodigoCod);

            if(strCodigo == ui->txtCodigo->text()){
                    existe = true;
                    strCodigoViejoCod = strCodigoCod;
                    sql.finish();
                }
            sql.next();
        }

        //
        // Miramos si es correctivo o preventivo
        //
        if(ui->cmbTipoMantenimiento->currentIndex() == 1){
            strCorrectivo = "Si";
            strPreventivo = "No";
        }
        else{
            strCorrectivo = "No";
            strPreventivo = "Si";
        }

        //
        // Obtenemos los valores codificados
        //
        strNombreCod        = pFuncAux->funCifrar_bis(ui->txtNombre->text());
        strDescripcionCod   = pFuncAux->funCifrar_bis(ui->txtDescripcion->text());
        strPeriodicidadCod  = pFuncAux->funCifrar_bis(ui->cmbPeriocidad->currentText());
        strLimiteHorasCod   = pFuncAux->funCifrar_bis(ui->txtLimiteHoras->text());
        strCorrectivoCod    = pFuncAux->funCifrar_bis(strCorrectivo);
        strPreventivoCod    = pFuncAux->funCifrar_bis(strPreventivo);
        strPrecioCod        = pFuncAux->funCifrar_bis(ui->txtPrecio->text());
        strCodigoCod        = pFuncAux->funCifrar_bis(ui->txtCodigo->text());


        //
        // Si existe editamos sino añadimos
        //
        if(existe){

            strSql= "UPDATE Mantenimiento SET Trabajo ='"       + strNombreCod + "', "
                                             "Descripcion ='"   + strDescripcionCod + "', "
                                             "Periodico ='"     + strPeriodicidadCod + "', "
                                             "MaxHoras ='"      + strLimiteHorasCod + "', "
                                             "Correctivo ='"    + strCorrectivoCod + "', "
                                             "Preventivo ='"    + strPreventivoCod + "', "
                                             "Precio ='"        + strPrecioCod + "', "
                                             "Codigo ='"        + strCodigoCod + "'  WHERE  Codigo ='" + strCodigoViejoCod + "';";
        }
        else{
            strSql = " INSERT INTO Mantenimiento(Trabajo,"
                     "                           Descripcion,"
                     "                           Periodico,"
                     "                           MaxHoras,"
                     "                           Correctivo,"
                     "                           Preventivo,"
                     "                           Precio,"
                     "                           Codigo) VALUES ('" + strNombreCod + "',"
                     "                                           '" + strDescripcionCod + "',"
                     "                                           '" + strPeriodicidadCod + "',"
                     "                                           '" + strLimiteHorasCod + "',"
                     "                                           '" + strCorrectivoCod + "',"
                     "                                           '" + strPreventivoCod + "',"
                     "                                           '" + strPrecioCod + "',"
                     "                                           '" + strCodigoCod + "');";
        }

        sql.exec(strSql);

        //
        // Cierro la BD
        //
        dbSql.close();
        dbSql.removeDatabase("conSql");

        //
        // Limpio los controles
        //
        funLimpiaControles();
        funRellenaListado();

        //
        // Activo el boton de +
        //
        ui->tbAgregar->setEnabled(true);
        ui->tbAgregar->setToolTip("Agregar una nueva OP. de Mantenimiento...");

        ui->cmdGuardar->setEnabled(false);
        ui->cmdGuardar->setToolTip("");

        //
        // Desactivo los controles
        //
        funDesHabilitaControles();

    }
    //
    // Si el campo del nombre esta vacio avisamos y lo situamos ahi
    //
    else{
        QMessageBox::information(this, GstrTitulo, "\t El Campo del nombre es obligatorio...");

        ui->txtNombre->setStyleSheet("color: black; background-color: yellow");
        ui->txtNombre->setFocus();
    }

    delete pFuncAux;

}

void frmRegMantenimiento::on_tbAgregar_clicked(){
    QString strRutaDb, strSql, strCodigo;
    int     codigo = 1;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Primero habilito los controles
    //
    ui->cmdGuardar->setEnabled(true);
    ui->cmdGuardar->setToolTip("Guardar los datos de esta Op. de Mantenimiento...");

    funLimpiaControles();
    funHabilitaControles();

    ui->tlMantenimiento->clearSelection();

    //
    // Deshabilito el boton de + y el de -
    //
    ui->tbAgregar->setEnabled(false);
    ui->tbAgregar->setToolTip("");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    //
    // Obtengo el codigo de empleado
    //
    strCodigo = pFuncAux->funNuevoCodigo("Mantenimiento");

    ui->txtCodigo->setText(strCodigo);

    //
    // Coloco los cmb en Puntual y correctivo
    //
    ui->cmbPeriocidad->setCurrentIndex(1);
    ui->cmbTipoMantenimiento->setCurrentIndex(1);

    ui->txtNombre->setFocus();

    delete pFuncAux;


}

void frmRegMantenimiento::on_tbEliminar_clicked(){
    QString strRutaDb, strSql;
    QString strCodigo, strCodigoCod, strNombreCod;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Abro la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    //
    // Recorro toda la base de Datos hasta encontrar el codigo
    //
    strSql = "SELECT *FROM Mantenimiento";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodigoCod    = sql.record().value("Codigo").toString();
        strCodigo       = pFuncAux->funDesCifrar_bis(strCodigoCod);

        if(strCodigo == ui->txtCodigo->text()){
            strNombreCod = sql.record().value("Trabajo").toString();
            sql.finish();
        }
        sql.next();
    }

    strSql = "DELETE FROM Mantenimiento WHERE Codigo='" + strCodigoCod + "'AND Trabajo='" + strNombreCod + "';";
    sql.exec(strSql);

    funLimpiaControles();
    funRellenaListado();

    //
    // desactivo boton -
    //
    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    //
    // Cierro la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    delete pFuncAux;

}

void frmRegMantenimiento::on_tlMantenimiento_itemClicked(QTableWidgetItem *item){
    QString strRutaDb, strSql, strCorrectivo, strPreventivo;
    QString strNombre, strDescripcion, strPeriodicidad, strLimiteHoras;
    QString strTipo, strPrecio, strCodigo;
    int fila;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Abro la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Mantenimiento";
    sql.exec(strSql);
    sql.first();
    fila = 0;
    while (sql.isValid()) {
        if(fila == item->row()){

            //
            // Desciframos los campos
            //
            strNombre           = pFuncAux->funDesCifrar_bis(sql.record().value("Trabajo").toString());
            strDescripcion      = pFuncAux->funDesCifrar_bis(sql.record().value("Descripcion").toString());
            strPeriodicidad     = pFuncAux->funDesCifrar_bis(sql.record().value("Periodico").toString());
            strLimiteHoras      = pFuncAux->funDesCifrar_bis(sql.record().value("MaxHoras").toString());
            strTipo             = pFuncAux->funDesCifrar_bis(sql.record().value("Correctivo").toString());
            strPrecio           = pFuncAux->funDesCifrar_bis(sql.record().value("Precio").toString());
            strCodigo           = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());

            //
            // Rellenamos los txt
            //
            ui->txtNombre->setText(strNombre);
            ui->txtDescripcion->setText(strDescripcion);
            ui->cmbPeriocidad->setCurrentText(strPeriodicidad);
            ui->txtLimiteHoras->setText(strLimiteHoras);
            if(strTipo == "Si"){
                ui->cmbTipoMantenimiento->setCurrentIndex(1);
            }
            else{
                ui->cmbTipoMantenimiento->setCurrentIndex(0);
            }
            ui->txtPrecio->setText(strPrecio);
            ui->txtCodigo->setText(strCodigo);

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

    //
    // Activo el boton de suprimir
    //
    if(ui->tlMantenimiento->item(item->row(), 7)->text() != ""){
        ui->tbEliminar->setEnabled(true);
        ui->tbEliminar->setToolTip("Eliminar los datos de esta OP. de Mantenimiento...");
    }
    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Agregar una nueva OP. de Mantenimiento...");

    //
    // Deshabilito controles
    //
    funDesHabilitaControles();

    delete pFuncAux;

}

void frmRegMantenimiento::on_tlMantenimiento_itemDoubleClicked(QTableWidgetItem *item){

    //
    // Activo los controles
    //
    funHabilitaControles();

    //
    // Activo el boton Guardar y desactivo los botones +`y -
    //
    ui->cmdGuardar->setEnabled(true);
    ui->cmdGuardar->setToolTip("Guardar los datos de esta Op. de Mantenimiento...");

    ui->tbAgregar->setEnabled(false);
    ui->tbAgregar->setToolTip("");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    //
    // situo el foco en txtNombre
    //
    ui->txtNombre->setFocus();
    ui->txtNombre->selectAll();

}

void frmRegMantenimiento::on_txtNombre_returnPressed(){

    ui->txtDescripcion->setFocus();
    ui->txtDescripcion->selectAll();
}

void frmRegMantenimiento::on_txtDescripcion_returnPressed(){

    //
    // si txtHoras está activado coge el foco sino el txtprecio
    //
    if(ui->txtLimiteHoras->isEnabled()){
        ui->txtLimiteHoras->setFocus();
    }
    else{
        ui->txtPrecio->setFocus();
        ui->txtPrecio->selectAll();
    }
}

void frmRegMantenimiento::on_txtLimiteHoras_returnPressed(){

    ui->txtPrecio->setFocus();
    ui->txtPrecio->selectAll();
}

void frmRegMantenimiento::on_txtPrecio_returnPressed(){

    on_cmdGuardar_clicked();
}

void frmRegMantenimiento::on_cmbPeriocidad_currentTextChanged(const QString &arg1){

    //
    // Si el nuevo texto es periodico, txt horas enabled y tipo mantenimiento preventivo
    //
    if(ui->cmbPeriocidad->currentIndex() == 0){
        ui->txtLimiteHoras->setText("");
        ui->txtLimiteHoras->setEnabled(true);
        ui->cmbTipoMantenimiento->setCurrentIndex(0);
    }
    else{
        ui->txtLimiteHoras->setEnabled(false);
        ui->cmbTipoMantenimiento->setCurrentIndex(1);
    }

}

void frmRegMantenimiento::on_cmbTipoMantenimiento_currentTextChanged(const QString &arg1){
    //
    // Si es correctivo, txthoras disabled, cmbPeriodico puntual

    if(ui->cmbTipoMantenimiento->currentIndex() == 0){
        ui->txtLimiteHoras->setEnabled(true);
        ui->cmbPeriocidad->setCurrentIndex(0);
        ui->txtNombre->setFocus();

    }
    else{
        ui->txtLimiteHoras->setText("");
        ui->txtLimiteHoras->setEnabled(false);
        ui->cmbPeriocidad->setCurrentIndex(1);
        ui->txtNombre->setFocus();

    }

}

void frmRegMantenimiento::on_txtLimiteHoras_textChanged(const QString &arg1){
    QString str = "";
    long  numero = 0;

    //
    // Tenemos que comprobar que sea numerico sino lo borramos
    //
    str     = arg1;
    numero  = str.toLong();
    if(numero == 0){
        str.remove(str.length() - 1, 1);
        ui->txtLimiteHoras->setText(str);
    }

}

void frmRegMantenimiento::on_txtPrecio_textChanged(const QString &arg1){
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

