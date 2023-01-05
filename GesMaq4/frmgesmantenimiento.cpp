#include "frmgesmantenimiento.h"
#include "ui_frmgesmantenimiento.h"

#include "funcaux.h"
#include "frmregmantenimiento.h"

#include <QDate>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QMessageBox>
#include <QImageReader>

frmGesMantenimiento::frmGesMantenimiento(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmGesMantenimiento){
    ui->setupUi(this);

    this->installEventFilter(this);

}

frmGesMantenimiento::~frmGesMantenimiento(){
    delete ui;
}

bool frmGesMantenimiento::eventFilter(QObject *obj, QEvent *event){

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

void frmGesMantenimiento::funInicio(){
    QString strHoras, strFecha, str;
    QLocale qlTraductor;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Borramos todo el tablet
    //
    while (ui->tlMantenimiento->rowCount()>0) {
        ui->tlMantenimiento->removeRow(0);
    }

    //
    // Borramos lo txt
    //
    ui->txtHorasActuales->setText("");
    ui->txtMaxHoras->setText("");

    //
    // Colocamos las columnas del tablelist
    //
    ui->tlMantenimiento->setColumnCount(6);

    QStringList headers;
    headers += tr("Trabajo");
    headers += tr("Maquina");
    headers += tr("Horas Actuales");
    headers += tr("Max. Horas");
    headers += tr("Estado del Trabajo");
    headers += tr("Código");

    ui->tlMantenimiento->setHorizontalHeaderLabels(headers);

    ui->tlMantenimiento->setColumnWidth(0, (int) ( ( this->width() * 2.3 ) / 6 ) );
    ui->tlMantenimiento->setColumnWidth(1, (int) ( ( this->width() * 1) / 6 ) );
    ui->tlMantenimiento->setColumnWidth(2, (int) ( ( this->width() * 0.5 ) / 6 ) );
    ui->tlMantenimiento->setColumnWidth(3, (int) ( ( this->width() * 0.4 ) / 6 ) );
    ui->tlMantenimiento->setColumnWidth(4, (int) ( ( this->width() * 1.2 ) / 6 ) );
    ui->tlMantenimiento->setColumnWidth(5, (int) ( ( this->width() * 0.4 ) / 6 ) );

    funRellenaCombos();
    funRellenaListado();


    //
    // Colocamos el tooltip de volver, de añadir y el de tbNuevaTarea
    //
    ui->cmdVolver->setToolTip("Volver a la pantalla Principal...");
    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Añadir un Nuevo Mantenimiento Programado...");

    //
    //deshabilitamos los controles
    //
    funDeshabilitaControles();

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    ui->cmdGuardar->setEnabled(false);
    ui->cmdGuardar->setToolTip("");

    ui->cmdRealizado->setEnabled(false);
    ui->cmdRealizado->setToolTip("");

    ui->txtHorasActuales->setEnabled(false);
    ui->txtCodigo->setEnabled(false);

    ui->tlMantenimiento->clearSelection();


    delete pFuncAux;

}

void frmGesMantenimiento::funHabilitaControles(){

    ui->cmbTarea->setEnabled(true);
    ui->cmbMaquina->setEnabled(true);
    ui->txtMaxHoras->setEnabled(true);

    ui->tbTarea->setEnabled(true);
    ui->tbTarea->setToolTip("Definir una Nueva tarea de Mantenimiento...");

    ui->txtHorasActuales->setStyleSheet("color: black; background-color: white");
    ui->txtMaxHoras->setStyleSheet("color: black; background-color: white");

}

void frmGesMantenimiento::funDeshabilitaControles(){

    ui->cmbTarea->setEnabled(false);
    ui->cmbMaquina->setEnabled(false);
    ui->txtMaxHoras->setEnabled(false);

    ui->tbTarea->setEnabled(false);
    ui->tbTarea->setToolTip("");

    ui->txtHorasActuales->setStyleSheet("color: gray; background-color: white");
    ui->txtMaxHoras->setStyleSheet("color: gray; background-color: white");

}

void frmGesMantenimiento::funRellenaListado(){
    QString strRutaDb, strSql;
    QString strTrabajo, strHoras, strMaquina, strCodMaquina, strMaxHoras, strEstado, strCodigo;
    int fila;

    FuncAux* pFuncAux = new FuncAux();

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
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql31");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM MantenimientoProgramado";
    sql.exec(strSql);
    sql.first();
    fila=0;

    while (sql.isValid()) {

        strTrabajo      = pFuncAux->funDesCifrar_bis(sql.record().value("Trabajo").toString());
        strCodMaquina   = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());
        strMaquina      = pFuncAux->funTxtToMaquinas(strCodMaquina);
        strHoras        = pFuncAux->funHorasToMaquina(pFuncAux->funCodigoToMaquina(strMaquina));
        strMaxHoras     = pFuncAux->funDesCifrar_bis(sql.record().value("MaxHoras").toString());
        strEstado       = pFuncAux->funDesCifrar_bis(sql.record().value("Estado").toString());
        strCodigo       = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());

        ui->tlMantenimiento->setRowCount(fila + 1);

        QTableWidgetItem *item_trabajo          = new QTableWidgetItem(strTrabajo);
        QTableWidgetItem *item_maquina          = new QTableWidgetItem(strMaquina);
        QTableWidgetItem *item_horas_actuales   = new QTableWidgetItem(strHoras);
        QTableWidgetItem *item_max_horas        = new QTableWidgetItem(strMaxHoras);
        QTableWidgetItem *item_estado           = new QTableWidgetItem(strEstado);
        QTableWidgetItem *item_Codigo           = new QTableWidgetItem(strCodigo);

        item_trabajo->setTextAlignment(Qt::AlignCenter);
        item_maquina->setTextAlignment(Qt::AlignCenter);
        item_horas_actuales->setTextAlignment(Qt::AlignCenter);
        item_max_horas->setTextAlignment(Qt::AlignCenter);
        item_estado->setTextAlignment(Qt::AlignCenter);
        item_Codigo->setTextAlignment(Qt::AlignCenter);

        ui->tlMantenimiento->setItem(fila, 0, item_trabajo);
        ui->tlMantenimiento->setItem(fila, 1, item_maquina);
        ui->tlMantenimiento->setItem(fila, 2, item_horas_actuales);
        ui->tlMantenimiento->setItem(fila, 3, item_max_horas);
        ui->tlMantenimiento->setItem(fila, 4, item_estado);
        ui->tlMantenimiento->setItem(fila, 5, item_Codigo);

        fila++;
        sql.next();
    }

    //
    // Ahora Ordenamos la Tabla
    //
    ui->tlMantenimiento->sortItems(4, Qt::DescendingOrder);

    dbSql.close();
    dbSql.removeDatabase("conSql31");

    delete pFuncAux;


}

