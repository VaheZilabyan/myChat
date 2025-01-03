#ifndef SIGN_IN_H
#define SIGN_IN_H

#include "dbmanager.h"
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLayout>

namespace Ui {
class Sign_in;
}

class Sign_in : public QWidget
{
    Q_OBJECT

public:
    explicit Sign_in(QWidget *parent = nullptr);
    ~Sign_in();

private slots:
    void on_login_clicked();
    void on_back_from_sign_in_clicked();
    void on_sign_up_clicked();

signals:
    void signal_back();
    void signal_sign_up();
    void signal_login_successfully(int);

private:
    //DBManager dbm;

private: //gui
    QLineEdit *lineEdit_login;
    QLineEdit *lineEdit_password;
    QPushButton *back_from_sign_in;
    QPushButton *login;
    QPushButton *sign_up;
};

#endif // SIGN_IN_H
