#include "Rectangle.h"
#include "GLUtils.h"
#include "Cuboid.h"
#include "CGA.h"
#include "Face.h"
#include <boost/lexical_cast.hpp>
#include "SplitOperator.h"

namespace cga {

Rectangle::Rectangle(const std::string& name, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float height, const glm::vec3& color) {
	this->_name = name;
	this->_removed = false;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope.x = width;
	this->_scope.y = height;
	this->_color = color;
	this->_scope = glm::vec3(width, height, 0);
	this->_textureEnabled = false;
}

Rectangle::Rectangle(const std::string& name, const glm::mat4& pivot, const glm::mat4& modelMat, float width, float height, const glm::vec3& color, const std::string& texture, float u1, float v1, float u2, float v2) {
	this->_name = name;
	this->_removed = false;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_scope.x = width;
	this->_scope.y = height;
	this->_color = color;
	this->_texture = texture;
	this->_scope = glm::vec3(width, height, 0);

	_texCoords.resize(4);
	_texCoords[0] = glm::vec2(u1, v1);
	_texCoords[1] = glm::vec2(u2, v1);
	_texCoords[2] = glm::vec2(u2, v2);
	_texCoords[3] = glm::vec2(u1, v2);
	this->_textureEnabled = true;
}

boost::shared_ptr<Shape> Rectangle::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new Rectangle(*this));
	copy->_name = name;
	return copy;
}

boost::shared_ptr<Shape> Rectangle::extrude(const std::string& name, float height) {
	return boost::shared_ptr<Shape>(new Cuboid(name, _pivot, _modelMat, _scope.x, _scope.y, height, _color));
}

void Rectangle::split(int splitAxis, const std::vector<float>& sizes, const std::vector<std::string>& names, std::vector<boost::shared_ptr<Shape> >& objects) {
	float offset = 0.0f;
	
	for (int i = 0; i < sizes.size(); ++i) {
		if (splitAxis == DIRECTION_X) {
			if (names[i] != "NIL") {
				glm::mat4 mat = glm::translate(glm::mat4(), glm::vec3(offset, 0, 0));
				if (_texCoords.size() > 0) {
					objects.push_back(boost::shared_ptr<Shape>(new Rectangle(names[i], _pivot, _modelMat * mat, sizes[i], _scope.y, _color, _texture,
						_texCoords[0].x + (_texCoords[1].x - _texCoords[0].x) * offset / _scope.x, _texCoords[0].y,
						_texCoords[0].x + (_texCoords[1].x - _texCoords[0].x) * (offset + sizes[i]) / _scope.x, _texCoords[2].y)));
				} else {
					objects.push_back(boost::shared_ptr<Shape>(new Rectangle(names[i], _pivot, _modelMat * mat, sizes[i], _scope.y, _color)));
				}
			}
			offset += sizes[i];
		} else if (splitAxis == DIRECTION_Y) {
			if (names[i] != "NIL") {
				glm::mat4 mat = glm::translate(glm::mat4(), glm::vec3(0, offset, 0));
				if (_texCoords.size() > 0) {
					objects.push_back(boost::shared_ptr<Shape>(new Rectangle(names[i], _pivot, _modelMat * mat, _scope.x, sizes[i], _color, _texture,
						_texCoords[0].x, _texCoords[0].y + (_texCoords[2].y - _texCoords[0].y) * offset / _scope.y,
						_texCoords[1].x, _texCoords[0].y + (_texCoords[2].y - _texCoords[0].y) * (offset + sizes[i]) / _scope.y)));
				} else {
					objects.push_back(boost::shared_ptr<Shape>(new Rectangle(names[i], _pivot, _modelMat * mat, _scope.x, sizes[i], _color)));
				}
			}
			offset += sizes[i];
		} else if (splitAxis == DIRECTION_Z) {
			objects.push_back(this->clone(this->_name));
		}
	}
}

void Rectangle::render(RenderManager* renderManager, const std::string& name, bool showScopeCoordinateSystem) const {
	if (_removed) return;

	std::vector<Vertex> vertices;

	vertices.resize(6);

	glm::vec4 p1(0, 0, 0, 1);
	p1 = _pivot * _modelMat * p1;
	glm::vec4 p2(_scope.x, 0, 0, 1);
	p2 = _pivot * _modelMat * p2;
	glm::vec4 p3(_scope.x, _scope.y, 0, 1);
	p3 = _pivot * _modelMat * p3;
	glm::vec4 p4(0, _scope.y, 0, 1);
	p4 = _pivot * _modelMat * p4;

	glm::vec4 normal(0, 0, 1, 0);
	normal = _pivot * _modelMat * normal;

	if (_textureEnabled) {
		vertices[0] = Vertex(glm::vec3(p1), glm::vec3(normal), _color, _texCoords[0]);
		vertices[1] = Vertex(glm::vec3(p2), glm::vec3(normal), _color, _texCoords[1], 1);
		vertices[2] = Vertex(glm::vec3(p3), glm::vec3(normal), _color, _texCoords[2]);

		vertices[3] = Vertex(glm::vec3(p1), glm::vec3(normal), _color, _texCoords[0]);
		vertices[4] = Vertex(glm::vec3(p3), glm::vec3(normal), _color, _texCoords[2]);
		vertices[5] = Vertex(glm::vec3(p4), glm::vec3(normal), _color, _texCoords[3], 1);

		renderManager->addObject(name.c_str(), _texture.c_str(), vertices);
	} else {
		vertices[0] = Vertex(glm::vec3(p1), glm::vec3(normal), _color);
		vertices[1] = Vertex(glm::vec3(p2), glm::vec3(normal), _color, 1);
		vertices[2] = Vertex(glm::vec3(p3), glm::vec3(normal), _color);

		vertices[3] = Vertex(glm::vec3(p1), glm::vec3(normal), _color);
		vertices[4] = Vertex(glm::vec3(p3), glm::vec3(normal), _color);
		vertices[5] = Vertex(glm::vec3(p4), glm::vec3(normal), _color, 1);

		renderManager->addObject(name.c_str(), "", vertices);
	}
	
	if (showScopeCoordinateSystem) {
		vertices.resize(0);
		glutils::drawAxes(0.1, 3, _pivot * _modelMat, vertices);
		renderManager->addObject("axis", "", vertices);
	}
}

