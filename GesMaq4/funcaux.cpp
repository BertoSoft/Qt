#include "funcaux.h"

#include <cryptopp/cryptlib.h>
#include <cryptopp/rijndael.h>
#include <cryptopp/modes.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/rdrand.h>



#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QApplication>
#include <QTime>
#include <QDate>
#include <QLabel>
#include <QImageReader>

#define KEY "U82HuXLNzbX3f6r"
#define IV  "epDKqfVtYXhdXgb"


using namespace CryptoPP;

FuncAux::FuncAux(){

    //
    // Constructor
    //
}

bool FuncAux::funExisteUsuario(){
    QString         strBd, strSql;
    QSqlDatabase    dbGesMaq;
    QSqlQuery       sql;

    bool bExiste = false;

    //
    // Abrimos la BD
    //
    strBd = qApp->applicationDirPath() + "/Data/GesMaq.db";
    dbGesMaq =QSqlDatabase::addDatabase("QSQLITE","conSql");
    dbGesMaq.setDatabaseName(strBd);
    dbGesMaq.open();

    //
    // Establecemos el sql query
    //
    sql = QSqlQuery(dbGesMaq);

    strSql = "SELECT *FROM Usuarios";
    sql.exec(strSql);
    sql.first();
    if(sql.isValid())
    {
        bExiste=true;
    }

    //
    // Cerramos la base de Datos y la conexion
    //
    dbGesMaq.close();
    dbGesMaq.removeDatabase("conSql");

    return bExiste;
}

QString FuncAux::funNuevoCodigo(QString strTabla){
    QString strCodigo;
    QString strRutaDb, strSql;
    int     iCodigo = 0;

    //
    // Abrimos la base de datos
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql ="SELECT *FROM ";
    strSql.append(strTabla);
    strSql.append(";");

    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {
        QString strCodigoCodDb, strCodigoDesDb;

        strCodigoCodDb  = sql.record().value("Codigo").toString();
        strCodigoDesDb  = funDesCifrar_bis(strCodigoCodDb);
        if( strCodigoDesDb.toInt() > iCodigo){
            iCodigo = strCodigoDesDb.toInt();
        }
        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    iCodigo++;

    strCodigo = QString::number(iCodigo);

    return strCodigo;;
}

bool FuncAux::funHayAvisos(){
    QString strRutaDb, strSql;
    QString strEstadoDb, strMaxHorasDb, strCodMaquinaDb;
    bool    bAvisos = false;
    double  horas_max, horas_actuales, horas_diferencia;

    //
    // Ahora abrimos los registros de la base de datos en la tabla Mantenimiento programado
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql-bis");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM MantenimientoProgramado";
    sql.exec(strSql);
    sql.first();
    //
    // Recorremos toda la tabla mantenimiento Programado
    //
    while (sql.isValid()) {
        //
        // Si el estado es pendiente debemos comprobar las horas
        //
        strEstadoDb = funDesCifrar_bis(sql.record().value("Estado").toString());

        if(strEstadoDb == "Pendiente de Realizar"){

            strMaxHorasDb   = funDesCifrar_bis(sql.record().value("MaxHoras").toString());
            strCodMaquinaDb = funDesCifrar_bis(sql.record().value("Maquina").toString());

            horas_max       = strMaxHorasDb.toDouble();
            horas_actuales  = funHorasToMaquina(strCodMaquinaDb).toDouble();

            horas_diferencia = horas_max - horas_actuales;

            //
            // Si vamos pasados de horas generamos un aviso y salimos
            //
            if(horas_diferencia < 0){
                bAvisos = true;
                sql.finish();
            }
        }

        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql-bis");

    return bAvisos;
}

QString FuncAux::funTarifaToMaquina(QString strCodigoEmpresa, QString strCodMaquina){
    QString strRutaDb, strSql, strTarifa;
    QString strCodEmpresaDb, strCodMaquinaDb;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql20");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Tarifas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodEmpresaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Empresa").toString());
        strCodMaquinaDb = pFuncAux->funDesCifrar_bis(sql.record().value("Maquina").toString());

        if(strCodEmpresaDb == strCodigoEmpresa &&
            strCodMaquinaDb == strCodMaquina){

            strTarifa = pFuncAux->funDesCifrar_bis(sql.record().value("Precio").toString());
        }
        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql20");

    delete pFuncAux;

    return strTarifa;
}

