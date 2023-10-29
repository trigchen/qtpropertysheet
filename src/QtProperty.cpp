#include <QLocale>
#include <algorithm>
#include <cassert>

#include "QtAttributeName.h"
#include "QtProperty.h"
#include "QtPropertyBrowserUtils.h"
#include "QtPropertyFactory.h"
#include "QtPropertyType.h"

QtProperty::QtProperty(Type type, QtPropertyFactory *factory) : QObject(factory), factory_(factory), type_(type) {
}


QtProperty::~QtProperty() {
    emit signalPropertyRemoved(this, parent_);

    removeAllChildren(true);
    removeFromParent();
}


QtProperty::Type QtProperty::type() const {
    return type_;
}


QtProperty *QtProperty::parent() {
    return parent_;
}


void QtProperty::setName(const QString &name) {
    if(name != name_) {
        name_ = name;
        emit signalPropertyChange(this);
    }
}


const QString &QtProperty::name() const {
    return name_;
}


const QString &QtProperty::title() const {
    return title_.isEmpty() ? name_ : title_;
}


void QtProperty::setToolTip(const QString &tip) {
    tips_ = tip;
}


const QString &QtProperty::toolTip() const {
    return tips_;
}


void QtProperty::setBackgroundColor(const QColor &cr) {
    bgColor_ = cr;
}


const QColor &QtProperty::backgroundColor() const {
    return bgColor_;
}


void QtProperty::setTitle(const QString &title) {
    if(title != title_) {
        title_ = title;
        emit signalPropertyChange(this);
    }
}


void QtProperty::setValue(const QVariant &value) {
    if(value_ != value) {
        value_ = value;
        emit signalValueChange(this);
    }
}


const QVariant &QtProperty::value() const {
    return value_;
}


void QtProperty::setVisible(bool visible) {
    if(visible != visible_) {
        visible_ = visible;
        emit signalPropertyChange(this);
    }
}


bool QtProperty::isVisible() const {
    return visible_;
}


void QtProperty::setSelfVisible(bool visible) {
    selfVisible_ = visible;
}


bool QtProperty::isSelfVisible() const {
    return selfVisible_;
}


void QtProperty::setMenuVisible(bool visible) {
    menuVisible_ = visible;
}


bool QtProperty::isMenuVisible() const {
    return menuVisible_;
}


QString QtProperty::valueString() const {
    return value_.toString();
}


QIcon QtProperty::valueIcon() const {
    return QIcon();
}


void QtProperty::setAttribute(const QString &name, const QVariant &value) {
    attributes_[name] = value;

    emit signalAttributeChange(this, name);
}


QVariant QtProperty::attribute(const QString &name) const {
    QtPropertyAttributes::const_iterator it = attributes_.constFind(name);
    if(it != attributes_.constEnd()) {
        return *it;
    }
    return QVariant();
}


QtPropertyAttributes &QtProperty::attributes() {
    return attributes_;
}


void QtProperty::addChild(QtProperty *child) {
    assert(child->parent() == nullptr);
    children_.push_back(child);
    child->parent_ = this;

    onChildAdd(child);
    emit signalPropertyInserted(child, this);
}


void QtProperty::removeChild(QtProperty *child) {
    assert(child->parent() == this);
    QtPropertyList::iterator it = std::find(children_.begin(), children_.end(), child);
    if(it != children_.end()) {
        child->parent_ = nullptr;
        children_.erase(it);

        onChildRemove(child);
        emit signalPropertyRemoved(child, this);
    }
}


void QtProperty::removeFromParent() {
    if(parent_ != nullptr) {
        parent_->removeChild(this);
    }
}


void QtProperty::removeAllChildren(bool clean) {
    QtPropertyList temp = children_;
    foreach(QtProperty *child, temp) {
        removeChild(child);

        if(clean) {
            delete child;
        }
    }
}


QtPropertyList &QtProperty::children() {
    return children_;
}


const QtPropertyList &QtProperty::children() const {
    return children_;
}


int QtProperty::indexChild(const QtProperty *child) const {
    return children_.indexOf(const_cast<QtProperty *>(child));
}


QtProperty *QtProperty::findChild(const QString &name) {
    foreach(QtProperty *child, children_) {
        if(child->name() == name) {
            return child;
        }
    }
    return nullptr;
}


