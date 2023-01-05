#include "frmlisthorascentrotrabajo.h"
#include "ui_frmlisthorascentrotrabajo.h"

#include "frmcalendario.h"
#include "frmverdia.h"
#include "funcaux.h"


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QMessageBox>
#include <QDate>


frmListHorasCentroTrabajo::frmListHorasCentroTrabajo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmListHorasCentroTrabajo){

    ui->setupUi(this);

    this->installEventFilter(this);

}

frmListHorasCentroTrabajo::~frmListHorasCentroTrabajo(){

    delete ui;
}

bool frmListHorasCentroTrabajo::eventFilter(QObject *obj, QEvent *event){

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

void frmListHorasCentroTrabajo::funInicio(){
    QString strRutaDb, strSql, str;
    QDate   datFechaAnt, datFechaPost;
    QString strCodEmpresa;
    QLocale qlTraductor;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Colocamos las columnas del tablelist
    //
    ui->tlListado->setColumnCount(5);

    QStringList headers;
    headers += tr("Fecha");
    headers += tr("H. Facturables");
    headers += tr("H. No Facturables");
    headers += tr("Horas Totales");
    headers += tr("Total.");

    ui->tlListado->setHorizontalHeaderLabels(headers);

    ui->tlListado->setColumnWidth(0, (int) ( ( this->width() * 2.1) / 5 ) );
    ui->tlListado->setColumnWidth(1, (int) ( ( this->width() * 0.65) / 5 ) );
    ui->tlListado->setColumnWidth(2, (int) ( ( this->width() * 0.65) / 5 ) );
    ui->tlListado->setColumnWidth(3, (int) ( ( this->width() * 0.65) / 5 ) );
    ui->tlListado->setColumnWidth(4, (int) ( ( this->width() * 0.65) / 5 ) );

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql16");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

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

        strCodEmpresa   = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());
        str = pFuncAux->funTxtToEmpresas(strCodEmpresa);

        ui->cmbEmpresa->addItem(str);
        sql.next();
    }

    delete pFuncAux;

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql16");

    //
    // Colocamos la fecha actual y el 1º de mes
    //
    datFechaPost = QDate::currentDate();
    ui->txtFecha1->setText(qlTraductor.toString(datFechaPost,"dd/MM/yyyy"));

    datFechaAnt = QDate::currentDate();
    datFechaAnt.setDate(datFechaPost.year(), datFechaPost.month(), 01);
    ui->txtFecha0->setText(qlTraductor.toString(datFechaAnt, "dd/MM/yyyy"));

    funRellenaListado();

    ui->cmdVer->setEnabled(false);
    ui->cmdVer->setToolTip("");

    ui->cmdVolver->setToolTip("Volver a la pantalla principal...");

    ui->lblTexto->setFocus();


}