QString FuncAux::funCifrar(QString strTextoPlano){
    QString                     strTextoCodHex;
    byte                        key[AES::DEFAULT_KEYLENGTH] = KEY;
    byte                        iv[AES::BLOCKSIZE] = IV;
    OFB_Mode<AES>::Encryption   t_Encrypt;
    std::string                 std_Str,std_NonHex,std_CompletelyEncrypted;

    t_Encrypt.SetKeyWithIV(key,sizeof(key),iv);
    std_Str = strTextoPlano.toStdString();
    StringSource(std_Str,true,new StreamTransformationFilter(t_Encrypt,new StringSink(std_NonHex)));
    StringSource(std_NonHex,true,new HexEncoder(new StringSink(std_CompletelyEncrypted)));
    strTextoCodHex = QString::fromStdString(std_CompletelyEncrypted);

    return strTextoCodHex;
}

QString FuncAux::funCifrar_bis(QString strTextoPlano){
    QString                     strTextoCod;
    byte                        key[AES::DEFAULT_KEYLENGTH] = KEY;
    byte                        iv[AES::BLOCKSIZE];
    CBC_Mode<AES>::Encryption   Encrypt;
    std::string                 std_Str, std_TextoCod, std_textoCodHex, std_strCompletelyEncrypted;
    RandomPool                  prng;
    std::string                 std_iv, std_ivHex;

    //
    // Generamos la Iv Aleatoria y no salimos hasta que tenga 16 bytes
    //
    std_iv.clear();

    while (std_iv.length() != 16) {
        std_iv.clear();
        prng.GenerateBlock(iv, 16);
        char char_iv[16];
        memcpy(&char_iv, iv, sizeof(iv));
        std_iv.append(char_iv, 0, 16);
    }

    //
    // Pasamos de QString a std::string
    //
    std_Str = strTextoPlano.toStdString();

    //
    // Codificamos
    //
    Encrypt.SetKeyWithIV(key,sizeof(key),iv);
    StringSource(std_Str,true,new StreamTransformationFilter(Encrypt,new StringSink(std_TextoCod)));

    //
    // Pasamos Texto a Hex
    //
    StringSource(std_TextoCod,true,new HexEncoder(new StringSink(std_textoCodHex)));

    //
    // Pasamos iv a Hex
    //
    StringSource(std_iv,true,new HexEncoder(new StringSink(std_ivHex)));

    //
    // Fusionamos iv y texto
    //
    std_strCompletelyEncrypted = std_ivHex;
    std_strCompletelyEncrypted.append(std_textoCodHex);

    strTextoCod = QString::fromStdString(std_strCompletelyEncrypted);

    return strTextoCod;
}

QString FuncAux::funDesCifrar(QString strTextoCodHex){
    QString                     strTextoDes;
    byte                        key[AES::DEFAULT_KEYLENGTH] = KEY;
    byte                        iv[AES::BLOCKSIZE] = IV;
    OFB_Mode<AES>::Decryption   Decrypt;
    std::string                 std_Str,std_NonHex,std_CompletelyEncrypted;

    Decrypt.SetKeyWithIV(key,sizeof(key),iv);
    std_CompletelyEncrypted = strTextoCodHex.toStdString();
    StringSource(std_CompletelyEncrypted,true,new HexDecoder(new StringSink(std_NonHex)));
    StringSource(std_NonHex,true,new StreamTransformationFilter(Decrypt,new StringSink(std_Str)));
    strTextoDes = QString::fromStdString(std_Str);

    return strTextoDes;
}

QString FuncAux::funDesCifrar_bis(QString strTextoCodHex){
    QString                     strTextoDes;
    byte                        key[AES::DEFAULT_KEYLENGTH] = KEY;
    byte                        iv[AES::BLOCKSIZE];
    RandomPool                  prng;
    CBC_Mode<AES>::Decryption   Decrypt;
    std::string                 std_Str, std_TextoCod, std_textoCodHex, std_strCompletelyEncrypted;
    std::string                 std_iv, std_ivHex;

    //
    // Sacamos la ivHex
    //
    std_strCompletelyEncrypted = strTextoCodHex.toStdString();

    std_ivHex       = std_strCompletelyEncrypted.substr(0, 32);
    std_textoCodHex = std_strCompletelyEncrypted.substr(32, std_strCompletelyEncrypted.length() - 32);


    //
    // Pasamos de Hex a Char
    //
    StringSource(std_textoCodHex,true,new HexDecoder(new StringSink(std_TextoCod)));
    StringSource(std_ivHex,true,new HexDecoder(new StringSink(std_iv)));

    //
    // Asignamos std_iv a byte iv
    //
    memcpy(iv, std_iv.c_str(), std_iv.length());

    //
    // Decodificamos
    //
    Decrypt.SetKeyWithIV(key,sizeof(key),iv);
    StringSource(std_TextoCod,true,new StreamTransformationFilter(Decrypt,new StringSink(std_Str)));

    strTextoDes = QString::fromStdString(std_Str);

    return strTextoDes;
}

