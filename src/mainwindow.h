#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyle>

#include "GlyphWidget.h"
#include "OpenCompositionDialog.h"

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

    QVBoxLayout *mainLayout;
    GlyphWidget *glyphWidget;

    QMenu *fileMenu;
    QAction *openFileAction;

    OpenCompositionDialog *openCompositionDialog;

    /**
     * @brief Loads the composition with the glyphWidget. Will be executed after the OpenCompositionDialog closes with the Accept return value.
     * @param openMode Contains the needed information to call the right loadComposition function.
     * @param str0 The first QString (most likely the path of the Audio file).
     * @param str1 The second QString (most likely light data in some form).
     */
    void processOpenCompositionDialogAccepted(const OpenCompositionDialog::CompositionOpenModeResult& openMode, const QString& str0, const QString& str1);

public slots:
    /**
     * @brief Will be called by the void QPushButton::clicked(bool checked) signal.
     * @param checked If the action is checked.
     */
    void button_onClicked(bool checked);
    /**
     * @brief Will be called by the void QAction::triggered(bool checked) signal.
     * @param checked If the action is checked.
     */
    void openFileAction_onTriggered(bool checked);
    /**
     * @brief openCompositionDialog_onFinished
     * @param result The result code.
     */
    void openCompositionDialog_onFinished(int result);

};
#endif // MAINWINDOW_H