void frmListHorasCentroTrabajo::funRellenaListado(){
    QString strRutaDb, strSql, str;
    QString strCodEmpresa, strCodMaquina, strFecha, strFechaLarga;
    QDate   datFechaAnt, datFechaPost, datFecha;
    QString strFechaDb, strCodEmpresaDb, strFacturableDb, strTiempoDb;
    QLocale qlTraductor;
    double  horas_facturables, horas_no_facturables;
    double  h_totales_fact, h_totales_no_Fact, tarifa, subtotal, total;
    int     fila = 0;

    FuncAux* pFuncAux = new FuncAux();

    //
    // si la tabla no esta vacia la borramos
    //
    while(ui->tlListado->rowCount()>0){
        ui->tlListado->removeRow(0);
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

    //
    // Obtenemos el codigo de esa Empresa
    //
    strCodEmpresa       = pFuncAux->funCodigoToEmpresa(ui->cmbEmpresa->currentText());
    h_totales_fact      = 0.0;
    h_totales_no_Fact   = 0.0;
    total               = 0.0;

    //
    // Recorremos todos los dias entre fecha inicial y final
    //
    datFecha = datFechaAnt;
    while (datFecha <= datFechaPost) {

        horas_facturables       = 0.0;
        horas_no_facturables    = 0.0;
        tarifa                  = 0.0;
        subtotal                = 0.0;

        //
        // Por cada Dia, Recorremos la tabla Partes diarios
        //
        strSql = "SELECT *FROM PartesDiarios";
        sql.exec(strSql);
        sql.first();
        while (sql.isValid()) {

            strFecha        = qlTraductor.toString(datFecha, "dd/MM/yyyy");
            strFechaLarga   = qlTraductor.toString(datFecha, "dddd ',\t' dd ' de ' MMMM ' del ' yyyy");

            strFechaDb      = pFuncAux->funDesCifrar_bis(sql.record().value("Fecha").toString());
            strCodEmpresaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Empresa").toString());

            if(strFechaDb == strFecha &&
               strCodEmpresaDb == strCodEmpresa){
                //
                // La hora es facturable
                //

                strFacturableDb = pFuncAux->funDesCifrar_bis(sql.record().value("Facturable").toString());

                if(strFacturableDb == "Si"){

                    strTiempoDb         = pFuncAux->funDesCifrar_bis(sql.record().value("Tiempo").toString());
                    horas_facturables   = horas_facturables + strTiempoDb.toDouble();
                    strCodMaquina       = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());
                    tarifa              = pFuncAux->funTarifaToMaquina(strCodEmpresa, strCodMaquina).toDouble();
                    subtotal            = horas_facturables * tarifa;
                }
                //
                // La hora no es facturable
                //
                else{

                    strTiempoDb             = pFuncAux->funDesCifrar_bis(sql.record().value("Tiempo").toString());
                    horas_no_facturables    = horas_no_facturables + strTiempoDb.toDouble();
                }
            }
            sql.next();
        }

        //
        // Insertamos los datos de ese dia si los hay
        //
        if(horas_facturables > 0 || horas_no_facturables > 0){

            ui->tlListado->setRowCount(fila +1);

            QTableWidgetItem *item_Fecha = new QTableWidgetItem(strFechaLarga);
            QTableWidgetItem *item_Horas = new QTableWidgetItem(QString::number(horas_facturables + horas_no_facturables));;
            QTableWidgetItem *item_Facturables = new QTableWidgetItem(QString::number(horas_facturables));
            QTableWidgetItem *item_NoFacturables = new QTableWidgetItem(QString::number(horas_no_facturables));


            str = QLocale(QLocale::Spanish, QLocale::Spain).toCurrencyString(subtotal);
            QTableWidgetItem *item_subtotal = new QTableWidgetItem(str);

            item_Fecha->setTextAlignment(Qt::AlignCenter);
            item_Horas->setTextAlignment(Qt::AlignCenter);
            item_Facturables->setTextAlignment(Qt::AlignCenter);
            item_NoFacturables->setTextAlignment(Qt::AlignCenter);
            item_subtotal->setTextAlignment(Qt::AlignCenter);

            ui->tlListado->setItem(fila, 0, item_Fecha);
            ui->tlListado->setItem(fila, 3, item_Horas);
            ui->tlListado->setItem(fila, 1, item_Facturables);
            ui->tlListado->setItem(fila, 2, item_NoFacturables);
            ui->tlListado->setItem(fila, 4, item_subtotal);

            //
            // Contabilizamos horas totales
            //
            h_totales_fact      = h_totales_fact + horas_facturables;
            h_totales_no_Fact   = h_totales_no_Fact + horas_no_facturables;
            total               = total + subtotal;

            fila++;
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
    ui->lblTexto->setStyleSheet("color: blue; background-color: white");
    ui->lblHT->setStyleSheet("color: blue; background-color: white");
    ui->lblHF->setStyleSheet("color: blue; background-color: white");
    ui->lblHNF->setStyleSheet("color: blue; background-color: white");
    ui->lblTotal->setStyleSheet("color: blue; background-color: white");


    //
    // Una vez rellenado el listado, ponemos el resumen si hay
    //
    if( h_totales_fact > 0 || h_totales_no_Fact > 0){
        QString strEmpresa;

        strEmpresa = ui->cmbEmpresa->currentText();

        str = "Horas de  ";
        str.append(strEmpresa);
        str.append(" en el periodo...");
        ui->lblTexto->setText(str);

        str = QString::number(h_totales_fact + h_totales_no_Fact);
        str.append(" Horas");
        ui->lblHT->setText(str);

        str = QString::number(h_totales_fact);
        str.append(" Horas");
        ui->lblHF->setText(str);

        str = QString::number(h_totales_no_Fact);
        str.append(" Horas");
        ui->lblHNF->setText(str);

        str = QLocale(QLocale::Spanish, QLocale::Spain).toCurrencyString(total);
        ui->lblTotal->setText(str);

    }
    else{
        str = "";

        ui->lblTexto->setText("");
        ui->lblHT->setText("");
        ui->lblHF->setText("");
        ui->lblHNF->setText("");
        ui->lblTotal->setText("");
    }


    //
    // Cada vez que cambia el listado anulamos fila seleccionada, y el cmd ver
    //
    Gfila_seleccionada = -1;

    ui->cmdVer->setEnabled(false);
    ui->cmdVer->setToolTip("");

    delete pFuncAux;


}

void frmListHorasCentroTrabajo::funProcesaFecha0(){
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
        ui->lblTexto->setFocus();
    }
    delete pFuncAux;

}

void frmListHorasCentroTrabajo::funProcesaFecha1(){
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
        ui->lblTexto->setFocus();
    }

    delete pFuncAux;

}

