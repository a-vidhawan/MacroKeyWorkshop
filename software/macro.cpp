#include "macro.h"
#include <cstdlib>


Macro::Macro(QObject* parent) : QObject(parent), type("defaultType"), content("defaultContent") {}

Macro::Macro(const QString& userType, const QString& userContent, QObject* parent)
    : QObject(parent), type(userType), content(userContent) {}

Macro::~Macro() {}

void Macro::setType(const QString& newType) {
    if (type != newType) {
        type = newType;
        emit typeChanged();
    }
}

void Macro::setContent(const QString& newContent) {
    if (content != newContent) {
        content = newContent;
        emit contentChanged();
    }
}

QString Macro::getType() const {
    return type;
}

QString Macro::getContent() const {
    return content;
}


