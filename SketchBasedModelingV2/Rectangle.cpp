﻿#include "Rectangle.h"
#include "GLUtils.h"
#include "Pyramid.h"
#include "HipRoof.h"
#include "GableRoof.h"
#include "Prism.h"
#include "Polygon.h"
#include "Cuboid.h"
#include "OffsetPolygon.h"
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

boost::shared_ptr<Shape> Rectangle::inscribeCircle(const std::string& name) {
	return boost::shared_ptr<Shape>();
}

boost::shared_ptr<Shape> Rectangle::offset(const std::string& name, float offsetDistance, int offsetSelector) {
	if (offsetSelector == SELECTOR_ALL) {
		std::vector<glm::vec2> points(4);
		points[0] = glm::vec2(0, 0);
		points[1] = glm::vec2(_scope.x, 0);
		points[2] = glm::vec2(_scope.x, _scope.y);
		points[3] = glm::vec2(0, _scope.y);

		return boost::shared_ptr<Shape>(new OffsetPolygon(name, _pivot, _modelMat, points, offsetDistance, _color, _texture));
	} else if (offsetSelector == SELECTOR_INSIDE) {
		float offset_width = _scope.x + offsetDistance * 2.0f;
		float offset_height = _scope.y + offsetDistance * 2.0f;
		glm::mat4 mat = glm::translate(_modelMat, glm::vec3(-offsetDistance, -offsetDistance, 0));
		if (_textureEnabled) {
			float offset_u1 = _texCoords[0].x;
			float offset_v1 = _texCoords[0].y;
			float offset_u2 = _texCoords[2].x;
			float offset_v2 = _texCoords[2].y;
			if (offsetDistance < 0) {
				float offset_u1 = (_texCoords[2].x - _texCoords[0].x) * (-offsetDistance) / _scope.x + _texCoords[0].x;
				float offset_v1 = (_texCoords[2].y - _texCoords[0].y) * (-offsetDistance) / _scope.y + _texCoords[0].y;
				float offset_u2 = (_texCoords[2].x - _texCoords[0].x) * (_scope.x + offsetDistance) / _scope.x + _texCoords[0].x;
				float offset_v2 = (_texCoords[2].y - _texCoords[0].y) * (_scope.y + offsetDistance) / _scope.y + _texCoords[0].y;
			}
			return boost::shared_ptr<Shape>(new Rectangle(name, _pivot, mat, offset_width, offset_height, _color, _texture, offset_u1, offset_v1, offset_u2, offset_v2));
		} else {
			return boost::shared_ptr<Shape>(new Rectangle(name, _pivot, mat, offset_width, offset_height, _color));
		}
	} else {
		throw "border of offset is not supported by rectangle.";
	}
}

boost::shared_ptr<Shape> Rectangle::roofGable(const std::string& name, float angle) {
	std::vector<glm::vec2> points(4);
	points[0] = glm::vec2(0, 0);
	points[1] = glm::vec2(_scope.x, 0);
	points[2] = glm::vec2(_scope.x, _scope.y);
	points[3] = glm::vec2(0, _scope.y);
	return boost::shared_ptr<Shape>(new GableRoof(name, _pivot, _modelMat, points, angle, _color));
}

boost::shared_ptr<Shape> Rectangle::roofHip(const std::string& name, float angle) {
	std::vector<glm::vec2> points(4);
	points[0] = glm::vec2(0, 0);
	points[1] = glm::vec2(_scope.x, 0);
	points[2] = glm::vec2(_scope.x, _scope.y);
	points[3] = glm::vec2(0, _scope.y);
	return boost::shared_ptr<Shape>(new HipRoof(name, _pivot, _modelMat, points, angle, _color));
}

void Rectangle::setupProjection(int axesSelector, float texWidth, float texHeight) {
	_texCoords.resize(4);
	_texCoords[0] = glm::vec2(0, 0);
	_texCoords[1] = glm::vec2(_scope.x / texWidth, 0);
	_texCoords[2] = glm::vec2(_scope.x / texWidth, _scope.y / texHeight);
	_texCoords[3] = glm::vec2(0, _scope.y / texHeight);
}

boost::shared_ptr<Shape> Rectangle::shapeL(const std::string& name, float frontWidth, float leftWidth) {
	std::vector<glm::vec2> points(6);
	points[0] = glm::vec2(0, 0);
	points[1] = glm::vec2(_scope.x, 0);
	points[2] = glm::vec2(_scope.x, frontWidth);
	points[3] = glm::vec2(leftWidth, frontWidth);
	points[4] = glm::vec2(leftWidth, _scope.y);
	points[5] = glm::vec2(0, _scope.y);

	return boost::shared_ptr<Shape>(new Polygon(name, _pivot, _modelMat, points, _color, _texture));
}

void Rectangle::size(float xSize, float ySize, float zSize) {
	_prev_scope = _scope;

	_scope.x = xSize;
	_scope.y = ySize;
	_scope.z = zSize;
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

boost::shared_ptr<Shape> Rectangle::taper(const std::string& name, float height, float top_ratio) {
	std::vector<glm::vec2> points(4);
	points[0] = glm::vec2(0, 0);
	points[1] = glm::vec2(_scope.x, 0);
	points[2] = glm::vec2(_scope.x, _scope.y);
	points[3] = glm::vec2(0, _scope.y);
	return boost::shared_ptr<Shape>(new Pyramid(name, _pivot, _modelMat, points, glm::vec2(_scope.x * 0.5, _scope.y * 0.5), height, top_ratio, _color, _texture));
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

		renderManager->addObject(_name.c_str(), _texture.c_str(), vertices);
	} else {
		vertices[0] = Vertex(glm::vec3(p1), glm::vec3(normal), _color);
		vertices[1] = Vertex(glm::vec3(p2), glm::vec3(normal), _color, 1);
		vertices[2] = Vertex(glm::vec3(p3), glm::vec3(normal), _color);

		vertices[3] = Vertex(glm::vec3(p1), glm::vec3(normal), _color);
		vertices[4] = Vertex(glm::vec3(p3), glm::vec3(normal), _color);
		vertices[5] = Vertex(glm::vec3(p4), glm::vec3(normal), _color, 1);

		renderManager->addObject(_name.c_str(), "", vertices);
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

void Rectangle::findRule(const std::vector<Stroke3D>& strokes3D, int sketch_step, RuleSet* ruleSet) {
	glm::mat4 mat = _pivot * _modelMat;
	mat = glm::inverse(mat);

	for (int i = 0; i < strokes3D.size(); ++i) {
		glm::vec3 p1 = glm::vec3(mat * glm::vec4(strokes3D[i].p1, 1));
		glm::vec3 p2 = glm::vec3(mat * glm::vec4(strokes3D[i].p2, 1));

		if (sketch_step == STEP_FLOOR) {
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
		} else {
		}
	}
}

}
