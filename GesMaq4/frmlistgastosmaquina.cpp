#include "frmlistgastosmaquina.h"
#include "ui_frmlistgastosmaquina.h"

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



frmListGastosMaquina::frmListGastosMaquina(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmListGastosMaquina){

    ui->setupUi(this);

    this->installEventFilter(this);

}

frmListGastosMaquina::~frmListGastosMaquina(){
    delete ui;
}

bool frmListGastosMaquina::eventFilter(QObject *obj, QEvent *event){

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

void frmListGastosMaquina::funInicio(){
    QString strRutaDb, strSql, str;
    QString strCodMaquina;
    QDate   datFechaAnt, datFechaPost;
    QLocale qlTraductor;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Colocamos las columnas del tablelist
    //
    ui->tlGastos->setColumnCount(5);

    QStringList headers;
    headers += tr("Fecha");
    headers += tr("Descripción");
    headers += tr("Cantidad");
    headers += tr("Precio Unidad");
    headers += tr("Total.");


    ui->tlGastos->setHorizontalHeaderLabels(headers);

    ui->tlGastos->setColumnWidth(0, (int) ( ( this->width() * 1.4) / 5 ) );
    ui->tlGastos->setColumnWidth(1, (int) ( ( this->width() * 1.70) / 5 ) );
    ui->tlGastos->setColumnWidth(2, (int) ( ( this->width() * 0.6) / 5 ) );
    ui->tlGastos->setColumnWidth(3, (int) ( ( this->width() * 0.6) / 5 ) );
    ui->tlGastos->setColumnWidth(4, (int) ( ( this->width() * 0.6) / 5 ) );

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql16");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    //
    //Rellenamos el cmbMaquinas
    //
    while(ui->cmbMaquina->count()>0){
        ui->cmbMaquina->removeItem(0);
    }
    strSql = "SELECT *FROM Maquinas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodMaquina   = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());
        str             = pFuncAux->funTxtToMaquinas(strCodMaquina);

        ui->cmbMaquina->addItem(str);
        sql.next();
    }

    //
    // Mostramos la foto
    //
    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, pFuncAux->funRutaFotoMaquinaToCodigo(pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText())));

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql16");

    delete pFuncAux;

    //
    // Colocamos la fecha actual y el 1º de mes
    //
    datFechaPost = QDate::currentDate();
    ui->txtFecha1->setText(qlTraductor.toString(datFechaPost,"dd/MM/yyyy"));

    datFechaAnt = QDate::currentDate();
    datFechaAnt.setDate(datFechaPost.year(), datFechaPost.month(), 01);
    ui->txtFecha0->setText(qlTraductor.toString(datFechaAnt, "dd/MM/yyyy"));

    funRellenaListado();

    ui->cmdVolver->setToolTip("Volver al la pantalla Principal...");

    ui->lblResumen->setFocus();


}

