#pragma once

#include <boost/shared_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include "Shape.h"

class RenderManager;

namespace cga {

class Polygon : public Shape {
private:
	std::vector<glm::vec2> _points;
	glm::vec2 _center;

public:
	Polygon() {}
	Polygon(const std::string& name, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec2>& points, const glm::vec3& color, const std::string& texture);
	boost::shared_ptr<Shape> clone(const std::string& name) const;
	boost::shared_ptr<Shape> extrude(const std::string& name, float height);
	boost::shared_ptr<Shape> inscribeCircle(const std::string& name);
	boost::shared_ptr<Shape> offset(const std::string& name, float offsetDistance, int offsetSelector);
	boost::shared_ptr<Shape> roofGable(const std::string& name, float angle);
	boost::shared_ptr<Shape> roofHip(const std::string& name, float angle);
	void setupProjection(float texWidth, float texHeight);
	void size(float xSize, float ySize, float zSize);
	//void split(int direction, const std::vector<float> ratios, const std::vector<std::string> names, std::vector<Object*>& objects);
	boost::shared_ptr<Shape> taper(const std::string& name, float height, float top_ratio = 0.0f);
	void render(RenderManager* renderManager, const std::string& name, bool showScopeCoordinateSystem) const;
};

}