bool FuncAux::funEsMultiplo64(int i){
    bool bEsMultiplo64 = false;

    if(i%64 == 0){
        bEsMultiplo64 = true;
    }
    else{
        bEsMultiplo64 = false;
    }

    return bEsMultiplo64;
}

bool FuncAux::funComprobarUsuarioPasswd(QString strUsuario, QString strPasswd){
    bool    acceso = false;
    QString strRutaDb, strSql;
    QString strUsuarioDbCod, strPasswdDbCod;
    QString strUsuarioDbDes, strPasswdDbDes;

    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Usuarios";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {
        //
        // Si existe el usuario, comprobamos su contraseña
        //
        strUsuarioDbCod = sql.record().value("Usuario").toString();
        strUsuarioDbDes = funDesCifrar_bis(strUsuarioDbCod);

        if(strUsuarioDbDes == strUsuario){
            //
            // Descodificamos la clave
            //
            strPasswdDbCod  = sql.record().value("Passwd").toString();
            strPasswdDbDes  = funDesCifrar_bis(strPasswdDbCod);
            if(strPasswdDbDes == strPasswd){
                acceso = true;
                sql.finish();
            }
        }
        sql.next();
    }

    dbSql.close();
    dbSql.removeDatabase("conSql");

    return acceso;;
}

QString FuncAux::funAddUsuario(QString strUsuario, QString strPasswd){
    QString         strError="";
    QString         strBd, strSql;
    QString         strUsuarioCod, strUsuarioDbCod, strUsuarioDb, strPasswdCod;
    QSqlDatabase    dbGesMaq;
    QSqlQuery       sql;
    bool            existe=false;

    //
    // Abrimos la BD
    //
    strBd = qApp->applicationDirPath() + "/Data/GesMaq.db";
    dbGesMaq =QSqlDatabase::addDatabase("QSQLITE","conSql");
    dbGesMaq.setDatabaseName(strBd);
    dbGesMaq.open();

    //
    // Establecemos el sql query
    //
    sql = QSqlQuery(dbGesMaq);

    //
    // Comprobamos si existe el usuario recorriendo toda la BD
    //
    strSql = "SELECT *FROM Usuarios";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        //
        // Obtenemos el usuario Codificado y lo descodificamos
        //
        strUsuarioDbCod = sql.record().value("Usuario").toString();
        strUsuarioDb    = funDesCifrar_bis(strUsuarioDbCod);

        //
        // Si usuarioDb == Usuario, existe = true
        //
        if(strUsuario == strUsuarioDb){
            existe = true;
            sql.finish();
        }
        sql.next();
    }

    //
    // Si el usuario ya existe devolvemos error
    //
    if(existe){
        strError = "Este Usuario ya existe en la base de datos del programa...";
    }
    //
    // Si no existe lo grabamos
    //
    else{
        //
        // Codificamos usuario y clave
        //
        strUsuarioCod   = funCifrar_bis(strUsuario);
        strPasswdCod    = funCifrar_bis(strPasswd);

        strSql="INSERT INTO Usuarios(Usuario,Passwd) VALUES('"+strUsuarioCod+"','"+strPasswdCod+"');";

        sql.exec(strSql);
    }


    //
    // Cerramos la base de Datos y la conexion
    //
    dbGesMaq.close();
    dbGesMaq.removeDatabase("conSql");

    return strError;
}

