#include "frmregcentrostrabajo.h"
#include "ui_frmregcentrostrabajo.h"

#include "funcaux.h"

#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QFileDialog>
#include <QImageReader>
#include <QPixmap>
#include <QTableWidgetItem>

frmregCentrosTrabajo::frmregCentrosTrabajo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmregCentrosTrabajo){

    ui->setupUi(this);

    //
    // Instalamos el filtro de eventos
    //
    this->installEventFilter(this);

}

frmregCentrosTrabajo::~frmregCentrosTrabajo(){

    delete ui;
}

bool frmregCentrosTrabajo::eventFilter(QObject *obj, QEvent *event){

    //
    // Si se redimensiona la ventana, la dejamos fija
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

void frmregCentrosTrabajo::on_cmdVolver_clicked(){

    this->close();
}

void frmregCentrosTrabajo::funInicio(){

    //
    // Primero rellenamos el list con los datos de la base de datos
    //
    funLimpiaControles();

    //
    // Colocamos las columnas del tablelist
    //
    ui->tlCentrosTrabajo->setColumnCount(6);

    QStringList headers;
    headers += tr("Empresa");
    headers += tr("Centro de Trabajo");
    headers += tr("C.I.F.");
    headers += tr("Razón Social");
    headers += tr("Teléfono");
    headers += tr("Código");

    ui->tlCentrosTrabajo->setHorizontalHeaderLabels(headers);

    ui->tlCentrosTrabajo->setColumnWidth(0, (int) ( ( this->width() * 0.6 ) / 6 ) );
    ui->tlCentrosTrabajo->setColumnWidth(1, (int) ( ( this->width() * 1.0 ) / 6 ) );
    ui->tlCentrosTrabajo->setColumnWidth(2, (int) ( ( this->width() * 0.4 ) / 6 ) );
    ui->tlCentrosTrabajo->setColumnWidth(3, (int) ( ( this->width() * 1.3 ) / 6 ) );
    ui->tlCentrosTrabajo->setColumnWidth(4, (int) ( ( this->width() * 0.4 ) / 6 ) );
    ui->tlCentrosTrabajo->setColumnWidth(5, (int) ( ( this->width() * 0.3 ) / 6 ) );

    funRellenaListado();
    funDesHabilitaControles();

    ui->cmdGuardar->setEnabled(false);
    ui->cmdGuardar->setToolTip("");

    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Añadir un centro de trabajo a la base de datos...");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    ui->cmdVolver->setToolTip("Volver a la pantalla principal...");

    ui->txtCodigo->setEnabled(false);

}

void frmregCentrosTrabajo::funRellenaListado(){
    QString strRutaDb, strSql;
    QString strEmpresa, strCif, strCentroTrabajo, strRazonSocial, strTelefono, strCodigo;
    int i;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Si el listado no esta vacio lo vaciamos
    //
    while (ui->tlCentrosTrabajo->rowCount()>0) {
        ui->tlCentrosTrabajo->removeRow(0);
    }

    //
    // Ahora miramos los registros de la base de datos
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Empresas";
    sql.exec(strSql);
    sql.first();
    i=0;

    while (sql.isValid()) {

        ui->tlCentrosTrabajo->setRowCount(i + 1);

        strEmpresa          = pFuncAux->funDesCifrar_bis(sql.record().value("Nombre").toString());
        strCif              = pFuncAux->funDesCifrar_bis(sql.record().value("Cif").toString());
        strCentroTrabajo    = pFuncAux->funDesCifrar_bis(sql.record().value("Seccion").toString());
        strRazonSocial      = pFuncAux->funDesCifrar_bis(sql.record().value("RazonSocial").toString());
        strTelefono         = pFuncAux->funDesCifrar_bis(sql.record().value("Telefono").toString());
        strCodigo           = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());


        QTableWidgetItem *item_nombre = new QTableWidgetItem(strEmpresa);
        QTableWidgetItem *item_cif = new QTableWidgetItem(strCif);
        QTableWidgetItem *item_seccion = new QTableWidgetItem(strCentroTrabajo);
        QTableWidgetItem *item_razonsocial = new QTableWidgetItem(strRazonSocial);
        QTableWidgetItem *item_telefono = new QTableWidgetItem(strTelefono);
        QTableWidgetItem *item_codigo = new QTableWidgetItem(strCodigo);

        item_nombre->setTextAlignment(Qt::AlignCenter);
        item_cif->setTextAlignment(Qt::AlignCenter);
        item_seccion->setTextAlignment(Qt::AlignCenter);
        item_razonsocial->setTextAlignment(Qt::AlignCenter);
        item_telefono->setTextAlignment(Qt::AlignCenter);
        item_codigo->setTextAlignment(Qt::AlignCenter);

        ui->tlCentrosTrabajo->setItem(i, 0, item_nombre);
        ui->tlCentrosTrabajo->setItem(i, 1, item_seccion);
        ui->tlCentrosTrabajo->setItem(i, 2, item_cif);
        ui->tlCentrosTrabajo->setItem(i, 3, item_razonsocial);
        ui->tlCentrosTrabajo->setItem(i, 4, item_telefono);
        ui->tlCentrosTrabajo->setItem(i, 5, item_codigo);

        i++;
        sql.next();
    }

    dbSql.close();
    dbSql.removeDatabase("conSql");

    delete pFuncAux;

}

