#pragma once

#include "common.h"
#include "formula.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_set>



using Value = CellInterface::Value;

class Impl {
public:

    virtual Value GetValue() const = 0;
    virtual std::string GetText() const = 0;

    virtual ~Impl() = default;
};

class EmptyImpl : public Impl {
public:

    Value GetValue() const override;
    std::string GetText() const override;
};

class TextImpl : public Impl {
public:

    explicit TextImpl(std::string text);
    Value GetValue() const override;
    std::string GetText() const override;

private:
    std::string text_;
};

class FormulaImpl : public Impl {
public:

    explicit FormulaImpl(std::string text, SheetInterface& sheet, std::vector<Position>& ref);
    Value GetValue() const override;
    std::string GetText() const override;


private:
    std::unique_ptr<FormulaInterface> formula_ptr_;
    SheetInterface& sheet_;
};

class Cell : public CellInterface {
public:
    Cell(SheetInterface& sheet);
    ~Cell();

    void Set(std::string text);

    void Clear();

    Value GetValue() const override;

    std::string GetText() const override;
    
    std::vector<Position>& GetReferencedCells() override;
private:
    void AddParent(CellInterface* parent) override;
    void RemoveParent(CellInterface* parent) override;
    void SetCahchedStatus(bool status) override;
    enum class State { WHITE, GREY, BLACK };
    State checkState = State::WHITE;

    void RevertState(std::unordered_set<Cell*>& allReferenced);
    void CheckForCycleDepeendancy(std::vector<Position>& referenceCells, std::unordered_set<Cell*>& allReferenced);
   
    void RemoveAsParent();
    void AddAsParent();

    SheetInterface& sheet_;
    std::unique_ptr<Impl> impl_;

    std::vector<Position> referenceCells_;
    std::vector<CellInterface*> parentCells_;

    bool isCached_;
    Value cahcedValue_;
};

