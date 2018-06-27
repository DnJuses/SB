#include "blocker.h"

#include <QCoreApplication>
#include <QWidget>
#include <QGridLayout>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QLineEdit>
#include <QSpinBox>
#include <QTimer>
#include <QLabel>
#include <QErrorMessage>
#include "confeditor.h"
#include "passwordConfirmationForm.h"
#include "passwordRecreationForm.h"
#define T_INTERVAL 1000


blocker::blocker(QWidget *mainWindow) : QWidget(mainWindow)
{
    // Конструктор создает окно приложения, меняет его иконку, создает большую часть коннектов приложения.
    // Размер окна - 475x300
    this->createAll();
    this->setFixedSize(475, 300);
    // Устанавливаем иконки для приложения
    trayIcon->setIcon(QIcon(":/images/icon.ico"));
    setWindowIcon(QIcon(":/images/icon.ico"));
    trayIcon->show();
    // Конфигурирование путей по нажатию одной из двух кнопок
    connect(openFile, SIGNAL(clicked()), this, SLOT(openFileClicked()));
    connect(openFile_2, SIGNAL(clicked()), this, SLOT(openFileClicked()));
    // Начать действие приложения по нажатию на кнопку
    connect(start, SIGNAL(clicked()), this, SLOT(startAction()));
    // Открытие трей-меню приложения по нажатии на иконку в трее.
    connect(trayIcon, &QSystemTrayIcon::activated, this, &blocker::onIconClick);
    // Открытие редактора конфигурации при нажатии на кнопку
    connect(openEditor, SIGNAL(clicked()), this, SLOT(showCedit()));
    // Использование конфигурации из редактора конфигурации напрямую
    connect(cedit, SIGNAL(tempFileCreated()), this, SLOT(insertConf()));

    // Все (дис)коннекты вне конструктора в данном файле и их расположения:
    // connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer())); - в функции startTimer.
    // connect(hideAct, SIGNAL(triggered()), this, SLOT(hide())); - в функции createActions.
    // connect(restoreAct, SIGNAL(triggered()), this, SLOT(showMaximized())); - в функции createActions.
    // connect(quitAct, SIGNAL(triggered()), this, SLOT(onQuitPressed())); - в функции createActions.
    // connect(recreatePass, SIGNAL(triggered()), this, SLOT(recreatePassword())); - в функции createAction.
    // connect(confirm, SIGNAL(shutdownApp()), this, SLOT(onAppShutdownReceived())); - в функции onQuitPressed.
    // disconnect(timer, 0, 0, 0); - в функции updateTimer.
}

void blocker::changeButtonText()
{
    openEditor->setText("Открыть\nменю создания\nконфигурации");

}

void blocker::createTimer()
{
    // Создаем таймер для пользователя.
    // Начальное значение - 00:00:00
    // Point size шрифта - 50
    // Weight шрифта - 50
    timerLine = new QLineEdit(this);
    timerLine->setMinimumSize(200,100);
    timerLine->setText("00:00:00");
    timerLine->setReadOnly(true);
    timerLine->setAlignment(Qt::AlignCenter);
    timerLine->setFont(QFont(nullptr, 50, 50, false));
    lay->addWidget(timerLine, 0, 0, 4, 12);
}

void blocker::createTipsLabel()
{
    // Создаем текстовые подсказки для QSpinBox'ов, которые распологаются над боксами.
    settersTips = new QLabel(this);
    settersTips->setText ("                  Часы                     "
                          "                  Минуты                   "
                          "                  Секунды");
    lay->addWidget(settersTips, 4, 0, 1, 12);
}

void blocker::createSetters()
{
    // Создает три QSpinbox:
    // 1. Отвечает за часы. Имеет макс. ограничение в 999 часов. Ширина - 138
    // 2. Отвечает за минуты. Имеет макс. ограничение в 59 минут. Ширина - 138
    // 3. Отвечает за секунды. Имеет макс. ограничение в 59 секунд. Ширина - 139
    // Все боксы имеют одинаковую длину, которая зависит от QGridLayout
    // Значения из боксов используются при старте и обновлении таймера
    hours = new QSpinBox(this);
    hours->setMinimum(0);
    hours->setMaximum(999);
    hours->setFixedWidth(146);
    minutes = new QSpinBox(this);
    minutes->setMinimum(0);
    minutes->setMaximum(59);
    minutes->setFixedWidth(146);
    seconds = new QSpinBox(this);
    seconds->setMinimum(0);
    seconds->setMaximum(59);
    seconds->setFixedWidth(147);
    lay->addWidget(hours, 5, 0, 1, 4);
    lay->addWidget(minutes, 5, 4, 1, 4);
    lay->addWidget(seconds, 5, 8, 1, 4);
}

