#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Add style sheets
    // TODO: [END] Test style sheet in white and black mode
    this->setStyleSheet("QLabel {color: rgb(240, 242, 242);} QMainWindow {background: rgb(30, 32, 34);} QMenuBar {background: rgb(47, 48, 51);color: rgb(240, 242, 242)}");

    // Add 'File' entry to MenuBar
    ui->menubar->addMenu("File");

    // Set window title
    this->setWindowTitle("GlyphVisualizer");

    // Set vertical layout
    vBoxLayout_mainLayout = new QVBoxLayout(ui->centralwidget);
    vBoxLayout_mainLayout->setSpacing(20);
    ui->centralwidget->setLayout(vBoxLayout_mainLayout);

    // Add GlyphWidget
    glyphWidget = new GlyphWidget(); // No parent needed - layout does this for us
    ui->centralwidget->layout()->addWidget(glyphWidget);

    // Test layout
    QLabel *label = new QLabel("Hello World!"); // No parent needed - layout does this for us
    ui->centralwidget->layout()->addWidget(label);
    ui->centralwidget->layout()->setAlignment(label, Qt::AlignHCenter);

    QPushButton *button = new QPushButton("Toggle Phone (1) and Phone (2) Glyphs"); // No parent needed - layout does this for us
    connect(button, SIGNAL(clicked(bool)), this, SLOT(button_onClicked(bool)));
    ui->centralwidget->layout()->addWidget(button);
    ui->centralwidget->layout()->setAlignment(button, Qt::AlignHCenter);
}

void MainWindow::button_onClicked(bool checked)
{
    switch (glyphWidget->getVisual())
    {
        case GlyphWidget::Visual::Phone1:
            glyphWidget->setVisual(GlyphWidget::Visual::Phone2);
            break;
        case GlyphWidget::Visual::Phone2:
            glyphWidget->setVisual(GlyphWidget::Visual::Phone1);
            break;
    }
}

MainWindow::~MainWindow()
{
    delete ui;

    delete vBoxLayout_mainLayout;
    delete glyphWidget;
}

