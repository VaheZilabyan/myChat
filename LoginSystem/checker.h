#ifndef CHECKER_H
#define CHECKER_H

#include "sign_up.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Checker
{
public:
    Checker() = default;
    static QPair<bool, QString> fields_is_correct(QVector<QLineEdit*>&& data);
private:
    static bool name_is_correct(const QString&);
    static bool surname_is_correct(const QString&);
    static bool phone_is_correct(const QString&);
    static bool mail_is_correct(const QString&);
    static bool login_is_correct(const QString&);
    static bool password_is_correct(const QString&);
};

#endif // CHECKER_H
