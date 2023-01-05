#include "frmtareaspendientes.h"
#include "ui_frmtareaspendientes.h"

#include "funcaux.h"

#include <QDate>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QMessageBox>

frmTareasPendientes::frmTareasPendientes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmTareasPendientes){

    ui->setupUi(this);

    this->installEventFilter(this);

}

frmTareasPendientes::~frmTareasPendientes(){

    delete ui;
}

bool frmTareasPendientes::eventFilter(QObject *obj, QEvent *event){

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

void frmTareasPendientes::funInicio(){

    //
    // Colocamos las columnas del tablelist
    //
    ui->tlAvisos->setColumnCount(3);

    QStringList headers;
    headers += tr("Tarea");
    headers += tr("Máquina");
    headers += tr("Horas");

    ui->tlAvisos->setHorizontalHeaderLabels(headers);

    ui->tlAvisos->setColumnWidth(0, (int) ( ( this->width() * 1.3 ) / 3 ) );
    ui->tlAvisos->setColumnWidth(1, (int) ( ( this->width() * 0.7 ) / 3 ) );
    ui->tlAvisos->setColumnWidth(2, (int) ( ( this->width() * 1 ) / 3 ) );

    ui->cmdVolver->setToolTip("Volver a la pantalla principal...");


    //
    // Rellenamos el listado
    //
    funRellenaListado();


}

void frmTareasPendientes::funRellenaListado(){
    QString strRutaDb, strSql, str, strCodMaquinaDb, strEstadoDb;
    QString strTrabajoDb, strMaquina;
    QColor  color;
    int     fila;
    double  h_tarea, h_actuales, h_diferencia;
    bool    pendiente;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Si el listado no esta vacio lo vaciamos
    //
    while (ui->tlAvisos->rowCount()>0) {
        ui->tlAvisos->removeRow(0);
    }

    //
    // Ahora miramos los registros de la base de datos
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql-bis");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM MantenimientoProgramado";
    sql.exec(strSql);
    fila = 0;
    h_tarea = h_actuales = h_diferencia = 0;

    sql.first();
    while (sql.isValid()) {

        //
        // Miramos si la tarea esta pendiente
        //
        strEstadoDb = pFuncAux->funDesCifrar_bis(sql.record().value("Estado").toString());

        if(strEstadoDb == "Pendiente de Realizar"){
            pendiente = true;
        }
        else{
            pendiente = false;
        }

        //
        //  Comprobamos las horas
        //
        strCodMaquinaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());
        str             = pFuncAux->funHorasToMaquina(strCodMaquinaDb);
        h_actuales      = str.toDouble();
        h_tarea         = pFuncAux->funDesCifrar_bis(sql.record().value("MaxHoras").toString()).toDouble();
        h_diferencia    = h_tarea - h_actuales;

        //
        // Si la tarea esta pendiente, añadimos una fila
        //
        if(pendiente){

            ui->tlAvisos->setRowCount(fila + 1);

            strTrabajoDb = pFuncAux->funDesCifrar_bis(sql.record().value("Trabajo").toString());
            QTableWidgetItem *item_tarea = new QTableWidgetItem(strTrabajoDb);

            strCodMaquinaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());
            strMaquina = pFuncAux->funTxtToMaquinas(strCodMaquinaDb);
            QTableWidgetItem *item_maquina = new QTableWidgetItem(strMaquina);

            //
            // Comprobamos si vamos pasados de horas
            //
            if(h_diferencia > 0){
                str = "Faltan ";
                str.append(QString::number(h_diferencia));
                str.append(" Horas.");
                color.setRgb(255,255,255);
            }
            else{
                str = "Se Pasa ";
                int i = h_diferencia * -1;
                str.append(QString::number(i));
                str.append(" Horas.");
                color.setRgb(255,0,0);
            }
            QTableWidgetItem *item_horas = new QTableWidgetItem(str);

            item_tarea->setTextAlignment(Qt::AlignCenter);
            item_maquina->setTextAlignment(Qt::AlignCenter);
            item_horas->setTextAlignment(Qt::AlignCenter);

            item_tarea->setBackground(color);
            item_maquina->setBackground(color);
            item_horas->setBackground(color);

            ui->tlAvisos->setItem(fila, 0, item_tarea);
            ui->tlAvisos->setItem(fila, 1, item_maquina);
            ui->tlAvisos->setItem(fila, 2, item_horas);

            fila++;
        }


        sql.next();
    }

    //
    // Ordenamos la Tabla
    //
    ui->tlAvisos->sortItems(1, Qt::DescendingOrder);


    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql-bis");

    delete pFuncAux;


}

void frmTareasPendientes::on_cmdVolver_clicked(){

    this->close();
}

