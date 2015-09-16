#pragma once

#include "RenderManager.h"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Vertex.h"
#include "Rule.h"

namespace cga {

enum { DIRECTION_X = 0, DIRECTION_Y, DIRECTION_Z, SCOPE_SX, SCOPE_SY };
enum { REVOLVE_X = 0, REVOLVE_Y };
enum { MODE_ABSOLUTE = 0, MODE_RELATIVE };
enum { COORD_SYSTEM_WORLD = 0, COORD_SYSTEM_OBJECT };
enum { AXES_SCOPE_XY = 0, AXES_SCOPE_XZ, AXES_SCOPE_YX, AXES_SCOPE_YZ, AXES_SCOPE_ZX, AXES_SCOPE_ZY, AXES_WORLD_XY, AXES_WORLD_XZ, AXES_WORLD_YX, AXES_WORLD_YZ, AXES_WORLD_ZX, AXES_WORLD_ZY };
enum { SELECTOR_ALL = 0, SELECTOR_INSIDE, SELECTOR_BORDER };
enum { AXES_SELECTOR_XYZ = 0, AXES_SELECTOR_X, AXES_SELECTOR_Y, AXES_SELECTOR_Z, AXES_SELECTOR_XY, AXES_SELECTOR_XZ, AXES_SELECTOR_YZ };

//const float M_PI = 3.1415926f;
float M_PI = 3.1415926f;

class BoundingBox {
public:
	glm::vec3 minPt;
	glm::vec3 maxPt;

public:
	BoundingBox(const std::vector<glm::vec2>& points);
	BoundingBox(const std::vector<glm::vec3>& points);
	float sx() { return maxPt.x - minPt.x; }
	float sy() { return maxPt.y - minPt.y; }
	float sz() { return maxPt.z - minPt.z; }
};

class Asset {
public:
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	//BoundingBox bbox;

public:
	Asset();
	Asset(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords);
};

class CGA {
public:
	glm::mat4 modelMat;

public:
	CGA();

	void generate(RenderManager* renderManager, const RuleSet& ruleSet, std::list<Shape*> stack, bool showScopeCoordinateSystem = false);
};

}
