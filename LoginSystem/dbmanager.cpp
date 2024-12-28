#include "dbmanager.h"

#include <QMessageBox>
#include <QCryptographicHash>

DBManager::DBManager() {}

QString DBManager::hashPassword(const QString& password) {
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

bool DBManager::connectToDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./data.db");
    if (!db.open()) {
        qDebug() << "Failed to connect to database...";
        return false;
    }
    qDebug() << "Connected to database!";
    return true;
}

QSqlDatabase DBManager::getDatabase()
{
    return db;
}

void DBManager::close()
{
    db.close();
}

