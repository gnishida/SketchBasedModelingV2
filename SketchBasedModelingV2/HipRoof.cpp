﻿#include "HipRoof.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/create_straight_skeleton_2.h>
#include "CGA.h"
#include "GLUtils.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;
typedef K::Point_2 KPoint;
typedef CGAL::Polygon_2<K> Polygon_2 ;
typedef CGAL::Straight_skeleton_2<K> Ss ;
typedef boost::shared_ptr<Ss> SsPtr ;

namespace cga {

HipRoof::HipRoof(const std::string& name, const glm::mat4& pivot, const glm::mat4& modelMat, const std::vector<glm::vec2>& points, float angle, const glm::vec3& color) {
	this->_name = name;
	this->_removed = false;
	this->_pivot = pivot;
	this->_modelMat = modelMat;
	this->_points = points;
	this->_angle = angle;
	this->_color = color;
}

boost::shared_ptr<Shape> HipRoof::clone(const std::string& name) const {
	boost::shared_ptr<Shape> copy = boost::shared_ptr<Shape>(new HipRoof(*this));
	copy->_name = name;
	return copy;
}

void HipRoof::render(RenderManager* renderManager, const std::string& name, bool showScopeCoordinateSystem) const {
	std::vector<Vertex> vertices;

	Polygon_2 poly;
	for (int i = 0; i < _points.size(); ++i) {
		poly.push_back(KPoint(_points[i].x, _points[i].y));
	}

	// You can pass the polygon via an iterator pair
	SsPtr iss = CGAL::create_interior_straight_skeleton_2(poly);

	for (auto face = iss->faces_begin(); face != iss->faces_end(); ++face) {
		// 各faceについて、ポリゴンを生成する
		auto edge0 = face->halfedge();
		auto edge = edge0;

		std::vector<glm::vec3> points;

		// 最初のエッジを保存する
		glm::vec2 p0, p1;
		bool first = true;

		do {
			auto head = edge->vertex();
			auto tail = edge->opposite()->vertex();

			if (edge->is_bisector()) {
				if (edge->is_inner_bisector()) { // 外側に接続されていない分割線
				} else { // 外側と接続されている分割線
				}
			} else { // 一番外側のボーダー
			}

			if (first) {
				p0 = glm::vec2(tail->point().x(), tail->point().y());
				p1 = glm::vec2(head->point().x(), head->point().y());
				first = false;

				points.push_back(glm::vec3(_pivot * _modelMat * glm::vec4(p0, 0, 1)));
				points.push_back(glm::vec3(_pivot * _modelMat * glm::vec4(p1, 0, 1)));
			} else {
				glm::vec2 p2 = glm::vec2(head->point().x(), head->point().y());

				if (p2 != p0) {
					// p2の高さを計算
					float z = glutils::distance(p0, p1, p2) * tanf(_angle * M_PI / 180.0f);

					points.push_back(glm::vec3(_pivot * _modelMat * glm::vec4(head->point().x(), head->point().y(), z, 1)));
				}
			}
		} while ((edge = edge->next()) != edge0);

		glm::vec3 normal = glm::normalize(glm::cross(points[1] - points[0], points[2] - points[0]));

		for (int i = 1; i < points.size() - 1; ++i) {
			vertices.push_back(Vertex(points[0], normal, _color));
			if (i < points.size() - 2) {
				vertices.push_back(Vertex(points[i], normal, _color, 1));
			} else {
				vertices.push_back(Vertex(points[i], normal, _color));
			}
			if (i > 1) {
				vertices.push_back(Vertex(points[i+1], normal, _color, 1));
			} else {
				vertices.push_back(Vertex(points[i+1], normal, _color, 1));
			}
		}
	}

	renderManager->addObject(_name.c_str(), "", vertices);

	if (showScopeCoordinateSystem) {
		drawAxes(renderManager, _pivot * _modelMat);
	}
}

}
