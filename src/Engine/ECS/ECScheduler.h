//
// Created by SIMEON on 4/5/2025.
//

#ifndef ECSCHEDULER_H
#define ECSCHEDULER_H

#include <utility>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <future>

namespace IDK
{
    class ECScheduler
    {
    public:
        enum class ExecutionPolicy
        {
            Immediate,
            MainThread,
            Parallel,
            ParallelBatch
        };

        struct System
        {
            std::function<void()> func;
            ExecutionPolicy policy;
            std::string name;
        };

        // for classes with update()
        template <typename T, typename... Args>
        void addSystem(ExecutionPolicy policy, Args&&... args)
        {
            auto system = std::make_shared<T>(std::forward<Args>(args)...);
            systems.push_back({
                [system]() { system->update(); },
                policy,
                typeid(T).name()
            });
        }

        // for raw func or classes with different method names
        template<typename T, typename Method, typename... Args>
        void addSystem(ExecutionPolicy policy, Method method, Args&&... args)
        {
            auto system = std::make_shared<T>(std::forward<Args>(args)...);
            systems.push_back({
                [system, method]() {(system.get()->*method)(); },
                policy,
                typeid(T).name()
            });
        }

        void addSystem(std::function<void()> func,
            ExecutionPolicy policy = ExecutionPolicy::MainThread,
            const std::string& name = "")
        {
            systems.push_back({std::move(func), policy, name});
        }

        void runFrame() {
            if (!running) return;

            executeStage<ExecutionPolicy::Immediate>();
            executeStage<ExecutionPolicy::Parallel>();
            executeStage<ExecutionPolicy::MainThread>();
        }

        void shutdown()
        {
            running = false;

            for (auto& f : futures)
            {
                if (f.valid())
                {
                    f.wait();
                }
            }
            futures.clear();
        }

    private:
        std::vector<System> systems;
        std::vector<std::future<void>> futures;
        std::atomic<bool> running{true};

        template<ExecutionPolicy policy>
        void executeStage()
        {
            for (auto& sys : systems)
            {
                if (sys.policy == policy){
                    if (sys.policy == ExecutionPolicy::Parallel)
                    {
                        futures.emplace_back(std::async(std::launch::async, sys.func));
                    }else
                    {
                        sys.func();
                    }
                }
            }
            if constexpr (policy == ExecutionPolicy::Parallel)
            {
                for (auto& f : futures)
                {
                    if (f.valid())
                    {
                        f.wait();
                    }
                }
                futures.clear();
            }
        }
    };
}
#endif //ECSCHEDULER_H
