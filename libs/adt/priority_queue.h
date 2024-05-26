#pragma once

#include "../amt/explicit_sequence.h"
#include "../amt/implicit_hierarchy.h"
#include "../amt/implicit_sequence.h"
#include "abstract_data_type.h"
#include <cmath>
#include <stdexcept>

namespace ds::adt {
template <typename P, typename T> struct PriorityQueueItem {
    P priority_;
    T data_;

    bool operator==(const PriorityQueueItem<P, T> &other) const {
        return priority_ == other.priority_ && data_ == other.data_;
    }

    bool operator!=(const PriorityQueueItem<P, T> &other) const { return !(*this == other); }
};

template <typename P, typename T> using PQItem = PriorityQueueItem<P, T>;

//----------

template <typename P, typename T> class PriorityQueue : virtual public ADT {
  public:
    virtual void push(P priority, T data) = 0;

    virtual T &peek() = 0;

    virtual T pop() = 0;
};

template <typename P, typename T> using PQ = PriorityQueue<P, T>;

//----------

template <typename P, typename T, typename SequenceType>
class SequencePriorityQueue : public PriorityQueue<P, T>, public ADS<PQItem<P, T>> {
  public:
    SequencePriorityQueue();

    SequencePriorityQueue(const SequencePriorityQueue &other);

    bool equals(const ADT &other) override;

  protected:
    SequenceType *getSequence() const;
};

template <typename P, typename T, typename SequenceType>
using SPQ = SequencePriorityQueue<P, T, SequenceType>;

//----------

template <typename P, typename T, typename SequenceType>
class UnsortedSequencePriorityQueue : public SequencePriorityQueue<P, T, SequenceType> {
  public:
    T &peek() override;

  protected:
    typename SequenceType::BlockType *findHighestPriorityBlock();
};

template <typename P, typename T, typename SequenceType>
using UnsortedSPQ = UnsortedSequencePriorityQueue<P, T, SequenceType>;

//----------

template <typename P, typename T, typename SequenceType>
class SortedSequencePriorityQueue : public SequencePriorityQueue<P, T, SequenceType> {
  public:
    T &peek() override;

    T pop() override;

  protected:
    virtual size_t indexOfHighestPriorityBlock() const = 0;
};

template <typename P, typename T, typename SequenceType>
using SortedSPQ = SortedSequencePriorityQueue<P, T, SequenceType>;

//----------

template <typename P, typename T>
class UnsortedImplicitSequencePriorityQueue
    : public UnsortedSequencePriorityQueue<P, T, amt::IS<PQItem<P, T>>> {
  public:
    void push(P priority, T data) override;

    T pop() override;

  private:
    using SequenceBlockType = typename amt::IS<PQItem<P, T>>::BlockType;
};

template <typename P, typename T> using UnsortedISPQ = UnsortedImplicitSequencePriorityQueue<P, T>;

//----------

template <typename P, typename T>
class UnsortedExplicitSequencePriorityQueue
    : public UnsortedSequencePriorityQueue<P, T, amt::SinglyLS<PQItem<P, T>>> {
  public:
    void push(P priority, T data) override;

    T pop() override;

  private:
    using SequenceBlockType = typename amt::SinglyLS<PQItem<P, T>>::BlockType;
};

template <typename P, typename T> using UnsortedESPQ = UnsortedExplicitSequencePriorityQueue<P, T>;

//----------

template <typename P, typename T>
class SortedImplicitSequencePriorityQueue
    : public SortedSequencePriorityQueue<P, T, amt::IS<PQItem<P, T>>> {
  public:
    void push(P priority, T data) override;

  protected:
    size_t indexOfHighestPriorityBlock() const override;

  private:
    using SequenceBlockType = typename amt::IS<PQItem<P, T>>::BlockType;
};

template <typename P, typename T> using SortedISPQ = SortedImplicitSequencePriorityQueue<P, T>;

//----------

template <typename P, typename T>
class SortedExplicitSequencePriorityQueue
    : public SortedSequencePriorityQueue<P, T, amt::SinglyLS<PQItem<P, T>>> {
  public:
    void push(P priority, T data) override;

  protected:
    size_t indexOfHighestPriorityBlock() const override;

  private:
    using SequenceBlockType = typename amt::SinglyLS<PQItem<P, T>>::BlockType;
};

template <typename P, typename T> using SortedESPQ = SortedExplicitSequencePriorityQueue<P, T>;

//----------

template <typename P, typename T>
class TwoLists : public AUMS<PQItem<P, T>>, public PriorityQueue<P, T> {
  public:
    TwoLists(size_t expectedSize);

    TwoLists(const TwoLists<P, T> &other);

    ~TwoLists();

    ADT &assign(const ADT &other) override;

