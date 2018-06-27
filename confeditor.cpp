#include "confeditor.h"
#include "blocker.h"
#include <QApplication>
#include <QDebug>
#include <QMoveEvent>
#include <QPoint>
#include <QCursor>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QDateTime>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QClipboard>
// Используется в расчетах для магнита
#define LEFT_EDGE 16
#define RIGHT_EDGE 19
#define SEPARATOR 16
#define TITLE_HEIGHT 38
#define DOWN_EDGE 3
#define UPPER_EDGE 0
confEditor::confEditor(blocker *mainBlocker, QWidget *mainWindow) :  QWidget(mainWindow)
{
    this->setWindowTitle("Редактор конфигураций");
    this->setLayout(createAll());
    blockerRef = mainBlocker;
    magnet = false;
    this->setMinimumWidth(210);
    this->setMaximumSize(475, 300);
    savePath = "";
}

// Примагничен ли виджет к основному окну
bool confEditor::isMagnetic()
{
    return magnet;
}

// Операция "Открыть"
bool confEditor::openFile() // СЛОТ
{
    const QString openFilePath = QFileDialog::getOpenFileName(nullptr, nullptr, nullptr, tr("Любые файлы (*.*);;Текстовые файлы (*.txt)"));
    if(openFilePath == savePath || openFilePath == "") return true;
    QFile openedFile; openedFile.setFileName(openFilePath);
    if(openedFile.open(QIODevice::ReadOnly))
    {
        QByteArray fileContent = openedFile.readAll();
        confEdit->setText(fileContent);
        savePath = openFilePath;
        openedFile.close();
        return true;
    }
    QMessageBox cantOpen;
    cantOpen.setWindowTitle("Редактор конфигураций");
    cantOpen.setText("Не удалось открыть файл.");
    cantOpen.setIcon(QMessageBox::Critical);
    cantOpen.exec();
    return false;
}

// Операция "Сохранить"
bool confEditor::saveFile() // СЛОТ
{
    if(savePath == "")
    {
        QMessageBox::StandardButton conf;
        conf = QMessageBox::question(0,
                                     "Редактор конфигураций",
                                     "Файла сохранений не существует. Сохранить в стандартный файл? (путь: " + QString(PRO_FILE_PWD) + ")",
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::Yes);
        if(conf == QMessageBox::Yes)
        {
            QDateTime datatime;
            QFile saveFile; saveFile.setFileName("redo_conf_" + datatime.currentDateTime().toString("dd.MM.yyyy.hh.mm.ss"));
            if(saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QByteArray toSave = "\n# Records added by site blocker:\n";
                saveFile.write(toSave);
                toSave = confEdit->toPlainText().toUtf8();
                saveFile.write(toSave);
                saveFile.close();
                return true;
            }
        }
    }
    else
    {
        QFile saveFile; saveFile.setFileName(savePath);
        if(saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QByteArray toSave = "\n# Records added by site blocker:\n";
            saveFile.write(toSave);
            toSave = confEdit->toPlainText().toUtf8();
            saveFile.write(toSave);
            saveFile.close();
            return true;
        }
    }
    QMessageBox cantSave;
    cantSave.setWindowTitle("Редактор конфигураций");
    cantSave.setText("Не удалось сохранить файл.");
    cantSave.setIcon(QMessageBox::Critical);
    cantSave.exec();
    return false;
}

// Операция "Сохранить как"
bool confEditor::saveAsFile() // СЛОТ
{
    const QString saveFilePath = QFileDialog::getSaveFileName(nullptr, nullptr, nullptr, tr("Текстовые файлы (*.txt)"));
    if(saveFilePath == "") return true;
    QFile saveFile; saveFile.setFileName(saveFilePath);
    if(saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QByteArray toSave = "\n# Records added by site blocker:\n";
        saveFile.write(toSave);
        toSave = confEdit->toPlainText().toUtf8();
        saveFile.write(toSave);
        savePath = saveFilePath;
        return true;
    }
    QMessageBox cantSave;
    cantSave.setWindowTitle("Редактор конфигураций");
    cantSave.setText("Не удалось сохранить файл.");
    cantSave.setIcon(QMessageBox::Critical);
    cantSave.exec();
    return false;
}

