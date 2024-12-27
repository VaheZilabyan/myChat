#include "chatwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChatWindow chat;
    chat.show();
    return a.exec();
}
