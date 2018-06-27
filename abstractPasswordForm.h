#ifndef ABSTRACT_PASSWORD_FORM_H
#define ABSTRACT_PASSWORD_FORM_H

#include <QCoreApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QDebug>
#include <QFile>
#include <QPixmap>
#include <QPushButton>
#include <QSettings>
#include <QCloseEvent>

class abstractPasswordForm
{
public:
     QSettings passSettings;  // Пароль храним в реестре зашифрованным.
protected:
    abstractPasswordForm(); // Класс абстрактный - создавать его объекты запрещено.
    virtual ~abstractPasswordForm() = 0;
    virtual QVBoxLayout* createForm() = 0; // Отвечает за создание парольных форм.
    virtual bool checkPassValidity() = 0; // Проверяет пароль на валидность.
    QString passEncrypt(QString message);  // Шифрует пароль с помощью XOR с ключом KEY (макрос определен в cpp файле).
    QString passDecrypt(QString message);  // Дешифрует пароль с помощь XOR с ключом KEY.
protected slots:
    virtual void hashShowed(bool) = 0; // Меняет EchoMode классовых QLineEdit'ов (0 - password, 1 - normal).
    virtual void onConfirmPress() = 0; // Запускает процесс проверки паролей.
};

#endif // ABSTRACT_PASSWORD_FORM_H
