// Copyright (C) 2024 Emanuel Strobel
// GPLv2

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QPlainTextEdit>

class LineNumberWidget;
class TextEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    TextEditor(QWidget *parent, const QString& fileName);
    ~TextEditor();

    void lineNumberPaintEvent(QPaintEvent *e);

    void load(QString fileName);
    bool save();
    bool saveAs();
    void printer();
    void update();

    QString getCompleteFileName() const;
    QString getFileName() const;

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