void frmListGastosMaquina::funRellenaListado(){
    QString strRutaDb, strSql, str;
    QString strCodMaquina, strCodEmpresaDb, strCombustibleDb, strFecha, strFechaLarga;
    QString strFechaDb, strCodMaquinaDb, strCantidadDb;
    QString strEstadoDb, strtrabajoDb, strPrecioDb, strDescripcionDb;
    QDate   datFechaAnt, datFechaPost, datFecha;
    QLocale qlTraductor;
    double  dSubtotal, dTotal, dCantidad, dPrecio;
    int     fila = 0;


    FuncAux* pFuncAux = new FuncAux();

    //
    // si la tabla no esta vacia la borramos
    //
    while(ui->tlGastos->rowCount()>0){
        ui->tlGastos->removeRow(0);
    }

    //
    // Establecemos la fecha anterior y posterior
    //
    datFechaAnt = pFuncAux->funStrToDate(ui->txtFecha0->text());
    datFechaPost = pFuncAux->funStrToDate(ui->txtFecha1->text());

    //
    // Si una de las dos fechas no es valida las igualo
    //
    if(!datFechaAnt.isValid() || !datFechaPost.isValid()){
        datFechaAnt = datFechaPost;
    }

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);


    strCodMaquina   = pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText());
    dTotal          = 0;
    //
    // Recorremos cada dia del periodo
    //
    datFecha = datFechaAnt;
    while (datFecha <= datFechaPost) {

        //
        // Ponemos los marcadores a cero
        //
        dCantidad  = 0;
        dPrecio    = 0;
        dSubtotal  = 0;

        //
        // Obtenemos el dia
        //
        strFecha = qlTraductor.toString(datFecha, "dd/MM/yyyy");

        //
        // Por cada dia del periodo miramos los repostajes, para sacar litros y tipo combustible
        //
        strSql = "SELECT *FROM Repostajes";
        sql.exec(strSql);
        sql.first();
        while (sql.isValid()) {

            strFechaDb  = pFuncAux->funDesCifrar_bis(sql.record().value("Fecha").toString());
            strCodMaquinaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());

            if(strFechaDb == strFecha &&
               strCodMaquinaDb == strCodMaquina){

                strCantidadDb       = pFuncAux->funDesCifrar_bis(sql.record().value("Litros").toString());
                dCantidad           = strCantidadDb.toDouble();
                strCombustibleDb    = pFuncAux->funDesCifrar_bis(sql.record().value("Carburante").toString());
                strCodEmpresaDb     = pFuncAux->funDesCifrar_bis(sql.record().value("Empresa").toString());
                dPrecio             = pFuncAux->funPrecioLitroToEmpresa(strFechaDb, strCodEmpresaDb, strCombustibleDb);
                dSubtotal           = dPrecio * dCantidad;

                //
                // Si Subtotal > 0 , sumanos al total y  aumentamos linea el tlGastos
                //
                if( dSubtotal > 0 ){

                    ui->tlGastos->setRowCount(fila +1);

                    str             = QLocale(QLocale::Spanish, QLocale::Spain).toCurrencyString(dSubtotal);
                    strFechaLarga   = qlTraductor.toString(datFecha, "dddd ',\t' dd ' de ' MMMM ' del ' yyyy");

                    QTableWidgetItem *item_Fecha = new QTableWidgetItem(strFechaLarga);
                    QTableWidgetItem *item_Concepto = new QTableWidgetItem("Repostaje");;
                    QTableWidgetItem *item_Cantidad = new QTableWidgetItem(QString::number(dCantidad));
                    QTableWidgetItem *item_Precio = new QTableWidgetItem(QString::number(dPrecio));
                    QTableWidgetItem *item_Subtotal = new QTableWidgetItem(str);

                    item_Fecha->setTextAlignment(Qt::AlignCenter);
                    item_Concepto->setTextAlignment(Qt::AlignCenter);
                    item_Cantidad->setTextAlignment(Qt::AlignCenter);
                    item_Precio->setTextAlignment(Qt::AlignCenter);
                    item_Subtotal->setTextAlignment(Qt::AlignCenter);

                    ui->tlGastos->setItem(fila, 0, item_Fecha);
                    ui->tlGastos->setItem(fila, 1, item_Concepto);
                    ui->tlGastos->setItem(fila, 2, item_Cantidad);
                    ui->tlGastos->setItem(fila, 3, item_Precio);
                    ui->tlGastos->setItem(fila, 4, item_Subtotal);

                    //
                    // Contabilizamos horas totales
                    //
                    dTotal  = dTotal + dSubtotal;

                    fila++;
                }



            }
            sql.next();
        }

        //
        // Ponemos los marcadores a cero
        //
        dCantidad  = 0;
        dPrecio    = 0;
        dSubtotal  = 0;

        //
        // Por cada dia del periodo miramos los Mantenimientos Programados
        //
        strSql = "SELECT *FROM MantenimientoProgramado";
        sql.exec(strSql);
        sql.first();
        while (sql.isValid()) {


            strEstadoDb     = pFuncAux->funDesCifrar_bis(sql.record().value("Estado").toString());
            strFechaDb      = "";

            str = strEstadoDb;
            str = str.remove(7, strEstadoDb.length() - 7);
            if(str == "Trabajo"){

                str = strEstadoDb;
                strFechaDb = str.remove(0, strEstadoDb.length() - 10);
            }

            strCodMaquinaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());

            if(strFechaDb == strFecha &&
               strCodMaquinaDb == strCodMaquina){

                dCantidad       = 1;
                strtrabajoDb    = pFuncAux->funDesCifrar_bis(sql.record().value("Trabajo").toString());
                dPrecio         = pFuncAux->funPrecioToTarea(strtrabajoDb);
                dSubtotal       = dCantidad * dPrecio;

                //
                // Si Subtotal > 0 , sumanos al total y  aumentamos linea el tlGastos
                //
                if( dSubtotal > 0 ){

                    ui->tlGastos->setRowCount(fila +1);

                    str             = QLocale(QLocale::Spanish, QLocale::Spain).toCurrencyString(dSubtotal);
                    strFechaLarga   = qlTraductor.toString(datFecha, "dddd ',\t' dd ' de ' MMMM ' del ' yyyy");


                    QTableWidgetItem *item_Fecha = new QTableWidgetItem(strFechaLarga);
                    QTableWidgetItem *item_Concepto = new QTableWidgetItem(strtrabajoDb);;
                    QTableWidgetItem *item_Cantidad = new QTableWidgetItem(QString::number(dCantidad));
                    QTableWidgetItem *item_Precio = new QTableWidgetItem(QString::number(dPrecio));
                    QTableWidgetItem *item_Subtotal = new QTableWidgetItem(str);

                    item_Fecha->setTextAlignment(Qt::AlignCenter);
                    item_Concepto->setTextAlignment(Qt::AlignCenter);
                    item_Cantidad->setTextAlignment(Qt::AlignCenter);
                    item_Precio->setTextAlignment(Qt::AlignCenter);
                    item_Subtotal->setTextAlignment(Qt::AlignCenter);

                    ui->tlGastos->setItem(fila, 0, item_Fecha);
                    ui->tlGastos->setItem(fila, 1, item_Concepto);
                    ui->tlGastos->setItem(fila, 2, item_Cantidad);
                    ui->tlGastos->setItem(fila, 3, item_Precio);
                    ui->tlGastos->setItem(fila, 4, item_Subtotal);

                    //
                    // Contabilizamos horas totales
                    //
                    dTotal  = dTotal + dSubtotal;

                    fila++;
                }

            }
            sql.next();
        }

        //
        // Ponemos los marcadores a cero
        //
        dCantidad  = 0;
        dPrecio    = 0;
        dSubtotal  = 0;

        //
        // Por cada dia del periodo miramos las Averías
        //
        strSql = "SELECT *FROM Averias";
        sql.exec(strSql);
        sql.first();
        while (sql.isValid()) {


            strCodMaquinaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());
            strFechaDb      = pFuncAux->funDesCifrar_bis(sql.record().value("Fecha").toString());

            if(strFechaDb == strFecha &&
               strCodMaquinaDb == strCodMaquina){

                dCantidad           = 1;
                strPrecioDb         = pFuncAux->funDesCifrar_bis(sql.record().value("Precio").toString());
                strDescripcionDb    = pFuncAux->funDesCifrar_bis(sql.record().value("Descripcion").toString());
                dPrecio             = strPrecioDb.toDouble();
                dSubtotal           = dCantidad * dPrecio;

                //
                // Si Subtotal > 0 , sumanos al total y  aumentamos linea el tlGastos
                //
                if( dSubtotal > 0 ){

                    ui->tlGastos->setRowCount(fila +1);

                    str             = QLocale(QLocale::Spanish, QLocale::Spain).toCurrencyString(dSubtotal);
                    strFechaLarga   = qlTraductor.toString(datFecha, "dddd ',\t' dd ' de ' MMMM ' del ' yyyy");


                    QTableWidgetItem *item_Fecha = new QTableWidgetItem(strFechaLarga);
                    QTableWidgetItem *item_Concepto = new QTableWidgetItem(strDescripcionDb);;
                    QTableWidgetItem *item_Cantidad = new QTableWidgetItem(QString::number(dCantidad));
                    QTableWidgetItem *item_Precio = new QTableWidgetItem(QString::number(dPrecio));
                    QTableWidgetItem *item_Subtotal = new QTableWidgetItem(str);

                    item_Fecha->setTextAlignment(Qt::AlignCenter);
                    item_Concepto->setTextAlignment(Qt::AlignCenter);
                    item_Cantidad->setTextAlignment(Qt::AlignCenter);
                    item_Precio->setTextAlignment(Qt::AlignCenter);
                    item_Subtotal->setTextAlignment(Qt::AlignCenter);

                    ui->tlGastos->setItem(fila, 0, item_Fecha);
                    ui->tlGastos->setItem(fila, 1, item_Concepto);
                    ui->tlGastos->setItem(fila, 2, item_Cantidad);
                    ui->tlGastos->setItem(fila, 3, item_Precio);
                    ui->tlGastos->setItem(fila, 4, item_Subtotal);

                    //
                    // Contabilizamos horas totales
                    //
                    dTotal  = dTotal + dSubtotal;

                    fila++;
                }

            }
            sql.next();
        }


        datFecha = datFecha.addDays(1);
    }


    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    //
    // Ponemos los lbl Con texto en azul
    //
    ui->lblResumen->setStyleSheet("color: red; background-color: white");
    ui->lblTotal->setStyleSheet("color: red; background-color: white");

    //
    // Una vez rellenado el listado, ponemos el resumen si hay
    //
    str = "Gastos Totales de la maquina : ";
    str.append(ui->cmbMaquina->currentText());
    str.append(" , Desde el  ");
    str.append(ui->txtFecha0->text());
    str.append("  al  ");
    str.append(ui->txtFecha1->text());
    ui->lblResumen->setText(str);

    str = QLocale(QLocale::Spanish, QLocale::Spain).toCurrencyString(dTotal);
    ui->lblTotal->setText(str);

    //
    // Cada vez que cambia el listado anulamos fila seleccionada, y el cmd ver
    //
    Gfila_seleccionada = -1;

    delete pFuncAux;


}

