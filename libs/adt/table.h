#pragma once

#include "../amt/explicit_hierarchy.h"
#include "../amt/implicit_sequence.h"
#include "abstract_data_type.h"
#include "amt/abstract_memory_type.h"
#include <cstddef>
#include <functional>
#include <limits>
#include <numeric>
#include <random>

namespace ds::adt {
template <typename K, typename T> struct TableItem {
    K key_;
    T data_;

    bool operator==(const TableItem<K, T> &other) const {
        return key_ == other.key_ && data_ == other.data_;
    }

    bool operator!=(const TableItem<K, T> &other) const { return !(*this == other); }
};

//----------

template <typename K, typename T> class Table : virtual public ADT {
  public:
    virtual void insert(const K &key, T data) = 0;

    virtual T &find(const K &key);

    virtual bool tryFind(const K &key, T *&data) const = 0;

    virtual bool contains(const K &key) const;

    virtual T remove(const K &key) = 0;

  protected:
    template <class TableT> bool areEqual(TableT &table1, const ADT &table2);
};

//----------

template <typename K, typename T, typename SequenceType>
class SequenceTable : public Table<K, T>, public ADS<TableItem<K, T>> {
  public:
    SequenceTable();

    SequenceTable(const SequenceTable &other);

    bool tryFind(const K &key, T *&data) const override;

    using BlockType = typename SequenceType::BlockType;
    using IteratorType = typename SequenceType::IteratorType;

    virtual BlockType *findBlockWithKey(const K &key) const = 0;

    IteratorType begin();

    IteratorType end();

  protected:
    SequenceType *getSequence() const;
};

//----------

template <typename K, typename T, typename SequenceType>
class UnsortedSequenceTable : public SequenceTable<K, T, SequenceType> {
  protected:
    using BlockType = typename SequenceType::BlockType;

    BlockType *findBlockWithKey(const K &key) const override;
};

template <typename K, typename T, typename SequenceType>
using UnsortedSTab = UnsortedSequenceTable<K, T, SequenceType>;

//----------

template <typename K, typename T>
class UnsortedImplicitSequenceTable : public UnsortedSequenceTable<K, T, amt::IS<TableItem<K, T>>> {
  public:
    void insert(const K &key, T data) override;

    T remove(const K &key) override;

    bool equals(const ADT &other) override;

  private:
    using BlockType = typename amt::IS<TableItem<K, T>>::BlockType;
};

template <typename K, typename T> using UnsortedISTab = UnsortedImplicitSequenceTable<K, T>;

//----------

template <typename K, typename T>
class UnsortedExplicitSequenceTable
    : public UnsortedSequenceTable<K, T, amt::SinglyLS<TableItem<K, T>>> {
  public:
    void insert(const K &key, T data) override;

    T remove(const K &key) override;

    bool equals(const ADT &other) override;

  private:
    using BlockType = typename amt::SinglyLS<TableItem<K, T>>::BlockType;
};

template <typename K, typename T> using UnsortedESTab = UnsortedExplicitSequenceTable<K, T>;

//----------

template <typename K, typename T>
class SortedSequenceTable : public SequenceTable<K, T, amt::IS<TableItem<K, T>>> {
  public:
    void insert(const K &key, T data) override;

    T remove(const K &key) override;

    bool equals(const ADT &other) override;

  protected:
    using BlockType = typename amt::IS<TableItem<K, T>>::BlockType;

    BlockType *findBlockWithKey(const K &key) const override;

  private:
    bool tryFindBlockWithKey(const K &key, size_t firstIndex, size_t lastIndex,
                             BlockType *&lastBlock) const;
};

template <typename K, typename T> using SortedSTab = SortedSequenceTable<K, T>;

//----------

template <typename K, typename T>
class HashTable : public Table<K, T>, public AUMS<TableItem<K, T>> {
  public:
    using HashFunctionType = std::function<size_t(const K &)>;

    HashTable();

    HashTable(const HashTable &other);

