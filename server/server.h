#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>

class server : public QTcpServer
{
    Q_OBJECT
public:
    QTcpSocket *socket;
    server();

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotSocketReadyRead();
    void slotSocketDisconnect();

private:
    void startServer();
    void SendToClient(QString message);
private:
    QVector<QTcpSocket*> sockets;
    QByteArray data;
    quint16 nextBlockSize{0};
};

#endif // SERVER_H
