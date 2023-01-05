#include "frmverpartes.h"
#include "ui_frmverpartes.h"

#include "funcaux.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QMessageBox>
#include <QDate>


frmVerPartes::frmVerPartes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmVerPartes){

    ui->setupUi(this);

    this->installEventFilter(this);
}

frmVerPartes::~frmVerPartes(){
    delete ui;
}

bool frmVerPartes::eventFilter(QObject *obj, QEvent *event){

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

void frmVerPartes::on_cmdVolver_clicked(){

    this->close();
}

void frmVerPartes::funInicio(){
    QString strRutaDb, strSql, str;
    QString strEmpleado, strEmpresa, strMaquina;
    QString strFechaDb, strCodEmpleadoDb, strCodEmpresaDb, strCodMaquinaDb;
    QString strTrabajoDb, strTiempoDb, strFacturableDb;
    int     fila = 0;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Ponemos la lblInicial
    //
    strEmpleado = pFuncAux->funNombreToCodEmpleado(strCodEmpleado);

    str = "Parte Diario de  ";
    str.append(strEmpleado);
    str.append("  , con fecha:  ");
    str.append(strFecha);

    ui->lblTitulo->setText(str);

    //
    // si no esta vacia borramos el tlParte
    //
    if(ui->tlVerParte->rowCount()>0){
        ui->tlVerParte->removeRow(0);
    }

    //
    // Colocamos las columnas del tablelist
    //
    ui->tlVerParte->setColumnCount(7);

    QStringList headers;
    headers += tr("Fecha");
    headers += tr("Empleado");
    headers += tr("Empresa");
    headers += tr("Maquina");
    headers += tr("Trabajo");
    headers += tr("Tiempo");
    headers += tr("Facturable");


    ui->tlVerParte->setHorizontalHeaderLabels(headers);

    ui->tlVerParte->setColumnWidth(0, (int) ( ( this->width() * 0.70) / 7 ) );
    ui->tlVerParte->setColumnWidth(1, (int) ( ( this->width() * 0.70) / 7 ) );
    ui->tlVerParte->setColumnWidth(2, (int) ( ( this->width() * 1.20) / 7 ) );
    ui->tlVerParte->setColumnWidth(3, (int) ( ( this->width() * 1.20) / 7 ) );
    ui->tlVerParte->setColumnWidth(4, (int) ( ( this->width() * 2) / 7 ) );
    ui->tlVerParte->setColumnWidth(5, (int) ( ( this->width() * 0.50) / 7 ) );
    ui->tlVerParte->setColumnWidth(6, (int) ( ( this->width() * 0.50) / 7 ) );

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql17");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM PartesDiarios";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strFechaDb          = pFuncAux->funDesCifrar_bis(sql.record().value("Fecha").toString());
        strCodEmpleadoDb    = pFuncAux->funDesCifrar_bis(sql.record().value("Empleado").toString());

        if(strFechaDb == strFecha &&
           strCodEmpleadoDb == strCodEmpleado){

            //
            // Añadimos una fila
            //
            strEmpleado     = pFuncAux->funNombreToCodEmpleado(strCodEmpleado);

            strCodEmpresaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Empresa").toString());
            strEmpresa      = pFuncAux->funTxtToEmpresas(strCodEmpresaDb);

            strCodMaquinaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());
            strMaquina      = pFuncAux->funTxtToMaquinas(strCodMaquinaDb);

            strTrabajoDb    = pFuncAux->funDesCifrar_bis(sql.record().value("Trabajo").toString());
            strTiempoDb     = pFuncAux->funDesCifrar_bis(sql.record().value("Tiempo").toString());
            strFacturableDb = pFuncAux->funDesCifrar_bis(sql.record().value("Facturable").toString());

            ui->tlVerParte->setRowCount(fila +1);

            QTableWidgetItem *item_fecha = new QTableWidgetItem(strFecha);
            QTableWidgetItem *item_empleado = new QTableWidgetItem(strEmpleado);
            QTableWidgetItem *item_empresa = new QTableWidgetItem(strEmpresa);
            QTableWidgetItem *item_maquina = new QTableWidgetItem(strMaquina);
            QTableWidgetItem *item_trabajo = new QTableWidgetItem(strTrabajoDb);
            QTableWidgetItem *item_tiempo = new QTableWidgetItem(strTiempoDb);
            QTableWidgetItem *item_facturable = new QTableWidgetItem(strFacturableDb);

            item_fecha->setTextAlignment(Qt::AlignCenter);
            item_empleado->setTextAlignment(Qt::AlignCenter);
            item_empresa->setTextAlignment(Qt::AlignCenter);
            item_maquina->setTextAlignment(Qt::AlignCenter);
            item_trabajo->setTextAlignment(Qt::AlignCenter);
            item_tiempo->setTextAlignment(Qt::AlignCenter);
            item_facturable->setTextAlignment(Qt::AlignCenter);

            ui->tlVerParte->setItem(fila, 0, item_fecha);
            ui->tlVerParte->setItem(fila, 1, item_empleado);
            ui->tlVerParte->setItem(fila, 2, item_empresa);
            ui->tlVerParte->setItem(fila, 3, item_maquina);
            ui->tlVerParte->setItem(fila, 4, item_trabajo);
            ui->tlVerParte->setItem(fila, 5, item_tiempo);
            ui->tlVerParte->setItem(fila, 6, item_facturable);

            fila++;
        }

        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql17");

    ui->cmdVolver->setFocus();

    delete pFuncAux;


}

