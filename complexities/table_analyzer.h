#pragma once

#include <complexities/complexity_analyzer.h>
#include <libs/adt/table.h>
#include <memory>
#include <random>

namespace ds::utils {
template <class Table> class TableAnalyzer : public ComplexityAnalyzer<Table> {
  protected:
    explicit TableAnalyzer(const std::string &name);

    void growToSize(Table &structure, size_t size) override;

    size_t getRandomIndex() const;

    int getRandomData() const;

  private:
    std::default_random_engine rngData_{144};
    std::default_random_engine rngIndex_{144};
    size_t index_{0};
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

template <class Table> class TableRemoveAnalyzer : public TableAnalyzer<Table> {
  public:
    explicit TableRemoveAnalyzer(const std::string &name);

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
        std::uniform_int_distribution<size_t> indexDist(0, table.size() - 1);
        index_ = indexDist(rngIndex_);
    });

    this->registerAfterOperation([](Table &table) { table.clear(); });
}

template <class Table> void TableAnalyzer<Table>::growToSize(Table &structure, size_t size) {
    //   unused
}

template <class Table> size_t TableAnalyzer<Table>::getRandomIndex() const { return index_; }

template <class Table> int TableAnalyzer<Table>::getRandomData() const { return data_; }

//----------

template <class Table>
TableInsertAnalyzer<Table>::TableInsertAnalyzer(const std::string &name)
    : TableAnalyzer<Table>(name) {}

template <class Table> void TableInsertAnalyzer<Table>::executeOperation(Table &structure) {
    structure.insert(this->getRandomIndex(), this->getRandomData());
    // structure.insert(structure.begin(), this->getRandomData());
}

//----------

template <class Table>
TableFindAnalyzer<Table>::TableFindAnalyzer(const std::string &name) : TableAnalyzer<Table>(name) {}

template <class Table> void TableFindAnalyzer<Table>::executeOperation(Table &structure) {
    try {
        structure.find(this->getRandomIndex());
    } catch (std::out_of_range &) {
        // ignore
    }
}

//----------

template <class Table>
TableRemoveAnalyzer<Table>::TableRemoveAnalyzer(const std::string &name)
    : TableAnalyzer<Table>(name) {}

template <class Table> void TableRemoveAnalyzer<Table>::executeOperation(Table &structure) {
    if (!structure.empty()) {
        structure.erase(structure.begin());
    }
}

//----------

inline TablesAnalyzer::TablesAnalyzer() : CompositeAnalyzer("Tables") {
    this->addAnalyzer(
        std::make_unique<TableInsertAnalyzer<ds::adt::HashTable<int, int>>>("hashTable-insert"));
    this->addAnalyzer(
        std::make_unique<TableFindAnalyzer<ds::adt::HashTable<int, int>>>("hashTable-find"));
    // this->addAnalyzer(std::make_unique<TableInsertAnalyzer<std::table<int>>>("table-insert"));
    // this->addAnalyzer(std::make_unique<TableRemoveAnalyzer<std::vector<int>>>("vector-remove"));
    // this->addAnalyzer(std::make_unique<TableRemoveAnalyzer<std::table<int>>>("table-remove"));
}
} // namespace ds::utils