void FuncAux::funSetUsuarioActivo(QString strUsuario){
    QString         strBd, strSql;
    QSqlDatabase    dbGesMaq;
    QSqlQuery       sql;
    QString         strHoraInicio, strFechaInicio, strUsuarioCod;
    QString         strHoraInicioCod, strFechaInicioCod;

    //
    // Establezco hora y fecha de Cambio de usuario activo
    //
    strHoraInicio   = QTime::currentTime().toString("hh:mm:ss");
    strFechaInicio  = QDate::currentDate().toString("dd/MM/yyyy");

    //
    // Guardo los valores decodificados en las variables globales
    //
    GstrFechaInicio = strFechaInicio;
    GstrHoaraInicio = strHoraInicio;
    GstrUsuario     = strUsuario;

    //
    // Codifico los datos
    //
    strHoraInicioCod    = funCifrar(strHoraInicio);
    strFechaInicioCod   = funCifrar(strFechaInicio);

    //
    // Abrimos la BD
    //
    strBd = qApp->applicationDirPath() + "/Data/GesMaq.db";
    dbGesMaq =QSqlDatabase::addDatabase("QSQLITE","conSql");
    dbGesMaq.setDatabaseName(strBd);
    dbGesMaq.open();

    //
    // Establecemos el sql query
    //
    sql = QSqlQuery(dbGesMaq);

    //
    // Codificamos el Usuario
    //
    strUsuarioCod = funCifrar_bis(strUsuario);

    strSql = " INSERT INTO RegistroSesiones(Usuario,"
             "                              FechaInicio,"
             "                              HoraInicio,"
             "                              FechaCierre,"
             "                              HoraCierre) VALUES ('" + strUsuarioCod + "',"
             "                                                  '" + strFechaInicioCod + "',"
             "                                                  '" + strHoraInicioCod + "',"
             "                                                  '" + "" + "',"
             "                                                  '" + "" + "');";
    sql.exec(strSql);

    //
    // Cerramos la base de Datos y la conexion
    //
    dbGesMaq.close();
    dbGesMaq.removeDatabase("conSql");



}

QString FuncAux::funGetUsuarioActivo(){
    QString         strUsuarioActivo = "", strUsuarioActivoCod;
    QString         strBd, strSql;
    QSqlDatabase    dbGesMaq;
    QSqlQuery       sql;

    //
    // Abrimos la BD
    //
    strBd = qApp->applicationDirPath() + "/Data/GesMaq.db";
    dbGesMaq =QSqlDatabase::addDatabase("QSQLITE","conSql");
    dbGesMaq.setDatabaseName(strBd);
    dbGesMaq.open();

    //
    // Establecemos el sql query
    //
    sql = QSqlQuery(dbGesMaq);

    strSql = "SELECT *FROM RegistroSesiones";
    sql.exec(strSql);
    sql.last();

    //
    // Obtenemos el usuario codificado
    //
    strUsuarioActivoCod = sql.record().value("Usuario").toString();
    strUsuarioActivo = funDesCifrar_bis(strUsuarioActivoCod);

    //
    // Cerramos la base de Datos y la conexion
    //
    dbGesMaq.close();
    dbGesMaq.removeDatabase("conSql");


    return strUsuarioActivo;
}

void FuncAux::funCierreSesion(){
    QString         strBd, strSql;
    QSqlDatabase    dbGesMaq;
    QSqlQuery       sql;
    QString         strHoraInicioCod, strFechaInicioCod;
    QString         strHoraFinalCod, strFechaFinalCod;
    QString         strHoraFinal, strFechaFinal;
    QString         strUsuarioCod;

    //
    // Establezco hora y fecha de Cierre de sesion
    //
    strHoraFinal   = QTime::currentTime().toString("hh:mm:ss");
    strFechaFinal  = QDate::currentDate().toString("dd/MM/yyyy");

    //
    // Codificamos
    //
    strHoraFinalCod     = funCifrar_bis(strHoraFinal);
    strFechaFinalCod    = funCifrar_bis(strFechaFinal);

    //
    // Abrimos la BD
    //
    strBd = qApp->applicationDirPath() + "/Data/GesMaq.db";
    dbGesMaq =QSqlDatabase::addDatabase("QSQLITE","conSql");
    dbGesMaq.setDatabaseName(strBd);
    dbGesMaq.open();

    //
    // Establecemos el sql query
    //
    sql = QSqlQuery(dbGesMaq);

    strSql = "SELECT *FROM RegistroSesiones";
    sql.exec(strSql);
    sql.last();

    strFechaInicioCod  = sql.record().value("FechaInicio").toString();
    strHoraInicioCod   = sql.record().value("HoraInicio").toString();
    strUsuarioCod      = sql.record().value("Usuario").toString();

    //
    // Guardamos lo valores del cierre de sesion
    //
    strSql= "UPDATE RegistroSesiones SET FechaCierre ='" + strFechaFinalCod + "', "
                                        "HoraCierre ='" + strHoraFinalCod + "'  WHERE  Usuario ='" + strUsuarioCod + "' AND "
                                                                               "FechaInicio ='" + strFechaInicioCod + "' AND "
                                                                                "HoraInicio ='" + strHoraInicioCod + "';";

    sql.exec(strSql);


    //
    // Cerramos la base de Datos y la conexion
    //
    dbGesMaq.close();
    dbGesMaq.removeDatabase("conSql");

}

