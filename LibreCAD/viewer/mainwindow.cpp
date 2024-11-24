#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "viewer_widget.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("ViewerLib Example");
    resize(800, 600);

    auto *mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);

    auto *mainlayout = new QVBoxLayout();
    mainWidget->setLayout(mainlayout);

    auto *toolLayout = new QHBoxLayout();
    mainlayout->addLayout(toolLayout);

    auto *button1 = new QPushButton("Button 1", this);
    auto *button2 = new QPushButton("Button 2", this);
    toolLayout->addWidget(button1);
    toolLayout->addWidget(button2);

    auto *viewerWidget = new ViewerWidget(mainWidget);
    mainlayout->addWidget(viewerWidget);
}

MainWindow::~MainWindow()
{
}
