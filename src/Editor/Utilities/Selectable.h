//
// Created by SIMEON on 3/23/2025.
//

#ifndef SELECTABLE_H
#define SELECTABLE_H

#include <string>

class Selectable {
    public:
        virtual ~Selectable() = default;
        virtual std::string getName() const = 0;
        virtual void select() = 0;
        virtual void deselect() = 0;
};
#endif //SELECTABLE_H
