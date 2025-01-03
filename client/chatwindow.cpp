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
        QString text = json["message"].toString();
        textWindow->append(sender + ": " + text);
    } else if (type == "message_history") {
        QJsonArray messages = json["messages"].toArray();
        for (const auto &msg : messages) {
            QJsonObject message = msg.toObject();
            QString sender = message["sender"].toString();
            QString text = message["message"].toString();
            textWindow->append(sender + ": " + text);
        }
    } else if (type == "info") {
        // use italic format
        //
    } else if (type == "login_success") {

    }
}

void ChatWindow::sendToServer(const QString &message)
{
    QJsonObject json;
    json["type"] = "message";
    json["sender"] = m_username;
    json["sender_id"] = m_userId;
    json["receiver_id"] = clicked_usernameID;
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
    clicked_username = item->text();
    int currentClickedID = clicked_usernameID; // if paymani hamar
    clicked_usernameID = DBManager::instance()->getIdByUsername(clicked_username);
    qDebug() << "clicked username: " << clicked_username << " " << QString::number(clicked_usernameID);

    if (currentClickedID != clicked_usernameID) {
        requestMessageHistory(clicked_usernameID);
    }
}

void ChatWindow::setUsername(const QString &username)
{
    m_username = username;
    m_userId = DBManager::instance()->getIdByUsername(m_username);
    qDebug() << "in Client code " << m_userId << " " << m_username;
    setWindowTitle(m_username);
}

void ChatWindow::setId(int id)
{
    if (!DBManager::instance()->connectToDatabase()) {
        qDebug() << "database not open in chatwindow";
    }
    m_userId = id;
    qDebug() << "setId -> " << id;
    m_username = DBManager::instance()->getUsernameById(id);
    qDebug() << "setId username -> " << m_username;
    setWindowTitle(m_username);
}

void ChatWindow::on_connectButton_clicked() {
    socket->connectToHost("127.0.0.1", 5555);
    qDebug() << "Connect button clicked";

    QJsonObject loginRequest;
    loginRequest["type"] = "login";
    loginRequest["sender"] = m_username;
    QJsonDocument doc(loginRequest);
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
