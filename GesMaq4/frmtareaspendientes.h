#ifndef FRMTAREASPENDIENTES_H
#define FRMTAREASPENDIENTES_H

#include <QDialog>
#include <QTableWidget>


namespace Ui {
class frmTareasPendientes;
}

class frmTareasPendientes : public QDialog
{
    Q_OBJECT

public:
    explicit frmTareasPendientes(QWidget *parent = nullptr);
    ~frmTareasPendientes();


    //
    // Variables Globales
    //
    QString GstrTitulo;
    int     Gfila_seleccionada = -1;

    void    funInicio(void);
    void    funRellenaListado(void);


protected:

    bool eventFilter(QObject *obj, QEvent *event);



private slots:
    void on_cmdVolver_clicked();

private:
    Ui::frmTareasPendientes *ui;
};

#endif // FRMTAREASPENDIENTES_H
