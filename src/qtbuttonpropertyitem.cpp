#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>

#include "qtbuttonpropertyitem.h"
#include "qtproperty.h"
#include "qtpropertyeditorfactory.h"

QtButtonPropertyItem::QtButtonPropertyItem() {
}


QtButtonPropertyItem::QtButtonPropertyItem(QtProperty *prop, QtButtonPropertyItem *parent, QtPropertyEditorFactory *editorFactory) : property_(prop) {
    if(parent != nullptr) {
        layout_ = parent->layout_;
    }

    if(!property_->getChildren().empty()) {
        int row = layout_->rowCount();

        titleButton_ = new QToolButton();
        titleButton_->setText(property_->getTitle());
        titleButton_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        titleButton_->setArrowType(Qt::UpArrow);
        titleButton_->setIconSize(QSize(16, 16));
        layout_->addWidget(titleButton_, row, 0);
        connect(titleButton_, SIGNAL(clicked(bool)), this, SLOT(onBtnExpand()));

        if(property_->isMenuVisible()) {
            QFont font = titleButton_->font();
            font.setBold(true);
            font.setPointSize(16);
            titleButton_->setFont(font);
            titleButton_->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));

            titleMenu_ = new QToolButton();
            titleMenu_->setText("...");
            layout_->addWidget(titleMenu_, row, 1, Qt::AlignRight);
            connect(titleMenu_, SIGNAL(clicked(bool)), this, SLOT(onBtnMenu()));
        } else {
            QFont font = titleButton_->font();
            font.setBold(true);
            titleButton_->setFont(font);

            valueLabel_ = new QLabel(property_->getValueString());
            valueLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            layout_->addWidget(valueLabel_, row, 1, Qt::AlignLeft);
        }

        QFrame *frame = new QFrame();
        frame->setFrameShape(QFrame::Panel);
        frame->setFrameShadow(QFrame::Raised);
        container_ = frame;
        container_->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));
        layout_->addWidget(container_, layout_->rowCount(), 0, 1, 2);

        layout_ = new QGridLayout();
        layout_->setSpacing(4);
        layout_->setContentsMargins(8, 8, 8, 8);
        container_->setLayout(layout_);
    } else {
        int row = layout_->rowCount();

        label_ = new QLabel(property_->getTitle());
        label_->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
        layout_->addWidget(label_, row, 0);

        editor_ = editorFactory->createEditor(prop, nullptr);
        if(editor_) {
            layout_->addWidget(editor_, row, 1);
        } else {
            valueLabel_ = new QLabel(property_->getValueString());
            valueLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            layout_->addWidget(valueLabel_, row, 1, Qt::AlignLeft);
        }
    }

    if(valueLabel_ != nullptr) {
        connect(property_, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(onPropertyValueChange(QtProperty*)));
    }
}


QtButtonPropertyItem::~QtButtonPropertyItem() {
    removeFromParent();

    foreach(QtButtonPropertyItem *item, children_) {
        if(item != nullptr) {
            item->parent_ = nullptr;
            delete item;
            item = nullptr;
        }
    }
    children_.clear();
    for(QObject *obj : QList<QObject *>({titleButton_, titleMenu_, label_, container_, editor_})) {
        if(obj != nullptr) {
            delete obj;
            obj = nullptr;
        }
    }
}


void QtButtonPropertyItem::update() {
}


void QtButtonPropertyItem::addChild(QtButtonPropertyItem *child) {
    children_.push_back(child);
}


void QtButtonPropertyItem::removeChild(QtButtonPropertyItem *child) {
    children_.removeOne(child);
}


void QtButtonPropertyItem::removeFromParent() {
    if(parent_ != nullptr) {
        parent_->removeChild(this);
        parent_ = nullptr;
    }
}


void QtButtonPropertyItem::setTitle(const QString &title) {
    if(titleButton_ != nullptr) {
        titleButton_->setText(title);
    }
    if(label_ != nullptr) {
        label_->setText(title);
    }
}


void QtButtonPropertyItem::setVisible(bool visible) {
    for(QWidget *w : QList<QWidget *>({titleButton_, titleMenu_, label_, editor_, valueLabel_})) {
        if(w != nullptr) {
            w->setVisible(visible);
        }
    }
    if(container_ != nullptr) {
        container_->setVisible(visible && bExpand_);
    }
}


void QtButtonPropertyItem::setExpanded(bool expand) {
    if(bExpand_ == expand) {
        return;
    }
    bExpand_ = expand;

    if(titleButton_) {
        titleButton_->setArrowType(expand ? Qt::UpArrow : Qt::DownArrow);
    }

    if(container_) {
        container_->setVisible(expand);
    }
}


void QtButtonPropertyItem::onBtnExpand() {
    setExpanded(!bExpand_);
}


void QtButtonPropertyItem::onBtnMenu() {
    emit property_->signalPopupMenu(property_);
}


void QtButtonPropertyItem::onPropertyValueChange(QtProperty *property) {
    QString text = property_->getValueString();
    if(text.size() > 20) {
        text.remove(20, text.size() - 20);
        text += "...";
    }
    valueLabel_->setText(text);
}
