#ifndef FRMREGPRECIOSCOMBUSTIBLE_H
#define FRMREGPRECIOSCOMBUSTIBLE_H

#include <QDialog>
#include <QTableWidgetItem>

namespace Ui {
class frmRegPreciosCombustible;
}

class frmRegPreciosCombustible : public QDialog
{
    Q_OBJECT

public:
    explicit frmRegPreciosCombustible(QWidget *parent = nullptr);
    ~frmRegPreciosCombustible();

    //
    // Variables Globales
    //
    QString GstrRutaFoto = "";
    QString GstrTitulo;
    int     fila_seleccionada = -1;

    void    funInicio(void);
    void    funRellenaListado(void);
    void    funLimpiaControles(void);
    void    funHabilitaControles(void);
    void    funDesHabilitaControles(void);
    QString funExistePeriodo(QString strFechaInicial, QString strFechaFinal);
    bool    funExisteRegistro(QString strCodigo);
    void    funProcesaFecha0();
    void    funProcesaFecha1();


protected:

    bool eventFilter(QObject *obj, QEvent *event);



private slots:


    void on_cmdVolver_clicked();

    void on_tbAgregar_clicked();

    void on_cmdGuardar_clicked();

    void on_txtPrecio_returnPressed();

    void on_txtPrecio_textChanged(const QString &arg1);

    void on_cmbCentroTrabajo_activated(int index);

    void on_cmbCombustible_activated(int index);

    void on_txtFecha0_returnPressed();

    void on_txtFecha1_returnPressed();

    void on_tbFecha0_clicked();

    void on_tbFecha1_clicked();

    void on_tlCombustible_itemClicked(QTableWidgetItem *item);

    void on_tlCombustible_itemDoubleClicked(QTableWidgetItem *item);

    void on_tbEliminar_clicked();

private:
    Ui::frmRegPreciosCombustible *ui;
};

#endif // FRMREGPRECIOSCOMBUSTIBLE_H
