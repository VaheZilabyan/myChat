#include "chatwindow.h"

#include "../LoginSystem/loginwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LoginWindow loginWindow;
    ChatWindow chatWindow;

    loginWindow.show();
    QObject::connect(&loginWindow, &LoginWindow::signal_login_successfully, [&](const QString &username) {
        loginWindow.hide();

        chatWindow.setUsername(username);
        chatWindow.show();
    });

    return a.exec();
}
