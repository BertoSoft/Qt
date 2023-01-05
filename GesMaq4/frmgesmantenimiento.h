#ifndef FRMGESMANTENIMIENTO_H
#define FRMGESMANTENIMIENTO_H

#include <QDialog>
#include <QTableWidgetItem>

namespace Ui {
class frmGesMantenimiento;
}

class frmGesMantenimiento : public QDialog
{
    Q_OBJECT

public:
    explicit frmGesMantenimiento(QWidget *parent = nullptr);
    ~frmGesMantenimiento();

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
    QString funHorasMaxToTarea(QString strTarea);
    QString funHorasTarea(QString strTarea);


protected:

    bool eventFilter(QObject *obj, QEvent *event);



private slots:
    void on_cmdVolver_clicked();

    void on_tbAgregar_clicked();

    void on_cmbMaquina_activated(int index);

    void on_cmbTarea_activated(int index);

    void on_tbTarea_clicked();

    void on_cmdGuardar_clicked();

    void on_tlMantenimiento_itemClicked(QTableWidgetItem *item);

    void on_tbEliminar_clicked();

    void on_tlMantenimiento_itemDoubleClicked(QTableWidgetItem *item);

    void on_cmdRealizado_clicked();

    void on_txtMaxHoras_textChanged(const QString &arg1);

    void on_txtMaxHoras_returnPressed();

private:
    Ui::frmGesMantenimiento *ui;
};

#endif // FRMGESMANTENIMIENTO_H
