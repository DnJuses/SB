#include "passwordConfirmationForm.h"

passwordConfirmationForm::passwordConfirmationForm(QWidget *parent) : QDialog(parent)
{
    if(passSettings.value("bin_pass").toString() == "" ||
       !passSettings.contains("bin_pass"))
    {
        qApp->exit();
        return;
    }
    isCorrect = false;
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->resize(400, 125);
    this->setFixedHeight(125);
    this->setLayout(createForm());
    this->show();
    connect(showHash, SIGNAL(clicked(bool)), this, SLOT(hashShowed(bool)));
    connect(confirm, SIGNAL(clicked()), this, SLOT(onConfirmPress()));
}

passwordConfirmationForm::~passwordConfirmationForm()
{
   disconnect(this, 0, 0, 0);
}

bool passwordConfirmationForm::isMatch()
{
    return isCorrect;
}

QVBoxLayout *passwordConfirmationForm::createForm()
{
    confirmationFormLay = new QVBoxLayout;
    horStr = new QHBoxLayout;
    confPassLab = new QLabel("Чтобы выйти, введите ваш пароль ниже:");
    confPassLab->setFont(QFont(nullptr, 10, 15));
    confPassLine = new QLineEdit(this);
    confPassLine->setMinimumHeight(25);
    confPassLine->setEchoMode(QLineEdit::Password);
    showHash = new QCheckBox("Показать пароль");
    confirm = new QPushButton("Подтвердить");
    confirm->setMinimumHeight(50);

    horStr->addWidget(showHash);
    horStr->addStretch();
    horStr->addWidget(confirm);

    confirmationFormLay->addStretch();
    confirmationFormLay->addWidget(confPassLab);
    confirmationFormLay->addWidget(confPassLine);
    confirmationFormLay->addLayout(horStr);
    confirmationFormLay->addStretch();
    return confirmationFormLay;
}

bool passwordConfirmationForm::checkPassValidity()
{
    const QString writtenPass = confPassLine->text();
    const QString realPass = passDecrypt(passSettings.value("bin_pass").toString());
    if(writtenPass == realPass)
    {
        return 1;
    }
    else
    {
        QMessageBox err;
        err.setWindowTitle  (tr("Подтверждение пароля"));
        err.setText         (tr("Пароль введен неверно!"));
        err.setIcon         (QMessageBox::Critical);
        err.exec();
    }
    return 0;
}

void passwordConfirmationForm::closeEvent(QCloseEvent *event)
{
    this->deleteLater();
    event->accept();
}

void passwordConfirmationForm::hashShowed(bool switcher)
{
    if(switcher == true)
    {
       confPassLine->setEchoMode(QLineEdit::Normal);
    }
    else
    {
       confPassLine->setEchoMode(QLineEdit::Password);
    }
}

void passwordConfirmationForm::onConfirmPress()
{
    if(!checkPassValidity())
    {
        return;
    }
    isCorrect = true;
    this->close();
}