void frmGesMantenimiento::funRellenaCombos(){
    QString strRutaDb, strSql;
    QString strTarea, strCodMaquina;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Vaciamos los combos
    //
    while (ui->cmbTarea->count()>0) {
        ui->cmbTarea->removeItem(0);
    }
    while (ui->cmbMaquina->count()>0) {
        ui->cmbMaquina->removeItem(0);
    }

    //
    // Colocamos los dos textos del titulo
    //
    ui->cmbTarea->addItem("Escoje una Tarea de Mantenimiento...");
    ui->cmbMaquina->addItem("Escoje una Máquina...");

    //
    // Ahora miramos los registros de la base de datos, para rellenar los combos
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    //
    // Rellenamos el combo de Tareas
    //
    strSql = "SELECT *FROM Mantenimiento";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strTarea = pFuncAux->funDesCifrar_bis(sql.record().value("Descripcion").toString());

        ui->cmbTarea->addItem(strTarea);
        sql.next();
    }

    //
    // Rellenamos el Combo de las Maquinas
    //
    strSql = "SELECT *FROM Maquinas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodMaquina = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());

        ui->cmbMaquina->addItem(pFuncAux->funTxtToMaquinas(strCodMaquina));
        sql.next();
    }

    //
    // Colocamos la foto de la primera Maquina
    //
    strCodMaquina = pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText());
    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, pFuncAux->funRutaFotoMaquinaToCodigo(strCodMaquina));

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    delete pFuncAux;

}

QString frmGesMantenimiento::funHorasMaxToTarea(QString strTarea){
    QString strRutaDb, strSql;
    QString strHoras = "", strDescripcion;

    FuncAux *pFuncAux = new FuncAux();

    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Mantenimiento";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strDescripcion = pFuncAux->funDesCifrar_bis(sql.record().value("Descripcion").toString());

        if(strDescripcion == strTarea){
            strHoras = pFuncAux->funDesCifrar_bis(sql.record().value("MaxHoras").toString());
            sql.finish();
        }
        sql.next();
    }

    dbSql.close();
    dbSql.removeDatabase("conSql");

    delete pFuncAux;

    return strHoras;
}

