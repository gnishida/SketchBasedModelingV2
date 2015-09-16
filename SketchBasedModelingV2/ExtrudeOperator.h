#pragma once

#include "Rule.h"

namespace cga {

class ExtrudeOperator : public Operator {
private:
	std::string height;

public:
	ExtrudeOperator(const std::string& height);

	Shape* apply(Shape* shape, const RuleSet& ruleSet, std::list<Shape*>& stack);
};

}