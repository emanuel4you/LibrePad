// Copyright (C) 2024 Emanuel Strobel
// GPLv2

#include "texteditor.h"

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QPainter>
#include <QTextBlock>
#include <QtPrintSupport/qtprintsupportglobal.h>
#include <QPrintDialog>
#include <QPrinter>
#include <QDir>


TextEditor::TextEditor(QWidget *parent, const QString& fileName)
    : QPlainTextEdit(parent)
    , m_lineNumberWidget(new LineNumberWidget(this))
    , m_fileName(fileName)
{
    setViewportMargins(25, 0, 0, 0);
    highlightCurrentLine();

    connect(this, &QPlainTextEdit::updateRequest, this, &TextEditor::updateLineNumber);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &TextEditor::highlightCurrentLine);
    connect(this, &QPlainTextEdit::blockCountChanged, this, &TextEditor::updateLineNumberMargin);

    //load(m_fileName);
}

TextEditor::~TextEditor()
{
    delete m_lineNumberWidget;
    m_lineNumberWidget = nullptr;
}

void TextEditor::update()
{
    document()->setModified(false);
    emit documentChanged();
}

void TextEditor::lineNumberPaintEvent(QPaintEvent *e)
{
    QTextBlock block = firstVisibleBlock();
    QPainter painter(m_lineNumberWidget);
    painter.fillRect(e->rect(), QColor(200, 200, 200, 100));
    painter.setPen(QColor(80, 80, 80));

    int top    = blockBoundingGeometry(block).translated(contentOffset()).top() + 1;
    int bottom = top + blockBoundingGeometry(block).height();

    // qDebug() << "start " << top;

    while (block.isValid() && top <= e->rect().bottom())
    {
        int lineNumber = block.blockNumber();
        int lineHeight = blockBoundingGeometry(block).height();
        if (!block.next().isValid())
        {
            lineHeight -= 4;
        }

        // qDebug() << "lineNumbe" << lineNumber << "top " << top << "bottom " << bottom;
        QRect rect(0, top, getLineNumberWidth() - 2, lineHeight);
        QFont font = painter.font();
        font.setPointSize(12);
        painter.setFont(font);
        painter.drawText(rect, Qt::AlignRight | Qt::AlignVCenter, QString::number(lineNumber + 1));

        block  = block.next();
        top    = bottom;
        bottom = top + blockBoundingGeometry(block).height();
    }
}

void TextEditor::load(QString fileName)
{
    if(fileName == "") {
        m_fileName = tr("newfile.txt");
        document()->setModified(false);
        emit documentChanged();
        return;
    }

    QFile file(fileName);
    if (!file.exists())
    {
        QMessageBox::warning(this, tr("Warning"), tr("file not found: ") + file.errorString());
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::critical(this, tr("Critical"), tr("Cannot read file: ") + file.errorString());
        return;
    }

    QString text = file.readAll();
    file.close();

    setPlainText(text);
    document()->setModified(false);
    emit documentChanged();
}

bool TextEditor::save()
{
    QFile file(m_fileName);

    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        QMessageBox::critical(this, tr("Critical"), tr("Cannot write file: ") + file.errorString());
        return false;
    }

    QTextStream stream(&file);
    stream << toPlainText();
    stream.flush();
    file.close();

    document()->setModified(false);
    emit documentChanged();

    return true;
}

bool TextEditor::saveAs()
{
    QFileDialog::saveFileContent(toPlainText().toUtf8(), QApplication::applicationDirPath() + QString(QDir::separator()) + m_fileName);
    document()->setModified(false);
    emit documentChanged();

    return true;
}

void TextEditor::printer()
{
    if (m_fileName.isEmpty())
    {
        return;
    }
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Rejected) {
        return;
    }
    print(&printer);
}

QString TextEditor::getCompleteFileName() const
{
    return m_fileName;
}

QString TextEditor::getFileName() const
{
    QFileInfo info(m_fileName);
    QString name = "";
    if (info.exists())
    {
        name = info.fileName();
    }
    return name;
}

void TextEditor::updateLineNumber(const QRect &rect, int dy)
{
    if (dy > 0)
    {
        m_lineNumberWidget->scroll(0, dy);
    }
    m_lineNumberWidget->update(0, rect.y(), getLineNumberWidth(), rect.height());
}

void TextEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    m_lineNumberWidget->setGeometry(0, 0, getLineNumberWidth(), contentsRect().height());
}

void TextEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(0, 100, 100, 20));
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();

    extraSelections.append(selection);

    setExtraSelections(extraSelections);
}

void TextEditor::updateLineNumberMargin()
{
    setViewportMargins(getLineNumberWidth(), 0, 0, 0);
}

int TextEditor::getLineNumberWidth()
{
    int defalut = 22;
    defalut     = 4 + QString::number(blockCount()).length() * fontMetrics().horizontalAdvance('0');
    defalut     = qMax(22, defalut);

    return defalut;
}

LineNumberWidget::LineNumberWidget(TextEditor *editor)
    : QWidget(editor)
{
    m_editor = editor;
}

void LineNumberWidget::paintEvent(QPaintEvent *event)
{
    m_editor->lineNumberPaintEvent(event);
}
