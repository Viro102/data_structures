#pragma once

#include <libds/constants.h>
#include <libds/heap_monitor.h>

namespace ds::mm {
template <typename BlockType> class MemoryManager {
  public:
    MemoryManager() = default;

    virtual ~MemoryManager();

    virtual BlockType *allocateMemory();

    virtual void releaseMemory(BlockType *pointer);

    void releaseAndSetNull(BlockType *&pointer);

    size_t getAllocatedBlockCount() const;

  protected:
    size_t allocatedBlockCount_{0};
};

template <typename BlockType> MemoryManager<BlockType>::~MemoryManager() {
    allocatedBlockCount_ = 0;
}

template <typename BlockType> BlockType *MemoryManager<BlockType>::allocateMemory() {
    allocatedBlockCount_++;
    return new BlockType();
}

template <typename BlockType> void MemoryManager<BlockType>::releaseMemory(BlockType *pointer) {
    allocatedBlockCount_--;
    delete pointer;
}

template <typename BlockType>
void MemoryManager<BlockType>::releaseAndSetNull(BlockType *&pointer) {
    this->releaseMemory(pointer);
    pointer = nullptr;
}

template <typename BlockType> size_t MemoryManager<BlockType>::getAllocatedBlockCount() const {
    return allocatedBlockCount_;
}
} // namespace ds::mm