QString frmGesMantenimiento::funHorasTarea(QString strTarea){
    QString strRutaDb, strSql;
    QString strHoras = "", strDescripcion;

    FuncAux *pFuncAux = new FuncAux();

    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Mantenimiento";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strDescripcion = pFuncAux->funDesCifrar_bis(sql.record().value("Descripcion").toString());

        if(strDescripcion == strTarea){
            strHoras = pFuncAux->funDesCifrar_bis(sql.record().value("MaxHoras").toString());
            sql.finish();
        }
        sql.next();
    }

    dbSql.close();
    dbSql.removeDatabase("conSql");

    delete pFuncAux;

    return strHoras;
}

void frmGesMantenimiento::on_cmdVolver_clicked(){

    this->close();

}

void frmGesMantenimiento::on_tbAgregar_clicked(){
    QString strCodigo;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Limpio y Habilito los controles
    //
    ui->txtHorasActuales->setText("");
    ui->txtMaxHoras->setText("");
    ui->cmbMaquina->setCurrentIndex(0);
    ui->cmbTarea->setCurrentIndex(0);
    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, "");
    funHabilitaControles();

    //
    // Deseleccionamos la tabla
    //
    ui->tlMantenimiento->clearSelection();

    //
    // Deshabilito el boton de + y el de -
    //
    ui->tbAgregar->setEnabled(false);
    ui->tbAgregar->setToolTip("");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    //
    // Deshabilito el cmdRealizado
    //
    ui->cmdRealizado->setEnabled(false);
    ui->cmdRealizado->setToolTip("");

    //
    // Habilitamos el cmdGuardar
    //
    ui->cmdGuardar->setEnabled(true);
    ui->cmdGuardar->setToolTip("Guardar esta tarea de mantenimiento...");

    //
    // Obtengo el codigo de Maquina
    //
    strCodigo = pFuncAux->funNuevoCodigo("MantenimientoProgramado");
    ui->txtCodigo->setText(strCodigo);

    ui->txtMaxHoras->setFocus();

    delete pFuncAux;

}

void frmGesMantenimiento::on_cmbMaquina_activated(int index){
    QString strCodMaquina;
    double  dHoras, dMaxHoras;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Colocamos la Foto
    //
    if(ui->cmbMaquina->currentIndex() >0){
        strCodMaquina = pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText());
        pFuncAux->funMostrarFoto(ui->lblFotoMaquina, pFuncAux->funRutaFotoMaquinaToCodigo(strCodMaquina));
    }


    //
    // Rellenamos los txtHoras Solo si los dos combos CurrentIndex > 0
    //
    if(ui->cmbMaquina->currentIndex() > 0 && ui->cmbTarea->currentIndex() > 0){

        ui->txtHorasActuales->setText(pFuncAux->funHorasToMaquina(pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText())));

        dHoras      = ui->txtHorasActuales->text().toDouble();
        dMaxHoras   = dHoras + funHorasTarea(ui->cmbTarea->currentText()).toDouble();
        ui->txtMaxHoras->setText(QString::number(dMaxHoras));

    }
    //
    // Uno de los dos combos tiene currentIndex == 0
    //
    else{

        ui->txtHorasActuales->setText(pFuncAux->funHorasToMaquina(pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText())));
        ui->txtMaxHoras->setText("");
    }

    //
    // Deseleccionamos la tabla
    //
    ui->tlMantenimiento->clearSelection();

    ui->txtMaxHoras->setFocus();
    ui->txtMaxHoras->selectAll();

    delete pFuncAux;
}

void frmGesMantenimiento::on_cmbTarea_activated(int index){
    double dHoras, dMaxHoras;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Rellenamos los txtHoras
    //
    ui->txtHorasActuales->setText(pFuncAux->funHorasToMaquina(pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText())));

    if(ui->txtHorasActuales->text() != ""){
        dHoras      = ui->txtHorasActuales->text().toDouble();
        dMaxHoras   = dHoras + funHorasTarea(ui->cmbTarea->currentText()).toDouble();
        ui->txtMaxHoras->setText(QString::number(dMaxHoras));
    }

    //
    // Deseleccionamos la tabla y desactivamos el boton realizado
    //
    ui->tlMantenimiento->clearSelection();

    ui->txtMaxHoras->setFocus();
    ui->txtMaxHoras->selectAll();

    delete pFuncAux;


}