bool Rectangle::hitFace(const glm::vec3& cameraPos, const glm::vec3& viewDir, Face& face, float& dist) {
	std::vector<glm::vec3> points;

	glm::vec4 p1(0, 0, 0, 1);
	points.push_back(glm::vec3(_pivot * _modelMat * p1));
	glm::vec4 p2(_scope.x, 0, 0, 1);
	points.push_back(glm::vec3(_pivot * _modelMat * p2));
	glm::vec4 p3(_scope.x, _scope.y, 0, 1);
	points.push_back(glm::vec3(_pivot * _modelMat * p3));
	glm::vec4 p4(0, _scope.y, 0, 1);
	points.push_back(glm::vec3(_pivot * _modelMat * p4));

	glm::vec3 normal = glm::vec3(_pivot * _modelMat * glm::vec4(0, 0, 1, 0));

	glm::vec3 intPt;
	dist = (std::numeric_limits<float>::max)();
	bool hit = false;

	for (int k = 1; k < points.size() - 1; ++k) {
		if (glutils::rayTriangleIntersection(cameraPos, viewDir, points[0], points[k], points[k+1], intPt)) {
			float d = glm::length(intPt - cameraPos);
			if (d < dist) {
				hit = true;
				dist = d;
				face = Face(this, points, normal, _color, _texCoords);
			}
		}
	}

	return hit;
}

void Rectangle::findRule(const std::vector<Stroke3D>& strokes3D, int sketch_step, CGA* cga) {
	glm::mat4 mat = _pivot * _modelMat;
	mat = glm::inverse(mat);

	if (sketch_step == STEP_FLOOR) {
		std::vector<float> floor_y;
		floor_y.push_back(0);
		for (int i = 0; i < strokes3D.size(); ++i) {
			glm::vec3 p1 = glm::vec3(mat * glm::vec4(strokes3D[i].p1, 1));
			glm::vec3 p2 = glm::vec3(mat * glm::vec4(strokes3D[i].p2, 1));

			floor_y.push_back((p1.y + p2.y) * 0.5f);
		}

		std::sort(floor_y.begin(), floor_y.end());
		
		std::vector<float> floor_heights;
		for (int i = 0; i < floor_y.size() - 1; ++i) {
			if (floor_y[i + 1] - floor_y[i] > 0.5f) {
				floor_heights.push_back(floor_y[i + 1] - floor_y[i]);
			}
		}

		if (floor_heights.size() == 1) {
			// pattern A*
			cga->ruleSet.attrs["floor_height"] = boost::lexical_cast<std::string>(floor_heights[0]);
			cga->ruleSet.rules[_name] = cga->ruleRepository["floors"][0];
		} else {
			// pattern { A | B* }
			cga->ruleSet.attrs["groundf_height"] = boost::lexical_cast<std::string>(floor_heights[0]);
			cga->ruleSet.attrs["floor_height"] = boost::lexical_cast<std::string>(floor_heights[1]);
			cga->ruleSet.rules[_name] = cga->ruleRepository["floors"][1];
		}
	}

	/*

			if (p1.x < _scope.x * 0.2 && p2.x > _scope.x * 0.8 && fabs(p1.y - p2.y) < _scope.y * 0.3) {
				float y = (p1.y + p2.y) * 0.5f;
				if (y < _scope.y * 0.5f) {
					// this line specifies the height of the first floor
					ruleSet->addRule(_name);
					std::string size = boost::lexical_cast<std::string>(y);
					std::vector<Value> sizes;
					sizes.push_back(Value(Value::TYPE_FLOATING, size, true));

					std::vector<std::string> names;
					names.push_back("Floor");
					ruleSet->addOperator(_name, boost::shared_ptr<Operator>(new SplitOperator(DIRECTION_Y, sizes, names)));
				} else {
					// this line specifies the height of the top floor
					ruleSet->addRule(_name);
					std::string size = boost::lexical_cast<std::string>(_scope.y - y);
					std::vector<Value> sizes;
					sizes.push_back(Value(Value::TYPE_FLOATING, size, true));

					std::vector<std::string> names;
					names.push_back("Floor");
					ruleSet->addOperator(_name, boost::shared_ptr<Operator>(new SplitOperator(DIRECTION_Y, sizes, names)));
				}
			}
		}
	*/
}

}
