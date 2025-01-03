#include "dbmanager.h"

#include <QCryptographicHash>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

DBManager* DBManager::instance() {
    static DBManager instance;
    return &instance;
}

DBManager::DBManager() {
    qDebug() << "DBManager created.";
}

DBManager::~DBManager() {
    close();
}

QString DBManager::hashPassword(const QString& password) {
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

bool DBManager::connectToDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../data.db");
    if (!db.open()) {
        qDebug() << "DBManager::connectToDatabase() Failed to connect to database...";
        return false;
    }
    qDebug() << "DBManager::connectToDatabase() Connected to database!";
    return true;
}

bool DBManager::storeMessage(int senderId, int receiverId, const QString &message) {
    QSqlQuery query;
    query.prepare("INSERT INTO messages (sender_id, receiver_id, message) "
                  "VALUES (:sender, :receiver, :message)");
    query.bindValue(":sender", senderId);
    query.bindValue(":receiver", receiverId);
    query.bindValue(":message", message);

    if (!db.isOpen()) {
        qDebug() << "DBManager::storeMessage Database not open!";
        return false;
    }

    if (!query.exec()) {
        qDebug() << "DBManager::storeMessage Error inserting message";
        qDebug() << query.lastError().text();
        return false;
    }
    return true;
}

QList<QVariantMap> DBManager::fetchMessages(int userId1, int userId2) {
    QSqlQuery query;
    query.prepare("SELECT sender_id, receiver_id, message, timestamp FROM messages "
                  "WHERE (sender_id = :user1 AND receiver_id = :user2) "
                  "   OR (sender_id = :user2 AND receiver_id = :user1) "
                  "ORDER BY timestamp ASC");
    query.bindValue(":user1", userId1);
    query.bindValue(":user2", userId2);

    QList<QVariantMap> messages;

    if (!db.isOpen()) {
        qDebug() << "DBManager::fetchMessages Database not open!";
        return messages;
    }
    if (!query.exec()) {
        qDebug() << "DBManager::fetchMessages Error fetching messages";
        qDebug() << query.lastError().text();
        return messages;
    }

    while (query.next()) {
        QVariantMap msg;
        msg["sender_id"] = query.value(0).toInt();
        msg["receiver_id"] = query.value(1).toInt();
        msg["message"] = query.value(2).toString();
        msg["timestamp"] = query.value(3).toDateTime();
        messages.append(msg);
    }

    return messages;
}

int DBManager::getIdByUsername(const QString &username) {
    QSqlQuery query;
    query.prepare("SELECT id FROM Users WHERE Username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "DBManager::getIdByUsername Query failed";
        return -1;
    }

    int id = -1; // Default ID if not found
    if (query.next()) {
        id = query.value(0).toInt(); // Get the ID
    } else {
        qDebug() << "DBManager::getIdByUsername Username not found.";
    }
    return id; // Return the ID or -1 if not found
}

QString DBManager::getUsernameById(int id)
{
    QSqlQuery query;
    query.prepare("SELECT Username FROM Users WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "DBManager::getUsernameById Query failed";
        return QString("Query failed");
    }

    QString username = "default"; // Default ID if not found
    if (query.next()) {
        username = query.value(0).toString(); // Get the ID
    } else {
        qDebug() << "DBManager::getUsernameById id not found.";
    }
    return username; // Return the ID or -1 if not found
}

bool DBManager::usernameExists(const QString &username)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM Users WHERE Username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "DBManager::usernameExists query exec failed";
        return false;
    }
    bool exists = false;
    if (query.next()) {
        int count = query.value(0).toInt();
        exists = (count > 0);
    }
    return exists;
}

QSqlDatabase DBManager::getDatabase()
{
    return db;
}

void DBManager::close()
{
    qDebug() << "DBManager::close() db.close()";
    if (db.isOpen()) {
        db.close();
    }
}

