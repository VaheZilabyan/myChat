#include "chatwindow.h"

#include "../LoginSystem/loginwindow.h"
#include "../LoginSystem/dbmanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LoginWindow loginWindow;
    ChatWindow chatWindow;

    loginWindow.show();
    QObject::connect(&loginWindow, &LoginWindow::signal_login_successfully, [&](int id) {
        loginWindow.hide();

        chatWindow.setId(id);
        chatWindow.show();
    });

    /*DBManager db;
    if (!db.connectToDatabase()) {
        qDebug() << "DB not connected";
        return -1;
    }

    ChatWindow chat;
    chat.show();*/

    return a.exec();
}
