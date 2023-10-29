#include <QApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemDelegate>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QTreeWidget>
#include <cassert>

#include "QtProperty.h"
#include "QtPropertyBrowserUtils.h"
#include "QtPropertyEditorFactory.h"
#include "QtPropertyTreeDelegate.h"
#include "QtPropertyTreeView.h"
#include "QtTreePropertyBrowser.h"

namespace {
    const int PropertyDataIndex = Qt::UserRole + 1;
}

QtTreePropertyBrowser::QtTreePropertyBrowser(QObject *parent) : QtPropertyBrowser(parent) {
}


QtTreePropertyBrowser::~QtTreePropertyBrowser() {
    removeAllProperties();
}


bool QtTreePropertyBrowser::init(QWidget *parent, QtPropertyEditorFactory *factory) {
    editorFactory_ = factory;

    QHBoxLayout *layout = new QHBoxLayout(parent);
    layout->setContentsMargins(0, 0, 0, 0);

    treeWidget_ = new QtPropertyTreeView(parent);
    treeWidget_->setEditorPrivate(this);
    treeWidget_->setIconSize(QSize(18, 18));
    layout->addWidget(treeWidget_);
    parent->setFocusProxy(treeWidget_);

    treeWidget_->setColumnCount(2);
    QStringList labels;
    labels.append(QCoreApplication::translate("QtTreePropertyBrowser", "Property"));
    labels.append(QCoreApplication::translate("QtTreePropertyBrowser", "Value"));
    treeWidget_->setHeaderLabels(labels);
    treeWidget_->setAlternatingRowColors(true);
    treeWidget_->setEditTriggers(QAbstractItemView::EditKeyPressed);

    delegate_ = new QtPropertyTreeDelegate(parent);
    delegate_->setEditorPrivate(this);
    treeWidget_->setItemDelegate(delegate_);

    // treeWidget_->header()->setMovable(false);
    // treeWidget_->header()->setResizeMode(QHeaderView::Stretch);

    expandIcon_ = QtPropertyBrowserUtils::drawIndicatorIcon(treeWidget_->palette(), treeWidget_->style());

    connect(treeWidget_, &QtPropertyTreeView::currentItemChanged, this, &QtTreePropertyBrowser::slotCurrentTreeItemChanged);
    connect(treeWidget_, &QtPropertyTreeView::destroyed, this,  &QtTreePropertyBrowser::slotTreeViewDestroy);
    return true;
}


bool QtTreePropertyBrowser::markPropertiesWithoutValue() {
    return true;
}


bool QtTreePropertyBrowser::lastColumn(int column) {
    return treeWidget_->header()->visualIndex(column) == treeWidget_->columnCount() - 1;
}


QColor QtTreePropertyBrowser::calculatedBackgroundColor(QtProperty *property) {
    if(property->parent() != nullptr) {
        int index = property->parent()->indexChild(property);
        return QColor(index % 2 ? Qt::blue : Qt::white);
    }

    if(dynamic_cast<QtGroupProperty *>(property) != nullptr) {
        return QColor(Qt::gray);
    }
    return QColor(Qt::white);
}


QWidget *QtTreePropertyBrowser::createEditor(QtProperty *property, QWidget *parent) {
    if(editorFactory_ != nullptr) {
        return editorFactory_->createEditor(property, parent);
    }
    return nullptr;
}


QTreeWidgetItem *QtTreePropertyBrowser::getEditedItem() {
    return delegate_->editedItem();
}


QtPropertyTreeView *QtTreePropertyBrowser::getTreeWidget() {
    return treeWidget_;
}


QTreeWidgetItem *QtTreePropertyBrowser::indexToItem(const QModelIndex &index) {
    return treeWidget_->indexToItem(index);
}


void QtTreePropertyBrowser::slotCurrentTreeItemChanged(QTreeWidgetItem *, QTreeWidgetItem *) {
}


QtProperty *QtTreePropertyBrowser::indexToProperty(const QModelIndex &index) {
    return itemToProperty(treeWidget_->indexToItem(index));
}


QtProperty *QtTreePropertyBrowser::itemToProperty(QTreeWidgetItem *item) {
    if(item != nullptr) {
        quintptr ptr = item->data(0, PropertyDataIndex).value<quintptr>();
        return reinterpret_cast<QtProperty *>(ptr);
    }
    return nullptr;
}


