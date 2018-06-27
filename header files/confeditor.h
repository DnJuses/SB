#ifndef CONFEDITOR_H
#define CONFEDITOR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class blocker;
class QObject;
class QMouseEvent;
class QPushButton;
class QTextEdit;
class QMenuBar;
class QToolBar;
class QVBoxLayout;
class QHBoxLayout;
class QStatusBar;
class QLabel;
QT_END_NAMESPACE

// Вид описания - сначала функция, потом ее описание ниже в комментариях.

class confEditor : public QWidget
{
    Q_OBJECT
public:
    confEditor(blocker *mainBlocker, QWidget *mainWindow = nullptr);
    bool isMagnetic(); // Получение аттрибута magnet
signals:
    void tempFileCreated();
private slots:
    bool openFile();
    /* При использовании действия openAct, вызывает этот слот
     * Слот вызывает файловый менеджер, который позволяет пользователю открыть текстовые файлы или файлы *any
     * После открытия файла, его содержимое переносится в confEdit, для последующей работы с его содержимым
     * Путь файла запоминается и может быть использован для сохранения без привязки к файловому менеджеру.
     */
    bool saveFile();
    /* При использовании действия saveAct, вызывается этот слот
     * Слот сохраняет текущий набранный текст confEdit в файл по пути savePath
     * По умолчанию savePath ничему не равен и зависит от ранее успешно открытого файла. Его путь запишется в эту переменную.
     * Если же во время сохранения, savePath будет пустым, то в папке с программой создается файл под названием "edited_conf" + текущее время и дата.
     */
    bool saveAsFile();
    /* При использовании действия saveAsAct, вызывается этот слот
     * Слот сохраняет текущий набранный текст confEdit в выбранный в проводнике файл.
     */
    void undo();
    /* При использовании действия undoAct, вызывается этот слот
     * Слот совершает операцию отмены на текст в confEdit.
     */
    void redo();
    /* При использовании действия redoAct, вызывается этот слот
     * Слот совершает операцию возврата на текст в confEdit.
     */
    void cut();
    /* При использовании действия cutAct, вызывается этот слот
     * Слот удаляет выделенный текст и помещает его в буфер обмена.
     */
    void copy();
    /* При использовании действия copyAct, вызывается этот слот
     * Слот помещает выделенный текст в буфер обмена.
     */
    void paste();
    /* При использовании действия pasteAct, вызывается этот слот
     * Слот размещает текст из буфера обмена на место галочки.
     */
    void clear();
    /* При использовании действия clearAct, вызывается этот слот
     * Слот полностью очищает confEdit от текста.
     */
    void insertLh();
    /* При использовании действия lhAct, вызывается этот слот
     * Слот вставляет на место галочки текст "127.0.0.1"
     */
    void insertLhp();
    /* При использовании действия lhpAct, вызывается этот слот
     * Слот вставляет на место галочки текст "127.0.0.1" + данные из буфера обмена
     */
    bool createTempFile();
    /* При нажатии на кнопку useAsConf, вызывается этот слот
     * Слот переносит всю текущую написанную конфигурацию в confEdit в путь для конфигураций основной программы.
     * Делает он это с помощью временного файла, который после запуска приложения удаляется
     * Если выполняется успешно, отдает сигнал tempFileCreated
     */
private:
    QPushButton* useAsConf;                 QPushButton* createUseAsConf();
    QTextEdit* confEdit;                    QTextEdit* createConfEdit();
    QVBoxLayout *mainLay;                   QVBoxLayout* createAll();
    QMenuBar* menuBar; QToolBar *fileTools; void createBars();
    blocker* blockerRef;
    bool magnet; // Если виджет примагничен к основному окну - true, иначе - false
    /* Перегрузка функции отслеживает близость виджета к основному окну
     * Если виджет находится близко к краям основного окна, "примагничивает" его к ним.
     * С этого момента виджет примагничен и меняет позицию вместе с основным окном.
     * Примагничивание происходит насильной сменой расположения данного виджета, а также курсора
     * Расстояние примагничивания - 3 пиксела.
     */
    void moveEvent(QMoveEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    QString savePath;
};

#endif // CONFEDITOR_H
