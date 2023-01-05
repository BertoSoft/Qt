#include "frmregmaquinas.h"
#include "ui_frmregmaquinas.h"

#include "funcaux.h"

#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QFileDialog>
#include <QImageReader>
#include <QPixmap>
#include <QListWidgetItem>

frmRegMaquinas::frmRegMaquinas(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmRegMaquinas){
    ui->setupUi(this);


    //
    // Instalamos el filtro de eventos
    //
    this->installEventFilter(this);

}

frmRegMaquinas::~frmRegMaquinas(){
    delete ui;
}

bool frmRegMaquinas::eventFilter(QObject *obj, QEvent *event){
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

void frmRegMaquinas::funInicio(){
    //
    // Primero rellenamos el list con los datos de la base de datos
    //
    funLimpiaControles();
    funRellenaListado();
    funDesHabilitaControles();

    ui->cmdGuardar->setEnabled(false);
    ui->cmdGuardar->setToolTip("");

    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Añadir una máquina a la base de datos...");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    ui->txtCodigo->setEnabled(false);

    ui->cmdVolver->setToolTip("Volver a la pantalla principal...");

    ui->lblFoto->setFocus();

}

void frmRegMaquinas::funRellenaListado(){
    QString strRutaDb, strSql;
    QString strApodo, strApodoCod;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Si el listado no esta vacio lo vaciamos
    //
    while (ui->lstMaquinas->count()>0) {
        ui->lstMaquinas->takeItem(0);
    }

    //
    // Ahora miramos los registros de la base de datos
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

        strApodoCod = sql.record().value("Apodo").toString();
        strApodo    = pFuncAux->funDesCifrar_bis(strApodoCod);

        ui->lstMaquinas->addItem(strApodo);

        sql.next();
    }

    dbSql.close();
    dbSql.removeDatabase("conSql");

    delete pFuncAux;

}

void frmRegMaquinas::funHabilitaControles(){

    //
    // Activamos controles
    //
    ui->txtMarca->setEnabled(true);
    ui->txtModelo->setEnabled(true);
    ui->txtApodo->setEnabled(true);
    ui->txtBastidor->setEnabled(true);
    ui->txtHoras->setEnabled(true);
    ui->txtMatricula->setEnabled(true);

    ui->tbFoto->setEnabled(true);
    ui->tbFoto->setToolTip("Añadir una foto de la máquina...");
}

void frmRegMaquinas::funDesHabilitaControles(){

    //
    // desactivamos controles
    //
    ui->txtMarca->setEnabled(false);
    ui->txtModelo->setEnabled(false);
    ui->txtApodo->setEnabled(false);
    ui->txtHoras->setEnabled(false);
    ui->txtBastidor->setEnabled(false);
    ui->txtMatricula->setEnabled(false);

    ui->tbFoto->setEnabled(false);
    ui->tbFoto->setToolTip("");
}

void frmRegMaquinas::funLimpiaControles(){

    ui->txtMarca->setText("");
    ui->txtModelo->setText("");
    ui->txtApodo->setText("");
    ui->txtBastidor->setText("");
    ui->txtHoras->setText("");
    ui->txtMatricula->setText("");
    ui->txtCodigo->setText("");

    ui->txtMarca->setStyleSheet("color: black; background-color: white");
    ui->txtModelo->setStyleSheet("color: black; background-color: white");
    ui->txtApodo->setStyleSheet("color: black; background-color: white");
    ui->txtBastidor->setStyleSheet("color: black; background-color: white");
    ui->txtHoras->setStyleSheet("color: black; background-color: white");
    ui->txtMatricula->setStyleSheet("color: black; background-color: white");
    ui->txtCodigo->setStyleSheet("color: black; background-color: white");

    ui->lblFoto->clear();

}

void frmRegMaquinas::on_cmdVolver_clicked(){

    this->close();
}

void frmRegMaquinas::on_tbAgregar_clicked(){
    QString strRutaDb, strSql, strCodigo;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Primero habilito los controles
    //
    ui->cmdGuardar->setEnabled(true);
    ui->cmdGuardar->setToolTip("Guardar los datos de esta máquina en la base de datos...");

    funLimpiaControles();
    funHabilitaControles();

    ui->lstMaquinas->clearSelection();

    ui->txtMarca->setFocus();

    //
    // Deshabilito el boton de + y el de -
    //
    ui->tbAgregar->setEnabled(false);
    ui->tbAgregar->setToolTip("");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    //
    // Obtengo el codigo de Maquina
    //
    strCodigo = pFuncAux->funNuevoCodigo("Maquinas");
    ui->txtCodigo->setText(strCodigo);

    delete pFuncAux;


}