    HashTable(HashFunctionType hashFunction, size_t capacity);

    ~HashTable() override;

    ADT &assign(const ADT &other) override;

    bool equals(const ADT &other) override;

    void clear() override;

    size_t size() const override;

    bool isEmpty() const override;

    void insert(const K &key, T data) override;

    bool tryFind(const K &key, T *&data) const override;

    T remove(const K &key) override;

  private:
    using SynonymTable = UnsortedESTab<K, T>;
    using SynonymTableIterator = typename SynonymTable::IteratorType;
    using PrimaryRegionIterator = typename amt::IS<SynonymTable *>::IteratorType;

    static const size_t CAPACITY = 100;

    amt::IS<SynonymTable *> *primaryRegion_;
    HashFunctionType hashFunction_;
    size_t size_;

  public:
    class HashTableIterator {
      public:
        HashTableIterator(PrimaryRegionIterator *tablesFirst, PrimaryRegionIterator *tablesLast);

        HashTableIterator(const HashTableIterator &other);

        ~HashTableIterator();

        HashTableIterator &operator++();

        HashTableIterator operator++(int);

        bool operator==(const HashTableIterator &other) const;

        bool operator!=(const HashTableIterator &other) const;

        TableItem<K, T> &operator*();

      private:
        PrimaryRegionIterator *tablesCurrent_;
        PrimaryRegionIterator *tablesLast_;
        SynonymTableIterator *synonymIterator_;
    };

    HashTableIterator begin() const;

    HashTableIterator end() const;
};

//----------

template <typename K, typename T, typename ItemType>
class GeneralBinarySearchTree : public Table<K, T>, public ADS<ItemType> {
  public:
    using IteratorType = typename amt::BinaryEH<ItemType>::IteratorType;

    GeneralBinarySearchTree();

    GeneralBinarySearchTree(const GeneralBinarySearchTree &other);

    ~GeneralBinarySearchTree() override;

    size_t size() const override;

    void clear() override;

    void insert(const K &key, T data) override;

    bool tryFind(const K &key, T *&data) const override;

    T remove(const K &key) override;

    IteratorType begin() const;

    IteratorType end() const;

  protected:
    using BSTNodeType = typename amt::BinaryEH<ItemType>::BlockType;

    amt::BinaryEH<ItemType> *getHierarchy() const;

    virtual void removeNode(BSTNodeType *node);

    virtual void balanceTree(BSTNodeType *node) {}

    bool tryFindNodeWithKey(const K &key, BSTNodeType *&node) const;

    void rotateLeft(BSTNodeType *node);

    void rotateRight(BSTNodeType *node);

  private:
    size_t size_;
};

//----------

template <typename K, typename T>
class BinarySearchTree : public GeneralBinarySearchTree<K, T, TableItem<K, T>> {
  public:
    bool equals(const ADT &other) override;
};

//----------

template <typename K, typename T> struct TreapItem : public TableItem<K, T> {
    int priority_;
};

template <typename K, typename T>
class Treap : public GeneralBinarySearchTree<K, T, TreapItem<K, T>> {
  public:
    Treap();

    bool equals(const ADT &other) override;

  protected:
    using BSTNodeType = typename GeneralBinarySearchTree<K, T, TreapItem<K, T>>::BSTNodeType;

    void removeNode(BSTNodeType *node) override;

    void balanceTree(BSTNodeType *node) override;

