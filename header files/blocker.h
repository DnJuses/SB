#ifndef BLOCKER_H
#define BLOCKER_H

#include <QSystemTrayIcon>
#include <QDialog>
#include <QSettings>
#include <QDockWidget>

QT_BEGIN_NAMESPACE
class QAction;
class QSpinBox;
class QLineEdit;
class QMenu;
class QLabel;
class QGridLayout;
class QSystemTrayIcon;
class QFile;
class QErrorMessage;
class passwordConfirmationForm;
class passwordRecreationForm;
class confEditor;
QT_END_NAMESPACE

class blocker : public QWidget
{
    Q_OBJECT

public:
    blocker(QWidget *mainWindow = nullptr);
    void changeButtonText();
    // Изменяет текст кнопки openEditor на "Открыть\nменю создания\nконфигурации"

private slots:

    // Информация по используемым паролевым формам может быть найдена ниже
    // Информация по паролевой форме passwordCreationForm может быть найдена в начале файла main.cpp

    void openFileClicked();
    /* При нажатии на кнопки openFile или openFile_2, вызывается этот слот.
     * Слот вызывает менеджер файлов, позволяя выбрать пути для файлов:
     *   1. *hosts, если нажата кнопка openFile.
     *   2. *conf, если нажата кнопка openFile_2.
     */
    void startAction();
    /* При нажатии на кнопку *start, вызывается этот слот
     * Слот запускает таймер и вносит изменения в файлы *hosts и *c_hosts
     * Значения таймера зависят от значений QSpinBox'ов после нажатия на кнопку
     * В файл *hosts записывается значения файла *conf
     * В файл *c_hosts записываются значения файла *hosts до его изменения
     * С этого момента приложение начинает свое действие
     */
    void updateTimer();
    /* Вызывается каждую секунду после начала действия приложения
     * Слот отвечает за обновление значений таймеров, как *timer, так и *timerLine
     * *timerLine обновляется по формуле: (X / 10)$(X % 10)
     * Где $ - конкантенация строк, а X - одна из мер времени(часы, минуты или секунды)
     * Для всех мер времени формула одинаковая, разделение происходит через двуеточие(:)
     */
    void onQuitPressed();
    /* После использования пользователем действия *quitAct, вызывается этот слот
     * Слот отвечает за корректный выход из приложения
     * Если приложение в данный момент совершает действие - произойдет запрос пароля у пользователя
     *     Если пароль введен верно - приложение мгновенно закроется и произойдет откат изменений
     *     Иначе - ничего не произойдет
     * Если приложение в данный момент НЕ совершает действие - приложение мгновенно закроется
     */
    void onIconClick(QSystemTrayIcon::ActivationReason reason);
    /* При клике на иконку в трее, вызывается этот слот
     * Слот развернет/свернет приложение, если оно свернуто/развернуто
     */
    void recreatePassword();
    /* После использования пользователем действия *recreatePassAct, вызывается этот слот
     * Слот запрашивает форму на пересоздание пароля
     */
     void showCedit();
     /* После нажатия на кнопку *openEditor, вызывается этот слот
      * Слот раскроет/скроет редактор конфигураций, если он скрыт/раскрыт
      */
     void insertConf();

private:

    // <Использованные виджеты и функции их настройки и расположения>

    QTimer *timer;
    QLineEdit *timerLine;                                         void createTimer();
    QLabel *settersTips;                                          void createTipsLabel();
    QSpinBox *hours;
    QSpinBox *minutes;
    QSpinBox *seconds;                                            void createSetters();
    QLineEdit *path;                                              void createPath();
    QPushButton *openFile;                                        void createOpenFile();
    QLineEdit *path_2;                                            void createPath_2();
    QPushButton *openFile_2;                                      void createOpenFile_2();
    QPushButton *start;                                           void createStart();
    QPushButton *openEditor;                                      void createOpenEditor();
    QAction *recreatePassAct;
    QAction *hideAct;
    QAction *restoreAct;
    QAction *quitAct;                                             void createActions();
    QMenu *trayIconMenu;
    QSystemTrayIcon *trayIcon;                                    void createTrayIcon();
    confEditor *cedit;                                            void createCedit();

    void createAll(); // Вызывает все create функции.

    // </Использованные виджеты и их функции настройки>

    // <Парольные формы>

    passwordConfirmationForm *confirm;
    /* Форма используется для подтверждения пароля приложения
     * Если пароль введен верно, форма дает "зеленый свет" на совершение действия
     * Если пароль введен неверно, то пользователя оповещает ошибка и ничего не происходит
     * Форму можно закрыть и приложение продолжит работу в штатном режиме.
     */
    passwordRecreationForm *recreate;
     /* Форма используется при взаимодействии с действием recreatePassAct.
      * Используется для смены старого пароля на новый.
      * Форма запрашивает ввода следующих данных:
      *  - Старого пароля.
      *  - Нового пароля.
      *  - Подтверждения нового пароля.
      * Если все данные введены корректно, пароль будет успешно сменен.
      * Если же данные введены некоректно, пользователя оповестит ошибка и ничего не произойдет.
      * Форму можно закрыть и приложение продолжит работу в штатном режиме.
      */

      // </Парольные формы>

    int h, min, sec; // Содержат значения таймер (h - часы, min - минуты, sec - секунды).
    QErrorMessage *err; // Вывод ошибок для пользователя происходит через этот объект.
    QGridLayout *lay;

    QFile *hosts, *conf, *c_hosts;

    /* *hosts - Представляет собой файл hosts.
     *   Файл hosts отвечает за параметры подключения к тому или иному адресу или сайту.
     *   Изначально указывает на файл по пути "C:/WINDOWS/System32/drivers/etc/hosts".
     * *conf - Файл, данные из которого будут заноситься в оригинальный файл hosts.
     *   Изначально указывает на файл со стандартной конфигурацией. Он находится в одной папке с проектом.
     *   Изменения заносятся с помощью функции makeRecord().
     *   Конфигурация стандартного файла:
     *   127.0.0.1 vk.com
     *   127.0.0.1 www.vk.com
     *   127.0.0.1 youtube.com
     *   127.0.0.1 www.youtube.com
     * *c_hosts - Копия файла hosts, не затронутая изменениями данной программы.
     *   После окончания работы таймера, измененный hosts файл будет заменен этим.
     *   Файл создается сразу после нажатия на кнопку *start
     *   Замена файла происходит в функции revertChanges().
     */
    QSettings passSettings;
    bool checkExceptions(); // Проверяет все пути к файлам на валидность.
    bool makeRecord();  // Делает запись из файла 'conf' в файл 'hosts' в режиме QIODevice::append.
    void setActivity();
    /* Делает все виджеты:
     *  Активными(с ними МОЖНО взаимодействовать), если таймер неактивен.
     *  Неактивными(с ними НЕЛЬЗЯ взаимодействовать), если таймер активен.
     */
    void revertChanges(); // Подменяет файл 'hosts' на 'с_hosts'.
    void startTimer(); // Запускает таймер с интервалом в 1 сек (можно изменить в макросе T_INTERVAL).
    void closeEvent(QCloseEvent* event) override; // Перегрузка функции не закрывает, а только скрывает приложение.
    void moveEvent(QMoveEvent* event) override;

};

#endif // BLOCKER_H
