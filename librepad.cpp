// Copyright (C) 2024 Emanuel Strobel
// GPLv2

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QtPrintSupport/qtprintsupportglobal.h>
#include <QPrintDialog>
#include <QPrinter>
#include <QFont>
#include <QFontDialog>
#include <QPainter>
#include <QTabBar>
#include <QToolBar>

#include "librepad.h"
#include "texteditor.h"
#include "ui_librepad.h"

Librepad::Librepad(QWidget *parent, const QString& fileName)
    : QMainWindow(parent)
    , m_fileName(fileName)
    , ui(new Ui::Librepad)
{
    this->hide();
    ui->setupUi(this);
    setCentralWidget(ui->tabWidget);
    ui->tabWidget->tabBar()->setTabsClosable(true);

    m_searchLineEdit = new QLineEdit;
    m_searchLineEdit->setMaximumWidth(180);
    ui->searchToolBar->addWidget(m_searchLineEdit);

    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &Librepad::slotTabClose);
    connect(ui->actionNew, &QAction::triggered, this, &Librepad::newDocument);
    connect(ui->actionOpen, &QAction::triggered, this, &Librepad::open);
    connect(ui->actionSave, &QAction::triggered, this, &Librepad::save);
    connect(ui->actionSave_as, &QAction::triggered, this, &Librepad::saveAs);
    connect(ui->actionPrint, &QAction::triggered, this, &Librepad::print);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionUndo, &QAction::triggered, this, &Librepad::undo);
    connect(ui->actionRedo, &QAction::triggered, this, &Librepad::redo);
    connect(ui->actionFont, &QAction::triggered, this, &Librepad::setFont);
    connect(ui->actionAbout, &QAction::triggered, this, &Librepad::about);
    connect(ui->actionPrevious, &QAction::triggered, this, [=]() {
        slotSearchChanged(m_searchLineEdit->text(), false, false);
    });
    connect(ui->actionNext, &QAction::triggered, this, [=]() {
        slotSearchChanged(m_searchLineEdit->text(), true, false);
    });
    connect(m_searchLineEdit, &QLineEdit::textChanged, this, [=]() {
        slotSearchChanged(m_searchLineEdit->text(), true, true);});

    connect(ui->actionCopy, &QAction::triggered, this, &Librepad::copy);
    connect(ui->actionPaste, &QAction::triggered, this, &Librepad::paste);

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));

    addNewTab(m_fileName);
}

void Librepad::slotTabChanged(int index) {
    TextEditor *editor = dynamic_cast<TextEditor *>(ui->tabWidget->widget(index));
    if (editor == nullptr)
    {
        return;
    }

    connect(editor, &QPlainTextEdit::redoAvailable, ui->actionRedo, &QAction::setEnabled);
    connect(editor, &QPlainTextEdit::undoAvailable, ui->actionUndo, &QAction::setEnabled);
    connect(editor, &TextEditor::documentChanged, this, [=]() {
        ui->tabWidget->tabBar()->setTabText(index, editor->fileName());
        ui->tabWidget->tabBar()->setTabToolTip(index, editor->fileName());
    });

    setWindowTitle(editor->fileName());
    ui->tabWidget->tabBar()->setTabText(index, editor->fileName());
}

void Librepad::closeEvent(QCloseEvent *event)
{
    for(int i = 0; i < ui->tabWidget->count(); i++) {
        TextEditor *editor = dynamic_cast<TextEditor *>(ui->tabWidget->widget(i));
        if (editor == nullptr)
        {
            return;
        }

        if (editor->document()->isModified())
        {
            QMessageBox::StandardButton btn = QMessageBox::question(this,
                                                                    tr("Save document"),
                                                                    tr("The changes were not saved. Do you still want to close it?"));
            if (btn != QMessageBox::Yes)
            {
                event->ignore();
                ui->tabWidget->setCurrentWidget(editor);
                editor->saveAs();
            }
            else {
                event->accept();
            }
        }
    }
}

void Librepad::redo()
{
    TextEditor *editor = dynamic_cast<TextEditor *>(ui->tabWidget->widget(ui->tabWidget->currentIndex()));
    if (editor == nullptr)
    {
        return;
    }
    connect(editor, &QPlainTextEdit::redoAvailable, ui->actionRedo, &QAction::setEnabled);
    editor->redo();
}

void Librepad::undo()
{
    TextEditor *editor = dynamic_cast<TextEditor *>(ui->tabWidget->widget(ui->tabWidget->currentIndex()));
    if (editor == nullptr)
    {
        return;
    }
    connect(editor, &QPlainTextEdit::undoAvailable, ui->actionUndo, &QAction::setEnabled);
    editor->undo();
}

void Librepad::copy()
{
    TextEditor *editor = dynamic_cast<TextEditor *>(ui->tabWidget->widget(ui->tabWidget->currentIndex()));
    if (editor == nullptr)
    {
        return;
    }
    editor->copy();
}

