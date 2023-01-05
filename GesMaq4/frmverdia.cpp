#include "frmverdia.h"
#include "ui_frmverdia.h"

#include "funcaux.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QMessageBox>
#include <QDate>


frmVerDia::frmVerDia(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmVerDia){

    ui->setupUi(this);

    this->installEventFilter(this);
}

frmVerDia::~frmVerDia(){
    delete ui;
}

bool frmVerDia::eventFilter(QObject *obj, QEvent *event){

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

void frmVerDia::funInicio(){
    QString strRutaDb, strSql, str;
    QString strEmpresa, strMaquina;
    QString strFechaDb, strCodEmpresaDb, strCodMaquinaDb, strTiempoDb, strFacturableDb, strTrabajoDb;
    int     fila = 0;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Ponemos la lblInicial
    //
    strEmpresa = pFuncAux->funTxtToEmpresas(strCodEmpresa);

    str = "Resumen de Horas en  ";
    str.append(strEmpresa);
    str.append("  , con fecha:  ");
    str.append(strFecha);

    ui->lblTitulo->setText(str);

    //
    // si no esta vacia borramos el tlParte
    //
    if(ui->tlDia->rowCount()>0){
        ui->tlDia->removeRow(0);
    }

    //
    // Colocamos las columnas del tablelist
    //
    ui->tlDia->setColumnCount(6);

    QStringList headers;
    headers += tr("Fecha");
    headers += tr("Maquina");
    headers += tr("Trabajo");
    headers += tr("Tiempo");
    headers += tr("Facturable");
    headers += tr("Total.");


    ui->tlDia->setHorizontalHeaderLabels(headers);

    ui->tlDia->setColumnWidth(0, (int) ( ( this->width() * 0.70) / 6 ) );
    ui->tlDia->setColumnWidth(1, (int) ( ( this->width() * 1.3) / 6 ) );
    ui->tlDia->setColumnWidth(2, (int) ( ( this->width() * 2.1) / 6 ) );
    ui->tlDia->setColumnWidth(3, (int) ( ( this->width() * 0.5) / 6 ) );
    ui->tlDia->setColumnWidth(4, (int) ( ( this->width() * 0.5) / 6 ) );
    ui->tlDia->setColumnWidth(5, (int) ( ( this->width() * 0.75) / 6 ) );

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql18");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM PartesDiarios";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strFechaDb      = pFuncAux->funDesCifrar_bis(sql.record().value("Fecha").toString());
        strCodEmpresaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Empresa").toString());

        if(strFechaDb == strFecha &&
           strCodEmpresaDb == strCodEmpresa){
            QString strTarifa;
            double total = 0.0, tarifa = 0.0, horas = 0.0;

            //
            // Añadimos una fila
            //
            strCodMaquinaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());
            strMaquina      = pFuncAux->funTxtToMaquinas(strCodMaquinaDb);

            strTarifa       = pFuncAux->funTarifaToMaquina(strCodEmpresa, strCodMaquinaDb);
            tarifa          = strTarifa.toDouble();
            strTiempoDb     = pFuncAux->funDesCifrar_bis(sql.record().value("Tiempo").toString());
            horas           = strTiempoDb.toDouble();

            //
            // Si es facturable ponemos el total, sino total = 0
            //
            strFacturableDb = pFuncAux->funDesCifrar_bis(sql.record().value("Facturable").toString());

            if(strFacturableDb == "Si"){

                total       = tarifa * horas;
            }
            else{

                total       = 0.0;
            }

            ui->tlDia->setRowCount(fila +1);

            strTrabajoDb    = pFuncAux->funDesCifrar_bis(sql.record().value("Trabajo").toString());
            strTiempoDb     = pFuncAux->funDesCifrar_bis(sql.record().value("Tiempo").toString());
            strFacturableDb = pFuncAux->funDesCifrar_bis(sql.record().value("Facturable").toString());

            QTableWidgetItem *item_fecha = new QTableWidgetItem(strFecha);
            QTableWidgetItem *item_maquina = new QTableWidgetItem(strMaquina);
            QTableWidgetItem *item_trabajo = new QTableWidgetItem(strTrabajoDb);
            QTableWidgetItem *item_tiempo = new QTableWidgetItem(strTiempoDb);
            QTableWidgetItem *item_facturable = new QTableWidgetItem(strFacturableDb);

            if(total == 0){
                str = "";
            }else{
                str = QLocale(QLocale::Spanish, QLocale::Spain).toCurrencyString(total);
            }
            QTableWidgetItem *item_total = new QTableWidgetItem(str);


            item_fecha->setTextAlignment(Qt::AlignCenter);
            item_maquina->setTextAlignment(Qt::AlignCenter);
            item_trabajo->setTextAlignment(Qt::AlignCenter);
            item_tiempo->setTextAlignment(Qt::AlignCenter);
            item_facturable->setTextAlignment(Qt::AlignCenter);
            item_total->setTextAlignment(Qt::AlignCenter);


            ui->tlDia->setItem(fila, 0, item_fecha);
            ui->tlDia->setItem(fila, 1, item_maquina);
            ui->tlDia->setItem(fila, 2, item_trabajo);
            ui->tlDia->setItem(fila, 3, item_tiempo);
            ui->tlDia->setItem(fila, 4, item_facturable);
            ui->tlDia->setItem(fila, 5, item_total);


            fila++;
        }

        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql18");

    ui->cmdVolver->setFocus();

    delete pFuncAux;


}

void frmVerDia::on_cmdVolver_clicked(){

    this->close();
}

