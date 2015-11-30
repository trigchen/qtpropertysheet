#ifndef QTTREEPROPERTYBROWSER_H
#define QTTREEPROPERTYBROWSER_H

#include <QObject>
#include <QIcon>
#include <QMap>

class QWidget;
class QModelIndex;
class QTreeWidgetItem;
class QtPropertyTreeView;
class QtPropertyTreeDelegate;
class QtProperty;
class QtPropertyEditorFactory;

class QtTreePropertyBrowser : public QObject
{
    Q_OBJECT
public:
    explicit QtTreePropertyBrowser(QObject *parent = 0);
    ~QtTreePropertyBrowser();

    bool init(QWidget *parent);

    void setEditorFactory(QtPropertyEditorFactory *factory);

    bool markPropertiesWithoutValue(){ return true; }
    bool lastColumn(int column);

    QWidget* createEditor(QtProperty *property, QWidget *parent);

    QTreeWidgetItem* editedItem();

    QTreeWidgetItem* indexToItem(const QModelIndex &index);
    QtProperty* indexToProperty(const QModelIndex &index);
    QtProperty* itemToProperty(QTreeWidgetItem* item);

    QtPropertyTreeView* treeWidget(){ return treeWidget_; }

    void addProperty(QtProperty *property);
    void removeProperty(QtProperty *property);
    void removeAllProperties();

signals:

public slots:
    void slotCollapsed(const QModelIndex &);
    void slotExpanded(const QModelIndex &);
    void slotCurrentTreeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*);

    void slotPropertyInsert(QtProperty *property, QtProperty *parent);
    void slotPropertyRemove(QtProperty *property, QtProperty *parent);
    void slotPropertyValueChange(QtProperty *property);
    void slotPropertyPropertyChange(QtProperty *property);

    void slotTreeViewDestroy(QObject *p);

private:
    void addProperty(QtProperty *property, QTreeWidgetItem *parentItem);
    void deleteTreeItem(QTreeWidgetItem *item);

    QtPropertyEditorFactory*    editorFactory_;
    QtPropertyTreeView*         treeWidget_;
    QtPropertyTreeDelegate*     delegate_;
    QIcon                       expandIcon_;

    typedef QMap<QtProperty*, QTreeWidgetItem*> Property2ItemMap;
    Property2ItemMap            property2items_;
};

#endif // QTTREEPROPERTYBROWSER_H