void QtTreePropertyBrowser::addProperty(QtProperty *property) {
    if(property2items_.contains(property)) {
        return;
    }

    addProperty(property, nullptr);
}


void QtTreePropertyBrowser::addProperty(QtProperty *property, QTreeWidgetItem *parentItem) {
    QTreeWidgetItem *item = nullptr;
    if(property->isSelfVisible()) {
        item = new QTreeWidgetItem();
        item->setText(0, property->title());
        item->setData(0, PropertyDataIndex, QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(property)));
        item->setToolTip(0, property->toolTip());
        item->setFlags(item->flags() | Qt::ItemIsEditable);

        if(parentItem != nullptr) {
            parentItem->addChild(item);
        } else {
            treeWidget_->addTopLevelItem(item);
        }

        if(property->hasValue()) {
            item->setIcon(1, property->valueIcon());
            item->setText(1, property->valueString());
        } else {
            item->setFirstColumnSpanned(true);
        }

        parentItem = item;
    }
    property2items_[property] = item;

    connect(property, &QtProperty::signalPropertyInserted, this, &QtTreePropertyBrowser::slotPropertyInsert);
    connect(property, &QtProperty::signalPropertyRemoved, this, &QtTreePropertyBrowser::slotPropertyRemove);
    connect(property, &QtProperty::signalValueChange, this, &QtTreePropertyBrowser::slotPropertyValueChange);
    connect(property, &QtProperty::signalPropertyChange, this, &QtTreePropertyBrowser::slotPropertyPropertyChange);

    // add it's children finaly.
    foreach(QtProperty *child, property->children()) {
        addProperty(child, parentItem);
    }
}


void QtTreePropertyBrowser::removeProperty(QtProperty *property) {
    Property2ItemMap::iterator it = property2items_.find(property);
    if(it != property2items_.end()) {
        QTreeWidgetItem *item = it.value();
        property2items_.erase(it);
        disconnect(property, 0, this, 0);

        // remove it's children first.
        foreach(QtProperty *child, property->children()) {
            removeProperty(child);
        }

        // then remove this QTreeWidgetItem
        deleteTreeItem(item);
    }
}


void QtTreePropertyBrowser::removeAllProperties() {
    QList<QtProperty *> properties = property2items_.keys();
    foreach(QtProperty *property, properties) {
        removeProperty(property);
    }
    property2items_.clear();
}


Property2ItemMap &QtTreePropertyBrowser::properties() {
    return property2items_;
}


void QtTreePropertyBrowser::slotPropertyInsert(QtProperty *property, QtProperty *parent) {
    QTreeWidgetItem *parentItem = property2items_.value(parent);
    addProperty(property, parentItem);
}


void QtTreePropertyBrowser::slotPropertyRemove(QtProperty *property, QtProperty * /*parent*/) {
    removeProperty(property);
}


void QtTreePropertyBrowser::slotPropertyValueChange(QtProperty *property) {
    QTreeWidgetItem *item = property2items_.value(property);
    if(item != nullptr) {
        item->setText(1, property->valueString());
        item->setIcon(1, property->valueIcon());
    }
}


void QtTreePropertyBrowser::slotPropertyPropertyChange(QtProperty *property) {
    QTreeWidgetItem *item = property2items_.value(property);
    if(item != nullptr) {
        item->setText(0, property->title());
        item->setHidden(!property->isVisible());
    }
}


void QtTreePropertyBrowser::slotTreeViewDestroy(QObject *p) {
    if(treeWidget_ == p) {
        treeWidget_ = nullptr;
    }
}


void QtTreePropertyBrowser::deleteTreeItem(QTreeWidgetItem *item) {
    if(treeWidget_) {
        delete item;
    }
}


bool QtTreePropertyBrowser::isExpanded(QtProperty *property) {
    QTreeWidgetItem *treeItem = property2items_.value(property);
    if(treeItem != nullptr) {
        return treeItem->isExpanded();
    }
    return false;
}


void QtTreePropertyBrowser::setExpanded(QtProperty *property, bool expand) {
    QTreeWidgetItem *treeItem = property2items_.value(property);
    if(treeItem != nullptr) {
        treeItem->setExpanded(expand);
    }
}