int FuncAux::funMesesToInt(QString strMes){

    int iMes=-1;

    if(strMes.compare("enero")==0){
        iMes=1;
    }else if(strMes.compare("febrero")==0){
        iMes=2;
    }else if(strMes.compare("marzo")==0){
        iMes=3;
    }else if(strMes.compare("abril")==0){
        iMes=4;
    }else if(strMes.compare("mayo")==0){
        iMes=5;
    }else if(strMes.compare("junio")==0){
        iMes=6;
    }else if(strMes.compare("julio")==0){
        iMes=7;
    }else if(strMes.compare("agosto")==0){
        iMes=8;
    }else if(strMes.compare("septiembre")==0){
        iMes=9;
    }else if(strMes.compare("octubre")==0){
        iMes=10;
    }else if(strMes.compare("noviembre")==0){
        iMes=11;
    }else if(strMes.compare("diciembre")==0){
        iMes=12;
    }

    return iMes;


}

void FuncAux::funMostrarFoto(QLabel *lblFoto, QString strRuta){
    QImage      foto_img;
    bool        estaCargada = false;

    if(strRuta!=""){
        estaCargada = foto_img.load(strRuta);

        //
        // Miramos si esta cargada si no la cargamos con qImageReader
        //
        if(!estaCargada){
            QImageReader    img_reader;
            double          factor = 0;
            int             pixels_ancho = 0, pixels_alto = 0;
            int             pixels_ancho_nuevo, pixels_alto_nuevo;

            img_reader.setFileName(strRuta);

            pixels_ancho    = img_reader.size().width();
            pixels_alto     = img_reader.size().height();
            factor          = (double) 24000000 / (pixels_ancho * pixels_alto);

            pixels_ancho_nuevo   = (int) pixels_ancho * factor;
            pixels_alto_nuevo    = (int) pixels_alto * factor;

            img_reader.setScaledSize(QSize(pixels_ancho_nuevo, pixels_alto_nuevo));
            foto_img = img_reader.read();
        }

        //
        // Con la foto ya cargada la escalamos y la enseñamos enseñamos
        //
        foto_img = foto_img.scaled(lblFoto->width(), lblFoto->height(), Qt::KeepAspectRatio);

        lblFoto->setAlignment(Qt::AlignCenter);
        lblFoto->setPixmap(QPixmap::fromImage(foto_img));
        lblFoto->show();

    }
    //
    // La ruta esta vacia
    //
    else{
        lblFoto->clear();
    }


}

QDate FuncAux::funStrToDate(QString strFecha){
    QDate datFecha;
    int iY,iM,iD;
    QString strY,strM,strD,str;

    //
    // Si el caracter 3 y el 6  son / formato correcto
    //
    if(strFecha.length() == 10 && strFecha[2] == '/' && strFecha[5] == '/'){
        str=strFecha;
        str.truncate(2);
        strD=str;
        str=strFecha;
        str.truncate(5);
        str.remove(0,3);
        strM=str;
        str=strFecha;
        str.remove(0,(str.length()-4));
        strY=str;
        iY=strY.toInt();
        iM=strM.toInt();
        iD=strD.toInt();
        datFecha.setDate(iY,iM,iD);
    }
    //
    // Formato incorrecto
    //
    else{
        datFecha.setDate(-1,-1,-1);
    }

    return datFecha;

}

QTime FuncAux::funStrToTime(QString strTiempo){
    QTime   tHora;
    QString strHora, strMin, strSeg, str;
    int     iHoras, iMin, iSeg;

    str     = strTiempo;
    str.truncate(2);
    strHora = str;
    str     = strTiempo;
    str.truncate(5);
    str.remove(0,3);
    strMin  = str;
    str     = strTiempo;
    str.remove(0,(str.length()-2));
    strSeg  = str;

    iHoras  = strHora.toInt();
    iMin    = strMin.toInt();
    iSeg    = strSeg.toInt();

    tHora.setHMS(iHoras, iMin, iSeg, 0);

    return tHora;
}

