﻿#include "GLWidget3D.h"
#include <iostream>
#include "RuleParser.h"

#include "Rectangle.h"
#include "Polygon.h"
#include "GLUtils.h"

GLWidget3D::GLWidget3D(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent) {
	setAutoFillBackground(false);

	// 光源位置をセット
	// ShadowMappingは平行光源を使っている。この位置から原点方向を平行光源の方向とする。
	light_dir = glm::normalize(glm::vec3(-4, -5, -8));

	// シャドウマップ用のmodel/view/projection行列を作成
	glm::mat4 light_pMatrix = glm::ortho<float>(-100, 100, -100, 100, 0.1, 200);
	glm::mat4 light_mvMatrix = glm::lookAt(-light_dir * 50.0f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	light_mvpMatrix = light_pMatrix * light_mvMatrix;

	// initialize keys
	ctrlPressed = false;
}

void GLWidget3D::keyPressEvent(QKeyEvent *e) {
	ctrlPressed = false;

	switch (e->key()) {
	case Qt::Key_Control:
		ctrlPressed = true;
		break;
	default:
		break;
	}
}

void GLWidget3D::keyReleaseEvent(QKeyEvent* e) {
	ctrlPressed = false;
}

/**
 * Draw the scene.
 */
void GLWidget3D::drawScene(int drawMode) {
	if (drawMode == 0) {
		glUniform1i(glGetUniformLocation(renderManager.program,"shadowState"), 1);
	} else {
		glUniform1i(glGetUniformLocation(renderManager.program,"shadowState"), 2);
	}

	if (showScopeCoordinateSystem) {
		renderManager.renderAll(showWireframe);
	} else {
		renderManager.renderAllExcept("axis", showWireframe);
	}
}

void GLWidget3D::drawLineTo(const QPoint &endPoint) {
	currentStroke->points.push_back(glm::vec2(endPoint.x(), endPoint.y()));

	lastPoint = endPoint;
}

void GLWidget3D::resizeSketch(int width, int height) {
/*	for (int i = 0; i < parametriclsystem::NUM_LAYERS; ++i) {
		QImage newImage(width, height, QImage::Format_ARGB32);
		newImage.fill(qRgba(255, 255, 255, 0));
		QPainter painter(&newImage);

		QImage img = sketch[i].scaled(width, height);
		painter.drawImage(0, 0, img);
		sketch[i] = newImage;
	}*/
}

bool GLWidget3D::compute3dCoordinates(Stroke* stroke) {
	int face_index;
	std::vector<glm::vec3> face_points;	

	// ワールド座標系でのカメラ座標
	glm::vec3 cameraPos = camera.cameraPosInWorld();

	// 視線ベクトル
	glm::vec3 view_v1 = viewVector(stroke->points[0], camera.mvMatrix, camera.f(), camera.aspect());

	std::cout << std::endl;

	if (points.hitFace(cameraPos, view_v1, camera.mvpMatrix, face_index)) {
		std::cout << "Hit face: " << face_index << std::endl;
		for (int i = 0; i < points.faces[face_index].points.size(); ++i) {
			face_points.push_back(points.points[points.faces[face_index].points[i]]);
		}
	} else {
		std::cout << "No face hit." << std::endl;
		face_points.push_back(glm::vec3(-1000, 0, -1000));
		face_points.push_back(glm::vec3(-1000, 0, 1000));
		face_points.push_back(glm::vec3(1000, 0, 1000));
		face_points.push_back(glm::vec3(1000, 0, -1000));
	}

	// faceの中心座標を計算
	glm::vec3 face_center;
	for (int i = 0; i < face_points.size(); ++i) {
		face_center += face_points[i];
	}
	face_center /= face_points.size();

	// faceの法線ベクトル
	glm::vec3 face_normal = glm::normalize(glm::cross(face_points[1] - face_points[0], face_points[2] - face_points[1]));

	// 法線ベクトルを、スクリーン座標系に変換
	glm::vec3 face_above= face_center + face_normal;
	glm::vec4 face_center_projected = camera.mvpMatrix * glm::vec4(face_center, 1);
	glm::vec4 face_above_projected = camera.mvpMatrix * glm::vec4(face_above, 1);
	glm::vec2 face_normal_projected = glm::normalize(glm::vec2(face_above_projected.x / face_above_projected.w - face_center_projected.x / face_center_projected.w,
		face_center_projected.y / face_center_projected.w - face_above_projected.y / face_above_projected.w));

	// strokeのベクトル
	glm::vec2 stroke_vec = glm::normalize(stroke->points[0] - stroke->points.back());

	int e1 = -1;
	int e2 = -1;
	if (fabs(glm::dot(face_normal_projected, stroke_vec)) > 0.8f) { // vertical line
		std::cout << "Vertical line." << std::endl;
		glm::vec3 p1 = unprojectByPlane(stroke->points[0], face_points[0], face_normal);
		std::cout << "Point " << glm::to_string(p1);
		points.snapPoint(p1, 8.0f, e1);
		if (e1 >= 0) {
			std::cout << " snapped to " << e1 << std::endl;
		} else {
			std::cout << " not snap" << std::endl;
		}
		glm::vec3 p2 = unprojectByLine(stroke->points.back(), p1, face_normal);
		std::cout << "Point " << glm::to_string(p2);
		points.snapPoint(p2, 2.0f, e2);
		if (e2 >= 0) {
			std::cout << " snapped to " << e2 << std::endl;
		} else {
			std::cout << " not snap" << std::endl;
		}
		if (points.addQuadEdge(p1, p2)) {
			return true;
		}
	} else { // horizontal line
		std::cout << "Non-vertical line." << std::endl;
		if (isStraightLine(stroke)) {
			glm::vec3 p1 = unprojectByPlane(stroke->points[0], face_points[0], face_normal);
			glm::vec3 p2 = unprojectByPlane(stroke->points.back(), face_points[0], face_normal);
			if (points.addQuadEdge(p1, p2)) {
				return true;
			}
		} else {
			glm::vec2 midPt = findTurningPoint(stroke);

			glm::vec3 p1, p2;
			int v1 = -1;
			int v2 = -1;
			points.snapPoint(normalizeScreenCoordinates(stroke->points[0]), camera.mvpMatrix, 5.0f, p1, v1);
			points.snapPoint(normalizeScreenCoordinates(stroke->points.back()), camera.mvpMatrix, 5.0f, p2, v2);
			std::cout << "p1 snapped to " << v1 << std::endl;
			std::cout << "p2 snapped to " << v2 << std::endl;
			if (v1 >= 0 && v2 >= 0) {
				glm::vec3 p12 = (p1 + p2) * 0.5f;
				/*
				glm::vec3 normal = glm::normalize(p2 - p1);
				glm::vec3 p3 = unprojectByPlane(midPt, p12, normal);
				*/
				glm::vec3 p3 = unprojectByLine(midPt, p12, glm::vec3(0, 1, 0)); // デモ用に、垂直方向の三角形のみ対応
				bool faceAdded1 = points.addTriangleEdge(p1, p3);
				bool faceAdded2 = points.addTriangleEdge(p2, p3);
				if (faceAdded1 || faceAdded2) {
					return true;
				}
			}
		}
	}
	
	return false;
}

glm::vec3 GLWidget3D::unprojectByPlane(const glm::vec2& point, const glm::vec3& face_point, const glm::vec3& face_normal) {
	glm::vec3 cameraPos = camera.cameraPosInWorld();
	glm::vec3 dir((point.x - width() * 0.5f) * 2.0f / width() * camera.aspect(), (height() * 0.5f - point.y) * 2.0f / height(), -camera.f());
	dir = glm::vec3(glm::inverse(camera.mvMatrix) * glm::vec4(dir, 0));

	glm::vec3 intPt = glutils::rayPlaneIntersection(cameraPos, dir, face_point, face_normal);
	return intPt;
}

glm::vec3 GLWidget3D::unprojectByLine(const glm::vec2& point, const glm::vec3& reference_point, const glm::vec3& vec) {
	glm::vec3 cameraPos = camera.cameraPosInWorld();
	glm::vec3 dir((point.x - width() * 0.5f) * 2.0f / width() * camera.aspect(), (height() * 0.5f - point.y) * 2.0f / height(), -camera.f());
	dir = glm::vec3(glm::inverse(camera.mvMatrix) * glm::vec4(dir, 0));

	glm::vec3 intPt = glutils::lineLineIntersection(cameraPos, dir, reference_point, vec, 0, 1);
	return intPt;
}

glm::vec2 GLWidget3D::normalizeScreenCoordinates(const glm::vec2& point) {
	return glm::vec2((point.x - width() * 0.5f) * 2.0f / width(), (height() * 0.5f - point.y) * 2.0f / height());
}

glm::vec3 GLWidget3D::viewVector(const glm::vec2& point, const glm::mat4& mvMatrix, float focalLength, float aspect) {
	glm::vec3 dir((point.x - width() * 0.5f) * 2.0f / width() * aspect, (height() * 0.5f - point.y) * 2.0f / height(), -focalLength);
	return glm::vec3(glm::inverse(mvMatrix) * glm::vec4(dir, 0));
}

bool GLWidget3D::isStraightLine(Stroke* stroke) {
	glm::vec2 midPt = stroke->points[stroke->points.size() * 0.5];
	
	glm::vec2 vec1 = glm::normalize(stroke->points.back() - stroke->points[0]);
	glm::vec2 vec2 = glm::normalize(midPt - stroke->points[0]);
	if (glm::dot(vec1, vec2) > 0.95f) {
		std::cout << "Straight line" << std::endl;
		std::cout << "[0]: " << stroke->points[0].x << "," << stroke->points[0].y << std::endl;
		std::cout << "[mid]: " << midPt.x << "," << midPt.y << std::endl;
		std::cout << "[back]: " << stroke->points.back().x << "," << stroke->points.back().y << std::endl;
		std::cout << "vec1: " << vec1.x << "," << vec1.y << std::endl;
		std::cout << "vec2: " << vec2.x << "," << vec2.y << std::endl;
		return true;
	} else {
		std::cout << "Not Straight line" << std::endl;
		return false;
	}
}

glm::vec2 GLWidget3D::findTurningPoint(Stroke* stroke) {
	int index1 = stroke->points.size() * 0.2;
	int index2 = stroke->points.size() * 0.8;
	
	glm::vec2 v = glm::normalize(stroke->points.back() - stroke->points[0]);

	float min_dot = 1.0f;
	int min_index = -1;

	for (int i = index1; i < index2; ++i) {
		float dot = fabs(glm::dot(glm::normalize(stroke->points[i] - stroke->points[0]), v));
		if (dot < min_dot) {
			min_dot = dot;
			min_index = i;
		}
	}

	return stroke->points[min_index];
}

void GLWidget3D::clear() {
	points.clear();
	strokes.clear();
	if (currentStroke != NULL) {
		delete currentStroke;
		currentStroke = NULL;
	}

	points.generate(&renderManager);
}

void GLWidget3D::resizeGL(int width, int height) {
	// sketch imageを更新
	resizeSketch(width, height);

	// OpenGLの設定を更新
	height = height ? height : 1;
	glViewport(0, 0, width, height);
	camera.updatePMatrix(width, height);

	rb.update(width, height);
}

void GLWidget3D::mousePressEvent(QMouseEvent *e) {
	if (ctrlPressed) { // move camera
		camera.mousePress(e->x(), e->y());
	} else { // sketch
		lastPoint = e->pos();
		currentStroke = new Stroke(glm::vec2(e->x(), e->y()));
	}

	update();
}

void GLWidget3D::mouseReleaseEvent(QMouseEvent *e) {
	if (currentStroke != NULL) {
		if (currentStroke->points.size() > 3 && glm::length(currentStroke->points.back() - currentStroke->points[0]) > 10) {
			// compute 3d coordinates of user stroke
			bool faceAdded = compute3dCoordinates(currentStroke);

			if (faceAdded) {
				strokes.clear();
				points.generate(&renderManager);
			} else {
				strokes.push_back(*currentStroke);
			}
		}

		delete currentStroke;
		currentStroke = NULL;
	}

	update();
}

void GLWidget3D::mouseMoveEvent(QMouseEvent *e) {
	if (ctrlPressed) {
		if (e->buttons() & Qt::LeftButton) { // Rotate
			camera.rotate(e->x(), e->y());
		} else if (e->buttons() & Qt::MidButton) { // Move
			camera.move(e->x(), e->y());
		} else if (e->buttons() & Qt::RightButton) { // Zoom
			camera.zoom(e->x(), e->y());
		}
	} else { // sketch
		drawLineTo(e->pos());
	}

	update();
}

void GLWidget3D::initializeGL() {
	renderManager.init("../shaders/vertex.glsl", "../shaders/geometry.glsl", "../shaders/fragment.glsl", 8192);
	showWireframe = true;
	showScopeCoordinateSystem = false;

	rb.init(renderManager.program, 4, 5, width(), height());

	std::vector<Vertex> vertices;
	glm::mat4 mat = glm::translate(glm::mat4(), glm::vec3(0, -1, 0));
	mat = glm::rotate(mat, (float)(-M_PI * 0.5f), glm::vec3(1, 0, 0));
	//glutils::drawQuad(100, 100, glm::vec3(1.0, 1.0, 1.0), mat, vertices);
	glutils::drawGrid(100, 100, 2, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), mat, vertices);
	//glutils::drawGrid(60, 60, 1, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::rotate(glm::mat4(), -3.1415926f * 0.5f, glm::vec3(1, 0, 0)), vertices);

	renderManager.addObject("grid", "", vertices);


	// CGA initial mass
	std::list<boost::shared_ptr<cga::Shape> > stack;
	cga::Rectangle* lot = new cga::Rectangle("Lot", glm::translate(glm::rotate(glm::mat4(), (float)(-M_PI * 0.5f), glm::vec3(1, 0, 0)), glm::vec3(-10, -17.5, 0)), glm::mat4(), 20, 35, glm::vec3(1, 1, 1));
	stack.push_back(boost::shared_ptr<cga::Shape>(lot));

	try {
		cga::RuleSet ruleSet;
		cga::parseRule("../cga/simpleMass.xml", ruleSet);
		system.generate(&renderManager, ruleSet, stack, true);
	} catch (const char* ex) {
		std::cout << "ERROR:" << std::endl << ex << std::endl;
	}

	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);


	currentStroke = NULL;
}