void frmListHorasCentroTrabajo::on_cmdVolver_clicked(){

    this->close();
}

void frmListHorasCentroTrabajo::on_tbFecha0_clicked(){

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
    ui->lblTexto->setFocus();

}

void frmListHorasCentroTrabajo::on_tbFecha1_clicked(){

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
    ui->lblTexto->setFocus();


}

void frmListHorasCentroTrabajo::on_txtFecha0_returnPressed(){
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

void frmListHorasCentroTrabajo::on_txtFecha1_returnPressed(){
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

void frmListHorasCentroTrabajo::on_cmbEmpresa_activated(int index){

    funRellenaListado();
}

void frmListHorasCentroTrabajo::on_cmdVer_clicked(){
    QString strFecha, str;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Obtenemos la fecha corta
    //
    strFecha = pFuncAux->funFLargaToCorta(ui->tlListado->item(Gfila_seleccionada, 0)->text());

    //
    // LLamamos a ver partes, pasando el dia
    //
    frmVerDia *pVerDia = new frmVerDia(this);

    pVerDia->setWindowTitle(GstrTitulo);
    pVerDia->strFecha = strFecha;
    pVerDia->strCodEmpresa = pFuncAux->funCodigoToEmpresa(ui->cmbEmpresa->currentText());

    pVerDia->funInicio();
    pVerDia->setWindowFlag(Qt::CustomizeWindowHint);
    pVerDia->setWindowModality(Qt::WindowModal);

    pVerDia->exec();

    delete pVerDia;

    //
    // Deseleccionamos la tabla,
    //
    ui->cmdVer->setEnabled(false);
    ui->cmdVer->setToolTip("");

    ui->tlListado->clearSelection();
    Gfila_seleccionada = -1;

    ui->lblTexto->setFocus();

    delete pFuncAux;


}

void frmListHorasCentroTrabajo::on_tlListado_itemClicked(QTableWidgetItem *item){

    if(ui->tlListado->rowCount()>0){
        Gfila_seleccionada = item->row();
        ui->cmdVer->setEnabled(true);
        ui->cmdVer->setToolTip("Ver detalles del un fecha...");
    }
    else{
        ui->cmdVer->setEnabled(false);
        ui->cmdVer->setToolTip("");
    }
}

void frmListHorasCentroTrabajo::on_tlListado_itemDoubleClicked(QTableWidgetItem *item){

    on_cmdVer_clicked();
}

