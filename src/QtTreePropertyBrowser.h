﻿#pragma once

#include <QIcon>
#include <QMap>

#include "QtPropertyBrowser.h"

class QWidget;
class QModelIndex;
class QTreeWidgetItem;
class QtPropertyTreeView;
class QtPropertyTreeDelegate;
class QtProperty;
class QtPropertyEditorFactory;

typedef QMap<QtProperty *, QTreeWidgetItem *> Property2ItemMap;

class QTPROPERTYSHEET_DLL QtTreePropertyBrowser : public QtPropertyBrowser {
    Q_OBJECT
  public:
    explicit QtTreePropertyBrowser(QObject *parent = nullptr);

    ~QtTreePropertyBrowser();

    virtual bool init(QWidget *parent, QtPropertyEditorFactory *factory);

    bool markPropertiesWithoutValue();

    bool lastColumn(int column);

    QColor calculatedBackgroundColor(QtProperty *property);

    QWidget *createEditor(QtProperty *property, QWidget *parent);

    QTreeWidgetItem *indexToItem(const QModelIndex &index);

    QtProperty *indexToProperty(const QModelIndex &index);

    QtProperty *itemToProperty(QTreeWidgetItem *item);

    QTreeWidgetItem *getEditedItem();

    QtPropertyTreeView *getTreeWidget();

    virtual void addProperty(QtProperty *property);

    virtual void removeProperty(QtProperty *property);

    virtual void removeAllProperties();

    Property2ItemMap &properties();

    virtual bool isExpanded(QtProperty *property);

    virtual void setExpanded(QtProperty *property, bool expand);

  public slots:
    void slotCurrentTreeItemChanged(QTreeWidgetItem *, QTreeWidgetItem *);

    void slotPropertyInsert(QtProperty *property, QtProperty *parent);

    void slotPropertyRemove(QtProperty *property, QtProperty *parent);

    void slotPropertyValueChange(QtProperty *property);

    void slotPropertyPropertyChange(QtProperty *property);

    void slotTreeViewDestroy(QObject *p);

  private:
    void addProperty(QtProperty *property, QTreeWidgetItem *parentItem);

    void deleteTreeItem(QTreeWidgetItem *item);

    QtPropertyEditorFactory *editorFactory_ = nullptr;
    QtPropertyTreeView *treeWidget_ = nullptr;
    QtPropertyTreeDelegate *delegate_ = nullptr;
    QIcon expandIcon_;
    Property2ItemMap property2items_;
};