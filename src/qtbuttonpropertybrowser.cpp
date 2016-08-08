﻿#include "qtbuttonpropertybrowser.h"
#include "qtproperty.h"
#include "qtpropertyeditorfactory.h"
#include "qtpropertybrowserutils.h"

#include <cassert>
#include <QApplication>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>

QtGroupItem::QtGroupItem()
    : property_(NULL)
    , label_(NULL)
    , editor_(NULL)
    , titleButton_(NULL)
    , titleMenu_(NULL)
    , container_(NULL)
    , layout_(NULL)
    , parent_(NULL)
    , bExpand_(true)
{

}

QtGroupItem::QtGroupItem(QtProperty *prop, QtGroupItem *parent, QtGroupPropertyBrowser *browser)
    : property_(prop)
    , label_(NULL)
    , editor_(NULL)
    , titleButton_(NULL)
    , titleMenu_(NULL)
    , container_(NULL)
    , layout_(NULL)
    , parent_(parent)
    , bExpand_(true)
{
    layout_ = parent->layout_;

    if(!property_->getChildren().empty())
    {
        int row = layout_->rowCount();

        titleButton_ = new QToolButton();
        titleButton_->setText(property_->getTitle());
        titleButton_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
        titleButton_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        titleButton_->setArrowType(Qt::UpArrow);
        titleButton_->setIconSize(QSize(16, 16));
        layout_->addWidget(titleButton_, row, 0);
        connect(titleButton_, SIGNAL(clicked(bool)), this, SLOT(onBtnExpand()));

        if(property_->isMenuVisible())
        {
            QFont font = titleButton_->font();
            font.setBold(true);
            font.setPointSize(16);
            titleButton_->setFont(font);

            titleMenu_ = new QToolButton();
            titleMenu_->setText("...");
            layout_->addWidget(titleMenu_, row, 1, Qt::AlignRight);
            connect(titleMenu_, SIGNAL(clicked(bool)), this, SLOT(onBtnMenu()));
        }
        else
        {
            QFont font = titleButton_->font();
            font.setBold(true);
            titleButton_->setFont(font);
        }

        QFrame *frame2 = new QFrame();
        frame2->setFrameShape(QFrame::Panel);
        frame2->setFrameShadow(QFrame::Raised);
        container_ = frame2; //new QWidget();
        container_->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));
        layout_->addWidget(container_, layout_->rowCount(), 0, 1, 2);

        layout_ = new QGridLayout();
        layout_->setSpacing(4);
        layout_->setMargin(8);
        container_->setLayout(layout_);
    }
    else
    {
        int row = layout_->rowCount();

        label_ = new QLabel(property_->getTitle());
        label_->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
        layout_->addWidget(label_, row, 0);

        editor_ = browser->createEditor(prop, NULL);
        if(editor_)
        {
            layout_->addWidget(editor_, row, 1);
        }
    }
}

QtGroupItem::~QtGroupItem()
{
    removeFromParent();

    foreach(QtGroupItem *item, children_)
    {
        item->parent_ = NULL;
        delete item;
    }

    if(titleButton_)
    {
        delete titleButton_;
    }
    if(titleMenu_)
    {
        delete titleMenu_;
    }
    if(label_)
    {
        delete label_;
    }
    if(container_)
    {
        delete container_;
    }
    if(editor_)
    {
        delete editor_;
    }
}

void QtGroupItem::update()
{}

void QtGroupItem::addChild(QtGroupItem *child)
{
    children_.push_back(child);
}

void QtGroupItem::removeChild(QtGroupItem *child)
{
    children_.removeOne(child);
}

void QtGroupItem::removeFromParent()
{
    if(parent_)
    {
        parent_->removeChild(this);
        parent_ = NULL;
    }
}

void QtGroupItem::setTitle(const QString &title)
{
    if(titleButton_)
    {
        titleButton_->setText(title);
    }
    if(label_)
    {
        label_->setText(title);
    }
}

void QtGroupItem::setVisible(bool visible)
{
    if(titleButton_)
    {
        titleButton_->setVisible(visible);
        titleMenu_->setVisible(visible);
    }
    if(container_)
    {
        container_->setVisible(visible && bExpand_);
    }
    if(label_)
    {
        label_->setVisible(visible);
    }
    if(editor_)
    {
        editor_->setVisible(visible);
    }
}

void QtGroupItem::setExpanded(bool expand)
{
    if(bExpand_ == expand)
    {
        return;
    }
    bExpand_ = expand;

    if(titleButton_)
    {
        titleButton_->setArrowType(expand ? Qt::UpArrow : Qt::DownArrow);
    }

    if(container_)
    {
        container_->setVisible(expand);
    }
}

void QtGroupItem::onBtnExpand()
{
    setExpanded(!bExpand_);
}

void QtGroupItem::onBtnMenu()
{
    emit property_->signalPopupMenu(property_);
}


QtGroupPropertyBrowser::QtGroupPropertyBrowser(QObject *parent)
    : QObject(parent)
    , editorFactory_(NULL)
    , rootItem_(NULL)
    , mainView_(NULL)
{

}

