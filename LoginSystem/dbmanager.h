#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>

class DBManager
{
public:
    DBManager();
    bool connectToDatabase();
    QSqlDatabase getDatabase();
    void close();
    QString hashPassword(const QString& password);
private:
    QSqlDatabase db;
};

#endif // DBMANAGER_H
