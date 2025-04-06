//
// Created by SIMEON on 3/31/2025.
//

#ifndef MAINALLOCATOR_H
#define MAINALLOCATOR_H

#include <iostream>
#include <mutex>
#include <unordered_map>
#include "GLFW/glfw3.h"
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cassert>
#include <memory>
#include <unordered_set>

#ifdef _WIN32
#include <malloc.h>
#else
#include <cstdlib>
#endif


namespace IDK
{
    class GLFWMemoryTracker
    {
    public:
        static GLFWwindow* TrackWindowCreation(GLFWwindow* window)
        {
            std::lock_guard<std::mutex> lock(mutex);

            int fbWidth, fbHeight;
            glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

            // - Framebuffer dimensions
            // - 4 bytes per pixel (RGBA)
            // - Triple buffering (3 buffers)
            const size_t &estimate = fbWidth * fbHeight  * 4 * 3;

            allocations[window] = estimate;
            totalAllocated += estimate;

            /*
            std::cerr << "[MEMORY] Window FBO size: ("
                          << fbWidth << "x" << fbHeight << ")"
                          << "[MEMORY] Estimated VRAM: "
                          << estimate / (1024 * 1024) << " MB ("
                          << "4 bytes/pixel, triple buffering)\n";*/

           // PrintCurrentUsage();
            return window;
        }

        static void SafeWindowDestruction(GLFWwindow* window)
        {
            if (!window) return;

            int fbWidth, fbHeight;
            glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

            size_t freed = 0;
            {
                std::lock_guard<std::mutex> lock(mutex);
                auto it = allocations.find(window);
                if (it != allocations.end())
                {
                    freed = it->second;
                    totalAllocated -= freed;
                    allocations.erase(it);
                }
            }

            glfwDestroyWindow(window);

            /*
            std::cerr << "[MEMORY] Window destroyed ("
                  << fbWidth << "x" << fbHeight << "), freed: "
                  << freed / (1024 * 1024) << " MB\n";*/
           // PrintCurrentUsage();
        }
        static size_t GetTotalUsage()
        {
            std::lock_guard<std::mutex> lock(mutex);
            return totalAllocated;
        }

        static void PrintCurrentUsage() {
            std::cout << "[MEMORY] Current GLFW usage: "
                  << totalAllocated / (1024 * 1024) << " MB ("
                  << allocations.size() << " active windows)\n";
        }

        static std::unordered_map<GLFWwindow*, size_t> GetAllocations() {
            std::lock_guard<std::mutex> lock(mutex);
            return allocations;
        }

    private:
        static inline std::unordered_map<GLFWwindow*, size_t> allocations;
        static inline size_t totalAllocated = 0;
        static inline std::mutex mutex;
    };

    class MemoryTracker
    {
    public:
        struct AllocationInfo
        {
            std::string type;
            size_t size;
        };

        static void TrackAllocation(const char* type, void* ptr, size_t size) {
            std::lock_guard<std::mutex> lock(mutex);
            allocations[ptr] = { type, size };
            totalAllocated += size;
           // std::cout << "[MEMORY] Allocated " << type << " at " << ptr << " size: " << size << " bytes.\n";
           // PrintCurrentUsage();
        }

        static void UntrackAllocation(const char* type, void* ptr) {
            std::lock_guard<std::mutex> lock(mutex);
            auto it = allocations.find(ptr);
            if (it != allocations.end()) {
                totalAllocated -= it->second.size;
               // std::cout << "[MEMORY] Deallocated " << type << " at " << ptr << " size: " << it->second.size << " bytes.\n";
                allocations.erase(it);
            } else {
               // std::cerr << "[MEMORY] Untracked deallocation for " << type << " at " << ptr << "\n";
            }
          //  PrintCurrentUsage();
        }

        template<typename T>
        static void TrackAllocation(const char* type, const std::shared_ptr<T>& ptr, size_t size)
        {
            TrackAllocation(type, static_cast<void*>(ptr.get()), size);
        }

        template<typename T>
        static void UntrackAllocation(const char* type, const std::shared_ptr<T>& ptr)
        {
            UntrackAllocation(type, static_cast<void*>(ptr.get()));
        }
        static size_t GetTotalUsage() {
            std::lock_guard<std::mutex> lock(mutex);
            return totalAllocated;
        }

        static void PrintCurrentUsage() {
            std::cout << "[MEMORY] Total allocated: " << totalAllocated / 1024 << " KB (" << allocations.size() << " allocations).\n";
        }

        static std::unordered_map<void*, AllocationInfo> GetAllocations() {
            std::lock_guard<std::mutex> lock(mutex);
            return allocations;
        }
    private:
        static inline std::unordered_map<void*, AllocationInfo> allocations;
        static inline size_t totalAllocated = 0;
        static inline std::mutex mutex;
    };

    template<typename T>
    class MeshPoolAllocator
    {
    public:
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;


        template<typename U>
        struct rebind
        {
            using other=  MeshPoolAllocator<U>;
        };

        // cross-platform aligned allocation
        static pointer aligned_allocate(size_type size, size_type alignment) {
            if (size == 0)
                return nullptr;
#ifdef _WIN32
            void *ptr = _aligned_malloc(size * sizeof(T), alignment);
            if (!ptr)
                throw std::bad_alloc();
#else
            void *ptr = nullptr;
            if (posix_memalign(&ptr, alignment, size * sizeof(T)) != 0)
                throw std::bad_alloc();
#endif
            return static_cast<pointer>(ptr);
        }