    void clear() override;

    size_t size() const override;

    bool isEmpty() const override;

    bool equals(const ADT &other) override;

    void push(P priority, T data) override;

    T &peek() override;

    T pop() override;

  private:
    using ShortSequenceType = amt::IS<PQItem<P, T>>;
    using LongSequenceType = amt::SinglyLS<PQItem<P, T>>;
    using ShortSequenceBlockType = typename ShortSequenceType::BlockType;
    using LongSequenceBlockType = typename LongSequenceType::BlockType;

    ShortSequenceType *shortSequence_;
    LongSequenceType *longSequence_;
};

//----------

template <typename P, typename T>
class BinaryHeap : public PriorityQueue<P, T>, public ADS<PQItem<P, T>> {
  public:
    BinaryHeap();

    BinaryHeap(const BinaryHeap &other);

    bool equals(const ADT &other) override;

    void push(P priority, T data) override;

    T &peek() override;

    T pop() override;

  private:
    using HierarchyBlockType = typename amt::BinaryIH<PQItem<P, T>>::BlockType;

    amt::BinaryIH<PQItem<P, T>> *getHierarchy();
};

//----------

template <typename P, typename T, typename SequenceType>
SequencePriorityQueue<P, T, SequenceType>::SequencePriorityQueue()
    : ADS<PQItem<P, T>>(new SequenceType()) {}

template <typename P, typename T, typename SequenceType>
SequencePriorityQueue<P, T, SequenceType>::SequencePriorityQueue(const SequencePriorityQueue &other)
    : ADS<PQItem<P, T>>(new SequenceType(), other) {}

template <typename P, typename T, typename SequenceType>
bool SequencePriorityQueue<P, T, SequenceType>::equals(const ADT &) {
    throw std::logic_error("Unsupported operation!");
}

template <typename P, typename T, typename SequenceType>
SequenceType *SequencePriorityQueue<P, T, SequenceType>::getSequence() const {
    return dynamic_cast<SequenceType *>(this->memoryStructure_);
}

template <typename P, typename T, typename SequenceType>
T &UnsortedSequencePriorityQueue<P, T, SequenceType>::peek() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty!");
    }

    return this->findHighestPriorityBlock()->data_.data_;
}

template <typename P, typename T, typename SequenceType>
typename SequenceType::BlockType *
UnsortedSequencePriorityQueue<P, T, SequenceType>::findHighestPriorityBlock() {
    typename SequenceType::BlockType *bestBlock = this->getSequence()->accessFirst();

    this->getSequence()->processBlocksForward(this->getSequence()->accessNext(*bestBlock),
                                              [&bestBlock](typename SequenceType::BlockType *b) {
                                                  if (bestBlock->data_.priority_ >
                                                      b->data_.priority_) {
                                                      bestBlock = b;
                                                  }
                                              });

    return bestBlock;
}

template <typename P, typename T, typename SequenceType>
T &SortedSequencePriorityQueue<P, T, SequenceType>::peek() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty!");
    }

    return this->getSequence()->access(this->indexOfHighestPriorityBlock())->data_.data_;
}

template <typename P, typename T, typename SequenceType>
T SortedSequencePriorityQueue<P, T, SequenceType>::pop() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty!");
    }

    size_t index = this->indexOfHighestPriorityBlock();
    T result = this->getSequence()->access(index)->data_.data_;
    this->getSequence()->remove(index);
    return result;
}

template <typename P, typename T>
void UnsortedImplicitSequencePriorityQueue<P, T>::push(P priority, T data) {
    PQItem<P, T> &queueData = this->getSequence()->insertLast().data_;
    queueData.priority_ = priority;
    queueData.data_ = data;
}

template <typename P, typename T> T UnsortedImplicitSequencePriorityQueue<P, T>::pop() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty!");
    }

    SequenceBlockType *bestBlock = this->findHighestPriorityBlock();
    T result = bestBlock->data_.data_;
    SequenceBlockType *lastBlock = this->getSequence()->accessLast();
    if (bestBlock != lastBlock) {
        using std::swap;
        swap(bestBlock->data_, lastBlock->data_);
    }
    this->getSequence()->removeLast();
    return result;
}

template <typename P, typename T>
void UnsortedExplicitSequencePriorityQueue<P, T>::push(P priority, T data) {
    PQItem<P, T> &queueData = this->getSequence()->insertFirst().data_;
    queueData.priority_ = priority;
    queueData.data_ = data;
}

template <typename P, typename T> T UnsortedExplicitSequencePriorityQueue<P, T>::pop() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty!");
    }

    SequenceBlockType *bestBlock = this->findHighestPriorityBlock();
    T result = bestBlock->data_.data_;
    SequenceBlockType *firstBlock = this->getSequence()->accessFirst();
    if (bestBlock != firstBlock) {
        std::swap(bestBlock->data_, firstBlock->data_);
    }
    this->getSequence()->removeFirst();
    return result;
}

