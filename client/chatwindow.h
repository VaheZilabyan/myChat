#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include "../LoginSystem/dbmanager.h"

#include <QMainWindow>
#include <QListWidget>
#include <QTcpSocket>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>

class ChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    ChatWindow(QWidget *parent = nullptr);
    void setUsername(const QString& username);
    void setId(int id);
    void requestMessageHistory(int partnerId);
    ~ChatWindow();

public slots:
    void slotSocketReadyRead();
private slots:
    void on_findButton_clicked();
    void on_connectButton_clicked();
    void on_sendButton_clicked();
    void on_aboutButton_clicked();
    void onUserSelected(QListWidgetItem *item);

private:
    void sendToServer(const QString& message);

private:
    QTcpSocket *socket;
    QString m_username;
    int m_userId;
    QString m_partnerUsername;
    int m_partnerID{0};
private:
    QTextBrowser *textWindow;
    QLineEdit *inputLine;
    QPushButton *sendButton;
    QPushButton *connectButton;
    QPushButton *aboutButton;
    QListWidget *usersList;
    QLineEdit *findLine;
    QPushButton *findButton;
};
#endif // CHATWINDOW_H