void frmregCentrosTrabajo::funLimpiaControles(){

    ui->txtEmpresa->setText("");
    ui->txtCif->setText("");
    ui->txtCentroTrabajo->setText("");
    ui->txtRazonSocial->setText("");
    ui->txtTelefono->setText("");
    ui->txtCodigo->setText("");

    ui->txtEmpresa->setStyleSheet("color: black; background-color: white");
    ui->txtCif->setStyleSheet("color: black; background-color: white");
    ui->txtCentroTrabajo->setStyleSheet("color: black; background-color: white");
    ui->txtRazonSocial->setStyleSheet("color: black; background-color: white");
    ui->txtTelefono->setStyleSheet("color: black; background-color: white");
    ui->txtCodigo->setStyleSheet("color: black; background-color: white");
}

void frmregCentrosTrabajo::funHabilitaControles(){

    //
    // Activamos controles
    //
    ui->txtEmpresa->setEnabled(true);
    ui->txtCif->setEnabled(true);
    ui->txtCentroTrabajo->setEnabled(true);
    ui->txtRazonSocial->setEnabled(true);
    ui->txtTelefono->setEnabled(true);

}

void frmregCentrosTrabajo::funDesHabilitaControles(){

    //
    // desactivamos controles
    //
    ui->txtEmpresa->setEnabled(false);
    ui->txtCif->setEnabled(false);
    ui->txtCentroTrabajo->setEnabled(false);
    ui->txtRazonSocial->setEnabled(false);
    ui->txtTelefono->setEnabled(false);
}

void frmregCentrosTrabajo::on_tbAgregar_clicked(){
    QString strRutaDb, strSql, strCodigo;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Primero habilito los controles
    //
    ui->cmdGuardar->setEnabled(true);
    ui->cmdGuardar->setToolTip("Guardar datos de un centro de trabajo...");

    funLimpiaControles();
    funHabilitaControles();

    ui->tlCentrosTrabajo->clearSelection();

    ui->txtEmpresa->setFocus();

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
    strCodigo = pFuncAux->funNuevoCodigo("Empresas");

    ui->txtCodigo->setText(strCodigo);

    delete pFuncAux;
}

void frmregCentrosTrabajo::on_txtEmpresa_returnPressed(){

    ui->txtCif->setFocus();
    ui->txtCif->selectAll();

}

void frmregCentrosTrabajo::on_txtCif_returnPressed(){

    ui->txtCentroTrabajo->setFocus();
    ui->txtCentroTrabajo->selectAll();
}

void frmregCentrosTrabajo::on_txtCentroTrabajo_returnPressed(){

    ui->txtRazonSocial->setFocus();
    ui->txtRazonSocial->selectAll();

}

void frmregCentrosTrabajo::on_txtRazonSocial_returnPressed(){

    ui->txtTelefono->setFocus();
    ui->txtTelefono->selectAll();

}

void frmregCentrosTrabajo::on_txtTelefono_returnPressed(){

    on_cmdGuardar_clicked();

}

