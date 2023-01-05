#ifndef FRMCALENDARIO_H
#define FRMCALENDARIO_H

#include <QDialog>

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class frmCalendario;
}

class frmCalendario : public QDialog
{
    Q_OBJECT

public:
    explicit frmCalendario(QWidget *parent = nullptr);
    ~frmCalendario();

    QString strFecha;



private slots:
    void on_cmdVolver_clicked();

    void on_cmdGuardar_clicked();

    void on_calCalendario_activated(const QDate &date);

private:
    Ui::frmCalendario *ui;
};

#endif // FRMCALENDARIO_H
