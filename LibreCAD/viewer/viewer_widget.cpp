#include <QMdiArea>
#include <QVBoxLayout>
#include <QMouseEvent>
#include "rs_graphic.h"
#include "rs_settings.h"
#include "qc_mdiwindow.h"
#include "qg_graphicview.h"
#include "qc_applicationwindow.h"
#include "viewer_widget.h"

#include "rs_fontlist.h"
#include "rs_document.h"
#include "rs_blocklist.h"
#include "rs_block.h"
#include "rs_entity.h"
#include "rs_text.h"
#include "rs_mtext.h"

static void changeFontInCot(RS_EntityContainer *container, const QString &fontName)
{
    for (int i = 0; i < container->count(); i++)
    {
        RS_Entity *entity = container->entityAt(i);
        if (entity->rtti() == RS2::EntityText)
        {
            RS_Text *text = static_cast<RS_Text *>(entity);
            text->setStyle(fontName);
            text->update();
            qDebug() << "Text: " << text->getText()
                     << " Style: " << text->getStyle();
        }
        if (entity->rtti() == RS2::EntityMText)
        {
            RS_MText *mtext = static_cast<RS_MText *>(entity);
            mtext->setStyle(fontName);
            mtext->update();
            qDebug() << "MText: " << mtext->getText()
                     << " Style: " << mtext->getStyle();
        }

        RS_EntityContainer *cot = dynamic_cast<RS_EntityContainer *>(entity);
        if (cot)
        {
            changeFontInCot(cot, fontName);
        }
    }
}

static void changeFontInDoc(RS_Document *document, const QString &fontName)
{
    RS_BlockList *blockList = document->getBlockList();
    if (blockList)
    {
        for (int i = 0; i < blockList->count(); i++)
        {
            RS_Block *block = blockList->at(i);
            changeFontInDoc(block, fontName);
        }
    }
    changeFontInCot(document, fontName);
}

QString LCReleaseLabel()
{
    return QStringLiteral("ViewerWidget");
}

void ViewerWidget::init(
    const QString &companyKey,
    const QString &appKey,
    const QString &fontDir)
{
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

void ViewerWidget::slotSetFont(const QString &fontName)
{
    RS_Document *doc = mdiWindow->getDocument();
    changeFontInDoc(doc, fontName);
    QG_GraphicView *graphicView = mdiWindow->getGraphicView();
    graphicView->redraw();
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