  private:
    std::default_random_engine rng_;
};

//----------

template <typename K, typename T>
template <class TableT>
bool Table<K, T>::areEqual(TableT &table1, const ADT &table2) {
    if (&table1 == &table2) {
        return true;
    }

    const auto *tab2 = dynamic_cast<const Table<K, T> *>(&table2);
    if (tab2 == nullptr) {
        return false;
    }

    if (table1.size() != table2.size()) {
        return false;
    }

    for (const TableItem<K, T> &item : table1) {
        T *otherData = nullptr;
        if (!tab2->tryFind(item.key_, otherData) || *otherData != item.data_) {
            return false;
        }
    }
    return true;
}

//----------

template <typename K, typename T> T &Table<K, T>::find(const K &key) {
    T *data = nullptr;
    if (!this->tryFind(key, data)) {
        throw std::out_of_range("No such key!");
    }
    return *data;
}

template <typename K, typename T> bool Table<K, T>::contains(const K &key) const {
    T *data = nullptr;
    return this->tryFind(key, data);
}

//----------

template <typename K, typename T, typename SequenceType>
SequenceTable<K, T, SequenceType>::SequenceTable() : ADS<TableItem<K, T>>(new SequenceType()) {}

template <typename K, typename T, typename SequenceType>
SequenceTable<K, T, SequenceType>::SequenceTable(const SequenceTable &other)
    : ADS<TableItem<K, T>>(new SequenceType(), other) {}

template <typename K, typename T, typename SequenceType>
bool SequenceTable<K, T, SequenceType>::tryFind(const K &key, T *&data) const {
    BlockType *block = this->findBlockWithKey(key);
    if (block == nullptr) {
        return false;
    }

    data = &block->data_.data_;
    return true;
}

template <typename K, typename T, typename SequenceType>
typename SequenceTable<K, T, SequenceType>::IteratorType
SequenceTable<K, T, SequenceType>::begin() {
    return this->getSequence()->begin();
}

template <typename K, typename T, typename SequenceType>
typename SequenceTable<K, T, SequenceType>::IteratorType SequenceTable<K, T, SequenceType>::end() {
    return this->getSequence()->end();
}

template <typename K, typename T, typename SequenceType>
SequenceType *SequenceTable<K, T, SequenceType>::getSequence() const {
    return dynamic_cast<SequenceType *>(this->memoryStructure_);
}

//----------

template <typename K, typename T, typename SequenceType>
typename UnsortedSequenceTable<K, T, SequenceType>::BlockType *
UnsortedSequenceTable<K, T, SequenceType>::findBlockWithKey(const K &key) const {
    return this->getSequence()->findBlockWithProperty(
        [&key](const SequenceType::BlockType *item) { return item->data_.key_ == key; });
}

//----------

template <typename K, typename T>
void UnsortedImplicitSequenceTable<K, T>::insert(const K &key, T data) {
    if (this->findBlockWithKey(key) != nullptr) {
        throw std::invalid_argument("Key already exists!");
    }

    TableItem<K, T> *newItem = &this->getSequence()->insertLast().data_;

    newItem->key_ = key;
    newItem->data_ = data;
}

template <typename K, typename T> T UnsortedImplicitSequenceTable<K, T>::remove(const K &key) {
    BlockType *block = this->findBlockWithKey(key);
    if (block == nullptr) {
        throw std::out_of_range("No such key!");
    }

    T result = block->data_.data_;

    if (BlockType *lastBlock = this->getSequence()->accessLast(); block != lastBlock) {
        std::swap(block->data_, lastBlock->data_);
    }
    this->getSequence()->removeLast();
    return result;
}

template <typename K, typename T>
bool UnsortedImplicitSequenceTable<K, T>::equals(const ADT &other) {
    return this->areEqual(*this, other);
}

//----------

template <typename K, typename T>
void UnsortedExplicitSequenceTable<K, T>::insert(const K &key, T data) {
    if (this->findBlockWithKey(key) != nullptr) {
        throw std::invalid_argument("Key already exists!");
    }

    TableItem<K, T> *newBlock = &this->getSequence()->insertFirst().data_;
    newBlock->key_ = key;
    newBlock->data_ = data;
}

template <typename K, typename T> T UnsortedExplicitSequenceTable<K, T>::remove(const K &key) {
    BlockType *block = this->findBlockWithKey(key);
    if (block == nullptr) {
        throw std::out_of_range("No such key!");
    }

    T result = block->data_.data_;

    if (BlockType *firstBlock = this->getSequence()->accessFirst(); block != firstBlock) {
        std::swap(block->data_, firstBlock->data_);
    }

    this->getSequence()->removeFirst();
    return result;
}

template <typename K, typename T>
bool UnsortedExplicitSequenceTable<K, T>::equals(const ADT &other) {
    return this->areEqual(*this, other);
}

//----------

template <typename K, typename T> void SortedSequenceTable<K, T>::insert(const K &key, T data) {
    TableItem<K, T> *item;

    if (this->isEmpty()) {
        item = &this->getSequence()->insertFirst().data_;
    } else {
        BlockType *block = nullptr;
        if (this->tryFindBlockWithKey(key, 0, this->size(), block)) {
            throw std::invalid_argument("Key already exists!");
        }

        item = key > block->data_.key_ ? &this->getSequence()->insertAfter(*block).data_
                                       : &this->getSequence()->insertBefore(*block).data_;
    }

    item->key_ = key;
    item->data_ = data;
}

template <typename K, typename T> T SortedSequenceTable<K, T>::remove(const K &key) {
    BlockType *block = nullptr;
    if (!this->tryFindBlockWithKey(key, 0, this->size(), block)) {
        throw std::out_of_range("No such key!");
    }

    T result = block->data_.data_;

    if (this->getSequence()->accessFirst() == block) {
        this->getSequence()->removeFirst();
    } else {
        this->getSequence()->removeNext(*this->getSequence()->accessPrevious(*block));
    }

    return result;
}

template <typename K, typename T>
typename SortedSequenceTable<K, T>::BlockType *
SortedSequenceTable<K, T>::findBlockWithKey(const K &key) const {
    BlockType *blockWithKey = nullptr;
    return this->tryFindBlockWithKey(key, 0, this->size(), blockWithKey) ? blockWithKey : nullptr;
}

template <typename K, typename T> bool SortedSequenceTable<K, T>::equals(const ADT &other) {
    const auto *otherTable = dynamic_cast<const SortedSequenceTable<K, T> *>(&other);
    if (otherTable == nullptr) {
        return false;
    }

    if (this->size() != otherTable->size()) {
        return false;
    }

    for (size_t i = 0; i < this->size(); ++i) {
        if (this->getSequence()->access(i)->data_ != otherTable->getSequence()->access(i)->data_) {
            return false;
        }
    }

    return true;
}

template <typename K, typename T>
bool SortedSequenceTable<K, T>::tryFindBlockWithKey(const K &key, size_t firstIndex,
                                                    size_t lastIndex, BlockType *&lastBlock) const {
    if (this->isEmpty()) {
        lastBlock = nullptr;
        return false;
    }

    size_t middleIndex = firstIndex;

    while (firstIndex < lastIndex) {
        middleIndex = std::midpoint(firstIndex, lastIndex);
        // middleIndex = firstIndex + (lastIndex - firstIndex) / 2;
        lastBlock = this->getSequence()->access(middleIndex);

        if (lastBlock->data_.key_ < key) {
            firstIndex = middleIndex + 1;
        } else if (lastBlock->data_.key_ > key) {
            lastIndex = middleIndex;
        } else {
            break;
        }
    }

    lastBlock = this->getSequence()->access(middleIndex);
    return lastBlock->data_.key_ == key;
}

//----------

template <typename K, typename T>
HashTable<K, T>::HashTable()
    : HashTable([](const K &key) { return std::hash<K>()(key); }, CAPACITY) {}

template <typename K, typename T>
HashTable<K, T>::HashTable(const HashTable &other)
    : primaryRegion_(new amt::IS<SynonymTable *>(other.primaryRegion_->size(), true)),
      hashFunction_(other.hashFunction_), size_(0) {
    assign(other);
}

template <typename K, typename T>
HashTable<K, T>::HashTable(HashFunctionType hashFunction, size_t capacity)
    : primaryRegion_(new amt::IS<SynonymTable *>(capacity, true)), hashFunction_(hashFunction),
      size_(0) {}

template <typename K, typename T> HashTable<K, T>::~HashTable() {
    this->clear();
    delete primaryRegion_;
}

template <typename K, typename T> ADT &HashTable<K, T>::assign(const ADT &other) {
    if (this != &other) {
        const auto &otherTable = dynamic_cast<const HashTable &>(other);
        this->clear();
        for (TableItem<K, T> &otherItem : otherTable) {
            this->insert(otherItem.key_, otherItem.data_);
        }
    }

    return *this;
}

template <typename K, typename T> bool HashTable<K, T>::equals(const ADT &other) {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}

template <typename K, typename T> void HashTable<K, T>::clear() {
    size_ = 0;
    primaryRegion_->processAllBlocksForward(
        [](typename amt::IS<SynonymTable *>::BlockType *synonymBlock) {
            delete synonymBlock->data_;
            synonymBlock->data_ = nullptr;
        });
}

template <typename K, typename T> size_t HashTable<K, T>::size() const { return size_; }

template <typename K, typename T> bool HashTable<K, T>::isEmpty() const {
    return this->size() == 0;
}

template <typename K, typename T> void HashTable<K, T>::insert(const K &key, T data) {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}

template <typename K, typename T> bool HashTable<K, T>::tryFind(const K &key, T *&data) const {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}

template <typename K, typename T> T HashTable<K, T>::remove(const K &key) {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}

template <typename K, typename T>
HashTable<K, T>::HashTableIterator::HashTableIterator(PrimaryRegionIterator *tablesFirst,
                                                      PrimaryRegionIterator *tablesLast)
    : tablesCurrent_(tablesFirst), tablesLast_(tablesLast) {
    while (*tablesCurrent_ != *tablesLast_ && **tablesCurrent_ == nullptr) {
        ++(*tablesCurrent_);
    }
    synonymIterator_ = *tablesCurrent_ != *tablesLast_
                           ? new SynonymTableIterator((**tablesCurrent_)->begin())
                           : nullptr;
}

template <typename K, typename T>
HashTable<K, T>::HashTableIterator::HashTableIterator(const HashTableIterator &other)
    : tablesCurrent_(new PrimaryRegionIterator(*other.tablesCurrent_)),
      tablesLast_(new PrimaryRegionIterator(*other.tablesLast_)),
      synonymIterator_(other.synonymIterator_ != nullptr
                           ? new SynonymTableIterator(*other.synonymIterator_)
                           : nullptr) {}

template <typename K, typename T> HashTable<K, T>::HashTableIterator::~HashTableIterator() {
    delete tablesCurrent_;
    delete tablesLast_;
    delete synonymIterator_;
}

template <typename K, typename T>
typename HashTable<K, T>::HashTableIterator &HashTable<K, T>::HashTableIterator::operator++() {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}

template <typename K, typename T>
typename HashTable<K, T>::HashTableIterator HashTable<K, T>::HashTableIterator::operator++(int) {
    HashTableIterator tmp(*this);
    this->operator++();
    return tmp;
}

template <typename K, typename T>
bool HashTable<K, T>::HashTableIterator::operator==(const HashTableIterator &other) const {
    return synonymIterator_ == other.synonymIterator_ ||
           (synonymIterator_ != nullptr && other.synonymIterator_ != nullptr &&
            *synonymIterator_ == *(other.synonymIterator_));
}

template <typename K, typename T>
bool HashTable<K, T>::HashTableIterator::operator!=(const HashTableIterator &other) const {
    return !(*this == other);
}

template <typename K, typename T> TableItem<K, T> &HashTable<K, T>::HashTableIterator::operator*() {
    return (**synonymIterator_);
}

template <typename K, typename T>
typename HashTable<K, T>::HashTableIterator HashTable<K, T>::begin() const {
    return HashTableIterator(new PrimaryRegionIterator(primaryRegion_->begin()),
                             new PrimaryRegionIterator(primaryRegion_->end()));
}

template <typename K, typename T>
typename HashTable<K, T>::HashTableIterator HashTable<K, T>::end() const {
    return HashTableIterator(new PrimaryRegionIterator(primaryRegion_->end()),
                             new PrimaryRegionIterator(primaryRegion_->end()));
}

//----------

template <typename K, typename T, typename ItemType>
GeneralBinarySearchTree<K, T, ItemType>::GeneralBinarySearchTree()
    : ADS<ItemType>(new amt::BinaryEH<ItemType>()), size_(0) {}

template <typename K, typename T, typename ItemType>
GeneralBinarySearchTree<K, T, ItemType>::GeneralBinarySearchTree(
    const GeneralBinarySearchTree &other)
    : ADS<ItemType>(new amt::BinaryEH<ItemType>(), other), size_(other.size_) {}

template <typename K, typename T, typename ItemType>
GeneralBinarySearchTree<K, T, ItemType>::~GeneralBinarySearchTree() {
    size_ = 0;
}

template <typename K, typename T, typename ItemType>
size_t GeneralBinarySearchTree<K, T, ItemType>::size() const {
    return size_;
}

template <typename K, typename T, typename ItemType>
void GeneralBinarySearchTree<K, T, ItemType>::clear() {
    ADS<ItemType>::clear();
    size_ = 0;
}

template <typename K, typename T, typename ItemType>
void GeneralBinarySearchTree<K, T, ItemType>::insert(const K &key, T data) {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}

template <typename K, typename T, typename ItemType>
bool GeneralBinarySearchTree<K, T, ItemType>::tryFind(const K &key, T *&data) const {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}

template <typename K, typename T, typename ItemType>
T GeneralBinarySearchTree<K, T, ItemType>::remove(const K &key) {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}

template <typename K, typename T, typename ItemType>
typename GeneralBinarySearchTree<K, T, ItemType>::IteratorType
GeneralBinarySearchTree<K, T, ItemType>::begin() const {
    return this->getHierarchy()->begin();
}

template <typename K, typename T, typename ItemType>
typename GeneralBinarySearchTree<K, T, ItemType>::IteratorType
GeneralBinarySearchTree<K, T, ItemType>::end() const {
    return this->getHierarchy()->end();
}

template <typename K, typename T, typename ItemType>
amt::BinaryEH<ItemType> *GeneralBinarySearchTree<K, T, ItemType>::getHierarchy() const {
    return dynamic_cast<amt::BinaryEH<ItemType> *>(this->memoryStructure_);
}

template <typename K, typename T, typename ItemType>
void GeneralBinarySearchTree<K, T, ItemType>::removeNode(BSTNodeType *node) {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}

template <typename K, typename T, typename ItemType>
bool GeneralBinarySearchTree<K, T, ItemType>::tryFindNodeWithKey(const K &key,
                                                                 BSTNodeType *&node) const {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}

template <typename K, typename T, typename ItemType>
void GeneralBinarySearchTree<K, T, ItemType>::rotateLeft(BSTNodeType *node) {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}

template <typename K, typename T, typename ItemType>
void GeneralBinarySearchTree<K, T, ItemType>::rotateRight(BSTNodeType *node) {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}

//----------

template <typename K, typename T> bool BinarySearchTree<K, T>::equals(const ADT &other) {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}

//----------

template <typename K, typename T> Treap<K, T>::Treap() : rng_(std::random_device()()) {}

template <typename K, typename T> void Treap<K, T>::removeNode(BSTNodeType *node) {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}

template <typename K, typename T> void Treap<K, T>::balanceTree(BSTNodeType *node) {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}

template <typename K, typename T> bool Treap<K, T>::equals(const ADT &other) {
    // TODO 11
    // po implementacii vymazte vyhodenie vynimky!
    throw std::runtime_error("Not implemented yet");
}
} // namespace ds::adt
