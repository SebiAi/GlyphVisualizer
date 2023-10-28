#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "GlyphWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QVBoxLayout *vBoxLayout_mainLayout;
    GlyphWidget *glyphWidget;

public slots:
    void button_onClicked(bool checked);

};
#endif // MAINWINDOW_H
