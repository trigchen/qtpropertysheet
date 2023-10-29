#pragma once

#include <QIcon>
#include <QMap>
#include <QObject>
#include <QVariant>
#include <QVector>

#include "QtPropertyConfig.h"

class QtProperty;
class QtPropertyFactory;

typedef QVector<QtProperty *> QtPropertyList;
typedef QMap<QString, QVariant> QtPropertyAttributes;

class QTPROPERTYSHEET_DLL QtProperty : public QObject {
    Q_OBJECT
  public:
    typedef QString Type;

    QtProperty(Type type, QtPropertyFactory *factory);

    virtual ~QtProperty();

    Type type() const;

    QtProperty *parent();

    const QString &name() const;

    void setName(const QString &name);

    const QString &title() const;

    void setTitle(const QString &title);

    const QString &toolTip() const;

    void setToolTip(const QString &tip);

    const QColor &backgroundColor() const;

    void setBackgroundColor(const QColor &cr);

    virtual const QVariant &value() const;

    virtual void setValue(const QVariant &value);

    virtual QString valueString() const;

    virtual QIcon valueIcon() const;


    QVariant attribute(const QString &attributeName) const;

    virtual void setAttribute(const QString &attributeName, const QVariant &value);

    QtPropertyAttributes &attributes();

    /** 添加子属性，由属性树负责delete child。*/
    void addChild(QtProperty *child);

    /** 移除子属性，不delete child。*/
    void removeChild(QtProperty *child);

    /** 将自己从属性树中取下，不delete自己。*/
    void removeFromParent();

    /** 移除所有子属性。
     *  @param clean 如果为true，则delete所有子属性。否则，仅从树中移除引用。
     */
    void removeAllChildren(bool clean);

    QtPropertyList &children();

    const QtPropertyList &children() const;

    int indexChild(const QtProperty *child) const;

    virtual QtProperty *findChild(const QString &name);

    virtual void setChildValue(const QString &childName, const QVariant &value);

    virtual bool hasValue() const;

    bool hasAttribute(const QString &attributeName);

    virtual bool isModified() const;

    void setVisible(bool visible);

    bool isVisible() const;

    void setSelfVisible(bool visible);

    bool isSelfVisible() const;

    void setMenuVisible(bool visible);

    bool isMenuVisible() const;

  signals:
    void signalValueChange(QtProperty *property);

    void signalPropertyInserted(QtProperty *property, QtProperty *parent);

    void signalPropertyRemoved(QtProperty *property, QtProperty *parent);

    void signalPropertyReordered(QtProperty *property);

    void signalAttributeChange(QtProperty *property, const QString &name);

    void signalPropertyChange(QtProperty *property);

    void signalPopupMenu(QtProperty *property);

  protected:
    virtual void onChildAdd(QtProperty *child);

    virtual void onChildRemove(QtProperty *child);

    QtPropertyFactory *factory_;
    Type type_;
    QString name_;
    QString title_;
    QString tips_;
    QVariant value_;
    QColor bgColor_;
    QtProperty *parent_ = nullptr;
    QtPropertyList children_;
    QMap<QString, QVariant> attributes_;
    bool visible_ = true;
    bool selfVisible_ = true;
    bool menuVisible_ = false;
};

/********************************************************************/
class QTPROPERTYSHEET_DLL QtContainerProperty : public QtProperty {
    Q_OBJECT
  public:
    QtContainerProperty(Type type, QtPropertyFactory *factory);

  protected slots:
    virtual void slotChildValueChange(QtProperty *property) = 0;

  protected:
    virtual void onChildAdd(QtProperty *child);

    virtual void onChildRemove(QtProperty *child);
};


/********************************************************************/
class QTPROPERTYSHEET_DLL QtListProperty : public QtContainerProperty {
    Q_OBJECT
  public:
    QtListProperty(Type type, QtPropertyFactory *factory);

    virtual void setValue(const QVariant &value);

    virtual QString valueString() const;

  protected slots:
    virtual void slotChildValueChange(QtProperty *property);
};


/********************************************************************/
class QTPROPERTYSHEET_DLL QtDictProperty : public QtContainerProperty {
    Q_OBJECT
  public:
    QtDictProperty(Type type, QtPropertyFactory *factory);

    virtual void setValue(const QVariant &value);