void frmListGastosMaquina::funProcesaFecha0(){
    QString str;
    QDate   datPosterior, datFecha;
    QLocale qlTraductor;

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

        ui->txtFecha0->setText(qlTraductor.toString(datPosterior,"dd/MM/yyyy"));

        ui->txtFecha0->setFocus();
        ui->txtFecha0->selectAll();
    }
    //
    // Todo correcto listamos
    //
    else{
        funRellenaListado();
        ui->lblResumen->setFocus();
    }
    delete pFuncAux;

}

void frmListGastosMaquina::funProcesaFecha1(){
    QString str;
    QDate   datActual, datFecha, datFecha0;
    QLocale qlTraductor;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Comprobamos que la fecha no sea mayor que la actual
    //
    datActual   = QDate::currentDate();
    datFecha0   = pFuncAux->funStrToDate(ui->txtFecha0->text());
    datFecha    = pFuncAux->funStrToDate(ui->txtFecha1->text());
    //
    // Si es mayor que la actual
    //
    if(datFecha > datActual){
        QMessageBox::warning(this, GstrTitulo, "\t La Fecha no puede ser posterior al día de hoy...\t");

        ui->txtFecha1->setText(qlTraductor.toString(datActual,"dd/MM/yyyy"));

        ui->txtFecha1->setFocus();
        ui->txtFecha1->selectAll();
    }
    //
    // Si es menor que la fecha 0
    else if(datFecha < datFecha0 ){
        str = "La Fecha no puede ser anterior al ";
        str.append(ui->txtFecha0->text());
        QMessageBox::warning(this, GstrTitulo, str);

        ui->txtFecha1->setText(qlTraductor.toString(datActual,"dd/MM/yyyy"));

        ui->txtFecha1->setFocus();
        ui->txtFecha1->selectAll();
    }
    //
    // Todo correcto listamos
    //
    else{
        funRellenaListado();
        ui->lblResumen->setFocus();
    }

    delete pFuncAux;


}

void frmListGastosMaquina::on_cmdVolver_clicked(){

    this->close();
}

void frmListGastosMaquina::on_tbFecha0_clicked(){
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

void frmListGastosMaquina::on_tbFecha1_clicked(){
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

void frmListGastosMaquina::on_txtFecha0_returnPressed(){
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

void frmListGastosMaquina::on_txtFecha1_returnPressed(){
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

void frmListGastosMaquina::on_cmbMaquina_activated(int index){
    FuncAux* pFuncAux = new FuncAux();

    //
    // Mostramos la foto
    //
    pFuncAux->funMostrarFoto(ui->lblFotoMaquina, pFuncAux->funRutaFotoMaquinaToCodigo(pFuncAux->funCodigoToMaquina(ui->cmbMaquina->currentText())));

    funRellenaListado();

    delete pFuncAux;


}