void frmregCentrosTrabajo::on_txtCodigo_returnPressed(){

}

void frmregCentrosTrabajo::on_cmdGuardar_clicked(){
    QString strRutaDb, strSql;
    bool    existe = false;
    QString strCodigoCod, strCodigo, strCodigoViejoCod;
    QString strEmpresaCod, strCifCod, strCentroTrabajoCod, strRazonSocialCod, strTelefonoCod;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Si el campo del nombre esta cubierto seguimos y guardamos los valores
    //
    if(ui->txtCentroTrabajo->text() != ""){

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
        strSql = "SELECT *FROM Empresas";
        sql.exec(strSql);
        sql.first();
        while (sql.isValid()) {

            strCodigoCod    = sql.record().value("Codigo").toString();
            strCodigo       = pFuncAux->funDesCifrar_bis(strCodigoCod);

            if(strCodigo == ui->txtCodigo->text()){
                    existe              = true;
                    strCodigoViejoCod   = strCodigoCod;
                    sql.finish();
                }
            sql.next();
        }

        //
        // Codificamos los valores
        //
        strEmpresaCod       = pFuncAux->funCifrar_bis(ui->txtEmpresa->text());
        strCifCod           = pFuncAux->funCifrar_bis(ui->txtCif->text());
        strCentroTrabajoCod = pFuncAux->funCifrar_bis(ui->txtCentroTrabajo->text());
        strRazonSocialCod   = pFuncAux->funCifrar_bis(ui->txtRazonSocial->text());
        strTelefonoCod      = pFuncAux->funCifrar_bis(ui->txtTelefono->text());
        strCodigoCod        = pFuncAux->funCifrar_bis(ui->txtCodigo->text());

        //
        // Si existe editamos sino añadimos
        //
        if(existe){

            strSql= "UPDATE Empresas SET Nombre ='"         + strEmpresaCod + "', "
                                         "Seccion ='"       + strCentroTrabajoCod + "', "
                                         "Cif ='"           + strCifCod + "', "
                                         "RazonSocial ='"   + strRazonSocialCod + "', "
                                         "Telefono ='"      + strTelefonoCod + "', "
                                         "Codigo ='"        + strCodigoCod + "'  WHERE  Codigo ='" + strCodigoViejoCod + "';";

        }
        else{

            strSql = " INSERT INTO Empresas(Nombre,"
                     "                      Seccion,"
                     "                      Cif,"
                     "                      RazonSocial,"
                     "                      Telefono,"
                     "                      Codigo) VALUES ('" + strEmpresaCod + "',"
                     "                                      '" + strCentroTrabajoCod + "',"
                     "                                      '" + strCifCod + "',"
                     "                                      '" + strRazonSocialCod + "',"
                     "                                      '" + strTelefonoCod + "',"
                     "                                      '" + strCodigoCod + "');";
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
        ui->tbAgregar->setToolTip("Añadir un centro de trabajo a la base de datos...");

        ui->cmdGuardar->setEnabled(false);
        ui->cmdGuardar->setToolTip("");

        //
        // Desactivo los controles
        //
        funDesHabilitaControles();
    }
    //
    // Si el campo del nombre o el del centro de trabajo esta vacio avisamos y lo situamos ahi
    //
    else{
        QMessageBox::information(this, GstrTitulo, "\t El Campo del centro de trabajo es obligatorio...");

        ui->txtCentroTrabajo->setStyleSheet("color: black; background-color: yellow");
        ui->txtCentroTrabajo->setFocus();
    }

    delete pFuncAux;

}

void frmregCentrosTrabajo::on_tbEliminar_clicked(){
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
    // Recorro toda la base de Datos hasta encontrar el codigo que debemo borrar
    //
    strSql = "SELECT *FROM Empresas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodigoCod    = sql.record().value("Codigo").toString();
        strCodigo       = pFuncAux->funDesCifrar_bis(strCodigoCod);

        if(strCodigo == ui->txtCodigo->text()){
            strNombreCod = sql.record().value("Nombre").toString();
            sql.finish();
        }
        sql.next();
    }



    strSql = "DELETE FROM Empresas WHERE Codigo='" + strCodigoCod + "'AND Nombre='" + strNombreCod + "';";
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

void frmregCentrosTrabajo::on_tlCentrosTrabajo_itemClicked(QTableWidgetItem *item){
    QString strRutaDb, strSql;
    int fila;
    QString strEmpresa, strCif, strCentroTrabajo, strRazonSocial, strTelefono, strCodigo;

    FuncAux *pFuncAux = new FuncAux();

    funLimpiaControles();
    funDesHabilitaControles();

    ui->cmdGuardar->setEnabled(false);
    ui->cmdGuardar->setToolTip("");

    //
    // Abro la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Empresas";
    sql.exec(strSql);
    sql.first();
    fila = 0;
    while (sql.isValid()) {
        if(fila == item->row()){

            strEmpresa       = pFuncAux->funDesCifrar_bis(sql.record().value("Nombre").toString());
            strCif           = pFuncAux->funDesCifrar_bis(sql.record().value("Cif").toString());
            strCentroTrabajo = pFuncAux->funDesCifrar_bis(sql.record().value("Seccion").toString());
            strRazonSocial   = pFuncAux->funDesCifrar_bis(sql.record().value("RazonSocial").toString());
            strTelefono      = pFuncAux->funDesCifrar_bis(sql.record().value("Telefono").toString());
            strCodigo        = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());

            ui->txtEmpresa->setText(strEmpresa);
            ui->txtCif->setText(strCif);
            ui->txtCentroTrabajo->setText(strCentroTrabajo);
            ui->txtRazonSocial->setText(strRazonSocial);
            ui->txtTelefono->setText(strTelefono);
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
    // Activo el boton de suprimir y el de agregar
    //
    if(ui->tlCentrosTrabajo->item(item->row(), 5)->text() != ""){
        ui->tbEliminar->setEnabled(true);
        ui->tbEliminar->setToolTip("Eliminar los datos de este centro de trabajo de la base de datos...");
    }

    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Añadir un centro de trabajo a la base de datos...");

    delete pFuncAux;
}

void frmregCentrosTrabajo::on_tlCentrosTrabajo_itemDoubleClicked(QTableWidgetItem *item){

    //
    // Activo los controles
    //
    funHabilitaControles();

    //
    // Activo el boton Guardar y desactivo los botones +`y -
    //
    ui->cmdGuardar->setEnabled(true);
    ui->cmdGuardar->setToolTip("Guardar datos de un centro de trabajo...");

    ui->tbAgregar->setEnabled(false);
    ui->tbAgregar->setToolTip("");

    ui->tbEliminar->setEnabled(false);
    ui->tbEliminar->setToolTip("");

    //
    // situo el foco en txtNombre
    //
    ui->txtEmpresa->setFocus();
    ui->txtEmpresa->selectAll();
}

void frmregCentrosTrabajo::on_txtCif_textChanged(const QString &arg1){
    QString str = "";
    long    numero = 0;
    int     iqch;
    QChar   qch;

    //
    // Si la longitud de la cadena sobrepasa los 9 caracteres borramos
    //
    str = arg1;
    if(arg1.length() > 9){
        str.remove(str.length() - 1, 1);
        ui->txtCif->setText(str);
    }

    //
    // Si la longitud esta entre 0 y 8 solo numeros
    //
    if(arg1.length() < 9){
        str     = arg1;
        numero  = str.toLong();
        if(numero == 0){
            str.remove(str.length() - 1, 1);
            ui->txtCif->setText(str);
        }
    }

    //
    // Si la Longitud es nueve solo letras
    //
    if(arg1.length() == 9){
        str = arg1;
        qch = str[str.length() -1];
        if(qch < 'A' || qch > 'Z'){
            str.remove(str.length() - 1, 1);
            ui->txtCif->setText(str);
        }

    }
}

void frmregCentrosTrabajo::on_txtTelefono_textChanged(const QString &arg1){
    QString str = "";
    long  numero = 0;

    //
    // Tenemos que comprobar que sea numerico sino lo borramos
    //
    str     = arg1;
    numero  = str.toLong();
    if(numero == 0){
        str.remove(str.length() - 1, 1);
        ui->txtTelefono->setText(str);
    }
}

