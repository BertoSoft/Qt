#ifndef FUNCAUX_H
#define FUNCAUX_H

#include <QString>
#include <QDate>
#include <QLabel>


class FuncAux
{
public:
    FuncAux();

    //
    // Variables Globales
    //
    QString     GstrUsuario;
    QString     GstrHoaraInicio, GstrFechaInicio;

    //
    // Funciones
    //
    bool    funExisteUsuario(void);
    QString funAddUsuario(QString strUsuario, QString strPasswd);
    bool    funComprobarUsuarioPasswd(QString strUsuario, QString strPasswd);
    void    funSetUsuarioActivo(QString strUsuario);
    QString funGetUsuarioActivo();
    void    funCierreSesion();
    QString funCifrar(QString strTextoPlano);
    QString funDesCifrar(QString strTextoCod);
    QString funCifrar_bis(QString strTextoPlano);
    QString funDesCifrar_bis(QString strTextoCod);
    bool    funEsMultiplo64(int i);
    QDate   funStrToDate(QString str);
    QTime   funStrToTime(QString str);
    QString funFLargaToCorta(QString strFechaLarga);
    QString funCodigoToMaquina(QString strMaquina);
    QString funCodigoToEmpresa(QString strEmpresaTxt);
    QString funCodigoToEmpleado(QString strEmpleadoTxt);
    QString funNombreToCodEmpleado(QString strCodEmpleado);
    void    funMostrarFoto(QLabel *lblFoto, QString strRuta);
    QString funTxtToEmpresas(QString strCodigo);
    QString funTxtToMaquinas(QString strCodigo);
    QString funTxtToEmpleados(QString strCodigo);
    QString funRutaFotoMaquinaToCodigo(QString strCodigo);
    QString funRutaFotoEmpleadoToCodigo(QString strCodigo);
    QString funHorasToMaquina(QString strCodigo);
    QString funNuevoCodigo(QString strTabla);
    int     funMesesToInt(QString strMes);
    QString funTarifaToMaquina(QString strCodigoEmpresa, QString strCodMaquina);
    double  funPrecioLitroToEmpresa(QString strFecha, QString strCodEmpresa, QString strCombustible);
    double  funPrecioToTarea(QString strTrabajo);
    bool    funHayAvisos();

};

#endif // FUNCAUX_H
