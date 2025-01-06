#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include "sign_in.h"
#include "sign_up.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

public slots:
    void slot_login_successfully(const QString&);
signals:
    void signal_login_successfully(const QString&);

private slots:
    void slot_back();
    void on_sign_in_clicked();
    void on_exit_clicked();
    void on_sign_up_clicked();

private:
    Sign_in *sign_in_window;
    Sign_up *sign_up_window;
    const int width = 210;
    const int height = 260;

private: //gui
    QStackedWidget *stackedWidget;
    QPushButton *sign_in_button;
    QPushButton *sign_up_button;
    QPushButton *exit_button;
};

#endif // LOGINWINDOW_H
