//
// Created by Simeon on 4/28/2024.
//

#ifndef LUPUSFIRE_CORE_SUBGAMEOBJECT_H
#define LUPUSFIRE_CORE_SUBGAMEOBJECT_H

class SubGameObject: public GameObject{
public:
    SubGameObject(const std::string& name) : GameObject(name) {}

    std::shared_ptr<GameObject> copyObjects(const GameObject& gameObj) {
        std::shared_ptr<GameObject> copiedGameObject = gameObj.clone();
        addObject(copiedGameObject);
        return copiedGameObject;
    }

    void setName(const std::string& name) {
        m_name = name;
    }

    virtual std::string getName() const override {
        return m_name;
    }


    virtual std::shared_ptr<GameObject> clone() const override {
        std::shared_ptr<SubGameObject> clonedObject = std::make_shared<SubGameObject>(*this);
        clonedObject->setName(this->getName() + "copied");
        return clonedObject;
    }

    void addObject(const std::shared_ptr<GameObject>& obj) {
        m_objects.push_back(obj);
    }

    // Override the Draw method
    virtual void Draw(const Shader& shader) override {
        for (const auto& obj : m_objects) {
            obj->Draw(shader);
        }
    }

    std::vector<std::shared_ptr<GameObject>> getGameObjects() const {
        return m_objects;
    }
private:
    std::vector<std::shared_ptr<GameObject>> m_objects;

};

#endif //LUPUSFIRE_CORE_SUBGAMEOBJECT_H