void blocker::createPath()
{
    // Создает QLineEdit
    // Используется для хранения пути файла "hosts"
    path = new QLineEdit(this);
    path->setText("C:/WINDOWS/System32/drivers/etc/hosts"); // Стандартный путь к "hosts" файлу.
    path->setMinimumSize(180, 25);
    lay->addWidget(path, 6, 0, 1, 8);
}

void blocker::createOpenFile()
{
    // Создает кнопку размером 30x25
    // Используется для изменения пути файла "hosts"
    openFile = new QPushButton(this);
    openFile->setText("..");
    openFile->setFixedSize(30, 25);
    lay->addWidget(openFile, 6, 8, 1, 1);
}

void blocker::createPath_2()
{
    // Создает QLineEdit
    // Используется для хранения пути файла конфигурации
    path_2 = new QLineEdit(this);
    path_2->setText("Выберите файл конфигурации (необязательно)..."); // Если не выбран, будет использован стандартный файл конфигурации
    path_2->setMinimumSize(180, 25);
    lay->addWidget(path_2, 7, 0, 1, 8);
}

void blocker::createOpenFile_2()
{
    // Создает кнопку размером 30x25
    // Используется для изменения пути файла конфигурации
    openFile_2 = new QPushButton(this);
    openFile_2->setText("..");
    openFile_2->setFixedSize(30, 25);
    lay->addWidget(openFile_2, 7, 8, 1, 1);
}

void blocker::createOpenEditor()
{
    // Создаем кнопку размером 113x57
    // Она открывает редактор конфигураций.
    openEditor = new QPushButton(this);
    openEditor->setFixedSize(113, 57);
    openEditor->setText("Открыть\nменю создания\nконфигурации");
    lay->addWidget(openEditor, 6, 9, 2, 2);
}

void blocker::createStart()
{
    // Создает кнопку
    // По нажатию на нее, приложение получает сигнал к действию
    start = new QPushButton(this);
    start->setText("Запустить таймер");
    start->setMinimumSize(200, 50);
    lay->addWidget(start, 8, 0, 1, 12);
}

void blocker::createActions()
{
    // Создает все 4 действия для трей-меню приложения
    // В трей-меню они добавятся в функции createTrayIcon

    // Действие по смене пароля
    recreatePassAct = new QAction(tr("Изменить пароль"), this);
    connect(recreatePassAct, SIGNAL(triggered()), this, SLOT(recreatePassword()));

    // Действие по сворачиваю окна
    hideAct = new QAction(tr("Свернуть"), this);
    connect(hideAct, SIGNAL(triggered()), this, SLOT(hide()));

    // Действие по разворачанию окна
    restoreAct = new QAction(tr("Развернуть"), this);
    connect(restoreAct, SIGNAL(triggered()), this, SLOT(showMaximized()));

    // Действие по выходу из приложения
    quitAct = new QAction(tr("Выход"), this);
    connect(quitAct, SIGNAL(triggered()), this, SLOT(onQuitPressed()));
}

void blocker::createTrayIcon()
{
    // Создает трей-меню приложения
    trayIconMenu = new QMenu(this);
    // Добавляет действия в трей-меню
    trayIconMenu->addAction(hideAct);
    trayIconMenu->addAction(restoreAct);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(recreatePassAct);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAct);

    // Привязка трей-меню к трей-иконке
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void blocker::createCedit()
{
    cedit = new confEditor(this);
}

void blocker::createAll()
{
    // Создаем виджеты и компонуем их
    lay = new QGridLayout(this);
    createTimer();
    createTipsLabel();
    createSetters();
    createPath();
    createOpenFile();
    createPath_2();
    createOpenFile_2();
    createOpenEditor();
    createStart();
    createActions();
    createTrayIcon();
    createCedit();
}