        // cross-platform aligne deallocation
        static void aligned_deallocate(pointer p)
        {
#ifdef _WIN32
            _aligned_free(p);
#else
            std::free(p)
#endif
        }

        void clear_all_chunks()
        {
            for (auto& chunk : m_chunks)
            {
                aligned_deallocate(chunk->memory);
            }
            m_chunks.clear();
        }

        // ctor with default chunk size
        explicit MeshPoolAllocator(const size_type& chunkSize = 1024)
            : m_chunkSize(chunkSize)
        {
            std::lock_guard<std::mutex> lock(GlobalCleanup::instance_mutex);
            GlobalCleanup::instances.insert(this);
        }

        // copy ctor
        template<typename U>
        MeshPoolAllocator(const MeshPoolAllocator<U>& other) :
            m_chunkSize(other.m_chunkSize){}

        ~MeshPoolAllocator()
        {
            std::lock_guard<std::mutex> lock(GlobalCleanup::instance_mutex);
            clear_all_chunks();
            GlobalCleanup::instances.erase(this);
        }

        // custom uniqut_ptr deleter
        template<typename E>
        struct MeshDeleter
        {
            void operator()(E* ptr)
            {
                if (ptr)
                {
                    ptr->~T();
                    MeshPoolAllocator<E>::aligned_deallocate(ptr);
                }
            }
        };

        pointer allocate(size_type n)
        {
            if (n == 0) return nullptr;

            // find a chunk with enough free space
            for (auto& chunk : m_chunks)
            {
                if (chunk->freeBlocks >= n)
                {
                    pointer ptr = chunk->memory + chunk->usedBlocks;
                    chunk->usedBlocks += n;
                    chunk->freeBlocks -= n;
                    return ptr;
                }
            }

            // no avail space - allocate new chunk
            size_type elementsInChunk = std::max(m_chunkSize, n);
            std::unique_ptr<Chunk> newChunk(new Chunk());
            newChunk->memory = aligned_allocate(elementsInChunk, alignof(T));
            newChunk->totalBlocks = elementsInChunk;
            newChunk->usedBlocks = n;
            newChunk->freeBlocks = elementsInChunk - n;
            pointer ret = newChunk->memory;
            m_chunks.push_back(std::move(newChunk));
            return ret;
        }

        void deallocate(pointer /*p*/, size_type /*n*/) {}

        void printMemUsage() const
        {
            size_type totalAllocated = 0;
            size_type totalUsed = 0;

            for (const auto& chunk : m_chunks)
            {
                totalAllocated += chunk->totalBlocks * sizeof(T);
                totalUsed += chunk->usedBlocks * sizeof(T);
            }
            std::cout << "Used memory: " << totalUsed << " bytes / total: " << totalAllocated << " bytes" << std::endl;
        }
        static size_type getMemoryUsage(size_type numElements) {
            return numElements * sizeof(T);
        }

    private:
        struct Chunk
        {
            pointer memory = nullptr;
            size_type usedBlocks = 0;
            size_type freeBlocks = 0;
            size_type totalBlocks = 0;

            ~Chunk()
            {
                if (memory)
                {
#ifdef _WIN32
                    _aligned_free(memory);
#else
                    free(memory)
#endif
                }
            }
        };

        std::vector<std::unique_ptr<Chunk>> m_chunks;
        size_type m_chunkSize;

        struct GlobalCleanup
        {
            ~GlobalCleanup()
            {
                std::lock_guard<std::mutex> lock(instance_mutex);
                for (auto instance : instances)
                    instance->clear_all_chunks();
            }

            static std::mutex instance_mutex;
            static std::unordered_set<MeshPoolAllocator*> instances;
        };

        static GlobalCleanup cleanup;
    };

    template<typename T>
    std::mutex MeshPoolAllocator<T>::GlobalCleanup::instance_mutex;

    template<typename T>
    std::unordered_set<MeshPoolAllocator<T>*> MeshPoolAllocator<T>::GlobalCleanup::instances;

    template<typename T>
    typename MeshPoolAllocator<T>::GlobalCleanup MeshPoolAllocator<T>::cleanup;

    template<typename T>
    struct MeshSharedAllocator
    {
        using value_type = T;

        MeshSharedAllocator() = default;

        template<typename U>
        MeshSharedAllocator(const MeshSharedAllocator<U>&) noexcept{}

        T* allocate(std::size_t n)
        {
            return static_cast<T*>(MeshPoolAllocator<T>::aligned_allocate(n, alignof(T)));
        }

        void deallocate(T* p, std::size_t n) noexcept
        {
            MeshPoolAllocator<T>::aligned_deallocate(p);
        }
    };

    template<typename T, typename U>
    bool operator==(const MeshSharedAllocator<T>&, const MeshSharedAllocator<U>&) { return true; }

    template<typename T, typename U>
    bool operator!=(const MeshSharedAllocator<T>&, const MeshSharedAllocator<U>&) { return false; }

}

#ifdef ENABLE_MEMORY_TRACKING
#define TRACK_ALLOC(ptr, type) IDK::MemoryTracker::TrackAllocation(#type, ptr, sizeof(type))
#define UNTRACK_ALLOC(ptr, type) IDK::MemoryTracker::UntrackAllocation(#type, ptr)
#else
#define TRACK_ALLOC(ptr, type)
#define UNTRACK_ALLOC(ptr, type)
#endif

#endif //MAINALLOCATOR_H
