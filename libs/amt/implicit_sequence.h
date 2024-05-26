#pragma once

#include "../constants.h"
#include "../mm/compact_memory_manager.h"
#include "../mm/memory_manager.h"
#include "abstract_memory_type.h"
#include "sequence.h"

namespace ds::amt {
template <typename DataType>
class ImplicitSequence : public Sequence<MemoryBlock<DataType>>, public ImplicitAMS<DataType> {
  public:
    using BlockType = MemoryBlock<DataType>;
    using MemoryManagerType = typename ImplicitAMS<DataType>::MemoryManagerType;
    using value_type = DataType;

    ImplicitSequence() = default;

    ImplicitSequence(size_t capacity, bool initBlocks);

    ImplicitSequence(const ImplicitSequence<DataType> &other);

    ~ImplicitSequence() override = default;

    size_t calculateIndex(BlockType &block) override;

    BlockType *accessFirst() const override;

    BlockType *accessLast() const override;

    BlockType *access(size_t index) const override;

    BlockType *accessNext(const BlockType &block) const override;

    BlockType *accessPrevious(const BlockType &block) const override;

    BlockType &insertFirst() override;

    BlockType &insertLast() override;

    BlockType &insert(size_t index) override;

    BlockType &insertAfter(BlockType &block) override;

    BlockType &insertBefore(BlockType &block) override;

    void removeFirst() override;

    void removeLast() override;

    void remove(size_t index) override;

    void removeNext(const BlockType &block) override;

    void removePrevious(const BlockType &block) override;

    void reserveCapacity(size_t capacity);

    virtual size_t indexOfNext(size_t currentIndex) const;

    virtual size_t indexOfPrevious(size_t currentIndex) const;

    class ImplicitSequenceIterator {
      public:
        ImplicitSequenceIterator(ImplicitSequence<DataType> *sequence, size_t index);

        ImplicitSequenceIterator(const ImplicitSequenceIterator &other);

        ImplicitSequenceIterator &operator++();

        ImplicitSequenceIterator operator++(int);

        bool operator==(const ImplicitSequenceIterator &other) const;

        bool operator!=(const ImplicitSequenceIterator &other) const;

        DataType &operator*();

      private:
        ImplicitSequence<DataType> *sequence_;
        size_t position_;
    };

    using IteratorType = ImplicitSequenceIterator;

    IteratorType begin();

