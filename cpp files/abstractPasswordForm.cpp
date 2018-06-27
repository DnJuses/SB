#include "abstractPasswordForm.h"
#include <QVariant>
#include <QString>
#define KEY 27

abstractPasswordForm::~abstractPasswordForm()
{

}

abstractPasswordForm::abstractPasswordForm()
{

}

QString abstractPasswordForm::passEncrypt(QString message)
{
    for(size_t i = 0; i < message.length(); i++)
    {
        int charCode = message[i].unicode();
        message[i] = charCode ^ KEY;
    }
    return message;
}

QString abstractPasswordForm::passDecrypt(QString message)
{
    return passEncrypt(message);
}