template <typename P, typename T>
void SortedImplicitSequencePriorityQueue<P, T>::push(P priority, T data) {
    PQItem<P, T> *queueData = nullptr;

    if (this->isEmpty() || priority <= this->getSequence()->accessLast()->data_.priority_) {
        queueData = &this->getSequence()->insertLast().data_;
    } else if (priority >= this->getSequence()->accessFirst()->data_.priority_) {
        queueData = &this->getSequence()->insertFirst().data_;
    } else {
        queueData = &this->getSequence()
                         ->insertBefore(*this->getSequence()->findBlockWithProperty(
                             [&priority](SequenceBlockType *block) -> bool {
                                 return block->data_.priority_ <= priority;
                             }))
                         .data_;
    }

    queueData->priority_ = priority;
    queueData->data_ = data;
}

template <typename P, typename T>
size_t SortedImplicitSequencePriorityQueue<P, T>::indexOfHighestPriorityBlock() const {
    return this->size() - 1;
}

template <typename P, typename T>
void SortedExplicitSequencePriorityQueue<P, T>::push(P priority, T data) {
    PQItem<P, T> *queueData = nullptr;

    if (this->isEmpty() || priority <= this->getSequence()->accessFirst()->data_.priority_) {
        queueData = &this->getSequence()->insertFirst().data_;
    } else if (priority >= this->getSequence()->accessLast()->data_.priority_) {
        queueData = &this->getSequence()->insertLast().data_;
    } else {
        SequenceBlockType *prevBlock = this->getSequence()->findPreviousToBlockWithProperty(
            [priority](SequenceBlockType *block) -> bool {
                return block->data_.priority_ >= priority;
            });
        queueData = &this->getSequence()->insertAfter(*prevBlock).data_;
    }

    queueData->priority_ = priority;
    queueData->data_ = data;
}

template <typename P, typename T>
size_t SortedExplicitSequencePriorityQueue<P, T>::indexOfHighestPriorityBlock() const {
    return 0;
}

template <typename P, typename T>
TwoLists<P, T>::TwoLists(size_t expectedSize)
    : shortSequence_(
          new ShortSequenceType(static_cast<size_t>(std::ceil(std::sqrt(expectedSize))), false)),
      longSequence_(new LongSequenceType()) {}

template <typename P, typename T>
TwoLists<P, T>::TwoLists(const TwoLists<P, T> &other)
    : shortSequence_(new ShortSequenceType()), longSequence_(new LongSequenceType()) {
    assign(other);
}

template <typename P, typename T> TwoLists<P, T>::~TwoLists() {
    delete shortSequence_;
    shortSequence_ = nullptr;
    delete longSequence_;
    longSequence_ = nullptr;
}

template <typename P, typename T> ADT &TwoLists<P, T>::assign(const ADT &other) {
    if (this != &other) {
        const TwoLists<P, T> &otherTwoLists = dynamic_cast<const TwoLists<P, T> &>(other);

        shortSequence_->assign(*otherTwoLists.shortSequence_);
        longSequence_->assign(*otherTwoLists.longSequence_);
    }

    return *this;
}

template <typename P, typename T> void TwoLists<P, T>::clear() {
    shortSequence_->clear();
    longSequence_->clear();
}

template <typename P, typename T> size_t TwoLists<P, T>::size() const {
    return shortSequence_->size() + longSequence_->size();
}

template <typename P, typename T> bool TwoLists<P, T>::isEmpty() const {
    return shortSequence_->isEmpty();
}

template <typename P, typename T> bool TwoLists<P, T>::equals(const ADT &other) {
    throw std::logic_error("Unsupported operation!");
}

template <typename P, typename T> void TwoLists<P, T>::push(P priority, T data) {
    PQItem<P, T> *dataFront = nullptr;

    if (shortSequence_->isEmpty() || priority < shortSequence_->accessFirst()->data_.priority_ ||
        (longSequence_->isEmpty() && shortSequence_->size() < shortSequence_->getCapacity())) {

        if (shortSequence_->size() == shortSequence_->getCapacity()) {
            ShortSequenceBlockType *shortBlock = shortSequence_->accessFirst();
            LongSequenceBlockType *longBlock = &longSequence_->insertLast();

            longBlock->data_.priority_ = shortBlock->data_.priority_;
            longBlock->data_.data_ = shortBlock->data_.data_;

            shortSequence_->removeFirst();
        }

        if (shortSequence_->isEmpty() || priority < shortSequence_->accessLast()->data_.priority_) {
            dataFront = &(shortSequence_->insertLast().data_);
        } else {
            if (priority > shortSequence_->accessFirst()->data_.priority_) {
                dataFront = &(shortSequence_->insertFirst().data_);
            } else {
                dataFront = &(shortSequence_
                                  ->insertBefore(*shortSequence_->findBlockWithProperty(
                                      [priority](ShortSequenceBlockType *block) {
                                          return block->data_.priority_ <= priority;
                                      }))
                                  .data_);
            }
        }
    } else {
        dataFront = &(longSequence_->insertLast().data_);
    }

    dataFront->priority_ = priority;
    dataFront->data_ = data;
}

