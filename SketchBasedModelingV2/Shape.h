#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <map>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include "Asset.h"
#include "Face.h"
#include "Stroke3D.h"

class RenderManager;

namespace cga {

class RuleSet;

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
	virtual boost::shared_ptr<Shape> clone(const std::string& name) const;
	virtual void comp(const std::map<std::string, std::string>& name_map, std::vector<boost::shared_ptr<Shape> >& shapes);
	virtual boost::shared_ptr<Shape> extrude(const std::string& name, float height);
	virtual boost::shared_ptr<Shape> inscribeCircle(const std::string& name);
	boost::shared_ptr<Shape> insert(const std::string& name, const std::string& geometryPath);
	void nil();
	virtual boost::shared_ptr<Shape> offset(const std::string& name, float offsetDistance, int offsetSelector);
	virtual boost::shared_ptr<Shape> roofGable(const std::string& name, float angle);
	virtual boost::shared_ptr<Shape> roofHip(const std::string& name, float angle);
	void rotate(const std::string& name, float xAngle, float yAngle, float zAngle);
	virtual void setupProjection(int axesSelector, float texWidth, float texHeight);
	virtual boost::shared_ptr<Shape> shapeL(const std::string& name, float frontWidth, float leftWidth);
	virtual void size(float xSize, float ySize, float zSize);
	virtual void split(int splitAxis, const std::vector<float>& sizes, const std::vector<std::string>& names, std::vector<boost::shared_ptr<Shape> >& objects);
	virtual boost::shared_ptr<Shape> taper(const std::string& name, float height, float top_ratio = 0.0f);
	void texture(const std::string& tex);
	void translate(int mode, int coordSystem, float x, float y, float z);
	virtual void render(RenderManager* renderManager, const std::string& name, bool showScopeCoordinateSystem) const;

	virtual bool hitFace(const glm::vec3& cameraPos, const glm::vec3& viewDir, Face& face, float& dist);
	virtual void findRule(const std::vector<Stroke3D>& strokes3D, int sketch_step, RuleSet* ruleSet);

protected:
	void drawAxes(RenderManager* renderManager, const glm::mat4& modelMat) const;
	static Asset getAsset(const std::string& filename);
};

}
