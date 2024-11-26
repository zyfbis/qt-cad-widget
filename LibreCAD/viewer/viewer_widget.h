#pragma once
#include <QEvent>
#include <QString>
#include <QWidget>

QString LCReleaseLabel();
class QMdiArea;
class QC_MDIWindow;

class ViewerWidget : public QWidget
{
    Q_OBJECT
public:
    // 全局初始化和销毁，程序开始和结束时调用一次
    static void init(
        const QString &companyKey = "CompanyName",
        const QString &appKey = "AppName",
        const QString &fontDir = "fonts");
    static void destroy();

    ViewerWidget(QWidget *parent = nullptr);
    ~ViewerWidget();

public slots:
    bool slotOpen(const QString &fileName);                      // 打开新的DXF文件
    void slotClear();                                            // 清空所有图形
    void slotSetFont(const QString &fontName, bool onlyChinese); // 递归地更改全部Text的字体，onlyChinese为true时只更改包含中文的Text

protected:
    bool eventFilter(QObject *obj, QEvent *event) override; // 鼠标左右键事件改为中键

private:
    QMdiArea *mdiArea;
    QC_MDIWindow *mdiWindow;
};
