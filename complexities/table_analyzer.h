#pragma once

#include "../libs/adt/table.h"
#include "complexities/complexity_analyzer.h"
#include <memory>
#include <random>

namespace ds::utils {
template <class Table> class TableAnalyzer : public ComplexityAnalyzer<Table> {
  protected:
    explicit TableAnalyzer(const std::string &name);

    void growToSize(Table &structure, size_t size) override;

    int getRandomKey() const;

    int getRandomData() const;

  private:
    std::default_random_engine rngData_{144};
    std::default_random_engine rngKey_{144};
    int key_{0};
    int data_{0};
};

template <class Table> class TableFindAnalyzer : public TableAnalyzer<Table> {
  public:
    explicit TableFindAnalyzer(const std::string &name);

  protected:
    void executeOperation(Table &structure) override;
};

template <class Table> class TableInsertAnalyzer : public TableAnalyzer<Table> {
  public:
    explicit TableInsertAnalyzer(const std::string &name);

  protected:
    void executeOperation(Table &structure) override;
};

class TablesAnalyzer : public CompositeAnalyzer {
  public:
    TablesAnalyzer();
};

//----------

template <class Table>
TableAnalyzer<Table>::TableAnalyzer(const std::string &name) : ComplexityAnalyzer<Table>(name) {
    this->registerBeforeOperation([this](Table &table) {
        data_ = static_cast<int>(rngData_());
        key_ = static_cast<int>(rngKey_());
    });

    this->registerAfterOperation([](Table &table) { table.clear(); });
}

template <class Table> void TableAnalyzer<Table>::growToSize(Table &structure, size_t size) {
    size_t newSize = size - structure.size();
    for (size_t i = 0; i < newSize; ++i) {
        int key = this->getRandomKey();
        if (!structure.contains(key)) {
            structure.insert(key, this->getRandomData());
        }
    }
}

template <class Table> int TableAnalyzer<Table>::getRandomKey() const { return key_; }

template <class Table> int TableAnalyzer<Table>::getRandomData() const { return data_; }

//----------

template <class Table>
TableInsertAnalyzer<Table>::TableInsertAnalyzer(const std::string &name)
    : TableAnalyzer<Table>(name) {}

template <class Table> void TableInsertAnalyzer<Table>::executeOperation(Table &structure) {
    int key = this->getRandomKey();
    if (!structure.contains(key)) {
        structure.insert(key, this->getRandomData());
    }
}

//----------

template <class Table>
TableFindAnalyzer<Table>::TableFindAnalyzer(const std::string &name) : TableAnalyzer<Table>(name) {}

template <class Table> void TableFindAnalyzer<Table>::executeOperation(Table &structure) {
    int key = this->getRandomKey();
    if (structure.contains(key)) {
        structure.find(key);
    }
}

inline TablesAnalyzer::TablesAnalyzer() : CompositeAnalyzer("Tables") {
    this->addAnalyzer(
        std::make_unique<TableInsertAnalyzer<ds::adt::HashTable<int, int>>>("hashTable-insert"));
    this->addAnalyzer(
        std::make_unique<TableFindAnalyzer<ds::adt::HashTable<int, int>>>("hashTable-find"));
}
} // namespace ds::utils
