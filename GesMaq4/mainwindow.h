#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //
    // Variables Globales
    //
    QString GstrUsuarioActivo;
    QString GstrHoraInicioSesion;
    QString GstrFechaInicioSesion;
    bool    bEstaCargado    = false;
    bool    bPrimerPaso     = true;
    QString strTitulo       = "Gestión de Maquinaria V 4.0";
    QString strTexto;
    QLabel  *lblTexto       = new QLabel();
    QLabel  *lblHora        = new QLabel();
    QLabel  *lblFecha       = new QLabel();

    //
    // Funciones Públicas
    //
    void    funInicio();
    void    funSetlblTexto();
    void    funRefrescaReloj();
    QString funCrearBd(QString strRuta);
    void    funActivaActions();


protected:

    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_actionSalir_triggered();

    void on_actionImportar_Datos_triggered();

    void on_actionExportar_Datos_triggered();

    void on_actionRegistro_de_Empleados_triggered();

    void on_actionRegistro_de_Centros_de_Trabajo_triggered();

    void on_actionRegistro_de_M_quinas_triggered();

    void on_actionRegistro_de_Operaciones_de_Mantenimiento_triggered();

    void on_actionRegistro_de_Precios_Combustible_triggered();

    void on_actionRegistro_de_Tarifas_de_las_M_quina_triggered();

    void on_actionGesti_n_Diaria_de_Partes_de_Trabajo_triggered();

    void on_actionRepostajes_triggered();

    void on_actionMantenimiento_triggered();

    void on_actionAver_as_triggered();

    void on_actionListado_de_Horas_Por_Empleado_triggered();

    void on_actionListado_de_Horas_Por_Maquina_triggered();

    void on_actionListado_Horas_Por_Centro_de_Trabajo_triggered();

    void on_actionListado_de_Gastos_Por_Maquina_triggered();

    void on_actionBuzn_de_Tareas_Pendientes_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
