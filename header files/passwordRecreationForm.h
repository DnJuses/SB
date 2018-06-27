#ifndef PASSWORD_RECREATION_FORM_H
#define PASSWORD_RECREATION_FORM_H

#include "abstractPasswordForm.h"

class passwordRecreationForm : public QDialog, abstractPasswordForm
{
    Q_OBJECT
public:
    passwordRecreationForm(QWidget *parent = nullptr);

private:
    QHBoxLayout *horStr;
    QLabel *oldPassLab;
    QLineEdit *oldPassLine;
    QLabel *newPassLab;
    QLineEdit *newPassLine;
    QLabel *confPassLab;
    QLineEdit *confPassLine;
    QCheckBox *showHash;
    QPushButton *confirm;
    QVBoxLayout *recreationFormLay;
    QVBoxLayout* createForm() override;  // Форма для пересоздания пароля (вызывается, при отсутствии пароля в реестре вместе с диалогом).
    bool checkPassValidity() override;  // Изначально проверяет пароль в old_pass_line со значением в реестре. Затем проверяет пароли в new_pass_line и conf_pass_line.
    void closeEvent(QCloseEvent* event) override;    // QCloseEvent overrided virtual function.

private slots:
    void hashShowed(bool switcher) override; // Меняет EchoMode классовых QLineEdit'ов (0 - password, 1 - normal).
    void onConfirmPress() override; // После нажатия на кнопку confirm, проверяет пароли на валидность и вносит новый пароль в реестр.
};

#endif // PASSWORD_RECREATION_FORM_H
