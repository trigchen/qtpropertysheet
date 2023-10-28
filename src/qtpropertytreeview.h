#pragma once

#include <QTreeWidget>

#include "qtpropertyconfig.h"

class QtTreePropertyBrowser;

class QTPROPERTYSHEET_DLL QtPropertyTreeView : public QTreeWidget {
    Q_OBJECT
  public:
    explicit QtPropertyTreeView(QWidget *parent = nullptr);

    void setEditorPrivate(QtTreePropertyBrowser *editorPrivate) {
        editorPrivate_ = editorPrivate;
    }


    QTreeWidgetItem *indexToItem(const QModelIndex &index) {
        return itemFromIndex(index);
    }


  protected:
    void keyPressEvent(QKeyEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

  private:
    QtTreePropertyBrowser *editorPrivate_ = nullptr;
};