template <typename P, typename T> T &TwoLists<P, T>::peek() {
    if (shortSequence_->isEmpty()) {
        throw std::out_of_range("Queue is empty!");
    }

    return shortSequence_->accessLast()->data_.data_;
}

template <typename P, typename T> T TwoLists<P, T>::pop() {
    if (isEmpty()) {
        throw std::runtime_error("Priority queue is empty!");
    }

    T result = shortSequence_->accessLast()->data_.data_;
    shortSequence_->removeLast();

    if (shortSequence_->size() == 0 && longSequence_->size() > 0) {
        amt::SinglyLS<PQItem<P, T>> *originalLongSequence = longSequence_;
        longSequence_ = new amt::SinglyLS<PQItem<P, T>>();
        shortSequence_->changeCapacity(ceil(sqrt(originalLongSequence->size())));

        while (!originalLongSequence->isEmpty()) {
            LongSequenceBlockType *block = originalLongSequence->accessFirst();
            push(block->data_.priority_, block->data_.data_);
            originalLongSequence->removeFirst();
        }

        delete originalLongSequence;
    }

    return result;
}

template <typename P, typename T>
BinaryHeap<P, T>::BinaryHeap() : ADS<PQItem<P, T>>(new amt::BinaryIH<PQItem<P, T>>()) {}

template <typename P, typename T>
BinaryHeap<P, T>::BinaryHeap(const BinaryHeap &other)
    : ADS<PQItem<P, T>>(new amt::BinaryIH<PQItem<P, T>>(), other) {}

template <typename P, typename T> inline bool BinaryHeap<P, T>::equals(const ADT &other) {
    throw std::logic_error("Unsupported operation!");
}

template <typename P, typename T> void BinaryHeap<P, T>::push(P priority, T data) {
    PQItem<P, T> *dataFront = &(this->getHierarchy()->insertLastLeaf().data_);
    dataFront->priority_ = priority;
    dataFront->data_ = data;

    auto *currentNode = this->getHierarchy()->accessLastLeaf();
    auto *parentNode = this->getHierarchy()->accessParent(*currentNode);

    while (parentNode != nullptr && currentNode->data_.priority_ < parentNode->data_.priority_) {
        std::swap(currentNode->data_, parentNode->data_);
        currentNode = parentNode;
        parentNode = this->getHierarchy()->accessParent(*currentNode);
    }
}

template <typename P, typename T> T &BinaryHeap<P, T>::peek() {
    if (getHierarchy()->isEmpty()) {
        throw std::out_of_range("Queue is empty!");
    }

    return this->getHierarchy()->accessRoot()->data_.data_;
}

template <typename P, typename T> T BinaryHeap<P, T>::pop() {
    if (this->getHierarchy()->isEmpty()) {
        throw std::out_of_range("Priority queue is empty!");
    }

    auto *currentNode = this->getHierarchy()->accessRoot();
    T result = currentNode->data_.data_;
    std::swap(currentNode->data_, this->getHierarchy()->accessLastLeaf()->data_);
    this->getHierarchy()->removeLastLeaf();

    if (!this->getHierarchy()->isEmpty()) {
        auto findHigherPriorityChild = [&](auto *parentNode) {
            auto *leftChild = this->getHierarchy()->accessLeftSon(*parentNode);
            auto *rightChild = this->getHierarchy()->accessRightSon(*parentNode);
            if (rightChild == nullptr || leftChild->data_.priority_ < rightChild->data_.priority_) {
                return leftChild;
            } else {
                return rightChild;
            }
        };

        auto *childNode = findHigherPriorityChild(currentNode);
        while (childNode != nullptr && currentNode->data_.priority_ > childNode->data_.priority_) {
            std::swap(currentNode->data_, childNode->data_);
            currentNode = childNode;
            childNode = findHigherPriorityChild(currentNode);
        }
    }

    return result;
}

template <typename P, typename T> amt::BinaryIH<PQItem<P, T>> *BinaryHeap<P, T>::getHierarchy() {
    return dynamic_cast<amt::BinaryIH<PQItem<P, T>> *>(this->memoryStructure_);
}
} // namespace ds::adt
