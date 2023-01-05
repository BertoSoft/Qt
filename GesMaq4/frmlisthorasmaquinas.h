#ifndef FRMLISTHORASMAQUINAS_H
#define FRMLISTHORASMAQUINAS_H

#include <QDialog>

namespace Ui {
class frmListHorasMaquinas;
}

class frmListHorasMaquinas : public QDialog
{
    Q_OBJECT

public:
    explicit frmListHorasMaquinas(QWidget *parent = nullptr);
    ~frmListHorasMaquinas();


    //
    // Variables Globales
    //
    QString GstrTitulo;
    int     Gfila_seleccionada = -1;

    void    funInicio(void);
    void    funRellenaListado(void);
    void    funProcesaFecha0();
    void    funProcesaFecha1();


protected:

    bool eventFilter(QObject *obj, QEvent *event);



private slots:
    void on_cmdVolver_clicked();

    void on_tbFecha0_clicked();

    void on_tbFecha1_clicked();

    void on_cmbMaquina_activated(int index);

    void on_txtFecha0_returnPressed();

    void on_txtFecha1_returnPressed();

private:
    Ui::frmListHorasMaquinas *ui;
};

#endif // FRMLISTHORASMAQUINAS_H
