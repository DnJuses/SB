#ifndef PASSWORD_CONFIRMATION_FORM_H
#define PASSWORD_CONFIRMATION_FORM_H

#include "abstractPasswordForm.h"

class passwordConfirmationForm : public QDialog, abstractPasswordForm
{
    Q_OBJECT
public:
    bool isMatch();
    passwordConfirmationForm(QWidget *parent = nullptr);
    ~passwordConfirmationForm();
private:
    QHBoxLayout *horStr;
    QLabel *confPassLab;
    QLineEdit *confPassLine;
    QCheckBox *showHash;
    QPushButton *confirm;
    QVBoxLayout *confirmationFormLay;
    QVBoxLayout* createForm() override;  // Форма для подтверждения пароля (вызывается, если пользователь хочет выйти из приложения. Кнопка выхода находится в трэе).
    bool checkPassValidity() override;  // Сверяет введенный пароль в conf_pass_line со значением в реестре.
    void closeEvent(QCloseEvent* event) override;    // QCloseEvent overrided virtual function.
    bool isCorrect;

private slots:
    void hashShowed(bool switcher) override; // Меняет EchoMode классовых QLineEdit'ов (0 - password, 1 - normal).
    void onConfirmPress() override; // После нажатия на кнопку confirm, проверяет пароль на валидность и, если пароль правильный - программа закрывается, иначе пиздец и по новой.
};

#endif // PASSWORD_CONFIRMATION_FORM_H
