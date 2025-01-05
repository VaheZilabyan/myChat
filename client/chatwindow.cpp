#include "chatwindow.h"
#include "../LoginSystem/dbmanager.h"

#include <QLayout>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

ChatWindow::ChatWindow(QWidget *parent)
    : QMainWindow(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &ChatWindow::slotSocketReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    this->resize(600, 350);

    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // chat layout
    QVBoxLayout *chatLayout = new QVBoxLayout();
    QHBoxLayout *southLayout = new QHBoxLayout();

    inputLine = new QLineEdit(this);
    sendButton = new QPushButton("Send", this);
    connectButton = new QPushButton("Connect to server", this);
    textWindow = new QTextBrowser(this);

    southLayout->addWidget(inputLine);
    southLayout->addWidget(sendButton);

    chatLayout->addWidget(connectButton);
    chatLayout->addWidget(textWindow);
    chatLayout->addLayout(southLayout);

    // users layout
    QVBoxLayout *usersLayout = new QVBoxLayout();
    findLine = new QLineEdit(this);
    findButton = new QPushButton("Find user", this);
    usersList = new QListWidget(this);
    QHBoxLayout *northLayout = new QHBoxLayout();
    northLayout->addWidget(findLine);
    northLayout->addWidget(findButton);

    usersLayout->addLayout(northLayout);
    usersLayout->addWidget(usersList);

    // mainLayout
    mainLayout->addLayout(usersLayout, 1);
    mainLayout->addLayout(chatLayout, 2);
    setCentralWidget(centralWidget);

    // connections
    connect(findButton, &QPushButton::clicked, this, &ChatWindow::on_findButton_clicked);
    connect(sendButton, &QPushButton::clicked, this, &ChatWindow::on_sendButton_clicked);
    connect(connectButton, &QPushButton::clicked, this, &ChatWindow::on_connectButton_clicked);
    connect(socket, &QTcpSocket::connected, this, []() {
        qDebug() << "Connected to server!";
    });
    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error){
        textWindow->append("Socket error: " + QString::number(error));
        qDebug() << "Socket error:" << error;
    });
    connect(usersList, &QListWidget::itemClicked, this, &ChatWindow::onUserSelected);
}

void ChatWindow::slotSocketReadyRead() {
    QByteArray data = socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isObject()) {
        textWindow->append("Invalid JSON received.");
        return;
    }

    QJsonObject json = doc.object();
    QString type = json["type"].toString();

    if (type == "message") {
        QString sender = json["sender"].toString();
        QString message = json["message"].toString();
        textWindow->append(sender + ": " + message);
    } else if (type == "message_history") {
        textWindow->clear();
        QJsonArray messages = json["messages"].toArray();
        if (messages.isEmpty()) {
            qDebug() << "No messages found!";
            textWindow->append("<i>Empty history...</i>");
        } else {
            for (const auto &msg : messages) {
                QJsonObject message = msg.toObject();
                QString sender = message["sender"].toString();
                QString text = message["message"].toString();
                textWindow->append(sender + ": " + text);
            }
        }
    } else if (type == "connect") {
        QString message = json["message"].toString();
        textWindow->append("<i>" + message + " (username: " + m_username + ") </i>");
    }
}

void ChatWindow::sendToServer(const QString &message)
{
    QJsonObject json;
    json["type"] = "message";
    json["sender"] = m_username;
    json["sender_id"] = m_userId;
    json["receiver_id"] = m_partnerID;
    json["message"] = message;

    QJsonDocument doc(json);
    socket->write(doc.toJson());
    inputLine->clear();
}

void ChatWindow::requestMessageHistory(int partnerId) {
    QJsonObject request;
    request["type"] = "request_message_history";
    request["user_id_1"] = m_userId; // Current user
    request["user_id_2"] = partnerId; // Chat partner

    QJsonDocument doc(request);
    socket->write(doc.toJson());
}

void ChatWindow::onUserSelected(QListWidgetItem *item) {
    m_partnerUsername = item->text();
    int prevClickedID = m_partnerID; // if paymani hamar
    m_partnerID = DBManager::instance()->getIdByUsername(m_partnerUsername);
    qDebug() << "clicked username: " << m_partnerUsername << " " << QString::number(m_partnerID);

    if (prevClickedID != m_partnerID) {
        qDebug() << "request for message history with " << m_partnerID;
        requestMessageHistory(m_partnerID);
    }
}

void ChatWindow::setUsername(const QString &username)
{
    if (!DBManager::instance()->connectToDatabase()) {
        qDebug() << "ChatWindow::setUsername can't connect to Database";
    }
    m_username = username;
    m_userId = DBManager::instance()->getIdByUsername(m_username);
    //qDebug() << "setId -> " << m_userId;
    //qDebug() << "username -> " << m_username;
    setWindowTitle(m_username);
}

void ChatWindow::setId(int id)
{
    if (!DBManager::instance()->connectToDatabase()) {
        qDebug() << "ChatWindow::setId can't connect to Database";
    }
    m_userId = id;
    m_username = DBManager::instance()->getUsernameById(id);
    setWindowTitle(m_username);
}

void ChatWindow::on_connectButton_clicked() {
    socket->connectToHost("127.0.0.1", 5555);

    // for adding clients to QMap
    QJsonObject addClient;
    addClient["type"] = "addClient";
    addClient["sender"] = m_username;
    QJsonDocument doc(addClient);
    socket->write(doc.toJson());
}

void ChatWindow::on_sendButton_clicked() {
    sendToServer(inputLine->text());
}

void ChatWindow::on_findButton_clicked() {
    QString input_username = findLine->text();
    if (DBManager::instance()->usernameExists(input_username)) {
        usersList->addItem(input_username);
        findLine->clear();
    } else {
        QMessageBox::information(0, "Wrong Username", "Input username again");
    }
}

ChatWindow::~ChatWindow() {}
