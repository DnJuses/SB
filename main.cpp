#include <QApplication>
#include <QtCore>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QMessageBox>
#include "passwordConfirmationForm.h"
#include "passwordCreationForm.h"
#include "confeditor.h"
#include "blocker.h"

/* Информация по паролевой форме passwordCreationForm:
 * Форма используется для создания пароля в случае, если приложение было запущено впервые или пароль не был установлен.
 * Вызывается сразу после запуска приложения, если пароль не установлен
 * Форма запрашивает ввода следующих данных:
 *  - Новый пароль.
 *  - Подтверждение нового пароля.
 * Если все данные введены корректно, пароль будет успешно установлен.
 * Если же данные введены некоректно, пользователя оповестит ошибка и ничего не произойдет(пароль не будет установлен).
 * Если же все поля будут пустыми, то пользователя оповестит соответствующий диалог и пароль не будет установлен, но приложение будет работать без его запроса.
 * Форму можно закрыть и пароль не будет установлен, но приложение продолжит работать без его запроса
 */



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Далее идет проверка количества копий приложения
    // Максимально допустимое кол-во копий - 1
    QSystemSemaphore singleApp("<uniq id>", 1);
    singleApp.acquire();
#ifndef Q_OS_WIN32 // Освободить память при аварийном закрытии программы в Linux
    QSharedMemory nix_fix_leak("<uniq id 2>");
    if(nix_fix_leak.attach())
        nix_fix_leak.detach();
#endif
    QSharedMemory q_sm("<uniq id 2>");
    bool appIsRunning = false;

    if(q_sm.attach())
    {
        appIsRunning = true;
    }
    else
    {
        q_sm.create(1);
        appIsRunning = false;
    }

    singleApp.release();
    if(appIsRunning)
    {
        QMessageBox piar; // Расшифровывается как "Programm is already running"
        piar.setText(QObject::tr("Программа уже запущена\nЕдиновременно может быть запущена только одна копия приложения!"));
        piar.setIcon(QMessageBox::Warning);
        piar.exec();
        return 0;
    }

    // Выставляем настройки приложения
    QApplication::setQuitOnLastWindowClosed(false);
    QCoreApplication::setOrganizationName("Suvo softworks");
    QCoreApplication::setApplicationName("Site Blocker");
    // Создаем инстанс класса главного окна
    blocker app;
    app.show();
    app.setDisabled(1); // На время проверок делаем окно неактивным
    app.setWindowTitle("SB");

    QSettings *passSettings = new QSettings;

    // Предложить пользователю создать пароль приложения
    if(!passSettings->contains("bin_pass") ||
       passSettings->value("bin_pass").toString() == "")
    {
        QMessageBox::StandardButton conf;
        conf = QMessageBox::question(0,
                                     "Менеджер паролей",
                                     "Пароль не установлен. Хотите создать? (Пароль используется для безопасного выхода из приложения)",
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::Yes);
        if(conf == QMessageBox::Yes)
        {
            passwordCreationForm *create = new passwordCreationForm;
            create->exec();
        }
    }
    // Если программа была закрыта аварийно, но в это время совершала работу
    // Значит предлагаем пользователю откатить изменения, которые сделала программа, вводом пароля приложения
    // Иначе изменения продолжат действовать
    if(passSettings->value("isRunning") == true)
    {
        QMessageBox conf;
        conf.setWindowTitle("Программа");
        conf.setText("Программа была завершена аварийно. Изменения продолжают действовать. Для того, чтобы отменить их, вам необходимо будет ввести пароль от приложения.");
        conf.setFont(QFont(nullptr, 10, 15, false));
        conf.exec();
        passwordConfirmationForm *confirm = new passwordConfirmationForm;
        confirm->exec();
        // Откат изменений (похоже на функцию revertChanges из класса blocker)
        if(confirm->isMatch())
        {
            passSettings->setValue("isRunning", false);
            QFile hosts, c_hosts;
            hosts.setFileName("C:/WINDOWS/System32/drivers/etc/hosts");
            c_hosts.setFileName("chosts");
            hosts.open(QIODevice::WriteOnly);
            c_hosts.open(QIODevice::ReadOnly);
            QByteArray comArr = c_hosts.readAll();
            hosts.write(comArr);
            hosts.close();
            c_hosts.close();
            c_hosts.remove();
        }
        else
        {
            return 0;
        }
    }
    delete passSettings;
    app.setEnabled(1);

    return a.exec();
}
