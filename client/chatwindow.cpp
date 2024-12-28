#include "chatwindow.h"

#include <QLayout>
#include <QDebug>

ChatWindow::ChatWindow(QWidget *parent)
    : QMainWindow(parent)
{
    nextBlockSize = 0;
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &ChatWindow::slotSocketReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    this->resize(400, 350);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QWidget *southWidget = new QWidget(this);
    QHBoxLayout *southLayout = new QHBoxLayout(southWidget);

    inputLine = new QLineEdit(southWidget);
    sendButton = new QPushButton("Send", southWidget);
    connectButton = new QPushButton("Connect to server", southWidget);
    textWindow = new QTextBrowser(this);

    southLayout->addWidget(inputLine);
    southLayout->addWidget(sendButton);

    mainLayout->addWidget(connectButton);
    mainLayout->addWidget(textWindow);
    mainLayout->addWidget(southWidget);

    setCentralWidget(centralWidget);

    connect(sendButton, &QPushButton::clicked, this, &ChatWindow::on_sendButton_clicked);
    connect(connectButton, &QPushButton::clicked, this, &ChatWindow::on_connectButton_clicked);
    connect(socket, &QTcpSocket::connected, this, []() {
        qDebug() << "Connected to server!";
    });

    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error){
        textWindow->append("Socket error: " + QString::number(error));
        qDebug() << "Socket error:" << error;
    });
}

void ChatWindow::setUsername(const QString &username)
{
    m_username = username;
    setWindowTitle(m_username);
}

void ChatWindow::slotSocketReadyRead() {
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_6);
    if (in.status() == QDataStream::Ok) {
        /*QString str;
        in >> str;
        textWindow->append(str);*/

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
            textWindow->append(str);
            break;
        }

    } else {
        qDebug() << "DataStream error...";
        textWindow->append("read error...");
    }
}

void ChatWindow::sendToServer(QString str)
{
    str += "$" + m_username;

    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_6);

    out << quint16(0) << str;
    out.device()->seek(0);
    out << quint16(data.size() - sizeof(quint16));

    socket->write(data);
    inputLine->clear();
}

void ChatWindow::on_connectButton_clicked() {
    socket->connectToHost("127.0.0.1", 5555);
    qDebug() << "Connect button clicked";
}

void ChatWindow::on_sendButton_clicked() {
    sendToServer(inputLine->text());
}

ChatWindow::~ChatWindow() {}
