#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>

class ChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();

public slots:
    void slotSocketReadyRead();
    void on_connectButton_clicked();
    void on_sendButton_clicked();

private:
    void sendToServer(QString message);

private:
    QTcpSocket *socket;
    QByteArray data;
    quint16 nextBlockSize{0};
private:
    QTextBrowser *textWindow;
    QLineEdit *inputLine;
    QPushButton *sendButton;
    QPushButton *connectButton;
};
#endif // CHATWINDOW_H
