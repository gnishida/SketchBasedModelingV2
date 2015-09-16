#pragma once

#include "Rule.h"
#include <glm/gtc/matrix_transform.hpp>

namespace cga {

class ColorOperator : public Operator {
private:
	std::string r;
	std::string g;
	std::string b;
	std::string s;

public:
	ColorOperator(const std::string& r, const std::string& g, const std::string& b);
	ColorOperator(const std::string& s);

	Shape* apply(Shape* shape, const RuleSet& ruleSet, std::list<Shape*>& stack);

private:
	static void decodeRGB(const std::string& str, float& r, float& g, float& b);
};

}