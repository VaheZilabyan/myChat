#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QVariantMap>
#include <QList>

class DBManager {
public:
    static DBManager* instance(); // Singleton accessor

    bool connectToDatabase();
    QSqlDatabase getDatabase();
    void close();
    QString hashPassword(const QString& password);
    bool storeMessage(int senderId, int receiverId, const QString& message);
    QList<QVariantMap> fetchMessages(int userId1, int userId2);

    // Static utility methods
    static int getIdByUsername(const QString& username);
    static QString getUsernameById(int id);
    static bool usernameExists(const QString& username);

private:
    DBManager();                                // Private constructor
    ~DBManager();                               // Private destructor
    DBManager(const DBManager&) = delete;       // Disable copy constructor
    DBManager& operator=(const DBManager&) = delete; // Disable assignment operator

    QSqlDatabase db; // The actual database connection
};

#endif // DBMANAGER_H
