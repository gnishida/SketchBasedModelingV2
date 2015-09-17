#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

class Stroke3D {
public:
	glm::vec3 p1;
	glm::vec3 p2;

public:
	Stroke3D() {}
	Stroke3D(const glm::vec3& p1, const glm::vec3& p2) : p1(p1), p2(p2) {}
};