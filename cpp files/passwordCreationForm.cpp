#include "passwordCreationForm.h"

passwordCreationForm::passwordCreationForm(QWidget *parent) : QDialog(parent)
{
    this->resize(400, 200);
    this->setFixedHeight(200);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setLayout(createForm());
    this->show();
    connect(showHash, SIGNAL(clicked(bool)), this, SLOT(hashShowed(bool)));
    connect(confirm, SIGNAL(clicked()), this, SLOT(onConfirmPress()));
}

QVBoxLayout* passwordCreationForm::createForm()
{
    creationFormLay = new QVBoxLayout;
    horStr = new QHBoxLayout;
    newPassLab = new QLabel("Введите новый пароль:");
    newPassLab->setFont(QFont(nullptr, 10, 15));
    newPassLine = new QLineEdit;
    newPassLine->setMinimumHeight(25);
    newPassLine->setEchoMode(QLineEdit::Password);
    confPassLab = new QLabel("Подтвердите введенный пароль:");
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

    creationFormLay->addStretch();
    creationFormLay->addWidget(newPassLab);
    creationFormLay->addWidget(newPassLine);
    creationFormLay->addWidget(confPassLab);
    creationFormLay->addWidget(confPassLine);
    creationFormLay->addLayout(horStr);
    creationFormLay->addStretch();
    return creationFormLay;
}

bool passwordCreationForm::checkPassValidity()
{
    const QString newPass = newPassLine->text();
    const QString confPass = confPassLine->text();
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
        success.setText         (tr("Пароль успешно создан!"));
        success.setIconPixmap   (QPixmap(":/images/tick.ico"));
        success.exec();
        return 1;
    }
    else
    {
        QMessageBox err;
        err.setWindowTitle  (tr("Менеджер паролей"));
        err.setText         (tr("Пароли не совпадают."));
        err.setIcon         (QMessageBox::Critical);
        err.exec();
        return 0;
    }
}

void passwordCreationForm::closeEvent(QCloseEvent *event)
{
    this->deleteLater();
    event->accept();
}

void passwordCreationForm::hashShowed(bool switcher)
{
    if(switcher == true)
    {
       newPassLine->setEchoMode(QLineEdit::Normal);
       confPassLine->setEchoMode(QLineEdit::Normal);
    }
    else
    {
       newPassLine->setEchoMode(QLineEdit::Password);
       confPassLine->setEchoMode(QLineEdit::Password);
    }
}

void passwordCreationForm::onConfirmPress()
{
    if(!checkPassValidity())
    {
        return;
    }
    passSettings.setValue("bin_pass", passEncrypt(newPassLine->text())); // Заносим пароль в реестр и шифруем его XOR'ом с ключом 27
    this->close();
}
