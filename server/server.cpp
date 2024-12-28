#include "server.h"
#include <QDateTime>

server::server() {
    startServer();
    nextBlockSize = 0;
}

void server::startServer()
{
    if (this->listen(QHostAddress::Any, 5555)) {
        qDebug() << "server started";
    } else {
        qDebug() << "server not started, error...";
    }
}

void server::slotSocketDisconnect() {
    socket = (QTcpSocket*)sender();
    sockets.erase(std::remove(sockets.begin(), sockets.end(), socket), sockets.end());
    socket->deleteLater();
    qDebug() << "Client disconnected and socket removed.";
}

void server::incomingConnection(qintptr socketDescriptor) {
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &server::slotSocketReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &server::slotSocketDisconnect);

    qDebug() << socketDescriptor << ": Client connected";
    sockets.push_back(socket);

    // send to connected client
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_6);

    out << quint16(0); // Placeholder for size
    out << "Client(" + QString::number(socketDescriptor) + "): connected!!!";

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    socket->write(block); // Send the framed message
}

void server::slotSocketReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_6);
    if (in.status() == QDataStream::Ok) {
        qDebug() << "read...";
        /*QString str;
        in >> str;
        qDebug() << "<data>: " << str;
        SendToClient(str);*/

        while(true) {
            if (nextBlockSize == 0) {
                qDebug() << "nextBlockSize = 0";
                if (socket->bytesAvailable() < 2) {
                    qDebug() << "Data < 2, break";
                    break;
                }
                in >> nextBlockSize;
                qDebug() << "nextBlockSize = " << nextBlockSize;
            }
            if (socket->bytesAvailable() < nextBlockSize) {
                qDebug() << "Data not full, break";
                break;
            }
            QString str;
            in >> str;
            nextBlockSize = 0;
            qDebug() << "data: " + str;
            SendToClient(str);
            break;
        }

    } else {
        qDebug() << "DataStream error...";
    }
}

void server::SendToClient(QString message)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_6);

    QStringList data_list = message.split('$');
    QString str = "<" + QDateTime::currentDateTime().toString() + ", " + data_list[1] + "> " + data_list[0];
    //out << str;

    out << quint16(0);
    out << str;
    out.device()->seek(0);
    out << quint16(data.size() - sizeof(quint16));

    for (auto &sock : sockets) {
        sock->write(data);
    }
}


