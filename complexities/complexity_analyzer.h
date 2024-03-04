#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace ds::utils {
/**
 *  @brief Analyzer with a name.
 */
class Analyzer {
  public:
    explicit Analyzer(std::string name);

    virtual ~Analyzer() = default;

    virtual void analyze() = 0;

    virtual void setOutputDirectory(std::string path) = 0;

    virtual void setReplicationCount(size_t count) = 0;

    virtual void setStepSize(size_t size) = 0;

    virtual void setStepCount(size_t count) = 0;

    const std::string &getName() const;

  private:
    std::string name_;
};

/**
 *  @brief Container of analyzers.
 */
class CompositeAnalyzer : public Analyzer {
  public:
    explicit CompositeAnalyzer(const std::string &name);

    void analyze() override;

    void setOutputDirectory(std::string path) override;

    void setReplicationCount(size_t count) override;

    void setStepSize(size_t size) override;

    void setStepCount(size_t count) override;

    void addAnalyzer(std::unique_ptr<Analyzer> analyzer);

    const std::vector<std::unique_ptr<Analyzer>> &getAnalyzers();

  private:
    std::vector<std::unique_ptr<Analyzer>> analyzers_;
};

/**
 *  @brief Specific analyzer.
 */
class LeafAnalyzer : public Analyzer {
  public:
    explicit LeafAnalyzer(const std::string &name);

    void setOutputDirectory(std::string path) override;

    void setReplicationCount(size_t count) override;

    void setStepSize(size_t size) override;

    void setStepCount(size_t count) override;

    std::filesystem::path getOutputPath() const;

    bool wasSuccessful() const;

  protected:
    void resetSuccess();

    void setSuccess();

    size_t getReplicationCount() const;

    size_t getStepSize() const;

    size_t getStepCount() const;

  private:
    static const size_t DEFAULT_REPLICATION_COUNT = 100;
    static const size_t DEFAULT_STEP_SIZE = 10'000;
    static const size_t DEFAULT_STEP_COUNT = 10;

    std::string outputDir_{"."};
    size_t replicationCount_{DEFAULT_REPLICATION_COUNT};
    size_t stepSize_{DEFAULT_STEP_SIZE};
    size_t stepCount_{DEFAULT_STEP_COUNT};
    bool wasSuccessful_{false};
};

/**
 *  @brief Interface for a structure prototype factory.
 *  @tparam Structure Type of the structure to create
 */
template <class Structure> class IPrototypeFactory {
  public:
    virtual ~IPrototypeFactory() = default;

  protected:
    virtual Structure createPrototype() = 0;
};

/**
 *  @brief Default implementation of the prototype factory.
 *  @tparam Structure Type of the structure to create.
 */
template <class Structure> class PrototypeFactory : public IPrototypeFactory<Structure> {
  protected:
    /**
     *  @brief Returns default-constructed instance of @p Structure.
     */
    Structure createPrototype() override { return Structure(); }
};

/**
 *  @brief Universal analyzer of an operation of any structure.
 *  @tparam Structure Type of the structure to create.
 */
template <class Structure>
class ComplexityAnalyzer
    : public LeafAnalyzer,
      public std::conditional_t<std::is_default_constructible_v<Structure>,
                                PrototypeFactory<Structure>, IPrototypeFactory<Structure>> {
  public:
    /**
     *  @brief Runs the analysis.
     */
    void analyze() override;

  protected:
    /**
     *  @brief Constructs complexity analyzer with given name.
     */
    explicit ComplexityAnalyzer(const std::string &name);

    /**
     *  @brief Must ensure that @p structure has size @p size after a call.
     */
    virtual void growToSize(Structure &structure, size_t size) = 0;

    /**
     *  @brief Executes the analyzed operation (and nothing else!).
     */
    virtual void executeOperation(Structure &structure) = 0;

    /**
     *  @brief Runs the analysis replications.
     *  @param structurePrototype Prototype used to initialize the structure.
     */
    void runReplications(Structure structurePrototype);

    /**
     *  @brief Registers hook that is run before each call of @c executeOperation
     * .
     */
    void registerBeforeOperation(std::function<void(Structure &)> op);

    /**
     *  @brief Registers hook that is run after each call of @c executeOperation .
     */
    void registerAfterOperation(std::function<void(Structure &)> op);

  private:
    using duration_t = std::chrono::nanoseconds;

    void saveToCsvFile(const std::vector<size_t> &sizes,
                       const std::vector<std::vector<duration_t>> &results) const;

    std::function<void(Structure &)> beforeOperation_;
    std::function<void(Structure &)> afterOperation_;
};

template <class Structure>
ComplexityAnalyzer<Structure>::ComplexityAnalyzer(const std::string &name)
    : LeafAnalyzer(name), beforeOperation_([](Structure &) {}),
      afterOperation_([](Structure &) {}) {}

template <class Structure> void ComplexityAnalyzer<Structure>::analyze() {
    this->resetSuccess();
    this->runReplications(this->createPrototype());
    this->setSuccess();
}

template <class Structure>
void ComplexityAnalyzer<Structure>::runReplications(Structure structurePrototype) {
    std::vector<size_t> capacities;
    std::vector<std::vector<duration_t>> durations;

    for (size_t step = 0; step < getStepCount(); ++step) {
        capacities.push_back((step + 1) * getStepSize());
    }

    for (size_t replication = 0; replication < getReplicationCount(); ++replication) {
        Structure testStructure(structurePrototype);
        std::vector<duration_t> durationVector;

        for (size_t step = 0; step < getStepCount(); ++step) {
            growToSize(testStructure, capacities[step]);
            beforeOperation_(testStructure);
            auto timeStart = std::chrono::high_resolution_clock::now();
            executeOperation(testStructure);
            auto timeEnd = std::chrono::high_resolution_clock::now();
            afterOperation_(testStructure);

            auto duration = duration_cast<duration_t>(timeEnd - timeStart);
            durationVector.push_back(duration);
        }

        durations.push_back(std::move(durationVector));
    }

    saveToCsvFile(capacities, durations);
}

template <class Structure>
void ComplexityAnalyzer<Structure>::registerBeforeOperation(std::function<void(Structure &)> op) {
    beforeOperation_ = std::move(op);
}

template <class Structure>
void ComplexityAnalyzer<Structure>::registerAfterOperation(std::function<void(Structure &)> op) {
    afterOperation_ = std::move(op);
}

template <class Structure>
void ComplexityAnalyzer<Structure>::saveToCsvFile(
    const std::vector<size_t> &sizes, const std::vector<std::vector<duration_t>> &results) const {
    constexpr char Separator = ';';
    const std::filesystem::path path = this->getOutputPath();
    std::ofstream ost(path);

    if (!ost.is_open()) {
        throw std::runtime_error("Failed to open output file.");
    }

    for (const size_t size : sizes) {
        ost << size << (size != sizes.back() ? Separator : '\n');
    }

    for (const std::vector<duration_t> &durations : results) {
        size_t col = 0;
        for (const duration_t duration : durations) {
            ost << duration.count() << (col != durations.size() - 1 ? Separator : '\n');
            ++col;
        }
    }
}
} // namespace ds::utils
