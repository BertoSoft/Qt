#include "frmregempleados.h"
#include "ui_frmregempleados.h"

#include "funcaux.h"

#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QFileDialog>
#include <QImageReader>
#include <QPixmap>
#include <QListWidgetItem>


frmRegEmpleados::frmRegEmpleados(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmRegEmpleados){
    ui->setupUi(this);

    //
    // Instalamos el filtro de eventos
    //
    this->installEventFilter(this);

}

frmRegEmpleados::~frmRegEmpleados(){
    delete ui;
}

bool frmRegEmpleados::eventFilter(QObject *obj, QEvent *event){

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

void frmRegEmpleados::funInicio(){

    //
    // Primero rellenamos el list con los datos de la base de datos
    //
    funLimpiaControles();
    funRellenaListado();
    funDesHabilitaControles();

    ui->cmdGuardar->setEnabled(false);

    //
    // Colocamos el tooltip de Agregar
    //
    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Añadir un nuevo empleado a la base de datos...");

    ui->tbEliminar->setEnabled(false);
    ui->txtCodigo->setEnabled(false);

    //
    // Colocamos el tooltip de cmdVolver
    //
    ui->cmdVolver->setToolTip("Volver a la pantalla principal...");

    ui->lblDni->setFocus();

}

void frmRegEmpleados::funLimpiaControles(){

    ui->txtNombre->setText("");
    ui->txtDni->setText("");
    ui->txtApellidos->setText("");
    ui->txtDireccion->setText("");
    ui->txtTelefono->setText("");
    ui->txtCodigo->setText("");

    ui->txtNombre->setStyleSheet("color: black; background-color: white");
    ui->txtDni->setStyleSheet("color: black; background-color: white");
    ui->txtApellidos->setStyleSheet("color: black; background-color: white");
    ui->txtDireccion->setStyleSheet("color: black; background-color: white");
    ui->txtTelefono->setStyleSheet("color: black; background-color: white");
    ui->txtCodigo->setStyleSheet("color: black; background-color: white");

    ui->lblFoto->clear();

}

void frmRegEmpleados::funRellenaListado(){
    QString strRutaDb, strSql;
    QString strNombreCod, strNombreDes;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Si el listado no esta vacio lo vaciamos
    //
    while (ui->lstEmpleados->count()>0) {
        ui->lstEmpleados->takeItem(0);
    }

    //
    // Ahora miramos los registros de la base de datos
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Empleados";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        //
        // Obtenemos el nombre codificado y lo tenemos que descodificar
        //
        strNombreCod = sql.record().value("Nombre").toString();
        strNombreDes = pFuncAux->funDesCifrar_bis(strNombreCod);

        //
        // Añadimos al lstEmpleados
        //
        ui->lstEmpleados->addItem(strNombreDes);

        sql.next();
    }

    dbSql.close();
    dbSql.removeDatabase("conSql");

    delete pFuncAux;

}

void frmRegEmpleados::funDesHabilitaControles(){

    //
    // desactivamos controles
    //
    ui->txtApellidos->setEnabled(false);
    ui->txtNombre->setEnabled(false);
    ui->txtDni->setEnabled(false);
    ui->txtDireccion->setEnabled(false);
    ui->txtTelefono->setEnabled(false);
    ui->tbFoto->setEnabled(false);
    ui->tbFoto->setToolTip("");

}

void frmRegEmpleados::funHabilitaControles(){

    //
    // Activamos controles
    //
    ui->txtApellidos->setEnabled(true);
    ui->txtDni->setEnabled(true);
    ui->txtNombre->setEnabled(true);
    ui->txtDireccion->setEnabled(true);
    ui->txtTelefono->setEnabled(true);
    ui->tbFoto->setEnabled(true);
    ui->tbFoto->setToolTip("Añadir una foto de este empleado...");
}

void frmRegEmpleados::on_cmdVolver_clicked(){

    this->close();

}