double FuncAux::funPrecioLitroToEmpresa(QString strFecha, QString strCodEmpresa, QString strCombustible){
    QString strRutaDb, strSql, str;
    QString strCodEmpresaDb, strCombustibleDb, strPrecioDb;
    QString strFecha0, strFecha1;
    double  dPrecio = 0.0;
    QDate   datFecha;

    datFecha = funStrToDate(strFecha);
    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql26");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM PrecioCombustible";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {
        QDate datFecha0, datFecha1;

        strFecha0 = funDesCifrar_bis(sql.record().value("FechaInicial").toString());
        strFecha1 = funDesCifrar_bis(sql.record().value("FechaFinal").toString());

        datFecha0 = funStrToDate(strFecha0);
        datFecha1 = funStrToDate(strFecha1);
        //
        // Miramos si coinciden fecha, combustible y empresa
        //
        strCodEmpresaDb     = funDesCifrar_bis(sql.record().value("Empresa").toString());
        strCombustibleDb    = funDesCifrar_bis(sql.record().value("Combustible").toString());

        if(strCodEmpresaDb == strCodEmpresa &&
           strCombustibleDb == strCombustible &&
           datFecha >= datFecha0 &&
           datFecha <=datFecha1){

            strPrecioDb = funDesCifrar_bis(sql.record().value("Precio").toString());
            dPrecio     = strPrecioDb.toDouble();
            sql.finish();
        }

        sql.next();
    }


    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql26");


    return dPrecio;

}

double FuncAux::funPrecioToTarea(QString strTrabajo){
    QString strRutaDb, strSql, str;
    QString strDescripcionDb, strPrecioDb;
    double dPrecio = 0;

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql27");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Mantenimiento";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strDescripcionDb = funDesCifrar_bis(sql.record().value("Descripcion").toString());

        if(strDescripcionDb == strTrabajo){

            strPrecioDb = funDesCifrar_bis(sql.record().value("Precio").toString());
            dPrecio = strPrecioDb.toDouble();
            sql.finish();
        }
        sql.next();
    }


    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql27");

    return dPrecio;
}

QString FuncAux::funFLargaToCorta(QString strFechaLarga){
    QString strFechaCorta, str;
    QString strAny, strMes, strDia;
    int i, iMes;

    str     = strFechaLarga;
    strAny  = str.mid(str.length() - 4, 4);
    str.remove(str.length() - 11, 11);
    i=str.length() - 1;
    while (i>0) {
        if(str[i] == ' '){
            strMes = str.mid(i + 1, str.length() - 1);
            i = 0;
        }
        i--;
    }
    iMes    = funMesesToInt(strMes);
    strMes  = QString::number(iMes);

    if(strMes.length() == 1){
        strMes.prepend("0");
    }
    i=0;
    while (i < str.length() - 1) {
        if(str[i] == ' '){
            strDia = str.mid(i + 4, 2);
            i = str.length();
        }
        i++;
    }

    //
    // Montamos la fecha corta
    //
    strFechaCorta = "";
    strFechaCorta.append(strDia);
    strFechaCorta.append("/");
    strFechaCorta.append(strMes);
    strFechaCorta.append("/");
    strFechaCorta.append(strAny);


    return strFechaCorta;

}

QString FuncAux::funRutaFotoMaquinaToCodigo(QString strCodigo){
    QString strRutaFoto;
    QString strRutaDb, strSql, strCodigoDb;


    //
    // Abrimos la BD
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

        strCodigoDb = funDesCifrar_bis(sql.record().value("Codigo").toString());
        if(strCodigoDb == strCodigo){

            strRutaFoto = funDesCifrar_bis(sql.record().value("Foto").toString());
            sql.finish();
        }
        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");

    return strRutaFoto;
}

QString FuncAux::funRutaFotoEmpleadoToCodigo(QString strCodigo){
    QString strRutaFoto, strCodigoDb;
    QString strRutaDb, strSql;

    FuncAux *pFuncAux = new FuncAux();

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql11");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Empleados";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodigoDb = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());
        if(strCodigoDb == strCodigo){

            strRutaFoto = pFuncAux->funDesCifrar_bis(sql.record().value("Foto").toString());
            sql.finish();
        }
        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql11");

    delete pFuncAux;

    return strRutaFoto;
}

