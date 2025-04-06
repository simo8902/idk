//
// Created by SIMEON on 4/5/2025.
//

#ifndef CORE_H
#define CORE_H

#include "EngineAccess.h"
#include "EngineSystems.h"

namespace IDK
{
    class Core
    {
    public:
        static Core& Instance();

        EngineSystems& systems() const { return *engineSystems; }

        template<typename T>
        T& getSystem()
        {
            /*
            if constexpr (std::is_same_v<T, Renderer>) {
                return *systems().getRenderer();
            }*/
            // return engineSystems->getSystem<T>();
        }

        EngineAccess getAccess() {
            return {
                systems().getWindow(),
                systems().getScene(),
                systems().getScheduler()
            };
        }
    private:
        Core();
        ~Core();

        std::unique_ptr<EngineSystems> engineSystems;
    };
}
#endif //CORE_H
