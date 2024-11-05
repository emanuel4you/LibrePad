// Copyright (C) 2024 Emanuel Strobel
// GPLv2

#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QMainWindow>
#include <QLineEdit>
#include <QCloseEvent>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui {
class Librepad;
}
QT_END_NAMESPACE

class Librepad : public QMainWindow
{
    Q_OBJECT
public:
    explicit Librepad(QWidget *parent = nullptr, const QString& fileName="");
    ~Librepad();

private slots:
    void slotTabChanged(int index);
    void slotSearchChanged(const QString &text, bool direction, bool reset);
    void slotTabClose(int index);
    void newDocument();
    void open();
    void save();
    void saveAs();
    void reload();
    void print();
    void undo();
    void redo();
    void copy();
    void paste();
    void setFont();
    void about();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QString m_fileName;
    QFont m_font;
    Ui::Librepad *ui;
    QLineEdit* m_searchLineEdit;

    void addNewTab(QString fileName = "");
    void writeSettings();
    void writeFontSettings();
    void readSettings();
};

#endif // NOTEPAD_H
