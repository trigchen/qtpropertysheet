#pragma once

#include <QColor>
#include <QList>
#include <QObject>
#include <QVariant>
#include <QVector>

#include "QtPropertyConfig.h"

class QtProperty;
class QWidget;
class QSpinBox;
class QDoubleSpinBox;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QToolButton;

class QtColorEditWidget;
class QtBoolEdit;
class QxtCheckComboBox;

class QtPropertyEditorFactory;

// QtPropertyEditor will be destroied when QtEditor destroied.
class QTPROPERTYSHEET_DLL QtPropertyEditor : public QObject {
    Q_OBJECT
  public:
    explicit QtPropertyEditor(QtProperty *property);

    virtual ~QtPropertyEditor();

    virtual QWidget *createEditor(QWidget *parent, QtPropertyEditorFactory *factory) = 0;

  public slots:
    virtual void onPropertyValueChange(QtProperty *property) = 0;

    virtual void onPropertyDestory(QObject *object);

    virtual void slotEditorDestory(QObject *object);

  protected:
    QtProperty *property_ = nullptr;
};


class QTPROPERTYSHEET_DLL QtIntSpinBoxEditor : public QtPropertyEditor {
    Q_OBJECT
  public:
    explicit QtIntSpinBoxEditor(QtProperty *property);

    virtual QWidget *createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

  public slots:
    virtual void onPropertyValueChange(QtProperty *property);

    void slotEditorValueChange(int value);

    void slotSetAttribute(QtProperty *property, const QString &name);

  private:
    int value_ = 0;
    QSpinBox *editor_ = nullptr;
};

class QTPROPERTYSHEET_DLL QtDoubleSpinBoxEditor : public QtPropertyEditor {
    Q_OBJECT
  public:
    explicit QtDoubleSpinBoxEditor(QtProperty *property);

    virtual QWidget *createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

  public slots:
    virtual void onPropertyValueChange(QtProperty *property);

    void slotEditorValueChange(double value);

    void slotSetAttribute(QtProperty *property, const QString &name);

  private:
    double value_ = 0.0;
    QDoubleSpinBox *editor_ = nullptr;
};

class QTPROPERTYSHEET_DLL QtStringEditor : public QtPropertyEditor {
    Q_OBJECT
  public:
    explicit QtStringEditor(QtProperty *property);

    virtual QWidget *createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

  public slots:
    virtual void onPropertyValueChange(QtProperty *property);

    void slotEditFinished();

    void slotSetAttribute(QtProperty *property, const QString &name);

  private:
    QString value_;
    QLineEdit *editor_ = nullptr;
};

class QTPROPERTYSHEET_DLL QtEnumEditor : public QtPropertyEditor {
    Q_OBJECT
  public:
    explicit QtEnumEditor(QtProperty *property);

    virtual QWidget *createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

  public slots:
    virtual void onPropertyValueChange(QtProperty *property);

    virtual void slotEditorValueChange(int index);

    virtual void slotSetAttribute(QtProperty *property, const QString &name);

  private:
    int value_ = 0;
    QComboBox *editor_ = nullptr;
};

class QTPROPERTYSHEET_DLL QtEnumPairEditor : public QtPropertyEditor {
    Q_OBJECT
  public:
    explicit QtEnumPairEditor(QtProperty *property);

    virtual QWidget *createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

  public slots:
    virtual void onPropertyValueChange(QtProperty *property);

    virtual void slotEditorValueChange(int index);

    virtual void slotSetAttribute(QtProperty *property, const QString &name);

  protected:
    int index_ = 0;
    QVariantList enumValues_;
    QComboBox *editor_ = nullptr;
};

class QTPROPERTYSHEET_DLL QtFlagEditor : public QtPropertyEditor {
    Q_OBJECT
  public:
    explicit QtFlagEditor(QtProperty *property);

    virtual QWidget *createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

    void setValueToEditor(int value);

  public slots:
    virtual void onPropertyValueChange(QtProperty *property);

    void checkedItemsChanged(const QStringList &items);

    void slotSetAttribute(QtProperty *property, const QString &name);

  private:
    int value_ = 0;
    QxtCheckComboBox *editor_ = nullptr;
    QStringList flagNames_;
    const QString separator = " | ";
};

class QTPROPERTYSHEET_DLL QtBoolEditor : public QtPropertyEditor {
    Q_OBJECT
  public:
    explicit QtBoolEditor(QtProperty *property);

    virtual QWidget *createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

  public slots:
    virtual void onPropertyValueChange(QtProperty *property);

    virtual void slotEditorValueChange(bool value);

  protected:
    bool value_ = false;
    QtBoolEdit *editor_ = nullptr;
};

class QTPROPERTYSHEET_DLL QtColorEditor : public QtPropertyEditor {
    Q_OBJECT
  public:
    explicit QtColorEditor(QtProperty *property);

    virtual QWidget *createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

  public slots:
    virtual void onPropertyValueChange(QtProperty *property);

    virtual void slotEditorValueChange(const QColor &color);

  protected:
    QColor value_;
    QtColorEditWidget *editor_ = nullptr;
};

class QTPROPERTYSHEET_DLL QtFileEditor : public QtPropertyEditor {
    Q_OBJECT
  public:
    enum DialogType {
        READ_FILE,
        WRITE_FILE,
        DIRECTORY
    };

    explicit QtFileEditor(QtProperty *property);

    virtual QWidget *createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

  public slots:
    virtual void slotButtonClicked();

    virtual void slotEditingFinished();

    virtual void onPropertyValueChange(QtProperty *property);

    virtual void slotEditorDestory(QObject *object);

    virtual void slotSetAttribute(QtProperty *property, const QString &name);

  protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);

    virtual void setValue(const QString &value);

    virtual void onFileSeleted(const QString &fullPath);

    QString value_;
    QString filter_;
    QString relativePath_;
    QWidget *editor_ = nullptr;
    QLineEdit *input_ = nullptr;
    QToolButton *button_ = nullptr;
    DialogType dialogType_ = READ_FILE;
};

class QTPROPERTYSHEET_DLL QtDynamicItemEditor : public QtPropertyEditor {
    Q_OBJECT
  public:
    explicit QtDynamicItemEditor(QtProperty *property);

    virtual QWidget *createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

  public slots:
    virtual void onPropertyValueChange(QtProperty *property);

    void onBtnMoveUp();

    void onBtnMoveDown();

    void onBtnDelete();

  private:
    QWidget *editor_ = nullptr;
    QtPropertyEditor *impl_ = nullptr;
};

class QTPROPERTYSHEET_DLL QtFloatListEditor : public QtPropertyEditor {
    Q_OBJECT
  public:
    explicit QtFloatListEditor(QtProperty *property);

    virtual QWidget *createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

  public slots:
    virtual void onPropertyValueChange(QtProperty *property);

    void slotEditorValueChange(double value);

    void slotSetAttribute(QtProperty *property, const QString &name);

  private:
    void setEditorAttribute(QDoubleSpinBox *editor, QtProperty *property, const QString &name);

    void variantList2Vector(const QList<QVariant> &input, QVector<float> &output);

    int size_ = 0;
    QVector<float> values_;
    QVector<QDoubleSpinBox *> editors_;
};