void frmGesMantenimiento::on_tbTarea_clicked(){

    frmRegMantenimiento *pMantenimiento = new frmRegMantenimiento(this);

    pMantenimiento->setWindowTitle(GstrTitulo);
    pMantenimiento->setWindowFlag(Qt::CustomizeWindowHint);
    pMantenimiento->setWindowModality(Qt::WindowModal);

    pMantenimiento->GstrTitulo = GstrTitulo;
    pMantenimiento->funInicio();

    pMantenimiento->exec();

    delete pMantenimiento;

    funInicio();
    on_tbAgregar_clicked();

}

void frmGesMantenimiento::on_cmdGuardar_clicked(){
    QString strRutaDb, strSql, str;
    QString strCodigo, strCodigoCod;
    QString strCodMaquina, strEstado;
    QString strTrabajoCod, strMaquinaCod, strMaxHorasCod, strEstadoCod;
    QLocale qlTraductor;
    double  dHoras, dMaxHoras;
    bool    bExiste = false;

    FuncAux *pFuncAux = new FuncAux();


    //
    // Si el ui->cmbMaquina.currenIndex == 0
    //
    if(ui->cmbMaquina->currentIndex() == 0){
        QMessageBox::information(this, GstrTitulo, "Debes seleccionar una máquina...");
    }
    //
    // Si el cmbTarea.currentIndex == 0
    //
    else if(ui->cmbTarea->currentIndex() == 0){
        QMessageBox::information(this, GstrTitulo, "Debes seleccionar una Tarea...");
    }
    //
    // Si ui.txtHorasMax.text == ""
    //
    else if(ui->txtMaxHoras->text() == ""){
        QMessageBox::information(this, GstrTitulo, "Debes indicar un límite de horas, para realizar el mantenimiento...");
   }
    //
    // Cualquier otro caso Guardamos
    //
    else{

        //
        // Primero comprobamos que el maxHOras sea mayor que horas actuales
        //
        dHoras      = ui->txtHorasActuales->text().toDouble();
        dMaxHoras   = ui->txtMaxHoras->text().toDouble();

        //
        // Si MaxHoras es menor que HorasActuales Avisamos
        //
        if(dMaxHoras < dHoras){
            QMessageBox::information(this, GstrTitulo, "El Límite Máximo de Horas, \nNo puede ser menor que las Horas Actuales...");
        }
        //
        // Todo Correcto Guardamos
        //
        else{

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
            strSql = "SELECT *FROM MantenimientoProgramado";
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
                strSql = "DELETE FROM MantenimientoProgramado WHERE Codigo ='" + strCodigoCod + "' ;";
                sql.exec(strSql);

            }


            //
            // Preparo los valores
            //
            strCodMaquina   = pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText());

            //
            // Compruebo el estado
            //
            if(ui->txtHorasActuales->text().toDouble() == ui->txtMaxHoras->text().toDouble()){
                strEstado = "Trabajo Realizado el ";
                strEstado.append(qlTraductor.toString(QDate::currentDate(),"dd/MM/yyyy"));
            }
            else{
                strEstado = "Pendiente de Realizar";
            }

            //
            // Codifico los valores
            //
            strTrabajoCod   = pFuncAux->funCifrar_bis(ui->cmbTarea->currentText());
            strMaquinaCod   = pFuncAux->funCifrar_bis(strCodMaquina);
            strMaxHorasCod  = pFuncAux->funCifrar_bis(ui->txtMaxHoras->text());
            strEstadoCod    = pFuncAux->funCifrar_bis(strEstado);
            strCodigoCod    = pFuncAux->funCifrar_bis(ui->txtCodigo->text());


            strSql = " INSERT INTO MantenimientoProgramado(Trabajo,"
                     "                                     Maquina,"
                     "                                     MaxHoras,"
                     "                                     Estado,"
                     "                                     Codigo) VALUES ('" + strTrabajoCod + "',"
                     "                                                     '" + strMaquinaCod + "',"
                     "                                                     '" + strMaxHorasCod + "',"
                     "                                                     '" + strEstadoCod + "',"
                     "                                                     '" + strCodigoCod + "');";
            sql.exec(strSql);

            //
            // Cierro la BD
            //
            dbSql.close();
            dbSql.removeDatabase("conSql");

            funInicio();

        }
    }

    delete pFuncAux;

}