void QtProperty::setChildValue(const QString &name, const QVariant &value) {
    QtProperty *child = findChild(name);
    if(child != nullptr) {
        child->setValue(value);
    }
}


bool QtProperty::hasValue() const {
    return true;
}


bool QtProperty::hasAttribute(const QString &name) {
    return attributes_.contains(name) && attributes_[name].isValid();
}


bool QtProperty::isModified() const {
    return false;
}


void QtProperty::onChildAdd(QtProperty * /*child*/) {
}


void QtProperty::onChildRemove(QtProperty * /*child*/) {
}


/********************************************************************/
QtContainerProperty::QtContainerProperty(Type type, QtPropertyFactory *factory) : QtProperty(type, factory) {
}


void QtContainerProperty::onChildAdd(QtProperty *child) {
    connect(child, &QtProperty::signalValueChange, this, &QtContainerProperty::slotChildValueChange);
}


void QtContainerProperty::onChildRemove(QtProperty *child) {
    disconnect(child, &QtProperty::signalValueChange, this, nullptr);
}


/********************************************************************/
static void ensureSize(QVariantList &list, int size) {
    while(list.size() < size) {
        list.push_back(QVariant());
    }
}


QtListProperty::QtListProperty(Type type, QtPropertyFactory *factory) : QtContainerProperty(type, factory) {
}


void QtListProperty::setValue(const QVariant &value) {
    if(value_ == value) {
        return;
    }

    QVariantList valueList = value.toList();
    ensureSize(valueList, children_.size());
    for(int i = 0; i < children_.size(); ++i) {
        children_[i]->setValue(valueList[i]);
    }
    value_ = valueList;

    emit signalValueChange(this);
}


QString QtListProperty::valueString() const {
    QString text;
    text += "[";
    foreach(QtProperty *child, children_) {
        text += child->valueString();
        text += ", ";
    }
    text += "]";
    return text;
}


void QtListProperty::slotChildValueChange(QtProperty *child) {
    int i = indexChild(child);
    if(i >= 0) {
        QVariantList valueList = value_.toList();
        ensureSize(valueList, children_.size());

        if(valueList[i] != child->value()) {
            valueList[i] = child->value();

            value_ = valueList;
            emit signalValueChange(this);
        }
    }
}


/********************************************************************/
QtDictProperty::QtDictProperty(Type type, QtPropertyFactory *factory) : QtContainerProperty(type, factory) {
}


void QtDictProperty::setValue(const QVariant &value) {
    if(value_ == value) {
        return;
    }
    value_ = value;

    QVariantMap valueMap = value_.toMap();
    foreach(QtProperty *child, children_) {
        QVariant value = valueMap.value(child->name());
        child->setValue(value);
    }

    emit signalValueChange(this);
}


void QtDictProperty::slotChildValueChange(QtProperty *property) {
    QVariantMap valueMap = value_.toMap();
    QVariant oldValue = valueMap.value(property->name());
    if(property->value() != oldValue) {
        valueMap[property->name()] = property->value();

        value_ = valueMap;
        emit signalValueChange(this);
    }
}


/********************************************************************/
QtGroupProperty::QtGroupProperty(Type type, QtPropertyFactory *factory) : QtContainerProperty(type, factory) {
}


bool QtGroupProperty::hasValue() const {
    return false;
}


void QtGroupProperty::setValue(const QVariant & /*value*/) {
}


QtProperty *QtGroupProperty::findChild(const QString &name) {
    QtProperty *result = nullptr;
    foreach(QtProperty *child, children_) {
        if(name == child->name()) {
            result = child;
        } else if(child->type() == QtPropertyType::GROUP) {
            result = child->findChild(name);
        }

        if(result != nullptr) {
            break;
        }
    }
    return result;
}


void QtGroupProperty::setChildValue(const QString &name, const QVariant &value) {
    foreach(QtProperty *child, children_) {
        if(child->type() ==  QtPropertyType::GROUP) {
            child->setChildValue(name, value);
        } else if(name == child->name()) {
            child->setValue(value);
        }
    }
}


void QtGroupProperty::slotChildValueChange(QtProperty *property) {
    // emit signal to listner directly.
    emit signalValueChange(property);
}


