#pragma once

#include <boost/shared_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include "Shape.h"

class RenderManager;

namespace cga {

class HipRoof : public Shape {
private:
	std::vector<glm::vec2> _points;
	float _angle;

public:
	HipRoof(const std::string& name, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec2>& points, float angle, const glm::vec3& color);
	boost::shared_ptr<Shape> clone(const std::string& name) const;
	void generate(RenderManager* renderManager, bool showScopeCoordinateSystem) const;
};

}