QString FuncAux::funCodigoToMaquina(QString strMaquina){
    QString strRutaDb, strSql;
    QString strCodigo = "-1";
    QString str, strTemp, strApodo, strMarca, strModelo;
    QString strMarcaDb, strModeloDb, strApodoDb;
    int     i;

    str = strMaquina;
    //
    // Sacamos el apodo
    //
    str.remove(str.length() - 2, 2);
    i       = str.length() - 1;
    strTemp = str;

    while (i>=0) {
        if( str[i] == '(' ){
            strTemp.remove(0, i + 1);
            strApodo = strTemp;
            str.remove(i - 1, ( str.length() ));
            i = 0;
        }
        i--;
    }

    //
    // Sacamos el Modelo
    //
    i       = str.length() - 1;
    strTemp = str;

    while (i>=0) {
        if(str[i] == ','){
            strTemp.remove(0, i);
            strTemp.remove(0,2);
            strModelo = strTemp;
            str.remove(i, ( str.length() - i ));
            i = 0;
        }
        i--;
    }

    //
    // Saccamos la marca
    //
    strMarca = str;

    //
    // Abrimos la BD
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql11");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Maquinas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strMarcaDb  = funDesCifrar_bis(sql.record().value("Marca").toString());
        strModeloDb = funDesCifrar_bis(sql.record().value("Modelo").toString());
        strApodoDb  = funDesCifrar_bis(sql.record().value("Apodo").toString());

        if(strMarcaDb == strMarca &&
                strModeloDb == strModelo &&
                strApodoDb == strApodo){

            strCodigo = funDesCifrar_bis(sql.record().value("Codigo").toString());
            sql.finish();
        }
        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql11");

    return strCodigo;
}

QString FuncAux::funCodigoToEmpleado(QString strEmpleadoTxt){
    QString strRutaDb, strSql;
    QString strCodigo;
    QString strNombreDb, strApellidosDb;
    QString strTemp, strNombre, strApellidos;
    int i;

    //
    // Sacamos el Nombre y los apellidos
    //
    i=0;
    strTemp = strEmpleadoTxt;
    while(i <= strTemp.length() - 1 ){
        if(strTemp[i] == ' ' || i == strTemp.length() - 1){
            strNombre       = strTemp.remove(i, strTemp.length() - i );
            strTemp         = strEmpleadoTxt;
            strApellidos    =strTemp.remove(0, i + 1);
            i               = strTemp.length();
        }
        i++;
    }

    //
    // Tenemos nombre y apellidos buscamos el codigo
    //

    //
    // Abrimos la tabla empleados
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql3");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Empleados";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strNombreDb     = funDesCifrar_bis(sql.record().value("Nombre").toString());
        strApellidosDb  = funDesCifrar_bis(sql.record().value("Apellidos").toString());

        if(strNombreDb == strNombre &&
           strApellidosDb == strApellidos){

            strCodigo = funDesCifrar_bis(sql.record().value("Codigo").toString());
        }

        sql.next();
    }

    //
    // cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql3");


    return strCodigo;
}

QString FuncAux::funNombreToCodEmpleado(QString strCodEmpleado){
    QString strRutaDb, strSql, strNombre;
    QString strCodEmpleadoDb;
    //
    // Abrimos la BD
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

        strCodEmpleadoDb = funDesCifrar_bis(sql.record().value("Codigo").toString());

        if(strCodEmpleadoDb == strCodEmpleado){
            strNombre = funDesCifrar_bis(sql.record().value("Nombre").toString());
        }
        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql");


    return strNombre;

}

QString FuncAux::funCodigoToEmpresa(QString strEmpresaTxt){
    QString strRutaDb, strSql;
    QString strCodigo;
    QString strEmpresa, strSeccion, strTemp;
    QString strEmpresaDb, strSeccionDb;
    int i;

    //
    // Sacamos Empresa y seccion
    //
    strTemp = strEmpresaTxt;
    strTemp.remove(strTemp.length() - 1, 1);
    i = strTemp.length() - 1;
    while (i > 0) {
        if(strTemp[i] == '"'){
            strSeccion  = strTemp.remove(0, i + 1);
            strTemp     = strEmpresaTxt;
            strEmpresa  = strTemp.remove(i - 3 , strTemp.length() - i + 3);
            strEmpresa.remove(0,1);
            i = 0;
        }
        i--;
    }

    //
    // Tenemos Empresa y Seccion buscamos el codigo
    //

    //
    // Abrimos la tabla empleados
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql4");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Empresas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strEmpresaDb = funDesCifrar_bis(sql.record().value("Nombre").toString());
        strSeccionDb = funDesCifrar_bis(sql.record().value("Seccion").toString());

        if(strEmpresaDb == strEmpresa &&
           strSeccionDb == strSeccion){

            strCodigo = funDesCifrar_bis(sql.record().value("Codigo").toString());
            sql.finish();
        }

        sql.next();
    }

    //
    // cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql4");


    return strCodigo;
}

