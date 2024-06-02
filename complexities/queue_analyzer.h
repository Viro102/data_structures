#pragma once

#include "complexities/complexity_analyzer.h"
#include "libs/adt/priority_queue.h"
#include <random>

namespace ds::utils {
template <class Queue> class QueueAnalyzer : public ComplexityAnalyzer<Queue> {
  protected:
    explicit QueueAnalyzer(const std::string &name);

    void growToSize(Queue &structure, size_t size) override;

    int getRandomPriority() const;

    int getRandomData() const;

  private:
    std::default_random_engine rngData_{144};
    std::default_random_engine rngPriority_{144};
    int data_{0};
    int priority_{0};
};

template <class Queue> class QueueInsertAnalyzer : public QueueAnalyzer<Queue> {
  public:
    explicit QueueInsertAnalyzer(const std::string &name);
    Queue createPrototype() override { return Queue(this->getStepCount() * this->getStepSize()); }

  protected:
    void executeOperation(Queue &structure) override;
};

template <class Queue> class QueueRemoveAnalyzer : public QueueAnalyzer<Queue> {
  public:
    explicit QueueRemoveAnalyzer(const std::string &name);
    Queue createPrototype() override { return Queue(this->getStepCount() * this->getStepSize()); }

  protected:
    void executeOperation(Queue &structure) override;
};

class QueuesAnalyzer : public CompositeAnalyzer {
  public:
    QueuesAnalyzer();
};

//----------

template <class Queue>
QueueAnalyzer<Queue>::QueueAnalyzer(const std::string &name) : ComplexityAnalyzer<Queue>(name) {
    this->registerBeforeOperation([this](Queue &queue) {
        data_ = static_cast<int>(rngData_());
        priority_ = static_cast<int>(rngPriority_());
    });

    this->registerAfterOperation([](Queue &queue) { queue.clear(); });
}

template <class Queue> void QueueAnalyzer<Queue>::growToSize(Queue &structure, size_t size) {
    size_t newSize = size - structure.size();

    for (size_t i = 0; i < newSize; ++i) {
        structure.push(this->getRandomPriority(), this->getRandomData());
    }
}
template <class Queue> int QueueAnalyzer<Queue>::getRandomPriority() const { return priority_; }

template <class Queue> int QueueAnalyzer<Queue>::getRandomData() const { return data_; }

//----------

template <class Queue>
QueueInsertAnalyzer<Queue>::QueueInsertAnalyzer(const std::string &name)
    : QueueAnalyzer<Queue>(name) {}

template <class Queue> void QueueInsertAnalyzer<Queue>::executeOperation(Queue &structure) {
    structure.push(this->getRandomPriority(), this->getRandomData());
}

//----------

template <class Queue>
QueueRemoveAnalyzer<Queue>::QueueRemoveAnalyzer(const std::string &name)
    : QueueAnalyzer<Queue>(name) {}

template <class Queue> void QueueRemoveAnalyzer<Queue>::executeOperation(Queue &structure) {
    if (!structure.isEmpty()) {
        structure.pop();
    }
}

//----------

inline QueuesAnalyzer::QueuesAnalyzer() : CompositeAnalyzer("Queues") {
    this->addAnalyzer(
        std::make_unique<QueueInsertAnalyzer<ds::adt::TwoLists<int, int>>>("twoLists-insert"));
    this->addAnalyzer(
        std::make_unique<QueueRemoveAnalyzer<ds::adt::TwoLists<int, int>>>("twoLists-pop"));
}
} // namespace ds::utils
