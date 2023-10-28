#pragma once

#include <QItemDelegate>

#include "QtPropertyConfig.h"

class QtProperty;
class QTreeWidgetItem;
class QtTreePropertyBrowser;

class QTPROPERTYSHEET_DLL QtPropertyTreeDelegate : public QItemDelegate {
    Q_OBJECT
  public:
    explicit QtPropertyTreeDelegate(QObject *parent = nullptr);

    void setEditorPrivate(QtTreePropertyBrowser *editorPrivate) {
        editorPrivate_ = editorPrivate;
    }


    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setModelData(QWidget *, QAbstractItemModel *, const QModelIndex &) const {
    }


    void setEditorData(QWidget *, const QModelIndex &) const {
    }


    bool eventFilter(QObject *object, QEvent *event);

    void closeEditor(QtProperty *property);

    QTreeWidgetItem *editedItem() const {
        return editedItem_;
    }


  protected:
    void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QPixmap &pixmap) const;

    void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const;

  private slots:
    void slotEditorDestroyed(QObject *object);

  private:
    int indentation(const QModelIndex &index) const;

    typedef QMap<QWidget *, QtProperty *> EditorToPropertyMap;
    mutable EditorToPropertyMap editorToProperty_;

    typedef QMap<QtProperty *, QWidget *> PropertyToEditorMap;
    mutable PropertyToEditorMap propertyToEditor_;
    QtTreePropertyBrowser *editorPrivate_ = nullptr;
    mutable QTreeWidgetItem *editedItem_ = nullptr;
    mutable QWidget *editedWidget_ = nullptr;
    mutable bool disablePainting_ = false;
};
