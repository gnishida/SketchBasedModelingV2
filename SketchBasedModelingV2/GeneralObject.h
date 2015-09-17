#pragma once

#include <boost/shared_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include "Shape.h"

class RenderManager;

namespace cga {

class GeneralObject : public Shape {
private:
	std::vector<glm::vec3> _points;
	std::vector<glm::vec3> _normals;
	std::vector<glm::vec2> _texCoords;

public:
	GeneralObject(const std::string& name, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals, const glm::vec3& color);
	GeneralObject(const std::string& name, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals, const glm::vec3& color, const std::vector<glm::vec2>& texCoords, const std::string& texture);
	boost::shared_ptr<Shape> clone(const std::string& name) const;
	void size(float xSize, float ySize, float zSize);
	void render(RenderManager* renderManager, const std::string& name, bool showScopeCoordinateSystem) const;
};

}
