#ifndef FRMGESAVERIAS_H
#define FRMGESAVERIAS_H

#include <QDialog>
#include <QTableWidgetItem>


namespace Ui {
class frmGesAverias;
}

class frmGesAverias : public QDialog
{
    Q_OBJECT

public:
    explicit frmGesAverias(QWidget *parent = nullptr);
    ~frmGesAverias();


    //
    // Variables Globales
    //
    QString GstrTitulo;
    int     Gfila_seleccionada = -1;


    void    funInicio(void);
    void    funRellenaListado(void);
    void    funRellenaCombos();
    void    funHabilitaControles();
    void    funDeshabilitaControles();

protected:

    bool eventFilter(QObject *obj, QEvent *event);


private slots:
    void on_cmdVolver_clicked();

    void on_cmbMaquina_activated(int index);

    void on_txtFecha_returnPressed();

    void on_txtDescripcion_returnPressed();

    void on_txtPrecio_returnPressed();

    void on_txtHoras_returnPressed();

    void on_cmdGuardar_clicked();

    void on_tbAgregar_clicked();

    void on_tbEliminar_clicked();

    void on_tbFecha_clicked();

    void on_tlAverias_itemClicked(QTableWidgetItem *item);

    void on_txtPrecio_textChanged(const QString &arg1);


    void on_txtHoras_textChanged(const QString &arg1);

    void on_tlAverias_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::frmGesAverias *ui;
};

#endif // FRMGESAVERIAS_H
