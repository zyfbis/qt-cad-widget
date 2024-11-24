/****************************************************************************
**
** This file is part of the LibreCAD project, a 2D CAD program
**
** Copyright (C) 2010 R. van Twisk (librecad@rvt.dds.nl)
** Copyright (C) 2001-2003 RibbonSoft. All rights reserved.
**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software 
** Foundation and appearing in the file gpl-2.0.txt included in the
** packaging of this file.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
** This copyright notice MUST APPEAR in all copies of the script!  
**
**********************************************************************/

#include "rs_actionblocksedit.h"
#include "rs_debug.h"
#include "rs_dialogfactory.h"
#include "rs_graphic.h"

RS_ActionBlocksEdit::RS_ActionBlocksEdit(RS_EntityContainer& container,
        RS_GraphicView& graphicView)
        :RS_ActionInterface("Edit Block", container, graphicView) {}

void RS_ActionBlocksEdit::trigger() {

    RS_DEBUG->print(RS_Debug::D_DEBUGGING, "RS_ActionBlocksEdit::trigger(): edit block");

    if (!graphic) {
        RS_DEBUG->print(RS_Debug::D_ERROR, "RS_ActionBlocksEdit::trigger(): nullptr graphic");
        return;
    }

    RS_BlockList *bl = graphic->getBlockList();

    if (!bl) {
        RS_DEBUG->print(RS_Debug::D_ERROR, "RS_ActionBlocksEdit::trigger(): nullptr block list in graphic");
        return;
    }

//  std::cout<<__func__<<" : "<<__LINE__<<" : graphic->getBlockList()->count()="<<graphic->getBlockList()->count()<<std::endl;
    RS_DIALOGFACTORY->requestEditBlockWindow(bl);

    finish(false);
    RS_DEBUG->print(RS_Debug::D_DEBUGGING, "RS_ActionBlocksEdit::trigger(): OK");
}

void RS_ActionBlocksEdit::init(int status) {
    RS_ActionInterface::init(status);
    trigger();
}