    IteratorType end();
};

template <typename DataType> using IS = ImplicitSequence<DataType>;

//----------

template <typename DataType> class CyclicImplicitSequence : public IS<DataType> {
  public:
    CyclicImplicitSequence();

    CyclicImplicitSequence(size_t initSize, bool initBlocks);

    size_t indexOfNext(size_t currentIndex) const override;

    size_t indexOfPrevious(size_t currentIndex) const override;
};

template <typename DataType> using CIS = CyclicImplicitSequence<DataType>;

//----------

template <typename DataType>
ImplicitSequence<DataType>::ImplicitSequence(size_t initialSize, bool initBlocks)
    : ImplicitAMS<DataType>(initialSize, initBlocks) {}

template <typename DataType>
ImplicitSequence<DataType>::ImplicitSequence(const ImplicitSequence<DataType> &other)
    : ImplicitAMS<DataType>::ImplicitAbstractMemoryStructure(other) {}

template <typename DataType> size_t ImplicitSequence<DataType>::calculateIndex(BlockType &block) {
    return this->getMemoryManager()->calculateIndex(block);
}

template <typename DataType>
typename ImplicitSequence<DataType>::BlockType *ImplicitSequence<DataType>::accessFirst() const {
    return this->size() > 0 ? &this->getMemoryManager()->getBlockAt(0) : nullptr;
}

template <typename DataType>
typename ImplicitSequence<DataType>::BlockType *ImplicitSequence<DataType>::accessLast() const {
    return this->size() > 0 ? &this->getMemoryManager()->getBlockAt(this->size() - 1) : nullptr;
}

template <typename DataType>
typename ImplicitSequence<DataType>::BlockType *
ImplicitSequence<DataType>::access(size_t index) const {
    return index >= 0 && index < this->size() ? &this->getMemoryManager()->getBlockAt(index)
                                              : nullptr;
}

template <typename DataType>
typename ImplicitSequence<DataType>::BlockType *
ImplicitSequence<DataType>::accessNext(const BlockType &block) const {
    size_t index = this->indexOfNext(this->getMemoryManager()->calculateIndex(block));
    return index < this->size() ? &this->getMemoryManager()->getBlockAt(index) : nullptr;
}

template <typename DataType>
typename ImplicitSequence<DataType>::BlockType *
ImplicitSequence<DataType>::accessPrevious(const BlockType &block) const {
    size_t index = this->indexOfPrevious(this->getMemoryManager()->calculateIndex(block));
    return index >= 0 && index < this->size() ? &this->getMemoryManager()->getBlockAt(index)
                                              : nullptr;
}

template <typename DataType>
typename ImplicitSequence<DataType>::BlockType &ImplicitSequence<DataType>::insertFirst() {
    return *this->getMemoryManager()->allocateMemoryAt(0);
}

template <typename DataType>
typename ImplicitSequence<DataType>::BlockType &ImplicitSequence<DataType>::insertLast() {
    return *this->getMemoryManager()->allocateMemory();
}

template <typename DataType>
typename ImplicitSequence<DataType>::BlockType &ImplicitSequence<DataType>::insert(size_t index) {
    return *this->getMemoryManager()->allocateMemoryAt(index);
}

template <typename DataType>
typename ImplicitSequence<DataType>::BlockType &
ImplicitSequence<DataType>::insertAfter(BlockType &block) {
    return *this->getMemoryManager()->allocateMemoryAt(
        this->getMemoryManager()->calculateIndex(block) + 1);
}

template <typename DataType>
typename ImplicitSequence<DataType>::BlockType &
ImplicitSequence<DataType>::insertBefore(BlockType &block) {
    return *this->getMemoryManager()->allocateMemoryAt(
        this->getMemoryManager()->calculateIndex(block));
}

template <typename DataType> void ImplicitSequence<DataType>::removeFirst() {
    this->getMemoryManager()->releaseMemoryAt(0);
}

template <typename DataType> void ImplicitSequence<DataType>::removeLast() {
    this->getMemoryManager()->releaseMemory();
}

template <typename DataType> void ImplicitSequence<DataType>::remove(size_t index) {
    this->getMemoryManager()->releaseMemoryAt(index);
}

template <typename DataType> void ImplicitSequence<DataType>::removeNext(const BlockType &block) {
    this->getMemoryManager()->releaseMemoryAt(
        this->indexOfNext(this->getMemoryManager()->calculateIndex(block)));
}

template <typename DataType>
void ImplicitSequence<DataType>::removePrevious(const BlockType &block) {
    this->getMemoryManager()->releaseMemoryAt(
        this->indexOfPrevious(this->getMemoryManager()->calculateIndex(block)));
}

template <typename DataType> void ImplicitSequence<DataType>::reserveCapacity(size_t capacity) {
    this->getMemoryManager()->changeCapacity(capacity);
}

template <typename DataType>
size_t ImplicitSequence<DataType>::indexOfNext(size_t currentIndex) const {
    return currentIndex >= this->size() - 1 ? INVALID_INDEX : currentIndex + 1;
}

template <typename DataType>
size_t ImplicitSequence<DataType>::indexOfPrevious(size_t currentIndex) const {
    return currentIndex <= 0 ? INVALID_INDEX : currentIndex - 1;
}

template <typename DataType>
ImplicitSequence<DataType>::ImplicitSequenceIterator::ImplicitSequenceIterator(
    ImplicitSequence<DataType> *sequence, size_t index)
    : sequence_(sequence), position_(index) {}

template <typename DataType>
ImplicitSequence<DataType>::ImplicitSequenceIterator::ImplicitSequenceIterator(
    const ImplicitSequenceIterator &other)
    : sequence_(other.sequence_), position_(other.position_) {}

template <typename DataType>
typename ImplicitSequence<DataType>::ImplicitSequenceIterator &
ImplicitSequence<DataType>::ImplicitSequenceIterator::operator++() {
    ++position_;
    return *this;
}

template <typename DataType>
typename ImplicitSequence<DataType>::ImplicitSequenceIterator
ImplicitSequence<DataType>::ImplicitSequenceIterator::operator++(int) {
    ImplicitSequenceIterator tmp(*this);
    this->operator++();
    return tmp;
}

template <typename DataType>
bool ImplicitSequence<DataType>::ImplicitSequenceIterator::operator==(
    const ImplicitSequenceIterator &other) const {
    return this->sequence_ == other.sequence_ && this->position_ == other.position_;
}

template <typename DataType>
bool ImplicitSequence<DataType>::ImplicitSequenceIterator::operator!=(
    const ImplicitSequenceIterator &other) const {
    return this->sequence_ != other.sequence_ || this->position_ != other.position_;
}

template <typename DataType>
DataType &ImplicitSequence<DataType>::ImplicitSequenceIterator::operator*() {
    return this->sequence_->access(this->position_)->data_;
}

template <typename DataType>
typename ImplicitSequence<DataType>::ImplicitSequenceIterator ImplicitSequence<DataType>::begin() {
    return ImplicitSequenceIterator(this, 0);
}

template <typename DataType>
typename ImplicitSequence<DataType>::ImplicitSequenceIterator ImplicitSequence<DataType>::end() {
    return ImplicitSequenceIterator(this, this->size());
}

template <typename DataType>
CyclicImplicitSequence<DataType>::CyclicImplicitSequence() : IS<DataType>() {}

template <typename DataType>
CyclicImplicitSequence<DataType>::CyclicImplicitSequence(size_t initCapacity, bool initBlocks)
    : IS<DataType>(initCapacity, initBlocks) {}

template <typename DataType>
size_t CyclicImplicitSequence<DataType>::indexOfNext(size_t currentIndex) const {
    size_t size = this->size();

    if (size != 0) {
        return (currentIndex >= size - 1) ? 0 : currentIndex + 1;
    } else {
        return INVALID_INDEX;
    }
}

template <typename DataType>
size_t CyclicImplicitSequence<DataType>::indexOfPrevious(size_t currentIndex) const {
    size_t size = this->size();

    if (size != 0) {
        return (currentIndex <= 0) ? size - 1 : currentIndex - 1;
    } else {
        return INVALID_INDEX;
    }
}
} // namespace ds::amt