void frmRegMaquinas::on_txtMarca_returnPressed(){

    ui->txtModelo->setFocus();
    ui->txtModelo->selectAll();
}

void frmRegMaquinas::on_txtModelo_returnPressed(){

    ui->txtApodo->setFocus();
    ui->txtApodo->selectAll();
}

void frmRegMaquinas::on_txtApodo_returnPressed(){

    ui->txtBastidor->setFocus();
    ui->txtBastidor->selectAll();
}

void frmRegMaquinas::on_txtBastidor_returnPressed(){

    ui->txtHoras->setFocus();
    ui->txtHoras->selectAll();
}


void frmRegMaquinas::on_txtHoras_returnPressed(){

    ui->txtMatricula->setFocus();
    ui->txtMatricula->selectAll();
}

void frmRegMaquinas::on_txtMatricula_returnPressed(){

    on_cmdGuardar_clicked();
}

void frmRegMaquinas::on_cmdGuardar_clicked(){
    QString strRutaDb, strSql;
    bool    existe = false;
    QString strMarcaCod, strModeloCod, strApodoCod, strBastidorCod, strCodigoViejoCod;
    QString strHorasCod, strMatriculaCod, strCodigo, strCodigoCod, strRutaFotoCod;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Si el campo del Apodo y el de las horas esta cubierto seguimos y guardamos los valores
    //
    if(ui->txtApodo->text() != "" && ui->txtHoras->text()!=""){

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
        strSql = "SELECT *FROM Maquinas";
        sql.exec(strSql);
        sql.first();
        while (sql.isValid()) {

            strCodigoCod = sql.record().value("Codigo").toString();
            strCodigo       = pFuncAux->funDesCifrar_bis(strCodigoCod);

            if(strCodigo == ui->txtCodigo->text()){
                    existe              = true;
                    strCodigoViejoCod   = strCodigoCod;
                    sql.finish();
                }
            sql.next();
        }

        //
        // Obtenemos los valores codificados
        //
        strMarcaCod     = pFuncAux->funCifrar_bis(ui->txtMarca->text());
        strModeloCod    = pFuncAux->funCifrar_bis(ui->txtModelo->text());
        strApodoCod     = pFuncAux->funCifrar_bis(ui->txtApodo->text());
        strBastidorCod  = pFuncAux->funCifrar_bis(ui->txtBastidor->text());
        strHorasCod     = pFuncAux->funCifrar_bis(ui->txtHoras->text());
        strMatriculaCod = pFuncAux->funCifrar_bis(ui->txtMatricula->text());
        strCodigoCod    = pFuncAux->funCifrar_bis(ui->txtCodigo->text());
        strRutaFotoCod  = pFuncAux->funCifrar_bis(GstrRutaFoto);

        //
        // Si existe editamos sino añadimos
        //
        if(existe){

            strSql= "UPDATE Maquinas SET Marca ='"      + strMarcaCod +"', "
                                        "Modelo ='"     + strModeloCod +"', "
                                        "Apodo ='"      + strApodoCod +"', "
                                        "Bastidor ='"   + strBastidorCod +"', "
                                        "Horas ='"      + strHorasCod +"', "
                                        "Matricula ='"  + strMatriculaCod +"', "
                                        "Codigo ='"     + strCodigoCod +"', "
                                        "Foto ='"       + strRutaFotoCod +"'  WHERE  Codigo ='"  + strCodigoViejoCod + "';";

        }
        else{

            strSql = " INSERT INTO Maquinas(Marca,"
                     "                      Modelo,"
                     "                      Apodo,"
                     "                      Bastidor,"
                     "                      Horas,"
                     "                      Matricula,"
                     "                      Codigo,"
                     "                      Foto) VALUES ('" + strMarcaCod + "',"
                     "                                    '" + strModeloCod + "',"
                     "                                    '" + strApodoCod + "',"
                     "                                    '" + strBastidorCod + "',"
                     "                                    '" + strHorasCod + "',"
                     "                                    '" + strMatriculaCod + "',"
                     "                                    '" + strCodigoCod + "',"
                     "                                    '" + strRutaFotoCod + "');";
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
        ui->tbAgregar->setToolTip("Añadir una máquina a la base de datos...");

        ui->cmdGuardar->setEnabled(false);
        ui->cmdGuardar->setToolTip("");

        //
        // Desactivo los controles
        //
        funDesHabilitaControles();
    }
    //
    // Si el campo del Apodo o de las horas esta vacio avisamos y lo situamos ahi
    //
    else{
        QMessageBox::information(this, GstrTitulo, "El Campo del Apodo y las Horas son obligatorios...");

        //
        // Si Apodo vacio
        //
        if(ui->txtApodo->text()==""){
            ui->txtApodo->setStyleSheet("color: black; background-color: yellow");
            if(ui->txtHoras->text() == ""){
                ui->txtHoras->setStyleSheet("color: black; background-color: yellow");
            }
            ui->txtApodo->setFocus();
        }
        //
        // Horas vacio
        //
        else{
            ui->txtHoras->setStyleSheet("color: black; background-color: yellow");
            ui->txtHoras->setFocus();
        }
    }

    delete pFuncAux;
}

void frmRegMaquinas::on_tbEliminar_clicked(){
    QString strRutaDb, strSql;
    QString strCodigo, strCodigoCod, strApodoCod;

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
    strSql = "SELECT *FROM Maquinas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodigoCod    = sql.record().value("Codigo").toString();
        strCodigo       = pFuncAux->funDesCifrar_bis(strCodigoCod);

        if(strCodigo == ui->txtCodigo->text()){
            strApodoCod = sql.record().value("Apodo").toString();
            sql.finish();
        }
        sql.next();
    }

    strSql = "DELETE FROM Maquinas WHERE Codigo='" + strCodigoCod + "'AND Apodo='" + strApodoCod + "';";
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

void frmRegMaquinas::on_tbFoto_clicked(){

    FuncAux *pFuncAux = new FuncAux();

    GstrRutaFoto = QFileDialog::getOpenFileName(this, GstrTitulo, QDir::homePath(), tr("Archivos de Imagen (*.png *.jpg *.jpeg *.bmp)"));
    pFuncAux->funMostrarFoto(ui->lblFoto, GstrRutaFoto);

    delete pFuncAux;

}

void frmRegMaquinas::on_lstMaquinas_itemClicked(QListWidgetItem *item){
    QString strRutaDb, strSql;
    QString strMarca, strModelo, strApodo, strCodigo, strBastidor;
    QString strHoras, strMatricula, strRutaFoto;

    FuncAux *pFuncAux = new FuncAux();

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

        strApodo = pFuncAux->funDesCifrar_bis(sql.record().value("Apodo").toString());

        if(item->text() == strApodo){

            //
            // Desciframos los campos
            //
            strMarca        = pFuncAux->funDesCifrar_bis(sql.record().value("Marca").toString());
            strModelo       = pFuncAux->funDesCifrar_bis(sql.record().value("Modelo").toString());
            strApodo        = pFuncAux->funDesCifrar_bis(sql.record().value("Apodo").toString());
            strCodigo       = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());
            strBastidor     = pFuncAux->funDesCifrar_bis(sql.record().value("Bastidor").toString());
            strHoras        = pFuncAux->funDesCifrar_bis(sql.record().value("Horas").toString());
            strMatricula    = pFuncAux->funDesCifrar_bis(sql.record().value("Matricula").toString());
            strRutaFoto     = pFuncAux->funDesCifrar_bis(sql.record().value("Foto").toString());

            //
            // Colocamos los valores en las cajas de texto
            //

            ui->txtMarca->setText(strMarca);
            ui->txtModelo->setText(strModelo);
            ui->txtApodo->setText(strApodo);
            ui->txtBastidor->setText(strBastidor);
            ui->txtHoras->setText(strHoras);
            ui->txtMatricula->setText(strMatricula);
            ui->txtCodigo->setText(strCodigo);

            pFuncAux->funMostrarFoto(ui->lblFoto, strRutaFoto);

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
    // Activo el boton de suprimir
    //
    if(item->text() != ""){
        ui->tbEliminar->setEnabled(true);
        ui->tbEliminar->setToolTip("Eliminar esta máquina de la base de datos...");
    }

    delete pFuncAux;

}

void frmRegMaquinas::on_lstMaquinas_itemDoubleClicked(QListWidgetItem *item){

    //
    // Activo los controles
    //
    funHabilitaControles();

    //
    // Activo el boton Guardar y desactivo los botones +`y -
    //
    ui->cmdGuardar->setEnabled(true);
    ui->cmdGuardar->setToolTip("Guardar los datos de esta máquina en la base de datos...");

    ui->tbAgregar->setEnabled(false);
    ui->tbAgregar->setToolTip("");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    //
    // situo el foco en txtNombre
    //
    ui->txtMarca->setFocus();
    ui->txtMarca->selectAll();
}

void frmRegMaquinas::on_txtHoras_textChanged(const QString &arg1){
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

