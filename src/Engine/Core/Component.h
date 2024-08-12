//
// Created by Simeon on 4/7/2024.
//

#ifndef LUPUSFIRE_CORE_COMPONENT_H
#define LUPUSFIRE_CORE_COMPONENT_H

#include <memory>

class Component {
public:
    virtual ~Component(){};
    virtual std::unique_ptr<Component> clone() const = 0;

};

#endif //LUPUSFIRE_CORE_COMPONENT_H
