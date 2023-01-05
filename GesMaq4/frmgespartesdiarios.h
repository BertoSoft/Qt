#ifndef FRMGESPARTESDIARIOS_H
#define FRMGESPARTESDIARIOS_H

#include <QDialog>
#include <QTableWidgetItem>


namespace Ui {
class frmGesPartesDiarios;
}

class frmGesPartesDiarios : public QDialog
{
    Q_OBJECT

public:
    explicit frmGesPartesDiarios(QWidget *parent = nullptr);
    ~frmGesPartesDiarios();

    //
    // Variables Globales
    //
    QString GstrTitulo;
    QString GstrUltimoEmpleado;
    QString GstrUltimaFecha;
    int     Gfila_seleccionada = -1;
    bool    Gprimer_paso = true;

    //
    // estructuras
    //
    struct s_HorasMaquina{
        QString strFechaCod;
        QString strCodEmpleadoCod;
        QString strCodMaquina;
        double  dHoras;
    };


    //
    // Funciones
    //
    void funInicio();
    void funRellenaListado(QString strFecha, QString strEmpleado);
    void funGrabarHorasMaquina(QString strCodigo, QString strHoras);
    void funProcesaFecha();
    bool funParteGuardado(QString strFecha, QString strCodEmpleado);


protected:

    bool eventFilter(QObject *obj, QEvent *event);




private slots:
    void on_cmdVolver_clicked();

    void on_cmdGuardar_clicked();

    void on_tbFecha_clicked();

    void on_cmbEmpleado_currentTextChanged(const QString &arg1);

    void on_cmbMaquina_currentTextChanged(const QString &arg1);

    void on_cmbEmpresa_currentTextChanged(const QString &arg1);

    void on_txtDescripcion_returnPressed();

    void on_cmdAnadir_clicked();

    void on_txtDuracion_returnPressed();

    void on_cmdSuprimir_clicked();

    void on_txtDuracion_textChanged(const QString &arg1);

    void on_chkFacturable_stateChanged(int arg1);

    void on_tlPartes_itemClicked(QTableWidgetItem *item);

    void on_tlPartes_itemDoubleClicked(QTableWidgetItem *item);

    void on_cmbEmpleado_activated(int index);

    void on_txtFecha_returnPressed();

    void on_lblCalendario_clicked(const QDate &date);

    void on_cmbMaquina_activated(int index);

private:
    Ui::frmGesPartesDiarios *ui;
};

#endif // FRMGESPARTESDIARIOS_H
