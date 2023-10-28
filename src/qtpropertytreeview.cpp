#include <QApplication>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

#include "qtproperty.h"
#include "qtpropertytreeview.h"
#include "qttreepropertybrowser.h"

namespace {
    bool isItemEditable(int flags) {
        return (flags & Qt::ItemIsEditable) && (flags & Qt::ItemIsEnabled);
    }
}

QtPropertyTreeView::QtPropertyTreeView(QWidget *parent) : QTreeWidget(parent) {
    connect(header(), &QHeaderView::sectionDoubleClicked, this, &QtPropertyTreeView::resizeColumnToContents);
}


void QtPropertyTreeView::drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItem opt = option;
    QColor bgColor;
    if(editorPrivate_ != nullptr) {
        QtProperty *property = editorPrivate_->indexToProperty(index);
        if(property) {
            bgColor = property->getBackgroundColor();
        }
    }

    if(bgColor.isValid()) {
        painter->fillRect(option.rect, bgColor);
        opt.palette.setColor(QPalette::AlternateBase, bgColor);
    }

    QTreeWidget::drawRow(painter, opt, index);
    // QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
    // painter->save();
    // painter->setPen(QPen(color));
    // painter->drawLine(opt.rect.x(), opt.rect.bottom(), opt.rect.right(), opt.rect.bottom());
    // painter->restore();
}


void QtPropertyTreeView::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:  // Trigger Edit
        if(editorPrivate_->getEditedItem() == nullptr) {
            const QTreeWidgetItem *item = currentItem();
            if(item && (item->columnCount() >= 2) && isItemEditable(item->flags())) {
                event->accept();
                // If the current position is at column 0, move to 1.
                QModelIndex index = currentIndex();
                if(index.column() == 0) {
                    index = index.sibling(index.row(), 1);
                    setCurrentIndex(index);
                }
                edit(index);
                return;
            }
        }
        break;
    default:
        break;
    }
    QTreeWidget::keyPressEvent(event);
}


void QtPropertyTreeView::mousePressEvent(QMouseEvent *event) {
    QTreeWidget::mousePressEvent(event);
    QTreeWidgetItem *item = itemAt(event->pos());

    if(item) {
        QtProperty *property = editorPrivate_->itemToProperty(item);

        if((item != editorPrivate_->getEditedItem()) && (event->button() == Qt::LeftButton)
           && (header()->logicalIndexAt(event->pos().x()) == 1) && isItemEditable(item->flags())) {
            editItem(item, 1);
        } else if((property != nullptr) && !property->hasValue() && editorPrivate_->markPropertiesWithoutValue() && !rootIsDecorated()) {
            if(event->pos().x() + header()->offset() < 20) {
                item->setExpanded(!item->isExpanded());
            }
        }
    }
}
