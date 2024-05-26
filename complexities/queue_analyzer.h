#pragma once

#include <complexities/complexity_analyzer.h>
#include <libs/adt/priority_queue.h>
#include <random>

namespace ds::utils {
/**
 * @brief Common base for list analyzers.
 */
template <class Queue> class QueueAnalyzer : public ComplexityAnalyzer<Queue> {
  protected:
    explicit QueueAnalyzer(const std::string &name);

    void growToSize(Queue &structure, size_t size) override;

    size_t getRandomIndex() const;

    int getRandomData() const;

  private:
    std::default_random_engine rngData_{144};
    std::default_random_engine rngIndex_{144};
    size_t index_{0};
    int data_{0};
};

/**
 * @brief Analyzes complexity of an insertion at the beginning.
 */
template <class Queue> class QueueInsertAnalyzer : public QueueAnalyzer<Queue> {
  public:
    explicit QueueInsertAnalyzer(const std::string &name);
    Queue createPrototype() override;

  protected:
    void executeOperation(Queue &structure) override;
};

/**
 * @brief Analyzes complexity of an erasure at the beginning.
 */
template <class Queue> class QueueRemoveAnalyzer : public QueueAnalyzer<Queue> {
  public:
    explicit QueueRemoveAnalyzer(const std::string &name);
    Queue createPrototype() override;

  protected:
    void executeOperation(Queue &structure) override;
};

/**
 * @brief Container for all list analyzers.
 */
class QueuesAnalyzer : public CompositeAnalyzer {
  public:
    QueuesAnalyzer();
};

//----------

template <class Queue>
QueueAnalyzer<Queue>::QueueAnalyzer(const std::string &name) : ComplexityAnalyzer<Queue>(name) {
    this->registerBeforeOperation([this](Queue &queue) {
        data_ = static_cast<int>(rngData_());
        std::uniform_int_distribution<size_t> indexDist(0, queue.size() - 1);
        index_ = indexDist(rngIndex_);
    });

    this->registerAfterOperation([](Queue &queue) { queue.clear(); });
}

template <class Queue> void QueueAnalyzer<Queue>::growToSize(Queue &structure, size_t size) {
    size_t newSize = size - structure.size();

    for (size_t i = 0; i < newSize; ++i) {
        structure.push(this->getRandomIndex(), static_cast<int>(rngData_()));
        // structure.push_back(static_cast<int>(rngData_()));
    }
}

template <class Queue> size_t QueueAnalyzer<Queue>::getRandomIndex() const { return index_; }

template <class Queue> int QueueAnalyzer<Queue>::getRandomData() const { return data_; }

//----------

template <class Queue>
QueueInsertAnalyzer<Queue>::QueueInsertAnalyzer(const std::string &name)
    : QueueAnalyzer<Queue>(name) {}

template <class Queue> void QueueInsertAnalyzer<Queue>::executeOperation(Queue &structure) {
    structure.push(this->getRandomIndex(), this->getRandomData());
    // structure.insert(structure.begin(), this->getRandomData());
}

template <class Queue> Queue QueueInsertAnalyzer<Queue>::createPrototype() { return Queue(100000); }

//----------

template <class Queue>
QueueRemoveAnalyzer<Queue>::QueueRemoveAnalyzer(const std::string &name)
    : QueueAnalyzer<Queue>(name) {}

template <class Queue> void QueueRemoveAnalyzer<Queue>::executeOperation(Queue &structure) {
    if (!structure.isEmpty()) {
        structure.pop();
        // structure.erase(structure.begin());
    }
}
template <class Queue> Queue QueueRemoveAnalyzer<Queue>::createPrototype() { return Queue(100000); }

//----------

inline QueuesAnalyzer::QueuesAnalyzer() : CompositeAnalyzer("Lists") {
    this->addAnalyzer(
        std::make_unique<QueueInsertAnalyzer<ds::adt::TwoLists<int, int>>>("twoLists-insert"));
    this->addAnalyzer(
        std::make_unique<QueueRemoveAnalyzer<ds::adt::TwoLists<int, int>>>("twoLists-pop"));
}
} // namespace ds::utils
