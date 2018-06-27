#include "passwordRecreationForm.h"

passwordRecreationForm::passwordRecreationForm(QWidget *parent) : QDialog(parent)
{
    this->resize(400, 250);
    this->setFixedHeight(250);
    this->setLayout(createForm());
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->show();
    connect(showHash, SIGNAL(clicked(bool)), this, SLOT(hashShowed(bool)));
    connect(confirm, SIGNAL(clicked()), this, SLOT(onConfirmPress()));
}

QVBoxLayout* passwordRecreationForm::createForm()
{
    recreationFormLay = new QVBoxLayout;
    horStr = new QHBoxLayout;
    oldPassLab = new QLabel("Введите старый пароль:");
    oldPassLab->setFont(QFont(nullptr, 10, 15));
    oldPassLine = new QLineEdit;
    oldPassLine->setMinimumHeight(25);
    oldPassLine->setEchoMode(QLineEdit::Password);
    newPassLab = new QLabel("Введите новый пароль:");
    newPassLab->setFont(QFont(nullptr, 10, 15));
    newPassLine = new QLineEdit;
    newPassLine->setMinimumHeight(25);
    newPassLine->setEchoMode(QLineEdit::Password);
    confPassLab = new QLabel("Подтвердите пароль:");
    confPassLab->setFont(QFont(nullptr, 10, 15));
    confPassLine = new QLineEdit(this);
    confPassLine->setMinimumHeight(25);
    confPassLine->setEchoMode(QLineEdit::Password);
    showHash = new QCheckBox("Показать пароли");
    confirm = new QPushButton("Подтвердить");
    confirm->setMinimumHeight(50);

    horStr->addWidget(showHash);
    horStr->addStretch();
    horStr->addWidget(confirm);

    recreationFormLay->addStretch();
    recreationFormLay->addWidget(oldPassLab);
    recreationFormLay->addWidget(oldPassLine);
    recreationFormLay->addWidget(newPassLab);
    recreationFormLay->addWidget(newPassLine);
    recreationFormLay->addWidget(confPassLab);
    recreationFormLay->addWidget(confPassLine);
    recreationFormLay->addLayout(horStr);
    recreationFormLay->addStretch();
    return recreationFormLay;
}

bool passwordRecreationForm::checkPassValidity()
{
    const QString oldPass = oldPassLine->text();
    const QString newPass = newPassLine->text();
    const QString confPass = confPassLine->text();
    const QString realPass = passDecrypt(passSettings.value("bin_pass").toString()); // Извлекаем из реестра запись bin_pass, переводим в QString и дешифруем.
    if(oldPass != realPass)
    {
        QMessageBox err;
        err.setWindowTitle  (tr("Менеджер паролей"));
        err.setText         (tr("Неверно введен старый пароль!"));
        err.setIcon         (QMessageBox::Critical);
        err.exec();
        return 0;
    }
    if(newPass == confPass && newPass.length() == 0)
    {
        QMessageBox note;
        note.setWindowTitle  (tr("Менеджер паролей"));
        note.setText         (tr("Пароль не установлен. Пароль не будет использоваться. Вы можете изменить его позже, кликнув по кнопке \"Сменить пароль\" в трей-меню приложения"));
        note.setIcon         (QMessageBox::Information);
        note.exec();
        return 1;
    }

    else if(newPass == confPass)
    {
        QMessageBox success;
        success.setWindowTitle  (tr("Менеджер паролей"));
        success.setText         (tr("Пароль успешно пересоздан!"));
        success.setIconPixmap   (QPixmap(":/images/tick.ico"));
        success.exec();
        return 1;
    }
    else
    {
        QMessageBox err;
        err.setWindowTitle  (tr("Менеджер паролей"));
        err.setText         (tr("Новый пароль не совпадает!"));
        err.setIcon         (QMessageBox::Critical);
        err.exec();
        return 0;
    }
}

void passwordRecreationForm::closeEvent(QCloseEvent *event)
{
    this->deleteLater();
    event->accept();
}

void passwordRecreationForm::hashShowed(bool switcher)
{
    if(switcher == true)
    {
        oldPassLine->setEchoMode(QLineEdit::Normal);
        newPassLine->setEchoMode(QLineEdit::Normal);
        confPassLine->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        oldPassLine->setEchoMode(QLineEdit::Password);
        newPassLine->setEchoMode(QLineEdit::Password);
        confPassLine->setEchoMode(QLineEdit::Password);
    }
}


void passwordRecreationForm::onConfirmPress()
{
    if(!checkPassValidity())
    {
        return;
    }
    passSettings.setValue("bin_pass", passEncrypt(newPassLine->text()));
    this->close();
}
