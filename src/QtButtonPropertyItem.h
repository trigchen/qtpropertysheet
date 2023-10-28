#pragma once

#include <QList>
#include <QObject>

#include "QtPropertyConfig.h"

class QWidget;
class QLabel;
class QGridLayout;
class QToolButton;

class QtProperty;
class QtButtonPropertyItem;
class QtPropertyEditorFactory;

class QTPROPERTYSHEET_DLL QtButtonPropertyItem : public QObject {
    Q_OBJECT
  public:
    QtButtonPropertyItem();

    QtButtonPropertyItem(QtProperty *prop, QtButtonPropertyItem *parent, QtPropertyEditorFactory *editorFactory);

    virtual ~QtButtonPropertyItem();

    void update();

    void addChild(QtButtonPropertyItem *child);

    void removeChild(QtButtonPropertyItem *child);

    void removeFromParent();

    void setTitle(const QString &title);

    void setVisible(bool visible);

    QtButtonPropertyItem *parent() {
        return parent_;
    }


    QtProperty *property() {
        return property_;
    }


    void setLayout(QGridLayout *layout) {
        layout_ = layout;
    }


    void setExpanded(bool expand);

    bool isExpanded() const {
        return bExpand_;
    }


  protected slots:
    void onBtnExpand();

    void onBtnMenu();

    void onPropertyValueChange(QtProperty *property);

  protected:
    QtProperty *property_ = nullptr;
    QLabel *label_ = nullptr;
    QWidget *editor_ = nullptr;  // can be null
    QLabel *valueLabel_ = nullptr;
    QToolButton *titleButton_ = nullptr;
    QToolButton *titleMenu_ = nullptr;
    QWidget *container_ = nullptr;
    QGridLayout *layout_ = nullptr;
    QtButtonPropertyItem *parent_ = nullptr;
    QList<QtButtonPropertyItem *> children_;
    bool bExpand_ = true;
};