  protected slots:
    virtual void slotChildValueChange(QtProperty *property);
};


/**
 * @brief The QtGroupProperty class
 *
 * QtGroupProperty doesn't have value, it emits the children's value change event
 * to it's listener directly.
 *
 * NOTICE: QtGroupProperty can only be root or child of another QtGroupProperty,
 * Adding QtGroupProperty as child of any other property, will cause unexpected problem.
 */
class QTPROPERTYSHEET_DLL QtGroupProperty : public QtContainerProperty {
    Q_OBJECT
  public:
    QtGroupProperty(Type type, QtPropertyFactory *factory);

    virtual bool hasValue() const;

    virtual void setValue(const QVariant &value);

    virtual QtProperty *findChild(const QString &name);

    virtual void setChildValue(const QString &name, const QVariant &value);

  protected slots:
    virtual void slotChildValueChange(QtProperty *property);
};


/********************************************************************/
class QTPROPERTYSHEET_DLL QtEnumProperty : public QtProperty {
    Q_OBJECT
  public:
    QtEnumProperty(Type type, QtPropertyFactory *factory);

    virtual QString valueString() const;
};

/********************************************************************/
class QTPROPERTYSHEET_DLL QtFlagProperty : public QtProperty {
    Q_OBJECT
  public:
    QtFlagProperty(Type type, QtPropertyFactory *factory);

    virtual QString valueString() const;

  private:
    const QString separator = " | ";
};

/********************************************************************/
class QTPROPERTYSHEET_DLL QtBoolProperty : public QtProperty {
    Q_OBJECT
  public:
    QtBoolProperty(Type type, QtPropertyFactory *factory);

    virtual QString valueString() const;

    virtual QIcon valueIcon() const;
};

/********************************************************************/
class QTPROPERTYSHEET_DLL QtDoubleProperty : public QtProperty {
    Q_OBJECT
  public:
    QtDoubleProperty(Type type, QtPropertyFactory *factory);

    virtual QString valueString() const;
};

/********************************************************************/
class QTPROPERTYSHEET_DLL QtColorProperty : public QtProperty {
    Q_OBJECT
  public:
    QtColorProperty(Type type, QtPropertyFactory *factory);

    virtual QString valueString() const;

    virtual QIcon valueIcon() const;
};

/********************************************************************/
class QTPROPERTYSHEET_DLL QtDynamicListProperty : public QtProperty {
    Q_OBJECT
  public:
    QtDynamicListProperty(Type type, QtPropertyFactory *factory);

    ~QtDynamicListProperty();

    virtual void setValue(const QVariant &value);

    virtual QString valueString() const;

  protected slots:
    void slotItemValueChange(QtProperty *item);

    void slotItemMoveUp(QtProperty *item);

    void slotItemMoveDown(QtProperty *item);

    void slotItemDelete(QtProperty *item);

    void slotLengthChange(QtProperty *property);

  protected:
    void setLength(int length);

    QtProperty *appendItem();

    void popItem();

    int length_ = 0;
    QtProperty *propLength_ = nullptr;
    QtPropertyList items_;
    QVariantList valueList_;

  private:
    const QString separator = ", ";
};

class QTPROPERTYSHEET_DLL QtDynamicItemProperty : public QtProperty {
    Q_OBJECT
  public:
    QtDynamicItemProperty(Type type, QtPropertyFactory *factory);

    ~QtDynamicItemProperty();

    void setValueType(Type type);

    QtProperty *getImpl();

    virtual void setValue(const QVariant &value) override;

    virtual const QVariant &value() const override;

    virtual QString valueString() const override;

    virtual QIcon valueIcon() const override;

  signals:
    void signalMoveUp(QtProperty *property);

    void signalMoveDown(QtProperty *property);

    void signalDelete(QtProperty *property);

  protected slots:
    void onImplValueChange(QtProperty *property);

  protected:
    QtProperty *impl_ = nullptr;
};

/********************************************************************/
class QTPROPERTYSHEET_DLL QtFloatListProperty : public QtProperty {
    Q_OBJECT
  public:
    QtFloatListProperty(Type type, QtPropertyFactory *factory);

    ~QtFloatListProperty();

    virtual QString valueString() const;

  private:
    const QString separator = ", ";
};
