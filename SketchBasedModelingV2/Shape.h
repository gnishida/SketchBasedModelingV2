#pragma once

#include "CGA.h"

namespace cga {

class Shape {
public:
	std::string _name;
	bool _removed;
	glm::mat4 _modelMat;
	glm::vec3 _color;
	bool _textureEnabled;
	std::string _texture;
	std::vector<glm::vec2> _texCoords;
	glm::vec3 _scope;
	glm::vec3 _prev_scope;
	glm::mat4 _pivot;

	static std::map<std::string, Asset> assets;

public:
	void center(int axesSelector);
	virtual Shape* clone(const std::string& name);
	virtual void comp(const std::map<std::string, std::string>& name_map, std::vector<Shape*>& shapes);
	virtual Shape* extrude(const std::string& name, float height);
	virtual Shape* inscribeCircle(const std::string& name);
	Shape* insert(const std::string& name, const std::string& geometryPath);
	void nil();
	virtual Shape* offset(const std::string& name, float offsetDistance, int offsetSelector);
	virtual Shape* roofGable(const std::string& name, float angle);
	virtual Shape* roofHip(const std::string& name, float angle);
	void rotate(const std::string& name, float xAngle, float yAngle, float zAngle);
	virtual void setupProjection(int axesSelector, float texWidth, float texHeight);
	virtual Shape* shapeL(const std::string& name, float frontWidth, float leftWidth);
	virtual void size(float xSize, float ySize, float zSize);
	virtual void split(int splitAxis, const std::vector<float>& sizes, const std::vector<std::string>& names, std::vector<Shape*>& objects);
	virtual Shape* taper(const std::string& name, float height, float top_ratio = 0.0f);
	void texture(const std::string& tex);
	void translate(int mode, int coordSystem, float x, float y, float z);
	virtual void generate(RenderManager* renderManager, bool showScopeCoordinateSystem);

protected:
	void drawAxes(RenderManager* renderManager, const glm::mat4& modelMat);
	static Asset getAsset(const std::string& filename);
};

}