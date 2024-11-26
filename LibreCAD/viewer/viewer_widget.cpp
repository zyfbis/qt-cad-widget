#include <queue>
#include <unordered_set>
#include <QMdiArea>
#include <QVBoxLayout>
#include <QMouseEvent>

#include "rs_debug.h"
#include "rs_graphic.h"
#include "rs_settings.h"
#include "qc_mdiwindow.h"
#include "qg_graphicview.h"
#include "qc_applicationwindow.h"

#include "rs_fontlist.h"
#include "rs_document.h"
#include "rs_blocklist.h"
#include "rs_block.h"
#include "rs_entity.h"
#include "rs_text.h"
#include "rs_mtext.h"

#include "viewer_widget.h"

QString LCReleaseLabel()
{
    return QStringLiteral("ViewerWidget");
}

void ViewerWidget::init(
    const QString &companyKey,
    const QString &appKey,
    const QString &fontDir)
{
    RS_DEBUG->setLevel(RS_Debug::D_NOTHING);
    RS_Settings::init(companyKey, appKey);
    {
        RS_Settings::instance()->beginGroup("Paths");
        RS_Settings::instance()->write("Fonts", fontDir);
        RS_Settings::instance()->endGroup();
    }
    RS_FontList::instance()->init();
}

void ViewerWidget::destroy()
{
    QC_ApplicationWindow::getAppWindow().reset();
}

ViewerWidget::ViewerWidget(QWidget *parent)
    : QWidget(parent), mdiArea(nullptr), mdiWindow(nullptr)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mdiArea = new QMdiArea(this);
    mdiArea->setObjectName("mdiArea");
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setFocusPolicy(Qt::ClickFocus);
    mdiArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);
    mdiArea->setTabsMovable(true);
    mdiArea->setTabsClosable(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mdiArea);
    setLayout(layout);

    mdiWindow = new QC_MDIWindow(nullptr, mdiArea);
    mdiWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mdiWindow->setWindowFlag(Qt::FramelessWindowHint, true);
    mdiWindow->showMaximized();
    QC_ApplicationWindow::getAppWindow()->slotWindowActivated(mdiWindow);

    QG_GraphicView *graphicView = mdiWindow->getGraphicView();
    graphicView->installEventFilter(this);
}

ViewerWidget::~ViewerWidget()
{
}

bool ViewerWidget::slotOpen(const QString &fileName)
{
    return mdiWindow->slotFileOpen(fileName, RS2::FormatType::FormatUnknown);
}

void ViewerWidget::slotClear()
{
    mdiWindow->slotFileNew();
}

static bool containsChinese(const QString &text)
{
    static QRegularExpression regExp(QStringLiteral("[一-龥]")); // 中文字符范围
    return regExp.match(text).hasMatch();
}

void ViewerWidget::slotSetFont(const QString &fontName, bool onlyChinese)
{
    RS_Document *doc = mdiWindow->getDocument();
    std::queue<RS_Entity *> nodeQueue;
    std::unordered_set<RS_Entity *> visited;
    nodeQueue.push(doc);
    while (!nodeQueue.empty())
    {
        RS_Entity *entity = nodeQueue.front();
        nodeQueue.pop();
        if (visited.find(entity) != visited.end())
        {
            continue;
        }
        visited.insert(entity);

        // block or document
        if (RS_Document *doc = dynamic_cast<RS_Document *>(entity))
        {
            RS_BlockList *blockList = doc->getBlockList();
            for (int i = 0; i < blockList->count(); i++)
            {
                RS_Block *block = blockList->at(i);
                nodeQueue.push(block);
            }
        }
        // entity container
        if (RS_EntityContainer *container = dynamic_cast<RS_EntityContainer *>(entity))
        {
            for (int i = 0; i < container->count(); i++)
            {
                RS_Entity *entity = container->entityAt(i);
                nodeQueue.push(entity);
            }
        }
    }

    // change font for all text and mtext
    for (RS_Entity *entity : visited)
    {
        if (entity->rtti() == RS2::EntityText)
        {
            RS_Text *text = static_cast<RS_Text *>(entity);
            if (onlyChinese && !containsChinese(text->getText()))
            {
                continue;
            }
            text->setStyle(fontName);
            // text->update();
        }
        if (entity->rtti() == RS2::EntityMText)
        {
            RS_MText *mtext = static_cast<RS_MText *>(entity);
            if (onlyChinese && !containsChinese(mtext->getText()))
            {
                continue;
            }
            mtext->setStyle(fontName);
            // mtext->update();
        }
    }
    doc->update(); // update all entities at once
}

bool ViewerWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (QG_GraphicView *graphicView = dynamic_cast<QG_GraphicView *>(obj))
    {
        if (QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent *>(event))
        {
            if (mouseEvent->button() == Qt::LeftButton || mouseEvent->button() == Qt::RightButton)
            {
                QMouseEvent midEvent(
                    mouseEvent->type(),       // 保留原事件的类型（按下或释放）
                    mouseEvent->pos(),        // 鼠标位置（相对于目标对象）
                    Qt::MiddleButton,         // 模拟中键
                    Qt::MiddleButton,         // 按下的按钮（中键）
                    mouseEvent->modifiers()); // 原事件的键盘修饰符
                QApplication::sendEvent(graphicView, &midEvent);
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}