QtGroupPropertyBrowser::~QtGroupPropertyBrowser()
{
    removeAllProperties();

    if(rootItem_ != NULL)
    {
        delete rootItem_;
    }
}

bool QtGroupPropertyBrowser::init(QWidget *parent)
{
    QVBoxLayout *parentLayout = new QVBoxLayout();
    parentLayout->setMargin(4);
    parentLayout->setSpacing(0);
    parent->setLayout(parentLayout);

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(4);

    QWidget *mainView_ = new QWidget(parent);
    mainView_->setLayout(mainLayout);
    parent->setFocusProxy(mainView_);
    parentLayout->addWidget(mainView_);

    QLayoutItem *item = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    parentLayout->addItem(item);

    rootItem_ = new QtGroupItem();
    rootItem_->setLayout(mainLayout);

    connect(mainView_, SIGNAL(destroyed(QObject*)), this, SLOT(slotViewDestroy(QObject*)));
    return true;
}

void QtGroupPropertyBrowser::setEditorFactory(QtPropertyEditorFactory *factory)
{
    editorFactory_ = factory;
}

QWidget* QtGroupPropertyBrowser::createEditor(QtProperty *property, QWidget *parent)
{
    if(editorFactory_ != NULL)
    {
        return editorFactory_->createEditor(property, parent);
    }
    return NULL;
}

QtProperty* QtGroupPropertyBrowser::itemToProperty(QtGroupItem* item)
{
    return item->property();
}

void QtGroupPropertyBrowser::addProperty(QtProperty *property)
{
    if(property2items_.contains(property))
    {
        return;
    }

    addProperty(property, rootItem_);
}

void QtGroupPropertyBrowser::addProperty(QtProperty *property, QtGroupItem *parentItem)
{
    assert(parentItem != NULL);

    QtGroupItem *item = NULL;
    if(property->isSelfVisible())
    {
        item = new QtGroupItem(property, parentItem, this);
        parentItem->addChild(item);
        parentItem = item;
    }
    property2items_[property] = item;

    connect(property, SIGNAL(signalPropertyInserted(QtProperty*,QtProperty*)), this, SLOT(slotPropertyInsert(QtProperty*,QtProperty*)));
    connect(property, SIGNAL(signalPropertyRemoved(QtProperty*,QtProperty*)), this, SLOT(slotPropertyRemove(QtProperty*,QtProperty*)));
    connect(property, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(slotPropertyValueChange(QtProperty*)));
    connect(property, SIGNAL(signalPropertyChange(QtProperty*)), this, SLOT(slotPropertyPropertyChange(QtProperty*)));

    // add it's children finaly.
    foreach(QtProperty *child, property->getChildren())
    {
        addProperty(child, parentItem);
    }
}

void QtGroupPropertyBrowser::removeProperty(QtProperty *property)
{
    Property2ItemMap::iterator it = property2items_.find(property);
    if(it != property2items_.end())
    {
        QtGroupItem *item = it.value();
        if(item != NULL)
        {
            item->removeFromParent();
        }

        property2items_.erase(it);
        disconnect(property, 0, this, 0);

        // remove it's children first.
        foreach(QtProperty *child, property->getChildren())
        {
            removeProperty(child);
        }

        // then remove this QtGroupItem
        if(item != NULL)
        {
            deleteItem(item);
        }
    }
}

void QtGroupPropertyBrowser::removeAllProperties()
{
    QList<QtProperty*> properties = property2items_.keys();
    foreach(QtProperty *property, properties)
    {
        removeProperty(property);
    }
    property2items_.clear();
}

void QtGroupPropertyBrowser::slotPropertyInsert(QtProperty *property, QtProperty *parent)
{
    QtGroupItem *parentItem = property2items_.value(parent);
    addProperty(property, parentItem);
}

void QtGroupPropertyBrowser::slotPropertyRemove(QtProperty *property, QtProperty * /*parent*/)
{
    removeProperty(property);
}

void QtGroupPropertyBrowser::slotPropertyValueChange(QtProperty *property)
{
//    QtGroupItem *item = property2items_.value(property);
//    if(item != NULL)
//    {
//        item->setText(1, property->getValueString());
//        item->setIcon(1, property->getValueIcon());
//    }
}

void QtGroupPropertyBrowser::slotPropertyPropertyChange(QtProperty *property)
{
    QtGroupItem *item = property2items_.value(property);
    if(item != NULL)
    {
        item->setTitle(property->getTitle());
        item->setVisible(property->isVisible());
    }
}

void QtGroupPropertyBrowser::slotViewDestroy(QObject *p)
{
    removeAllProperties();
}

void QtGroupPropertyBrowser::deleteItem(QtGroupItem *item)
{
    delete item;
}

bool QtGroupPropertyBrowser::isExpanded(QtProperty *property)
{
    QtGroupItem *item = property2items_.value(property);
    if(item != NULL)
    {
        return item->isExpanded();
    }
    return false;
}

void QtGroupPropertyBrowser::setExpanded(QtProperty *property, bool expand)
{
    QtGroupItem *item = property2items_.value(property);
    if(item != NULL)
    {
        item->setExpanded(expand);
    }
}
