#ifndef PASSWORD_CREATION_FORM_H
#define PASSWORD_CREATION_FORM_H


#include "abstractPasswordForm.h"

class passwordCreationForm : public QDialog, abstractPasswordForm
{
    Q_OBJECT
public:
    passwordCreationForm(QWidget *parent = nullptr);

private:
    QHBoxLayout *horStr;
    QLabel *newPassLab;
    QLineEdit *newPassLine;
    QLabel *confPassLab;
    QLineEdit *confPassLine;
    QCheckBox *showHash;
    QPushButton *confirm;
    QVBoxLayout *creationFormLay;
    QVBoxLayout* createForm() override;  // Форма для создания пароля (вызывается, если пароль не установлен или пользователь захотел его сменить).
    bool checkPassValidity() override;  // Сверяет значения паролей в new_pass_line и conf_pass_line.
    void closeEvent(QCloseEvent* event) override;    // QCloseEvent overrided virtual function.

private slots:
    void hashShowed(bool switcher) override; // Меняет EchoMode классовых QLineEdit'ов (0 - password, 1 - normal).
    void onConfirmPress() override;  // После нажатия на кнопку confirm, проверяет пароли на валидность и вносит новый пароль в реестр.
};

#endif // PASSWORD_CREATION_FORM_H
