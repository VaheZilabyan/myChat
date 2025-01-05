#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class server : public QTcpServer
{
    Q_OBJECT
public:
    explicit server();
    QTcpSocket *socket;

public slots:
    void incomingConnection(qintptr socketDescriptor) override; // Handle new connections
    void slotSocketReadyRead();   // Handle incoming data
    void slotSocketDisconnect();  // Handle disconnections

private:
    void startServer(); // Start the server
    void sendToClient(QTcpSocket* client, const QJsonObject &message); // Send data to a specific client
    void broadcastToClients(const QJsonObject &message); // Broadcast data to all clients
    int authenticateUser(QTcpSocket *socket);
private:
    QMap<int, QTcpSocket*> clients; // id, socket
};

#endif // SERVER_H