bool blocker::checkExceptions()
{
    QString pathStr = path->text();
    QFile test(pathStr);
    // <Тест-блок файла hosts>
    if(pathStr.length() >= 5)
    {
        // Является ли данный файл файлом "hosts".
        if(!pathStr.endsWith("hosts"))
        {
            err->showMessage("Используемый файл не является \"hosts\" файлом.");
            return 0;
        }
        // Проверка пути на валидность
        if(!test.open(QIODevice::ReadOnly))
        {
            err->showMessage("Невозможно открыть файл \"hosts\", пожалуйста, проверьте путь к файлу.");
            return 0;
        }
        test.close();
    }
    else
    {
        err->showMessage("Невозможно открыть файл \"hosts\", пожалуйста, проверьте путь к файлу.");
        return 0;
    }
    // </Тест-блок файла hosts>
    pathStr = path_2->text();
    // <Тест-блок файла conf>
    // Проверка на необходимость использования стандартной конфигурации
    if(pathStr == ""
       || pathStr == "Выберите файл конфигурации (необязательно)..."
       || pathStr.endsWith("default_conf"))
    {
        path_2->setText("default_conf");
    }
    // Проверка пути на валидность.
    if(!test.open(QIODevice::ReadOnly))
    {
        err->showMessage("Невозможно открыть файл конфигурации. Пожалуйста, проверьте путь к файлу и убедитесь, что он использует форматы *txt или *any");
        return 0;
    }
    test.close();
    // </Тест-блок файла conf>
    return 1;
}

void blocker::setActivity()
{
    static int switcher = 0;
    switcher = (switcher + 1) % 2;
    // если switcher == 1, тогда виджеты становятся неактивными.
    // если switcher == 0, тогда виджеты становятся активными.
    start->setDisabled(switcher);
    path->setDisabled(switcher);
    path_2->setDisabled(switcher);
    openFile->setDisabled(switcher);
    openFile_2->setDisabled(switcher);
    hours->setDisabled(switcher);
    minutes->setDisabled(switcher);
    seconds->setDisabled(switcher);
    settersTips->setHidden(switcher);
}

bool blocker::makeRecord()
{
    // Вносим изменения в файл "hosts" из файла "conf"
    if( hosts->open(QIODevice::Append | QIODevice::Text)
     && conf->open(QIODevice::ReadOnly))
    {
        passSettings.setValue("isRunning", true);
        QByteArray comArr = conf->readAll();
        hosts->write(comArr);
        hosts->close();
        conf->close();
        startTimer();
        trayIcon->showMessage(tr("Таймер"),
                               tr("Таймер теперь активен!"));
        setActivity();
        conf->setFileName("tempfile");
        if(conf->exists())
        {
            path_2->setText("default_conf");
            conf->remove();
        }
        conf->close();
        return 1;
    }
    return 0;
}

void blocker::revertChanges()
{
    // Откатываем изменения после отработки приложения
    if(hosts->open(QIODevice::WriteOnly)
      && c_hosts->open(QIODevice::ReadOnly))
    {
        passSettings.setValue("isRunning", false);
        QByteArray comArr = c_hosts->readAll();
        hosts->write(comArr);
        setActivity();
        trayIcon->showMessage(tr("Таймер"),
                               tr("Таймер остановлен!"));
    }
    hosts->close();
    c_hosts->close();
    c_hosts->remove();
    delete hosts;
    delete c_hosts;
    delete conf;
}


void blocker::openFileClicked() // СЛОТ
{
    // Обращение к QFileDialog для выбора путей к необходимым текстовым файлам
    QLineEdit *paths;
    if(sender() == openFile)
    {
       paths = path;
    }
    else
    {
       paths = path_2;
    }
    paths->setText(QFileDialog::getOpenFileName(nullptr, nullptr, nullptr, tr("Любые файлы (*.*);;Текстовые файлы (*.txt)")));
}

