#pragma once

#include "Rule.h"

namespace cga {

class SplitOperator : public Operator {
private:
	int splitAxis;
	std::vector<Value> sizes;
	std::vector<std::string> output_names;

public:
	SplitOperator(int splitAxis, const std::vector<Value>& sizes, const std::vector<std::string>& output_names);
	Shape* apply(Shape* shape, const RuleSet& ruleSet, std::list<Shape*>& stack);
};

}