void frmRegEmpleados::on_cmdGuardar_clicked(){
    QString strRutaDb, strSql;
    QString strNombreCod, strDniCod, strApellidosCod, strDireccionCod;
    QString strTelefonoCod, strCodigoCod, strRutaFotoCod;
    bool    existe = false;
    QString strCodigoViejo;

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
        strSql = "SELECT *FROM Empleados";
        sql.exec(strSql);
        sql.first();
        while (sql.isValid()) {

            //
            // Obtenemos el codigo Codificado de la BD y los descodificamos
            //
            strCodigoCod = sql.record().value("Codigo").toString();
            QString strCodigo = pFuncAux->funDesCifrar_bis(strCodigoCod);

            if(strCodigo == ui->txtCodigo->text()){
                    existe = true;
                    strCodigoViejo  = strCodigoCod;
                    sql.finish();
                }
            sql.next();
        }

        //
        // Obtenemos los valores codificados
        //
        strNombreCod    = pFuncAux->funCifrar_bis(ui->txtNombre->text());
        strDniCod       = pFuncAux->funCifrar_bis(ui->txtDni->text());
        strApellidosCod = pFuncAux->funCifrar_bis(ui->txtApellidos->text());
        strDireccionCod = pFuncAux->funCifrar_bis(ui->txtDireccion->text());
        strTelefonoCod  = pFuncAux->funCifrar_bis(ui->txtTelefono->text());
        strCodigoCod    = pFuncAux->funCifrar_bis(ui->txtCodigo->text());
        strRutaFotoCod  = pFuncAux->funCifrar_bis(GstrRutaFoto);

        //
        // Si existe borramos primero el registro anterior
        //
        if(existe){

            strSql= "UPDATE Empleados SET Nombre ='"    + strNombreCod + "', "
                                         "Dni ='"       + strDniCod + "', "
                                         "Apellidos ='" + strApellidosCod + "', "
                                         "Direccion ='" + strDireccionCod + "', "
                                         "Telefono ='"  + strTelefonoCod + "', "
                                         "Codigo ='"    + strCodigoCod + "', "
                                         "Foto ='"      + strRutaFotoCod + "'  WHERE  Codigo ='" + strCodigoViejo + "';";

        }
        else{
            strSql = " INSERT INTO Empleados(Nombre,"
                     "                          Dni,"
                     "                          Apellidos,"
                     "                          Direccion,"
                     "                          Telefono,"
                     "                          Codigo,"
                     "                          Foto) VALUES ('" + strNombreCod + "',"
                     "                                        '" + strDniCod + "',"
                     "                                        '" + strApellidosCod + "',"
                     "                                        '" + strDireccionCod + "',"
                     "                                        '" + strTelefonoCod + "',"
                     "                                        '" + strCodigoCod + "',"
                     "                                        '" + strRutaFotoCod + "');";

        }

        //
        // Ahora ejecutamos la instruccion
        //
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
        GstrRutaFoto = "";
        funRellenaListado();

        //
        // Activo el boton de + y desctivo el cmdGuardar
        //
        ui->tbAgregar->setEnabled(true);
        ui->tbAgregar->setToolTip("Añadir un nuevo empleado a la base de datos...");

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

void frmRegEmpleados::on_tbAgregar_clicked(){
    QString strCodigo;

    FuncAux* pFuncAux = new FuncAux();

    //
    // Primero habilito los controles
    //
    ui->cmdGuardar->setEnabled(true);
    ui->cmdGuardar->setToolTip("Guardar los datos de este empleado...");

    funLimpiaControles();
    funHabilitaControles();

    ui->lstEmpleados->clearSelection();

    ui->txtNombre->setFocus();

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
    strCodigo = pFuncAux->funNuevoCodigo("Empleados");

    ui->txtCodigo->setText(strCodigo);

    delete pFuncAux;
}

void frmRegEmpleados::on_txtNombre_returnPressed(){

    ui->txtApellidos->setFocus();
    ui->txtApellidos->selectAll();
}

void frmRegEmpleados::on_txtApellidos_returnPressed(){

    ui->txtDni->setFocus();
    ui->txtDni->selectAll();
}

void frmRegEmpleados::on_txtDni_returnPressed(){

    ui->txtDireccion->setFocus();
    ui->txtDireccion->selectAll();
}

void frmRegEmpleados::on_txtDireccion_returnPressed(){
    ui->txtTelefono->setFocus();
    ui->txtTelefono->selectAll();
}

void frmRegEmpleados::on_txtTelefono_returnPressed(){

    on_cmdGuardar_clicked();
}

void frmRegEmpleados::on_txtTelefono_textChanged(const QString &arg1){
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

void frmRegEmpleados::on_txtDni_textChanged(const QString &arg1){
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
        ui->txtDni->setText(str);
    }

    //
    // Si la longitud esta entre 0 y 8 solo numeros
    //
    if(arg1.length() < 9){
        str     = arg1;
        numero  = str.toLong();
        if(numero == 0){
            str.remove(str.length() - 1, 1);
            ui->txtDni->setText(str);
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
            ui->txtDni->setText(str);
        }

    }
}

void frmRegEmpleados::on_tbEliminar_clicked(){
    QString strRutaDb, strSql;
    QString strCodigoCod, strNombreCod, strCodigo;

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
    strSql = "SELECT *FROM Empleados";
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

    strSql = "DELETE FROM Empleados WHERE Codigo='"+strCodigoCod+"'AND Nombre='"+strNombreCod+"';";
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

void frmRegEmpleados::on_lstEmpleados_itemClicked(QListWidgetItem *item){
    QString strRutaDb, strSql;
    QString strNombreDes, strApellidoDes, strDniDes, strDireccionDes;
    QString strTelefonoDes, strCodigoDes, strRutaFotoDes;


    FuncAux *pFuncAux = new FuncAux();

    //
    // Desactivo los controles
    //
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

    strSql = "SELECT *FROM Empleados";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strNombreDes = pFuncAux->funDesCifrar_bis(sql.record().value("Nombre").toString());

        if(item->text() == strNombreDes){

            //
            // Desciframos los campos
            //
            strNombreDes    = pFuncAux->funDesCifrar_bis(sql.record().value("Nombre").toString());
            strApellidoDes  = pFuncAux->funDesCifrar_bis(sql.record().value("Apellidos").toString());
            strDniDes       = pFuncAux->funDesCifrar_bis(sql.record().value("Dni").toString());
            strDireccionDes = pFuncAux->funDesCifrar_bis(sql.record().value("Direccion").toString());
            strTelefonoDes  = pFuncAux->funDesCifrar_bis(sql.record().value("Telefono").toString());
            strCodigoDes    = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());
            strRutaFotoDes  = pFuncAux->funDesCifrar_bis(sql.record().value("Foto").toString());

            ui->txtNombre->setText(strNombreDes);
            ui->txtApellidos->setText(strApellidoDes);
            ui->txtDni->setText(strDniDes);
            ui->txtDireccion->setText(strDireccionDes);
            ui->txtTelefono->setText(strTelefonoDes);
            ui->txtCodigo->setText(strCodigoDes);

            pFuncAux->funMostrarFoto(ui->lblFoto, strRutaFotoDes);

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
        ui->tbEliminar->setToolTip("Eliminar este empleado de la base de datos...");
    }

    //
    // Colocamos el tooltip de Agregar
    //
    ui->tbAgregar->setEnabled(true);
    ui->tbAgregar->setToolTip("Añadir un nuevo empleado a la base de datos...");


    delete pFuncAux;

}

void frmRegEmpleados::on_tbFoto_clicked(){

    FuncAux *pFuncAux = new FuncAux();

    GstrRutaFoto = QFileDialog::getOpenFileName(this, GstrTitulo, QDir::homePath(), tr("Archivos de Imagen (*.png *.jpg *.jpeg *.bmp)"));
    pFuncAux->funMostrarFoto(ui->lblFoto, GstrRutaFoto);

    delete pFuncAux;
}

void frmRegEmpleados::on_lstEmpleados_itemDoubleClicked(QListWidgetItem *item){

    //
    // Activo los controles
    //
    funHabilitaControles();

    //
    // Activo el boton Guardar y desactivo los botones +`y -
    //
    ui->cmdGuardar->setEnabled(true);
    ui->cmdGuardar->setToolTip("Guardar los datos de este empleado...");

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

