//
// Created by SIMEON on 3/23/2025.
//

#ifndef SELECTIONEVENT_H
#define SELECTIONEVENT_H

#include <memory>
#include "Selectable.h"

class SelectionEvent {
public:
    enum class Type {
        SELECT,
        DESELECT,
        CLEAR
    };

    SelectionEvent(Type type, std::shared_ptr<Entity> object)
        : type(type), object(object) {}

    Type getType() const { return type; }
    std::shared_ptr<Entity> getObject() const { return object; }

private:
    Type type;
    std::shared_ptr<Entity> object;
};

#endif //SELECTIONEVENT_H