void blocker::startAction() // СЛОТ
{
    err = new QErrorMessage;
    // Проверка путей
    if(!checkExceptions())
    {
        return;
    }
    // Создаем оперируемые файлы
    hosts = new QFile(path->text());
    hosts->copy("chosts");
    conf = new QFile(path_2->text());
    c_hosts = new QFile("chosts");
    // Совершаем попытку записи в файлы
    if(!makeRecord())
    {
        // Так как файл "hosts" защищен системой и антивирусами, это может произойти
        err->showMessage("Невозможно открыть файл \"hosts\" для записи. Возможно, ее блокирует антивирус. Внесите программу в список исключений вашего антивируса");
        return;
    }
}

void blocker::closeEvent(QCloseEvent *event)
{
    // Вместо закрытия приложения - прячем его
    this->hide();
    event->ignore();
}

void blocker::moveEvent(QMoveEvent *event)
{
    // Если редактор примагничен к нашему окну - двигаем его вместе с ним.
    if(cedit->isMagnetic())
    {
        QPoint ceditPos = event->oldPos() - event->pos();
        cedit->move(cedit->x() - ceditPos.x(), cedit->y() - ceditPos.y());
    }
}

void blocker::showCedit()
{
    static int ceditX = 210, ceditY = 250;
    if(cedit->isHidden())
    {
        openEditor->setText("Закрыть\nменю создания\nконфигурации");
        cedit->move(this->x() + this->width() + 17, this->y());
        cedit->resize(ceditX, ceditY);
        cedit->show();
    }
    else
    {
        openEditor->setText("Открыть\nменю создания\nконфигурации");
        ceditX = cedit->width();
        ceditY = cedit->height();
        cedit->hide();
    }
}

void blocker::insertConf()
{
    path_2->setText("tempfile");
}

void blocker::onQuitPressed() // СЛОТ
{
    // Если приложение в данный момент не совершает действие - выходим
    if(passSettings.value("isRunning").toBool() == false)
    {
         qApp->quit();
    }
    // Иначе - запрос на подтверждение пароля.
    confirm = new passwordConfirmationForm;
    confirm->exec();
    if(confirm->isMatch())
    {
        if(passSettings.value("isRunning").toBool() == true)
        {
            passSettings.setValue("isRunning", false);
            revertChanges();
        }
        qApp->quit();
    }
    else
    {
        return;
    }
}

void blocker::onIconClick(QSystemTrayIcon::ActivationReason reason) // СЛОТ
{
    switch(reason){
    // Развернуть/свернуть приложение
    case QSystemTrayIcon::Trigger:
        if(this->isHidden())
        {
            this->show();
        }
        else
        {
            this->hide();
        }
        break;
    // Ничего
    default:
        ;
    }
}

void blocker::recreatePassword()
{
    // Запрос на пересоздание пароля
    recreate = new passwordRecreationForm;
    recreate->exec();
}


void blocker::startTimer()
{
    // Получение значений из трех QSpinBox в отдельные переменные
    h = hours->value(), min = minutes->value(), sec = seconds->value();
    // Создаем и запускаем таймер
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));
    timer->start(T_INTERVAL);
    // Установка значений таймера для пользователя
    timerLine->setText( QString::number(h / 10)
                  + QString::number(h % 10)
                  + ":"
                  + QString::number(min / 10)
                  + QString::number(min % 10)
                  + ":"
                  + QString::number(sec / 10)
                  + QString::number(sec % 10)
                  );
}

void blocker::updateTimer() //  СЛОТ
{
    // Каждую секунду идет проверка и изменение значений таймера
    if(min == 0 && sec == 0 && h != 0)
    {
        min = 60;
        h--;
    }
    if(sec == 0 && min != 0)
    {
        sec = 60;
        min--;
    }
    // Если таймер отработал - завершаем действие приложения.
    if(sec == 0 && min == 0 && h == 0)
    {
        sec++;
        disconnect(timer, 0, 0, 0);
        delete timer;
        revertChanges();
    }
    sec--;
    // Установка значений таймера для пользователя
    timerLine->setText( QString::number(h / 10)
                      + QString::number(h % 10)
                      + ":"
                      + QString::number(min / 10)
                      + QString::number(min % 10)
                      + ":"
                      + QString::number(sec / 10)
                      + QString::number(sec % 10)
                      );
}