/********************************************************************/
QtEnumProperty::QtEnumProperty(Type type, QtPropertyFactory *factory) : QtProperty(type, factory) {
}


QString QtEnumProperty::valueString() const {
    int index = value_.toInt();
    QStringList enumNames = attributes_.value(QtAttributeName::EnumName).toStringList();
    if((index >= 0) && (index < enumNames.size())) {
        return enumNames[index];
    }
    return QString();
}


/********************************************************************/
QtFlagProperty::QtFlagProperty(Type type, QtPropertyFactory *factory) : QtProperty(type, factory) {
}


QString QtFlagProperty::valueString() const {
    int value = value_.toInt();
    QStringList enumNames = attributes_.value(QtAttributeName::FlagName).toStringList();
    QStringList selected;
    for(int i = 0; i < enumNames.size(); ++i) {
        if(value & (1 << i)) {
            selected.push_back(enumNames[i]);
        }
    }
    return selected.join(separator);
}


/********************************************************************/
QtBoolProperty::QtBoolProperty(Type type, QtPropertyFactory *factory) : QtProperty(type, factory) {
}


QString QtBoolProperty::valueString() const {
    return value_.toBool() ? "True" : "False";
}


QIcon QtBoolProperty::valueIcon() const {
    return QtPropertyBrowserUtils::drawCheckBox(value_.toBool());
}


/********************************************************************/
QtDoubleProperty::QtDoubleProperty(Type type, QtPropertyFactory *factory) : QtProperty(type, factory) {
}


QString QtDoubleProperty::valueString() const {
    QVariant v = attribute(QtAttributeName::Decimals);
    int decimals = v.typeId() == QVariant::Int ? v.toInt() : 2;
    return QLocale::system().toString(value_.toDouble(), 'f', decimals);
}


/********************************************************************/
QtColorProperty::QtColorProperty(Type type, QtPropertyFactory *factory) : QtProperty(type, factory) {
}


QString QtColorProperty::valueString() const {
    QColor color = QtPropertyBrowserUtils::variant2color(value_);
    return QtPropertyBrowserUtils::colorValueText(color);
}


QIcon QtColorProperty::valueIcon() const {
    QColor color = QtPropertyBrowserUtils::variant2color(value_);
    return QtPropertyBrowserUtils::brushValueIcon(QBrush(color));
}


/********************************************************************/
QtDynamicListProperty::QtDynamicListProperty(Type type, QtPropertyFactory *factory) : QtProperty(type, factory) {
    propLength_ = factory_->createProperty(QtPropertyType::INT);
    propLength_->setName("length");
    propLength_->setTitle(tr("Length"));
    propLength_->setAttribute(QtAttributeName::MinValue, 0);
    addChild(propLength_);
    connect(propLength_, &QtProperty::signalValueChange, this, &QtDynamicListProperty::slotLengthChange);
}


QtDynamicListProperty::~QtDynamicListProperty() {
}


void QtDynamicListProperty::setValue(const QVariant &value) {
    if(value_ == value) {
        return;
    }

    QVariantList valueList = value.toList();
    setLength(valueList.size());

    for(int i = 0; i < items_.size(); ++i) {
        valueList_[i] = valueList[i];
        items_[i]->setValue(valueList[i]);
    }

    value_ = valueList_;
    emit signalValueChange(this);
}


QString QtDynamicListProperty::valueString() const {
    QString ret = "[";
    foreach(QtProperty *item, items_) {
        ret += item->valueString();
        ret += separator;
    }
    ret += "]";
    return ret;
}


void QtDynamicListProperty::slotItemValueChange(QtProperty *item) {
    int i = items_.indexOf(item);
    if(valueList_[i] != item->value()) {
        valueList_[i] = item->value();
        value_ = valueList_;

        emit signalValueChange(this);
    }
}


void QtDynamicListProperty::slotItemMoveUp(QtProperty *item) {
    int i = items_.indexOf(item);
    if(i <= 0) {
        return;
    }
    QtProperty *prev = items_[i - 1];
    QVariant value = prev->value();
    prev->setValue(item->value());
    item->setValue(value);
}


void QtDynamicListProperty::slotItemMoveDown(QtProperty *item) {
    int i = items_.indexOf(item);
    if(i + 1 >= items_.size()) {
        return;
    }
    QtProperty *next = items_[i + 1];
    QVariant value = next->value();
    next->setValue(item->value());
    item->setValue(value);
}


