#pragma once

#include <QHash>
#include <QObject>

#include "QtPropertyType.h"

class QtProperty;
class QtPropertyFactory;

class QtPropertyCreator {
  public:
    virtual ~QtPropertyCreator() {
    }


    virtual QtProperty *create() = 0;
};


class QTPROPERTYSHEET_DLL QtPropertyFactory : public QObject {
    Q_OBJECT
  public:
    explicit QtPropertyFactory(QObject *parent = nullptr);

    virtual ~QtPropertyFactory();

    QtProperty *createProperty(QtPropertyType::Type type);

    void registerCreator(QtPropertyType::Type type, QtPropertyCreator *method);

    template<typename T>
    void registerSimpleCreator(QtPropertyType::Type type);

  private:
    typedef QHash<QtPropertyType::Type, QtPropertyCreator *> CreatorMap;
    CreatorMap propertyCreator_;
};


template<typename T>
class QTPROPERTYSHEET_DLL QtSimplePropertyCreator : public QtPropertyCreator {
    QtPropertyType::Type type_;
    QtPropertyFactory *factory_;
  public:
    QtSimplePropertyCreator(QtPropertyType::Type type, QtPropertyFactory *factory) : type_(type), factory_(factory) {
    }


    virtual QtProperty *create() {
        return new T(type_, factory_);
    }
};

template<typename T>
void QtPropertyFactory::registerSimpleCreator(QtPropertyType::Type type) {
    registerCreator(type, new QtSimplePropertyCreator<T>(type, this));
}
