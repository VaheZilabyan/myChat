#include "server.h"
#include "../LoginSystem/dbmanager.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QSqlQuery>

server::server() {
    startServer();
}

void server::startServer()
{
    if (this->listen(QHostAddress::Any, 5555)) {
        qDebug() << "Server started";
    } else {
        qDebug() << "Server not started, error...";
    }
}

void server::slotSocketDisconnect() {
    socket = (QTcpSocket*)sender();
    //sockets.erase(std::remove(sockets.begin(), sockets.end(), socket), sockets.end());
    socket->deleteLater();
    qDebug() << "Client disconnected and socket removed.";
}

void server::incomingConnection(qintptr socketDescriptor) {
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &server::slotSocketReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &server::slotSocketDisconnect);

    qDebug() << socketDescriptor << ": Client connected";
    //sockets.push_back(socket);

    // Send welcome message
    QJsonObject message;
    message["type"] = "info";
    message["message"] = "Client connected!";
    sendToClient(socket, message);
}

void server::slotSocketReadyRead()
{
    socket = (QTcpSocket*)sender();
    QByteArray data = socket->readAll();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qDebug() << "Invalid JSON format received.";
        return;
    }

    QJsonObject json = doc.object();
    QString type = json["type"].toString();

    if (type == "login") {
        if (!DBManager::instance()->connectToDatabase()) qDebug() << "error database in readyred";
        QString username = json["sender"].toString();
        QSqlQuery query;
        query.prepare("SELECT id FROM Users WHERE Username = :username");
        query.bindValue(":username", username);

        if (!query.exec() || !query.next()) {
            qDebug() << "User not found:" << username;
            socket->disconnectFromHost(); // Close connection for invalid user
            return;
        }

        int userId = query.value(0).toInt(); // Get user ID
        clients[userId] = socket; // Map user ID to the socket
        qDebug() << "User ID" << userId << "authenticated and added to clients map.";

        // Send success response
        QJsonObject response;
        response["type"] = "login_success";
        response["user_id"] = userId;

        QJsonDocument responseDoc(response);
        socket->write(responseDoc.toJson());
        return;
    }

    if (type == "message") {
        QString sender = json["sender"].toString();
        int senderId = json["sender_id"].toInt();
        int receiverId = json["receiver_id"].toInt();
        QString message = json["message"].toString();
        qDebug() << "Message from" << sender << " to " << receiverId << ", message: " << message;
        // Store the message in the database
        if (!DBManager::instance()->storeMessage(senderId, receiverId, message)) {
            qDebug() << "Failed to store message in DB!";
        }
        // Forward the message to the receiver if online
        if (clients.contains(receiverId)) {
            QTcpSocket *receiverSocket = clients[receiverId]; // Find receiver
            QJsonObject response;
            response["type"] = "message";
            response["sender"] = sender;
            response["sender_id"] = senderId;
            response["message"] = message;

            QJsonDocument doc(response);
            receiverSocket->write(doc.toJson());
        } else {
            qDebug() << "PARTNER OFFLINE";
        }
        socket->write(doc.toJson());    //add to sender for update textBrowser (partner was offline)

    } else if (type == "request_message_history") {
        int userId1 = json["user_id_1"].toInt();
        int userId2 = json["user_id_2"].toInt();
        if (!DBManager::instance()->connectToDatabase()) {
            qDebug() << "server::slotSocketReadyRead not connection";
        }
        QList<QVariantMap> messages = DBManager::instance()->fetchMessages(userId1, userId2);

        QJsonArray jsonMessages;
        for (const auto &msg : messages) {
            QJsonObject jsonMsg;
            jsonMsg["sender_id"] = msg["sender_id"].toInt();
            jsonMsg["sender"] = DBManager::instance()->getUsernameById(msg["sender_id"].toInt());
            jsonMsg["receiver_id"] = msg["receiver_id"].toInt();
            jsonMsg["message"] = msg["message"].toString();
            jsonMsg["timestamp"] = msg["timestamp"].toDateTime().toString(Qt::ISODate);
            jsonMessages.append(jsonMsg);
        }

        QJsonObject response;
        response["type"] = "message_history";
        response["messages"] = jsonMessages;

        QJsonDocument doc(response);
        socket->write(doc.toJson());
    }
}

void server::sendToClient(QTcpSocket* client, const QJsonObject &message)
{
    QJsonDocument doc(message);
    QByteArray data = doc.toJson();
    client->write(data);
}

void server::broadcastToClients(const QJsonObject &message)
{
    QJsonDocument doc(message);
    QByteArray data = doc.toJson();

    //for (auto &sock : sockets) {
    for (auto &sock : clients) {
        sock->write(data);
    }
}