void frmGesMantenimiento::on_tlMantenimiento_itemClicked(QTableWidgetItem *item){
    QString strRuta;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Deshabilitamos los controles
    //
    funDeshabilitaControles();

    //
    // Obtenemos la fila seleccionada
    //
    Gfila_seleccionada = item->row();

    //
    // Activamos los controles
    //
    ui->tbEliminar->setEnabled(true);
    ui->tbEliminar->setToolTip("Eliminar un Mantenimiento Programado...");

    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Añadir un Nuevo Mantenimiento Programado...");

    //
    // Desactivamos el cmdGuardar
    //
    ui->cmdGuardar->setEnabled(false);
    ui->cmdGuardar->setToolTip("");

    if(ui->tlMantenimiento->item(Gfila_seleccionada, 4)->text() == "Pendiente de Realizar"){
        ui->cmdRealizado->setEnabled(true);
        ui->cmdRealizado->setToolTip("Cambiar el estado de esta tarea a Realizado...");
    }
    else{
        ui->cmdRealizado->setEnabled(false);
        ui->cmdRealizado->setToolTip("");
    }

    //
    // Rellenamos los combos
    //
    ui->cmbTarea->setCurrentText(ui->tlMantenimiento->item(Gfila_seleccionada, 0)->text());
    ui->cmbMaquina->setCurrentText(ui->tlMantenimiento->item(Gfila_seleccionada, 1)->text());
    ui->txtHorasActuales->setText(ui->tlMantenimiento->item(Gfila_seleccionada, 2)->text());
    ui->txtMaxHoras->setText(ui->tlMantenimiento->item(Gfila_seleccionada, 3)->text());
    ui->txtCodigo->setText(ui->tlMantenimiento->item(Gfila_seleccionada, 5)->text());

    //
    // Colocamos la foto de la maquina
    //
    strRuta = pFuncAux->funRutaFotoMaquinaToCodigo(pFuncAux->funCodigoToMaquina(ui->tlMantenimiento->item(Gfila_seleccionada, 1)->text()));
    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, strRuta);


    delete pFuncAux;
}

void frmGesMantenimiento::on_tbEliminar_clicked(){
    QString strRutaDb, strSql;
    QString strTrabajoDbCod, strMaquinaDbCod, strMaxHorasDbCod, strEstadoDbCod;
    QString strCodMaquinaDb, strMaquinaDb, strTrabajoDb, strMaxHorasDb, strEstadoDb;

    FuncAux* pFuncAux = new FuncAux();

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
        strSql = "SELECT *FROM MantenimientoProgramado";
        sql.exec(strSql);
        sql.first();
        while (sql.isValid()) {

            strTrabajoDbCod     = sql.record().value("Trabajo").toString();
            strMaquinaDbCod     = sql.record().value("Maquina").toString();
            strMaxHorasDbCod    = sql.record().value("MaxHoras").toString();
            strEstadoDbCod      = sql.record().value("Estado").toString();

            strTrabajoDb    = pFuncAux->funDesCifrar_bis(strTrabajoDbCod);
            strCodMaquinaDb = pFuncAux->funDesCifrar_bis(strMaquinaDbCod);
            strMaxHorasDb   = pFuncAux->funDesCifrar_bis(strMaxHorasDbCod);
            strEstadoDb     = pFuncAux->funDesCifrar_bis(strEstadoDbCod);
            strMaquinaDb    = pFuncAux->funTxtToMaquinas(strCodMaquinaDb);

            if(strTrabajoDb == ui->tlMantenimiento->item(Gfila_seleccionada, 0)->text() &&
                    strMaquinaDb == ui->tlMantenimiento->item(Gfila_seleccionada, 1)->text() &&
                    strMaxHorasDb == ui->tlMantenimiento->item(Gfila_seleccionada, 3)->text() &&
                    strEstadoDb == ui->tlMantenimiento->item(Gfila_seleccionada, 4)->text()){

                sql.finish();
            }

            sql.next();
        }

        //
        // Borramos el registro
        //
        strSql = "DELETE FROM MantenimientoProgramado WHERE Trabajo ='" + strTrabajoDbCod + "' AND Maquina ='" + strMaquinaDbCod + "' "
                                                                                              "AND MaxHoras ='" + strMaxHorasDbCod + "' "
                                                                                              "AND Estado ='" + strEstadoDbCod + "';";
        sql.exec(strSql);


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
        // Colocamos los txt y cmb como al principio
        //
        ui->cmbTarea->setCurrentIndex(0);
        ui->cmbMaquina->setCurrentIndex(0);
        ui->txtHorasActuales->setText("");
        ui->txtMaxHoras->setText("");
        pFuncAux->funMostrarFoto(ui->lblFotoMaquina, "");

        //
        // Deseleccionamos la tabla y desactivamos el boton realizado
        //
        ui->tlMantenimiento->clearSelection();

        funRellenaListado();

        ui->cmdRealizado->setEnabled(false);
        ui->cmdRealizado->setToolTip("");

        Gfila_seleccionada = -1;

        ui->txtMaxHoras->setFocus();
        ui->txtMaxHoras->selectAll();
    }

    delete pFuncAux;

}

