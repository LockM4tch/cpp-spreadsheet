#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::IsPosValid(Position pos) const {

    if (pos.col >= Position::MAX_COLS || pos.row >= Position::MAX_ROWS){
        throw InvalidPositionException("cell is not in range");
    }
    if (pos.col < 0 || pos.row < 0) {
            throw InvalidPositionException("cell is not in range");
    }
}

void Sheet::SetCell(Position pos, std::string text) {
    IsPosValid(pos);
    size_t col = pos.col;
    size_t row = pos.row;

    if (row >= sheet_.size()) {
        sheet_.resize(row + 1);
    }
    if (col >= sheet_[row].size()) {
        sheet_[row].resize(col + 1);
    }

    auto& currentCell = sheet_[row][col];

    if (currentCell == nullptr) {
        printable_.addToRow(row).addToColumn(col);
        currentCell.reset(new Cell(*this));
    }

    if (currentCell->GetText() == text) { return; }
    currentCell->Set(text);

    for (auto cell : currentCell->GetReferencedCells()) {
        if (GetCell(cell) == nullptr) { SetCell(cell, ""); };
    }
}

const CellInterface* Sheet::GetCell(Position pos) const {
    IsPosValid(pos);
    size_t col = pos.col;
    size_t row = pos.row;

    if (row >= sheet_.size() || col >= sheet_[row].size()) {
        return nullptr;
    }
    return sheet_[row][col].get();
}
CellInterface* Sheet::GetCell(Position pos) {
    IsPosValid(pos);

    size_t col = pos.col;
    size_t row = pos.row;

    if (row >= sheet_.size() || (col >= sheet_[row].size())) {
        return nullptr;
    }
    return sheet_[row][col].get();
}


void Sheet::ClearCell(Position pos) {
    IsPosValid(pos);
    size_t col = pos.col;
    size_t row = pos.row;

    if (row >= sheet_.size() || col >= sheet_[row].size()) {
        return;
    }

    auto& currentCell = sheet_[row][col];
    if (currentCell != nullptr) {
        currentCell->Clear();
        currentCell.reset(nullptr);
        printable_.deliteFromRow(row).deliteFromColumn(col);
    }

}

Size Sheet::GetPrintableSize() const { return printable_.GetPrintable(); }

void Sheet::PrintValues(std::ostream& output) const {

    for (int i = 0; i < printable_.GetPrintable().rows; i++){
        int columnt = sheet_[i].size();
        for (int j = 0; j < printable_.GetPrintable().cols; j++) {
            if (j != 0) { output << "\t";}
            if (j < columnt && sheet_[i][j] != nullptr) {

                auto value = sheet_[i][j]->GetValue();

                if (std::holds_alternative<std::string>(value))
                {
                    output << std::get<std::string>(value);
                }
                else if (std::holds_alternative<double>(value))
                {
                    output << std::get<double>(value);
                }
                else if (std::holds_alternative<FormulaError>(value))
                {
                    output << std::get<FormulaError>(value);
                }
            }
        }
        output << "\n";
    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    for (int i = 0; i < printable_.GetPrintable().rows; i++) {
        int columns = sheet_[i].size();
        for (int j = 0; j < printable_.GetPrintable().cols; j++) {
            if (j != 0) { output << "\t"; }

            if (j < columns &&  sheet_[i][j] != nullptr) {
                output << sheet_[i][j]->GetText();
            }
        }
        output << "\n";
    }
}


std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}

