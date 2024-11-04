// Copyright (C) 2024 Emanuel Strobel
// GPLv2

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QPlainTextEdit>
#include <QFileInfo>

class LineNumberWidget;
class TextEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    TextEditor(QWidget *parent, const QString& fileName);
    ~TextEditor();

    void lineNumberPaintEvent(QPaintEvent *e);

    void load(QString fileName);
    void save();
    void saveAs();
    void printer();

    QString path() const { return m_fileName; }

    QString fileName() const
    {
        QFileInfo info(m_fileName);
        return info.fileName();
    }

signals:
    void documentChanged();

public slots:
    void updateLineNumber(const QRect &rect, int dy);

protected:
    void resizeEvent(QResizeEvent *e) override;

private slots:
    void highlightCurrentLine();
    void updateLineNumberMargin();
    int getLineNumberWidth();

private:
    LineNumberWidget *m_lineNumberWidget;
    QString m_fileName;
    bool m_firstSave;

    void setFirstSave(bool state) { m_firstSave = state; }
    bool firstSave() const { return m_firstSave; }
    void saveFileContent(const QByteArray &fileContent, const QString &fileNameHint);
};

class LineNumberWidget : public QWidget
{
public:
    LineNumberWidget(TextEditor *editor);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    TextEditor *m_editor;
};

#endif   // TEXTEDITOR_H
