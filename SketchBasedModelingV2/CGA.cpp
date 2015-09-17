﻿#include "CGA.h"
#include "GLUtils.h"
#include "OBJLoader.h"
#include <map>
#include <iostream>

namespace cga {

CGA::CGA() {
}

void CGA::generate() {
	shapes.clear();
	stack.clear();
	stack.push_back(axiom->clone(axiom->_name));

	while (!stack.empty()) {
		boost::shared_ptr<Shape> shape = stack.front();
		stack.pop_front();

		if (ruleSet.contain(shape->_name)) {
			ruleSet.getRule(shape->_name).apply(shape, ruleSet, stack);
		} else {
			if (shape->_name.back() != '!' && shape->_name.back() != '.') {
				std::cout << "Warning: " << "no rule is found for " << shape->_name << "." << std::endl;
			}
			shapes.push_back(shape);
		}
	}
}

void CGA::render(RenderManager* renderManager, bool showScopeCoordinateSystem) {
	renderManager->removeObject("shape");

	for (int i = 0; i < shapes.size(); ++i) {
		shapes[i]->render(renderManager, "shape", showScopeCoordinateSystem);
	}
}

bool CGA::hitFace(const glm::vec3& cameraPos, const glm::vec3& viewDir, Face& face) {
	float min_dist = (std::numeric_limits<float>::max)();
	bool hit = false;

	for (int i = 0; i < shapes.size(); ++i) {
		Face f;
		float dist;
		if (shapes[i]->hitFace(cameraPos, viewDir, f, dist)) {
			if (dist < min_dist) {
				hit = true;
				min_dist = dist;
				face = f;
			}
		}
	}

	return hit;
}

}
