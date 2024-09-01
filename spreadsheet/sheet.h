#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <vector>

struct sheetStat {
    sheetStat& addToRow(size_t row) {
        if (rows.size() <= row) {
            rows.resize(row + 1);
        }
        ++rows.at(row);
        setPrintable();
        return *this;
    }
    sheetStat& addToColumn(size_t col) {
        if (columns.size() <= col) {
            columns.resize(col + 1);
        }
        ++columns.at(col);
        setPrintable();
        return *this;
    }

    sheetStat& deliteFromRow(size_t row) {
        --rows.at(row);
        while (!rows.empty() && rows.back() == 0) {
            rows.pop_back();
        }
        setPrintable();
        return *this;
    }
    sheetStat& deliteFromColumn(size_t col) {
        --columns.at(col);

        while (!columns.empty() && columns.back() == 0) {
            columns.pop_back();
        }

        setPrintable();
        return *this;
    }

    Size GetPrintable() const {
        return printableArea;
    }
private:
    void setPrintable() {
        printableArea.rows = rows.size();
        printableArea.cols = columns.size();
    }

    std::vector<int> rows;
    std::vector<int> columns;
    Size printableArea;
};

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;
    

private:
    void IsPosValid(Position pos) const;


    sheetStat printable_;
    std::vector<std::vector<std::unique_ptr<CellInterface>>> sheet_;

};