void frmGesMantenimiento::on_tlMantenimiento_itemDoubleClicked(QTableWidgetItem *item){
    QString strEstado;
    int     iFila;

    iFila       = item->row();
    strEstado   = ui->tlMantenimiento->item(iFila, 4)->text();

    //
    // Desactivamos el boton Realizado
    //
    ui->cmdRealizado->setEnabled(false);
    ui->cmdRealizado->setToolTip("");

    //
    // Activamos los controles
    //
    funHabilitaControles();

    //
    // Deseleccionamos la tabla
    //
    ui->tlMantenimiento->clearSelection();

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

    ui->txtMaxHoras->setFocus();
    ui->txtMaxHoras->selectAll();



}

void frmGesMantenimiento::on_cmdRealizado_clicked(){
    QString strSql, strRutaDb;
    QString strCodigoDb, strCodigoDbCod, strEstadoCod;
    QString strFecha, str;
    QLocale qlTraductor;
    bool    bEncontrado;

    FuncAux *pFuncAux = new FuncAux();

    strFecha = qlTraductor.toString(QDate::currentDate(),"dd/MM/yyyy");

    str = "Trabajo Realizado el ";
    str.append(strFecha);
    //
    // Establecemos el trabajo como realizado en el dia de hoy
    //
    ui->tlMantenimiento->item(Gfila_seleccionada, 4)->setText(str);
    ui->tlMantenimiento->item(Gfila_seleccionada, 3)->setText(ui->tlMantenimiento->item(Gfila_seleccionada, 2)->text());

    //
    // Abrimos la BD y Editamos el Registro en la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strEstadoCod = pFuncAux->funCifrar_bis(ui->tlMantenimiento->item(Gfila_seleccionada, 4)->text());

    strSql = "SELECT *FROM MantenimientoProgramado";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodigoDbCod  = sql.record().value("Codigo").toString();
        strCodigoDb     = pFuncAux->funDesCifrar_bis(strCodigoDbCod);

        if(strCodigoDb == ui->tlMantenimiento->item(Gfila_seleccionada, 5)->text()){
            bEncontrado = true;
            sql.finish();
        }

        sql.next();
    }

    if(bEncontrado){
        strSql= "UPDATE MantenimientoProgramado SET Estado ='" + strEstadoCod + "'  WHERE  Codigo ='" + strCodigoDbCod + "';";
        sql.exec(strSql);
    }


    //
    // Cierro la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");


    //
    // Colocamos los combos y txt en cero
    //
    ui->txtHorasActuales->setText("");
    ui->txtMaxHoras->setText("");
    ui->txtCodigo->setText("");
    ui->cmbTarea->setCurrentIndex(0);
    ui->cmbMaquina->setCurrentIndex(0);

    funDeshabilitaControles();

    //
    // Borramos la foto de la maquina
    //
    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, "");

    //
    // DEsactivamos el borton Realizado y el eliminar
    //
    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    ui->cmdRealizado->setEnabled(false);
    ui->cmdRealizado->setToolTip("");

    //
    // Borramos el valor de fila_seleccionada
    //
    Gfila_seleccionada = -1;

    ui->tlMantenimiento->clearSelection();
    ui->tlMantenimiento->sortItems(4, Qt::DescendingOrder);


    delete pFuncAux;

}

void frmGesMantenimiento::on_txtMaxHoras_textChanged(const QString &arg1){
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
                ui->txtMaxHoras->setText(str);
            }
        }

    }

    //
    // Si su longitud es dos solo permito str == 0.
    if(str.length() == 2){
        if(str != "0."){
            if(numero == 0){
                str.remove(str.length() - 1, 1);
                ui->txtMaxHoras->setText(str);
            }
        }
    }

    //
    // S i su longitud es mayor de 2 solo si es numero
    //
    if(str.length() > 2 && numero == 0){
        str.remove(str.length() - 1, 1);
        ui->txtMaxHoras->setText(str);
    }


}

void frmGesMantenimiento::on_txtMaxHoras_returnPressed(){

    on_cmdGuardar_clicked();
}

