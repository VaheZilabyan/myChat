#include "sign_in.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QLabel>
#include <QFile>

Sign_in::Sign_in(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QLabel *label_sign_in = new QLabel("Sign in");
    QFont font = label_sign_in->font();
    font.setBold(true);
    font.setPointSize(25);
    label_sign_in->setFont(font);
    label_sign_in->setAlignment(Qt::AlignCenter);

    // grid_layout
    QGridLayout *grid_layout = new QGridLayout();
    QLabel *label_username = new QLabel("Username");
    QLabel *label_password = new QLabel("Password");
    label_username->setAlignment(Qt::AlignRight);
    label_password->setAlignment(Qt::AlignRight);
    lineEdit_login = new QLineEdit(this);
    lineEdit_password = new QLineEdit(this);
    lineEdit_password->setEchoMode(QLineEdit::Password);

    grid_layout->addWidget(label_username, 0, 0);
    grid_layout->addWidget(lineEdit_login, 0, 1);
    grid_layout->addWidget(label_password, 1, 0);
    grid_layout->addWidget(lineEdit_password, 1, 1);

    // login_layout
    QHBoxLayout *login_layout = new QHBoxLayout();
    back_from_sign_in = new QPushButton("Back");
    login = new QPushButton("Login");
    login_layout->addStretch();
    login_layout->addWidget(back_from_sign_in);
    login_layout->addWidget(login);

    // sign up layout
    QHBoxLayout *sign_up_layout = new QHBoxLayout();
    QLabel *label_sign_up = new QLabel("Don't have account, sign up ");
    sign_up = new QPushButton("Sign Up");
    sign_up_layout->addWidget(label_sign_up);
    sign_up_layout->addWidget(sign_up);

    mainLayout->addWidget(label_sign_in);
    mainLayout->addLayout(grid_layout);
    mainLayout->addLayout(login_layout);
    mainLayout->addStretch();
    mainLayout->addLayout(sign_up_layout);

    //signals and slots
    connect(login, &QPushButton::clicked, this, &Sign_in::on_login_clicked);
    connect(back_from_sign_in, &QPushButton::clicked, this, &Sign_in::on_back_from_sign_in_clicked);
    connect(sign_up, &QPushButton::clicked, this, &Sign_in::on_sign_up_clicked);
}

void Sign_in::on_login_clicked()
{
    if (dbm.connectToDatabase() && dbm.getDatabase().tables().size() > 0) {
        QSqlQuery *query = new QSqlQuery(dbm.getDatabase());
        if(query->exec("select * from User where Username='" + lineEdit_login->text()+ "' and Password='" + dbm.hashPassword(lineEdit_password->text()) + "'")) {
            // counti 1-i depqum e petq mutq gorcel
            int count = 0;
            QString name = "";
            QString surname = "";
            while (query->next()) {
                ++count;
                name = query->value(2).toString();
                surname = query->value(3).toString();
            }
            if (count == 1) {
                qDebug() << "USER FINDED" << '\n';
                QMessageBox::information(0, "You Log in", "You login successfully, " + name + " " + surname);
                //lineEdit_login->clear();
                //lineEdit_password->clear();

                emit signal_login_successfully(lineEdit_login->text());
            } else {
                QMessageBox::information(0, "Error", "Wrong login or password...");
            }
        }
        dbm.close();
    } else {
        qDebug() << "error when open...\n";
        QMessageBox::information(0, "Error", "Database is empty");
    }
}


void Sign_in::on_back_from_sign_in_clicked()
{
    emit signal_back();
}


void Sign_in::on_sign_up_clicked()
{
    emit signal_sign_up();
}

Sign_in::~Sign_in()
{
}

