#include <QDir>
#include <QFileInfo>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include "viewer_widget.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 字体文件路径
    QDir codeDir = QFileInfo(__FILE__).dir();
    QDir fontDir = codeDir.filePath("../fonts");
    ViewerWidget::init("CompanyName", "AppName", fontDir.absolutePath());

    setWindowTitle("ViewerLib Example");
    resize(800, 600);

    auto *mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);

    auto *mainlayout = new QVBoxLayout();
    mainWidget->setLayout(mainlayout);

    auto *toolLayout = new QHBoxLayout();
    mainlayout->addLayout(toolLayout);

    auto *openButton = new QPushButton(QStringLiteral("打开"), this);
    auto *resetButton = new QPushButton(QStringLiteral("重置"), this);
    toolLayout->addWidget(openButton);
    toolLayout->addWidget(resetButton);
    connect(openButton, &QPushButton::clicked, this, &MainWindow::slotOpen);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::slotReset);

    auto *viewerWidget = new ViewerWidget(mainWidget);
    mainlayout->addWidget(viewerWidget);
}

MainWindow::~MainWindow()
{
    ViewerWidget::destroy();
}

void MainWindow::slotOpen()
{
    auto *viewerWidget = findChild<ViewerWidget *>();
    if (!viewerWidget)
    {
        QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("未找到ViewerWidget"));
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("打开DXF文件"),
        QStringLiteral("."),
        QStringLiteral("DXF文件(*.dxf);;所有文件(*.*)"));
    if (fileName.isEmpty())
    {
        return;
    }
    if (!viewerWidget->slotOpen(fileName))
    {
        QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("打开文件失败"));
    }
    viewerWidget->slotSetFont("simsun", true); // simhei
}

void MainWindow::slotReset()
{
    auto *viewerWidget = findChild<ViewerWidget *>();
    if (!viewerWidget)
    {
        QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("未找到ViewerWidget"));
        return;
    }
    viewerWidget->slotClear();
}