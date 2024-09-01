#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

//---------------cell--------------------------
Cell::Cell(SheetInterface& sheet) : sheet_(sheet), impl_(new EmptyImpl{}) {}

Cell::~Cell() {}

void Cell::Set(std::string text) {
	std::vector<Position> referenceCells;
	size_t size = text.size();
	if (size == 0) {
		impl_.reset(new EmptyImpl());
	}
	else if (text.at(0) == '=' && size != 1) {
		try {
			auto form = new FormulaImpl(text, sheet_, referenceCells);
			std::unordered_set<Cell*> allReferenced;
			CheckForCycleDepeendancy(referenceCells, allReferenced);
			RevertState(allReferenced);
			impl_.reset(form);
		}
		catch (const FormulaException& exptn) {
			throw FormulaException("invalid formula");
		}
	}
	else {
		impl_.reset(new TextImpl(text));
	}

	referenceCells_ = referenceCells;
	cahcedValue_ = impl_->GetValue();
	isCahed_ = true;
}



void Cell::RevertState(std::unordered_set<Cell*>& allReferenced) {
	for (auto cell : allReferenced) {
		cell->checkState = State::WHITE;
	}
}

void Cell::CheckForCycleDepeendancy(std::vector<Position>& referenceCells, std::unordered_set<Cell*>& allReferenced) {
	if (checkState == State::GREY) {
		RevertState(allReferenced);
		throw CircularDependencyException("curcular");
	}
	if (checkState == State::BLACK) {
		return;
	}

	allReferenced.insert(dynamic_cast<Cell*>(this));
	checkState = State::GREY;

	for (auto pos : referenceCells) {
		auto cell = sheet_.GetCell(pos);
		if (cell == nullptr) { continue; }
		auto ref = cell->GetReferencedCells();
		dynamic_cast<Cell*>(cell)->CheckForCycleDepeendancy(ref, allReferenced);
	}

	checkState = State::BLACK;
	return;
}

std::vector<Position> Cell::GetReferencedCells() const {
	return referenceCells_;
}


void Cell::Clear() {
	impl_.reset(new EmptyImpl{});
}

Cell::Value Cell::GetValue() const
{
	if (!isCahed_) {
		return cahcedValue_;
	}
	return impl_->GetValue();
}
std::string Cell::GetText() const {
	return impl_->GetText();
}

//---------------empty--------------------------
Value EmptyImpl::GetValue() const {
	return 0.;
}
std::string	 EmptyImpl::GetText() const {
	return "";
}

//---------------text--------------------------
TextImpl::TextImpl(std::string text) :text_(text) {

}
Value TextImpl::GetValue() const {
	if (text_.at(0) == '\'') {
		return std::string(text_.begin() + 1, text_.end());
	}
	return text_;
}
std::string TextImpl::GetText() const {
	return text_;
}

//---------------formula--------------------------
FormulaImpl::FormulaImpl(std::string text, SheetInterface& sheet, std::vector<Position>& ref)
	: formula_ptr_(ParseFormula(text.erase(0, 1))), sheet_(sheet) {
	ref = formula_ptr_->GetReferencedCells();
}
Value FormulaImpl::GetValue() const {
	return formula_ptr_->Evaluate(sheet_);
}
std::string FormulaImpl::GetText() const {
	return "=" + formula_ptr_->GetExpression();
}