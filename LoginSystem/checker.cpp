#include "checker.h"

#include <QRegularExpression>

QPair<bool, QString> Checker::fields_is_correct(QVector<QLineEdit *> &&data)
{
    QString error_message = "";
    size_t count = 0;
    if (!name_is_correct(data[0]->text())) {
        ++count;
        error_message += "Uncorrect name input\n";
    }
    if (!surname_is_correct(data[1]->text())) {
        ++count;
        error_message += "Uncorrect surname input\n";
    }
    if (!phone_is_correct(data[2]->text())) {
        ++count;
        error_message += "Uncorrect phone input\n";
    }
    if (!mail_is_correct(data[3]->text())) {
        ++count;
        error_message += "Uncorrect mail input\n";
    }
    if (!login_is_correct(data[4]->text())) {
        ++count;
        error_message += "Uncorrect login input\n";
    }
    if (!password_is_correct(data[5]->text())) {
        ++count;
        error_message += "Uncorrect password input\n";
    }
    if (count != 0) return std::make_pair(false, error_message);
    else return std::make_pair(true, "true");
}

bool Checker::name_is_correct(const QString& name)
{
    QRegularExpression re("[A-Z]\\w+");
    if (re.match(name).hasMatch() && name.length() <= 15 && !name.contains(" ") && !QRegularExpression("\\d").match(name).hasMatch())
        return true;
    else return false;
}

bool Checker::surname_is_correct(const QString& surname)
{
    QRegularExpression re("[A-Z]\\w+");
    if (re.match(surname).hasMatch() && surname.length() <= 20 && !surname.contains(" ") && !QRegularExpression("\\d").match(surname).hasMatch())
        return true;
    else return false;
}

bool Checker::phone_is_correct(const QString& phone)
{
    QRegularExpression re("(0(43|44|55|60|77|93|94|98))\\d");
    if (re.match(phone).hasMatch() && phone.length() == 9 && !phone.contains(" "))
        return true;
    else return false;
}

bool Checker::mail_is_correct(const QString& mail)
{
    QRegularExpression re("^(\\w+)\\@(mail|gmail|yahoo|synopsys)\\.(ru|com)");
    if (re.match(mail).hasMatch() && !mail.contains(" "))
        return true;
    else return false;
}

bool Checker::login_is_correct(const QString& login)
{
    QRegularExpression re("\\w+");
    if (re.match(login).hasMatch() && login.length() <= 20 && !login.contains(" "))
        return true;
    else return false;
}

bool Checker::password_is_correct(const QString& password)
{
    if (password.length() >= 8 && password.length() <= 20)
        return true;
    else return false;
}