// Операция отмены
void confEditor::undo() // СЛОТ
{
    QTextDocument *document = confEdit->document();
    if(document->isUndoAvailable())
    document->undo();
}
// Операция возврата
void confEditor::redo() // СЛОТ
{
    QTextDocument *document = confEdit->document();
    if(document->isRedoAvailable())
    document->redo();
}
// Операция "Вырезать"
void confEditor::cut() // СЛОТ
{
    QApplication::clipboard()->clear(QClipboard::Clipboard);
    confEdit->cut();
}
// Операция "Копировать"
void confEditor::copy() // СЛОТ
{
    QApplication::clipboard()->clear(QClipboard::Clipboard);
    confEdit->copy();
}
// Операция "Вставить"
void confEditor::paste() // СЛОТ
{
    confEdit->paste();
}
// Операция "Очистить"
void confEditor::clear() // СЛОТ
{
    confEdit->clear();
}

// Операция "Вставить localhost"
void confEditor::insertLh() // СЛОТ
{
    confEdit->insertPlainText("127.0.0.1");
}

// Операция "Вставить localhost + буфер обмена"
void confEditor::insertLhp() // СЛОТ
{
    confEdit->insertPlainText("127.0.0.1 " + QApplication::clipboard()->text(QClipboard::Clipboard));
}

bool confEditor::createTempFile()
{
    QFile tempFile; tempFile.setFileName("tempfile");
    if(tempFile.exists())
    {
        tempFile.remove();
    }
    if(tempFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QByteArray toTransmit = "\n# Records added by site blocker:\n";
        tempFile.write(toTransmit);
        toTransmit = confEdit->toPlainText().toUtf8();
        tempFile.write(toTransmit);
        tempFile.close();
        emit tempFileCreated();
        return true;
    }
    return false;
}

QPushButton *confEditor::createUseAsConf()
{
    // Создается кнопка с мин размером 100x30
    // Кнопка связана со слотом createTempFile, который создает временный файл, куда помещается вся конфигурация.
    // Временный файл сразу же будет вставлен основное окно как конфигурация
    // Файл будет удален сразу же после старта программы
    useAsConf = new QPushButton(this);
    useAsConf->setMinimumSize(100, 30);
    useAsConf->setText("В конфигурацию");
    connect(useAsConf, SIGNAL(clicked()), this, SLOT(createTempFile()));
    return useAsConf;
}

QTextEdit *confEditor::createConfEdit()
{
    // Тут создаем TextEdit, в который пользователь записывает конфигурацию
    confEdit = new QTextEdit(this);
    return confEdit;
}

QVBoxLayout *confEditor::createAll()
{
    // Расположение виджетов на окне
    mainLay = new QVBoxLayout(this);
    QHBoxLayout* lowerEnd = new QHBoxLayout;

    lowerEnd->addStretch();
    lowerEnd->addWidget(createUseAsConf());
    mainLay->addSpacing(40);
    mainLay->addWidget(createConfEdit());
    mainLay->addStretch();
    mainLay->addLayout(lowerEnd);
    createBars();
    return mainLay;
}