void Librepad::paste()
{
    TextEditor *editor = dynamic_cast<TextEditor *>(ui->tabWidget->widget(ui->tabWidget->currentIndex()));
    if (editor == nullptr)
    {
        return;
    }
    editor->paste();
}

void Librepad::slotSearchChanged(const QString &text, bool direction, bool reset)
{
    QString search_text = text;
    if (search_text.trimmed().isEmpty())
    {
        return;
    }

    TextEditor *editor = dynamic_cast<TextEditor *>(ui->tabWidget->currentWidget());
    if (editor == nullptr)
    {
        return;
    }

    QTextDocument *document = editor->document();
    QTextCursor    cur      = editor->textCursor();

    static QList<QTextCursor> highlight_cursors;
    static int                index = 0;
    if (reset)
    {
        /* Traverse and search all */
        cur = editor->textCursor();
        cur.clearSelection();
        cur.movePosition(QTextCursor::Start);

        highlight_cursors.clear();
        QTextCursor highlight_cursor = document->find(search_text);
        while (!highlight_cursor.isNull())
        {
            highlight_cursors.append(highlight_cursor);
            highlight_cursor = document->find(search_text, highlight_cursor);
        }
    }
    else
    {
        if (direction)
        {
            index += 1;
        }
        else
        {
            index -= 1;
        }
        index = qMax(0, index);

        index = index % highlight_cursors.size();
    }

    QList<QTextEdit::ExtraSelection> list; /* = editor->extraSelections();*/

    if (highlight_cursors.size() > 0 && index < highlight_cursors.size())
    {
        QTextCharFormat highlightFormat;
        highlightFormat.setBackground(Qt::yellow);
        highlightFormat.setForeground(Qt::blue);
        QTextEdit::ExtraSelection selection;
        selection.cursor = highlight_cursors[index];
        selection.format = highlightFormat;

        list.append(selection);

        editor->setTextCursor(highlight_cursors[index]);
        editor->setExtraSelections(list);
    }
}

void Librepad::slotTabClose(int index)
{
    TextEditor *editor = dynamic_cast<TextEditor *>(ui->tabWidget->widget(index));
    if (editor == nullptr)
    {
        return;
    }

    if (editor->document()->isModified())
    {
        QMessageBox::StandardButton btn = QMessageBox::question(this,
                                                                tr("Save document"),
                                                                tr("The changes were not saved. Do you still want to close it?"));
        if (btn != QMessageBox::Yes)
        {
            ui->tabWidget->setCurrentWidget(editor);
            editor->saveAs();
        }
    }
    ui->tabWidget->removeTab(index);
    setWindowTitle("Librepad");
    delete editor;
}

void Librepad::addNewTab(QString fileName)
{
    QFileInfo info(fileName);
    TextEditor *editor = new TextEditor(this, fileName);

    ui->tabWidget->addTab(editor, info.fileName());
    int index = ui->tabWidget->count() - 1;
    ui->tabWidget->setCurrentIndex(index);
    ui->tabWidget->tabBar()->setTabText(index, editor->fileName());
    ui->tabWidget->tabBar()->setTabToolTip(index, editor->fileName());
    setWindowTitle(editor->fileName());

    connect(editor, &TextEditor::documentChanged, this, [=]() {
        setWindowTitle(editor->fileName());
        ui->tabWidget->tabBar()->setTabText(index, editor->fileName());
        ui->tabWidget->tabBar()->setTabToolTip(index, editor->fileName());
    });
    editor->setFocus();
}

Librepad::~Librepad()
{
    delete ui;
}

void Librepad::newDocument()
{
    addNewTab();
}

void Librepad::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file", "All Files (*.*)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }
    setWindowTitle(fileName);

    addNewTab(fileName);
}

void Librepad::save()
{
    TextEditor *editor = dynamic_cast<TextEditor *>(ui->tabWidget->widget(ui->tabWidget->currentIndex()));
    if (editor == nullptr)
    {
        return;
    }
    editor->save();
}

void Librepad::saveAs()
{
    TextEditor *editor = dynamic_cast<TextEditor *>(ui->tabWidget->widget(ui->tabWidget->currentIndex()));
    if (editor == nullptr)
    {
        return;
    }
    editor->saveAs();
}

void Librepad::print()
{
    TextEditor *editor = dynamic_cast<TextEditor *>(ui->tabWidget->widget(ui->tabWidget->currentIndex()));
    if (editor == nullptr)
    {
        return;
    }
    editor->printer();
}

void Librepad::setFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(
        &ok, QFont("", 10), this);
    if (ok) {
        TextEditor *editor = dynamic_cast<TextEditor *>(ui->tabWidget->widget(ui->tabWidget->currentIndex()));
        if (editor == nullptr)
        {
            return;
        }
        editor->setFont(font);
    }
}

void Librepad::about()
{
    QMessageBox::about(this,
                       tr("About Librepad"),
                       tr("<b>Librepad</b> is a code Editor<br>Emanuel Strobel GPLv2 (c) 2024</br>")
                      );
}

