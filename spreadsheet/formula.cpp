#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#ARITHM!";
}

namespace {
    class Formula : public FormulaInterface {
    public:
        explicit Formula(std::string expression)
            :ast_(ParseFormulaAST(expression)){
            }
        Value Evaluate(const SheetInterface& sheet) const override {
        try {
            return ast_.Execute(sheet);
        }
        catch (const FormulaError& evaluate_error) {
            return evaluate_error;
        }
        }
        std::string GetExpression() const override {
            std::ostringstream out;
            ast_.PrintFormula(out);
            return out.str();
        }

        std::vector<Position> GetReferencedCells() const {
            std::vector<Position> vec{ ast_.GetCells().begin(), ast_.GetCells().end() };
           auto it = std::unique(vec.begin(), vec.end());
           return { vec.begin() , it};
        }
        

    private:
        FormulaAST ast_;
    };
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try { return std::make_unique<Formula>(std::move(expression)); }
    catch (const std::exception&) {
        throw FormulaException("Bad formula");
    }
     
}