void confEditor::createBars()
{
    // Основной меню-бар
    menuBar = new QMenuBar(this);
    // Основной тулбар
    fileTools = new QToolBar(this);
    fileTools->resize(300, 24);
    menuBar->resize(300, 24);
    fileTools->move(this->x() + 10, this->y() + 24);
    // Файл-меню содержит операции для работы с файлами (открыть, сохранить, закрыть)
    QMenu *fileMenu = menuBar->addMenu(tr("&Файл"));

    // Операция "Открыть..."
    // Открыть указанный в проводнике текстовый файл
    // Содержимое файла отобразится в *confEdit
    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.ico"));
    QAction *openAct = new QAction(openIcon, tr("&Открыть..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, QAction::triggered, this, confEditor::openFile);
    fileMenu->addAction(openAct);
    fileTools->addAction(openAct);

    // Операция "Сохранить..."
    // Сохранить содержимое *confEdit в выбранный при открытии файл.
    // Если никаких файлов не открывалось - пользователя оповещает диалог, свидетельствующий о сохранении в стандартный файл.
    fileMenu->addSeparator();
    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.ico"));
    QAction *saveAct = new QAction(saveIcon, tr("&Сохранить..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    connect(saveAct, QAction::triggered, this, confEditor::saveFile);
    fileMenu->addAction(saveAct);
    fileTools->addAction(saveAct);

    // Операция "Сохранить как..."
    // Сохранить содержимое *confEdit в указанный в проводнике файл.
    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as", QIcon(":/images/saveAs.ico"));
    QAction *saveAsAct = new QAction(saveAsIcon, tr("&Сохранить как..."), this);
    saveAct->setShortcuts(QKeySequence::SaveAs);
    connect(saveAsAct, QAction::triggered, this, confEditor::saveAsFile);
    fileMenu->addAction(saveAsAct);

    // Операция "Закрыть"
    // Закрыть окно редактора конфигураций
    fileMenu->addSeparator();
    QAction *closeAct = new QAction(tr("&Закрыть"), this);
    closeAct->setShortcuts(QKeySequence::Close);
    connect(closeAct, QAction::triggered, this, confEditor::hide);
    fileMenu->addAction(closeAct);

    fileTools->addSeparator();
    // Правочное меню содержит операции для работы с текстом(копировать, вставить, вырезать, очистить, отмена и возврат)
    QMenu *editMenu = menuBar->addMenu(tr("&Правка"));

    // Операция "Отмена"
    // Отменяет последнее изменение (undo)
    const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon(":/images/undo.ico"));
    QAction *undoAct = new QAction(undoIcon, tr("&Отменить"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    connect(undoAct, QAction::triggered, this, confEditor::undo);
    editMenu->addAction(undoAct);
    fileTools->addAction(undoAct);

    // Операция "Возврат"
    // Возвращает последнюю изменение до отмены (redo)
    const QIcon redoIcon = QIcon::fromTheme("edit-redo", QIcon(":/images/redo.ico"));
    QAction *redoAct = new QAction(redoIcon, tr("&Вернуть"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    connect(redoAct, QAction::triggered, this, confEditor::redo);
    editMenu->addAction(redoAct);
    fileTools->addAction(redoAct);

    // Операция "Вырезать"
    // Удаляет выделенный текст и помещает его в буффер обмена
    editMenu->addSeparator();
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.ico"));
    QAction *cutAct = new QAction(cutIcon, tr("&Вырезать"), this);
    connect(cutAct, QAction::triggered, this, confEditor::cut);
    cutAct->setShortcuts(QKeySequence::Cut);
    editMenu->addAction(cutAct);

    // Операция "Копировать"
    // Помещает выделенный текст в буфер обмена
    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.ico"));
    QAction *copyAct = new QAction(copyIcon, tr("&Копировать"), this);
    connect(copyAct, QAction::triggered, this, confEditor::copy);
    copyAct->setShortcuts(QKeySequence::Copy);
    editMenu->addAction(copyAct);

    // Операция "Вставить"
    // Вставляет текст из буфера обмена в текущую позицию галочки
    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.ico"));
    QAction *pasteAct = new QAction(pasteIcon, tr("&Вставить"), this);
    connect(pasteAct, QAction::triggered, this, confEditor::paste);
    pasteAct->setShortcuts(QKeySequence::Paste);
    editMenu->addAction(pasteAct);

    // Операция "Очистить"
    // Удаляет весь текст в confEdit
    editMenu->addSeparator();
    const QIcon clearIcon = QIcon::fromTheme("edit-clear", QIcon(":/images/trash.ico"));
    QAction *clearAct = new QAction(clearIcon, tr("&Очистить"), this);
    connect(clearAct, QAction::triggered, this, confEditor::clear);
    editMenu->addAction(clearAct);

    QMenu *toolsMenu = menuBar->addMenu(tr("&Инструменты"));

    // Операция "Вставить localhost"
    // Вставляет в выбранный на тексте участок текст "127.0.0.1"
    fileTools->addSeparator();
    const QIcon lhIcon = QIcon(":/images/lh.ico");
    QAction *lhAct = new QAction(lhIcon, tr("&Вставить localhost"), this);
    lhAct->setShortcut(QKeySequence("Ctrl+L"));
    connect(lhAct, QAction::triggered, this, confEditor::insertLh);
    toolsMenu->addAction(lhAct);
    fileTools->addAction(lhAct);

    // Операция "Вставить localhost + Вставить из буфера"
    // Вставляет в выбранный на тексте участок текст "127.0.0.1" и, через пробел, данные из буфера.
    const QIcon lhpIcon = QIcon(":/images/lhp.ico");
    QAction *lhpAct = new QAction(lhpIcon, tr("&Вставить localhost + буфер обмена"), this);
    lhpAct->setShortcut(QKeySequence("Ctrl+N"));
    connect(lhpAct, QAction::triggered, this, confEditor::insertLhp);
    toolsMenu->addAction(lhpAct);
    fileTools->addAction(lhpAct);

}

void confEditor::moveEvent(QMoveEvent *event)
{
    // Переопределение событийного обработчика позволяет окну текущего объекта магнититься к краям основного окна.
    // Из недочетов - резкие подергивания окна при перемещении, когда окно уже примагничено.
    // UPD - исправлено путем изменения расположения курсора после "намагничивания"

    // Магнит к правому верхнему углу основного окна
    if(((blockerRef->x() + blockerRef->width()) - this->x() <= -LEFT_EDGE && (blockerRef->x() + blockerRef->width()) - this->x() >= -RIGHT_EDGE) && (blockerRef->y() - this->y() >= -7 && blockerRef->y() - this->y() <= 4))
    {
        QCursor::setPos((blockerRef->x() + blockerRef->width() + SEPARATOR) + (QCursor::pos().x() - this->x()), QCursor::pos().y());
        this->move(blockerRef->x() + blockerRef->width() + SEPARATOR, blockerRef->y());
    }
    // Магнит к правому нижнему углу основного окна
    else if(((blockerRef->x() + blockerRef->width()) - this->x() <= -LEFT_EDGE && (blockerRef->x() + blockerRef->width()) - this->x() >= -RIGHT_EDGE) && ((blockerRef->y() + blockerRef->height()) - (this->y() + this->height()) >= -7 && ((blockerRef->y() + blockerRef->height()) - (this->y() + this->height()) <= 4)))
    {
        QCursor::setPos((blockerRef->x() + blockerRef->width() + SEPARATOR) + (QCursor::pos().x() - this->x()), QCursor::pos().y());
        this->move(blockerRef->x() + blockerRef->width() + SEPARATOR, blockerRef->y() + blockerRef->height() - this->height());
    }
    // Магнит к правой стороне основного окна
    else if(((blockerRef->x() + blockerRef->width()) - this->x() <= -LEFT_EDGE && (blockerRef->x() + blockerRef->width()) - this->x() >= -RIGHT_EDGE) && ((this->y() + this->height()) >= (blockerRef->y() - TITLE_HEIGHT) && this->y() <= (blockerRef->y() + blockerRef->height() + TITLE_HEIGHT)))
    {
        QCursor::setPos((blockerRef->x() + blockerRef->width() + SEPARATOR) + (QCursor::pos().x() - this->x()), QCursor::pos().y());
        this->move(blockerRef->x() + blockerRef->width() + SEPARATOR, this->y());
    }
    // Магнит к левому верхнему углу основного окна
    else if((blockerRef->x() - (this->x() + this->width()) >= LEFT_EDGE && blockerRef->x() - (this->x() + this->width()) <= RIGHT_EDGE) && (blockerRef->y() - this->y() >= -7 && blockerRef->y() - this->y() <= 4))
    {
        QCursor::setPos((blockerRef->x() - SEPARATOR) + (QCursor::pos().x() - (this->x() + this->width())), QCursor::pos().y());
        this->move(blockerRef->x() - this->width() - SEPARATOR, blockerRef->y());
    }
    // Магнит к левому нижнему углу основного окна
    else if((blockerRef->x() - (this->x() + this->width()) >= LEFT_EDGE && blockerRef->x() - (this->x() + this->width()) <= RIGHT_EDGE) && ((blockerRef->y() + blockerRef->height()) - (this->y() + this->height()) >= -7 && ((blockerRef->y() + blockerRef->height()) - (this->y() + this->height()) <= 4)))
    {
        QCursor::setPos((blockerRef->x() - SEPARATOR) + (QCursor::pos().x() - (this->x() + this->width())), QCursor::pos().y());
        this->move(blockerRef->x() - this->width() - SEPARATOR, blockerRef->y() + blockerRef->height() - this->height());
    }
    // Магнит к левой стороне основного окна
    else if(blockerRef->x() - (this->x() + this->width()) >= LEFT_EDGE && blockerRef->x() - (this->x() + this->width()) <= RIGHT_EDGE && ((this->y() + this->height()) >= (blockerRef->y() - TITLE_HEIGHT) && this->y() <= (blockerRef->y() + blockerRef->height() + TITLE_HEIGHT)))
    {
        QCursor::setPos((blockerRef->x() - SEPARATOR) + (QCursor::pos().x() - (this->x() + this->width())), QCursor::pos().y());
        this->move(blockerRef->x() - this->width() - SEPARATOR, this->y());
    }
    // Магнит к нижнему правому углу
    else if(((blockerRef->y() + blockerRef->height()) - this->y() + TITLE_HEIGHT <= -UPPER_EDGE && (blockerRef->y() + blockerRef->height()) - this->y() + TITLE_HEIGHT >= -DOWN_EDGE) && ((blockerRef->x() + blockerRef->width() + SEPARATOR) - (this->x() + this->width()) >= LEFT_EDGE && (blockerRef->x() + blockerRef->width() + SEPARATOR) - (this->x() + this->width()) <= RIGHT_EDGE))
    {
        this->move(blockerRef->x() + blockerRef->width() - this->width(), blockerRef->y() + blockerRef->height() + TITLE_HEIGHT);
    }
    // Магнит к нижнему левому углу
    else if(((blockerRef->y() + blockerRef->height()) - this->y() + TITLE_HEIGHT <= -UPPER_EDGE && (blockerRef->y() + blockerRef->height()) - this->y() + TITLE_HEIGHT >= -DOWN_EDGE) && (blockerRef->x() - SEPARATOR - this->x() <= -LEFT_EDGE && blockerRef->x() - SEPARATOR - this->x() >= -RIGHT_EDGE))
    {
        this->move(blockerRef->x(), blockerRef->y() + blockerRef->height() + TITLE_HEIGHT);
    }
    // Магнит к нижней стороне основного окна
    else if(((blockerRef->y() + blockerRef->height()) - this->y() + TITLE_HEIGHT <= -UPPER_EDGE && (blockerRef->y() + blockerRef->height()) - this->y() + TITLE_HEIGHT >= -DOWN_EDGE) && ((this->x() + this->width()) >= blockerRef->x() - SEPARATOR && this->x() <= blockerRef->x() + blockerRef->width() + SEPARATOR))
    {
        QCursor::setPos(QCursor::pos().x(), blockerRef->y() + blockerRef->height() + TITLE_HEIGHT + (QCursor::pos().y() - this->y()));
        this->move(this->x(), blockerRef->y() + blockerRef->height() + TITLE_HEIGHT);
    }
    // Магнит к верхнему правому углу основного окна
    else if(((blockerRef->y() - TITLE_HEIGHT) - (this->y() + this->height()) >= UPPER_EDGE && (blockerRef->y() - TITLE_HEIGHT) - (this->y() + this->height()) <= DOWN_EDGE) && ((blockerRef->x() + blockerRef->width() + SEPARATOR) - (this->x() + this->width()) >= LEFT_EDGE && (blockerRef->x() + blockerRef->width() + SEPARATOR) - (this->x() + this->width()) <= RIGHT_EDGE))
    {
        this->move(blockerRef->x() + blockerRef->width() - this->width(), blockerRef->y() - this->height() - TITLE_HEIGHT);
    }
    // Магнит к верхнему левому углу основного окна
    else if(((blockerRef->y() - TITLE_HEIGHT) - (this->y() + this->height()) >= UPPER_EDGE && (blockerRef->y() - TITLE_HEIGHT) - (this->y() + this->height()) <= DOWN_EDGE) && (blockerRef->x() - SEPARATOR - this->x() <= -LEFT_EDGE && blockerRef->x() - SEPARATOR - this->x() >= -RIGHT_EDGE))
    {
        this->move(blockerRef->x(), blockerRef->y() - this->height() - TITLE_HEIGHT);
    }
    // Магнит к верхней стороне основного окна
    else if(((blockerRef->y() - TITLE_HEIGHT) - (this->y() + this->height()) >= UPPER_EDGE && (blockerRef->y() - TITLE_HEIGHT) - (this->y() + this->height()) <= DOWN_EDGE) && ((this->x() + this->width()) >= blockerRef->x() - SEPARATOR && this->x() <= blockerRef->x() + blockerRef->width() + SEPARATOR))
    {
        QCursor::setPos(QCursor::pos().x(), blockerRef->y() - TITLE_HEIGHT + (QCursor::pos().y() - (this->y() + this->height())));
        this->move(this->x(), blockerRef->y() - this->height() - TITLE_HEIGHT);
    }
    // Окно в текущий момент не магнитится
    else
    {
        magnet = false;
        return;
    }
    magnet = true;
}

void confEditor::closeEvent(QCloseEvent *event)
{
    this->hide();
    blockerRef->changeButtonText();
    event->ignore();
}