void QtDynamicListProperty::slotItemDelete(QtProperty *item) {
    int i = items_.indexOf(item);
    for( ; i < items_.size() - 1; ++i) {
        items_[i]->setValue(items_[i + 1]->value());
    }

    setLength(length_ - 1);
}


void QtDynamicListProperty::slotLengthChange(QtProperty *property) {
    int length = property->value().toInt();
    setLength(length);

    emit signalValueChange(this);
}


void QtDynamicListProperty::setLength(int length) {
    length = std::max(0, length);
    if(length_ == length) {
        return;
    }

    while(length_ < length) {
        appendItem();
        ++length_;
    }
    while(length_ > length) {
        popItem();
        --length_;
    }
    propLength_->setValue(length_);
    value_ = valueList_;
}


QtProperty *QtDynamicListProperty::appendItem() {
    QtDynamicItemProperty *prop = dynamic_cast<QtDynamicItemProperty *>(factory_->createProperty(QtPropertyType::DYNAMIC_ITEM));
    prop->setValueType(attribute("valueType").toString());

    QVariant valueDefault = attribute("valueDefault");
    prop->setValue(valueDefault);

    QVariantMap attr = attribute("valueAttributes").toMap();
    for(QVariantMap::iterator it = attr.begin(); it != attr.end(); ++it) {
        prop->getImpl()->setAttribute(it.key(), it.value());
    }

    connect(prop, &QtDynamicItemProperty::signalValueChange, this, &QtDynamicListProperty::slotItemValueChange);
    connect(prop, &QtDynamicItemProperty::signalMoveUp, this, &QtDynamicListProperty::slotItemMoveUp);
    connect(prop, &QtDynamicItemProperty::signalMoveDown, this, &QtDynamicListProperty::slotItemMoveDown);
    connect(prop, &QtDynamicItemProperty::signalDelete, this, &QtDynamicListProperty::slotItemDelete);

    valueList_.append(valueDefault);
    items_.append(prop);
    prop->setName(QString::number(items_.size()));
    this->addChild(prop);
    return prop;
}


void QtDynamicListProperty::popItem() {
    QtProperty *item = items_.back();
    items_.pop_back();
    valueList_.pop_back();

    removeChild(item);
    delete item;
}


/********************************************************************/
QtDynamicItemProperty::QtDynamicItemProperty(Type type, QtPropertyFactory *factory) : QtProperty(type, factory) {
}


QtDynamicItemProperty::~QtDynamicItemProperty() {
    if(impl_ != nullptr) {
        delete impl_;
    }
}


void QtDynamicItemProperty::setValueType(Type type) {
    if(impl_ != nullptr) {
        if(impl_->type() == type) {
            return;
        }
        delete impl_;
    }
    impl_ = factory_->createProperty(type);
    assert(impl_ != nullptr);

    connect(impl_, &QtProperty::signalValueChange, this, &QtDynamicItemProperty::onImplValueChange);
}


QtProperty *QtDynamicItemProperty::getImpl() {
    return impl_;
}


void QtDynamicItemProperty::setValue(const QVariant &value) {
    impl_->setValue(value);
}


const QVariant &QtDynamicItemProperty::value() const {
    return impl_->value();
}


QString QtDynamicItemProperty::valueString() const {
    return impl_->valueString();
}


QIcon QtDynamicItemProperty::valueIcon() const {
    return impl_->valueIcon();
}


void QtDynamicItemProperty::onImplValueChange(QtProperty *property) {
    emit signalValueChange(this);
}


/********************************************************************/
QtFloatListProperty::QtFloatListProperty(Type type, QtPropertyFactory *factory) : QtProperty(type, factory) {
}


QtFloatListProperty::~QtFloatListProperty() {
}


QString QtFloatListProperty::valueString() const {
    int size = attribute(QtAttributeName::Size).toInt();
    QString ret;
    ret += "[";

    QVariantList lst = value_.toList();
    for(int i = 0; i < size; ++i) {
        if(i != 0) {
            ret += separator;
        }

        double v = 0.0;
        if(i < lst.size()) {
            v = lst[i].toDouble();
        }
        ret += QString::number(v);
    }

    ret += "]";
    return ret;
}
