#pragma once

#include <QMap>

#include "qtpropertybrowser.h"

class QWidget;

class QtProperty;
class QtPropertyEditorFactory;
class QtButtonPropertyItem;


class QTPROPERTYSHEET_DLL QtButtonPropertyBrowser : public QtPropertyBrowser {
    Q_OBJECT
  public:
    typedef QMap<QtProperty *, QtButtonPropertyItem *> Property2ItemMap;

    explicit QtButtonPropertyBrowser(QObject *parent = nullptr);

    ~QtButtonPropertyBrowser();

    virtual bool init(QWidget *parent, QtPropertyEditorFactory *factory);

    virtual QWidget *createEditor(QtProperty *property, QWidget *parent);

    QtProperty *itemToProperty(QtButtonPropertyItem *item);

    virtual void addProperty(QtProperty *property);

    virtual void removeProperty(QtProperty *property);

    virtual void removeAllProperties();

    Property2ItemMap &getProperties() {
        return property2items_;
    }


    virtual bool isExpanded(QtProperty *property);

    virtual void setExpanded(QtProperty *property, bool expand);

  public slots:
    void slotPropertyInsert(QtProperty *property, QtProperty *parent);

    void slotPropertyRemove(QtProperty *property, QtProperty *parent);

    void slotPropertyValueChange(QtProperty *property);

    void slotPropertyPropertyChange(QtProperty *property);

    void slotViewDestroy(QObject *p);

  private:
    void addProperty(QtProperty *property, QtButtonPropertyItem *parentItem);

    void deleteItem(QtButtonPropertyItem *item);

    QtPropertyEditorFactory *editorFactory_ = nullptr;
    QtButtonPropertyItem *rootItem_ = nullptr;
    QWidget *mainView_ = nullptr;
    Property2ItemMap property2items_;
};