QString FuncAux::funTxtToEmpresas(QString strCodigo){
    QString strRutaDb, strSql;
    QString strTxt, strEmpresa, strSeccion, strCodigoDb;

    //
    // Abrimos la tabla Empresas
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql6");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Empresas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodigoDb = funDesCifrar_bis(sql.record().value("Codigo").toString());

        if(strCodigoDb == strCodigo){

            strEmpresa = funDesCifrar_bis(sql.record().value("Nombre").toString());
            strSeccion = funDesCifrar_bis(sql.record().value("Seccion").toString());

            sql.finish();
        }
        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql6");

    strTxt=" ";
    strTxt.append(strEmpresa);
    strTxt.append("   ");
    strTxt.append('"');
    strTxt.append(strSeccion);
    strTxt.append('"');

    return strTxt;
}

QString FuncAux::funTxtToMaquinas(QString strCodigo){
    QString strRutaDb, strSql;
    QString strTxt, strMarca, strModelo, strApodo, strCodigoDb;

    //
    // Abrimos la tabla Empresas
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql7");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Maquinas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodigoDb = funDesCifrar_bis(sql.record().value("Codigo").toString());

        if(strCodigoDb == strCodigo){

            strMarca    = funDesCifrar_bis(sql.record().value("Marca").toString());
            strModelo   = funDesCifrar_bis(sql.record().value("Modelo").toString());
            strApodo    = funDesCifrar_bis(sql.record().value("Apodo").toString());

            sql.finish();
        }
        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql7");

    strTxt="";
    strTxt.append(strMarca);
    strTxt.append(", ");
    strTxt.append(strModelo);
    strTxt.append(" (");
    strTxt.append(strApodo);
    strTxt.append(").");

    return strTxt;

}

QString FuncAux::funTxtToEmpleados(QString strCodigo){
    QString strRutaDb, strSql;
    QString strTxt, strNombre, strApellidos, strCodigoDb;

    //
    // Abrimos la tabla Empresas
    //
    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql8");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Empleados";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodigoDb = funDesCifrar_bis(sql.record().value("Codigo").toString());

        if(strCodigoDb == strCodigo){

            strNombre    = funDesCifrar_bis(sql.record().value("Nombre").toString());
            strApellidos = funDesCifrar_bis(sql.record().value("Apellidos").toString());

            sql.finish();
        }
        sql.next();
    }

    //
    // Cerramos la BD
    //
    dbSql.close();
    dbSql.removeDatabase("conSql8");

    strTxt = "";
    strTxt.append(strNombre);
    strTxt.append(" ");
    strTxt.append(strApellidos);

    return strTxt;


}

QString FuncAux::funHorasToMaquina(QString strCodigo){
    QString strRutaDb, strSql;
    QString strCodigoDb;
    QString strHoras = "";

    FuncAux *pFuncAux = new FuncAux();

    strRutaDb = qApp->applicationDirPath() + "/Data/GesMaq.db";
    QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "conSql2");
    dbSql.setDatabaseName(strRutaDb);
    dbSql.open();
    QSqlQuery sql = QSqlQuery(dbSql);

    strSql = "SELECT *FROM Maquinas";
    sql.exec(strSql);
    sql.first();
    while (sql.isValid()) {

        strCodigoDb = pFuncAux->funDesCifrar_bis(sql.record().value("Codigo").toString());

        if(strCodigoDb == strCodigo){

            strHoras = pFuncAux->funDesCifrar_bis(sql.record().value("Horas").toString());

            sql.finish();
        }
        sql.next();
    }

    dbSql.close();
    dbSql.removeDatabase("conSql2");

    delete pFuncAux;

    return strHoras;
}