void GLWidget3D::paintEvent(QPaintEvent *event) {
	// OpenGLで描画
	makeCurrent();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glUseProgram(renderManager.program);

	//renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);
	glClearColor(0.443, 0.439, 0.458, 0.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);

	// Model view projection行列をシェーダに渡す
	glUniformMatrix4fv(glGetUniformLocation(renderManager.program, "mvpMatrix"),  1, GL_FALSE, &camera.mvpMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(renderManager.program, "mvMatrix"),  1, GL_FALSE, &camera.mvMatrix[0][0]);

	// pass the light direction to the shader
	//glUniform1fv(glGetUniformLocation(renderManager.program, "lightDir"), 3, &light_dir[0]);
	glUniform3f(glGetUniformLocation(renderManager.program, "lightDir"), light_dir.x, light_dir.y, light_dir.z);
	
	//rb.pass1();
	//drawScene(0);
	//rb.pass2();
	drawScene(0);

	// OpenGLの設定を元に戻す
	glShadeModel(GL_FLAT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// QPainterで描画
	QPainter painter(this);
	painter.setPen(QPen(QColor(0, 0, 0)));
	painter.setOpacity(0.5);
	for (int i = 0; i < strokes.size(); ++i) {
		for (int k = 0; k < strokes[i].points.size() - 1; ++k) {
			painter.drawLine(strokes[i].points[k].x, strokes[i].points[k].y, strokes[i].points[k+1].x, strokes[i].points[k+1].y);
		}
	}
	if (currentStroke != NULL) {
		for (int k = 0; k < currentStroke->points.size() - 1; ++k) {
			painter.drawLine(currentStroke->points[k].x, currentStroke->points[k].y, currentStroke->points[k+1].x, currentStroke->points[k+1].y);
		}
	}
	painter.end();

	glEnable(GL_DEPTH_TEST);
}
