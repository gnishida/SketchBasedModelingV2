#include "RuleParser.h"
#include "CenterOperator.h"
#include "ColorOperator.h"
#include "CompOperator.h"
#include "CopyOperator.h"
#include "ExtrudeOperator.h"
#include "InsertOperator.h"
#include "OffsetOperator.h"
#include "RoofGableOperator.h"
#include "RoofHipOperator.h"
#include "RotateOperator.h"
#include "SetupProjectionOperator.h"
#include "ShapeLOperator.h"
#include "SizeOperator.h"
#include "SplitOperator.h"
#include "TaperOperator.h"
#include "TextureOperator.h"
#include "TranslateOperator.h"
#include "CGA.h"
#include <iostream>

namespace cga {

void parseRule(char* filename, RuleSet& ruleSet) {
	QFile file(filename);

	QDomDocument doc;
	doc.setContent(&file, true);
	QDomElement root = doc.documentElement();

	QDomNode child_node = root.firstChild();
	while (!child_node.isNull()) {
		if (child_node.toElement().tagName() == "attr") {
			if (!child_node.toElement().hasAttribute("name")) {
				std::cout << "<attr> tag must contain name attribute." << std::endl;
				throw "<attr> tag must contain name attribute.";
			}
			std::string name = child_node.toElement().attribute("name").toUtf8().constData();

			if (!child_node.toElement().hasAttribute("value")) {
				std::cout << "<attr> tag must contain value attribute." << std::endl;
				throw "<attr> tag must contain value attribute.";
			}
			std::string value = child_node.toElement().attribute("value").toUtf8().constData();

			ruleSet.addAttr(name, value);
		} else if (child_node.toElement().tagName() == "rule") {
			if (!child_node.toElement().hasAttribute("name")) {
				std::cout << "<rule> tag must contain name attribute." << std::endl;
				throw "<rule> tag must contain name attribute.";
			}
			std::string name = child_node.toElement().attribute("name").toUtf8().constData();

			ruleSet.addRule(name);

			QDomNode operator_node = child_node.firstChild();
			while (!operator_node.isNull()) {
				std::string operator_name = operator_node.toElement().tagName().toUtf8().constData();

				if (operator_name == "center") {
					ruleSet.addOperator(name, parseCenterOperator(operator_node));
				} else if (operator_name == "color") {
					ruleSet.addOperator(name, parseColorOperator(operator_node));
				} else if (operator_name == "comp") {
					ruleSet.addOperator(name, parseCompOperator(operator_node));
				} else if (operator_name == "copy") {
					ruleSet.addOperator(name, parseCopyOperator(operator_node));
				} else if (operator_name == "extrude") {
					ruleSet.addOperator(name, parseExtrudeOperator(operator_node));
				} else if (operator_name == "insert") {
					ruleSet.addOperator(name, parseInsertOperator(operator_node));
				} else if (operator_name == "offset") {
					ruleSet.addOperator(name, parseOffsetOperator(operator_node));
				} else if (operator_name == "roofGable") {
					ruleSet.addOperator(name, parseRoofGableOperator(operator_node));
				} else if (operator_name == "roofHip") {
					ruleSet.addOperator(name, parseRoofHipOperator(operator_node));
				} else if (operator_name == "rotate") {
					ruleSet.addOperator(name, parseRotateOperator(operator_node));
				} else if (operator_name == "setupProjection") {
					ruleSet.addOperator(name, parseSetupProjectionOperator(operator_node));
				} else if (operator_name == "shapeL") {
					ruleSet.addOperator(name, parseShapeLOperator(operator_node));
				} else if (operator_name == "size") {
					ruleSet.addOperator(name, parseSizeOperator(operator_node));
				} else if (operator_name == "split") {
					ruleSet.addOperator(name, parseSplitOperator(operator_node));
				} else if (operator_name == "taper") {
					ruleSet.addOperator(name, parseTaperOperator(operator_node));
				} else if (operator_name == "texture") {
					ruleSet.addOperator(name, parseTextureOperator(operator_node));
				} else if (operator_name == "translate") {
					ruleSet.addOperator(name, parseTranslateOperator(operator_node));
				}

				operator_node = operator_node.nextSibling();
			}
		}

		child_node = child_node.nextSibling();
	}
}

Operator* parseCenterOperator(const QDomNode& node) {
	int axesSelector;

	if (!node.toElement().hasAttribute("axesSelector")) {
		throw "center node has to have axesSelector attribute.";
	}

	if (node.toElement().attribute("axesSelector") == "xyz") {
		axesSelector = AXES_SELECTOR_XYZ;
	} else if (node.toElement().attribute("axesSelector") == "x") {
		axesSelector = AXES_SELECTOR_X;
	} else if (node.toElement().attribute("axesSelector") == "y") {
		axesSelector = AXES_SELECTOR_Y;
	} else if (node.toElement().attribute("axesSelector") == "z") {
		axesSelector = AXES_SELECTOR_Z;
	} else if (node.toElement().attribute("axesSelector") == "xy") {
		axesSelector = AXES_SELECTOR_XY;
	} else if (node.toElement().attribute("axesSelector") == "xz") {
		axesSelector = AXES_SELECTOR_XZ;
	} else {
		axesSelector = AXES_SELECTOR_YZ;
	}

	return new CenterOperator(axesSelector);
}

Operator* parseColorOperator(const QDomNode& node) {
	std::string r;
	std::string g;
	std::string b;
	std::string s;

	if (node.toElement().hasAttribute("r")) {
		r = node.toElement().attribute("r").toUtf8().constData();
	}
	if (node.toElement().hasAttribute("g")) {
		g = node.toElement().attribute("g").toUtf8().constData();
	}
	if (node.toElement().hasAttribute("b")) {
		b = node.toElement().attribute("b").toUtf8().constData();
	}
	if (node.toElement().hasAttribute("s")) {
		s = node.toElement().attribute("s").toUtf8().constData();
	}

	if (s.empty()) {
		return new ColorOperator(r, g, b);
	} else {
		return new ColorOperator(s);
	}
}

Operator* parseCompOperator(const QDomNode& node) {
	std::string front_name;
	std::string side_name;
	std::string top_name;
	std::string bottom_name;
	std::string inside_name;
	std::string border_name;
	std::string vertical_name;
	std::map<std::string, std::string> name_map;

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			QString name = child.toElement().attribute("name");
			std::string value = child.toElement().attribute("value").toUtf8().constData();

			if (name == "front") {
				name_map["front"] = value;
			} else if (name == "right") {
				name_map["right"] = value;
			} else if (name == "left") {
				name_map["left"] = value;
			} else if (name == "back") {
				name_map["back"] = value;
			} else if (name == "side") {
				name_map["side"] = value;
			} else if (name == "top") {
				name_map["top"] = value;
			} else if (name == "bottom") {
				name_map["bottom"] = value;
			} else if (name == "inside") {
				name_map["inside"] = value;
			} else if (name == "border") {
				name_map["border"] = value;
			} else if (name == "vertical") {
				name_map["vertical"] = value;
			}
		}

		child = child.nextSibling();
	}

	return new CompOperator(name_map);
}

Operator* parseCopyOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("name")) {
		throw "copy node has to have name attribute.";
	}

	std::string copy_name = node.toElement().attribute("name").toUtf8().constData();

	return new CopyOperator(copy_name);
}

Operator* parseExtrudeOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("height")) {
		throw "extrude node has to have height attribute.";
	}

	std::string height = node.toElement().attribute("height").toUtf8().constData();

	return new ExtrudeOperator(height);
}

Operator* parseInsertOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("geometryPath")) {
		throw "insert node has to have geometryPath attribute.";
	}

	std::string geometryPath = node.toElement().attribute("geometryPath").toUtf8().constData();

	return new InsertOperator(geometryPath);
}

Operator* parseOffsetOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("offsetDistance")) {
		throw "offset node has to have offsetDistance attribute.";
	}

	std::string offsetDistance = node.toElement().attribute("offsetDistance").toUtf8().constData();

	int offsetSelector = SELECTOR_ALL;
	if (node.toElement().hasAttribute("offsetSelector")) {
		if (node.toElement().attribute("offsetSelector") == "all") {
			offsetSelector = SELECTOR_ALL;
		} else if (node.toElement().attribute("offsetSelector") == "inside") {
			offsetSelector = SELECTOR_INSIDE;
		} else {
			offsetSelector = SELECTOR_BORDER;
		}
	}

	return new OffsetOperator(offsetDistance, offsetSelector);
}

Operator* parseRoofGableOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("angle")) {
		throw "roofGable node has to have angle attribute.";
	}

	std::string angle = node.toElement().attribute("angle").toUtf8().constData();

	return new RoofGableOperator(angle);
}

Operator* parseRoofHipOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("angle")) {
		throw "roofHip node has to have angle attribute.";
	}

	std::string angle = node.toElement().attribute("angle").toUtf8().constData();

	return new RoofHipOperator(angle);
}

Operator* parseRotateOperator(const QDomNode& node) {
	float xAngle = 0.0f;
	float yAngle = 0.0f;
	float zAngle = 0.0f;

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			QString name = child.toElement().attribute("name");

			if (name == "xAngle") {
				xAngle = child.toElement().attribute("value").toFloat();
			} else if (name == "yAngle") {
				yAngle = child.toElement().attribute("value").toFloat();
			} else if (name == "zAngle") {
				zAngle = child.toElement().attribute("value").toFloat();
			}
		}

		child = child.nextSibling();
	}

	return new RotateOperator(xAngle, yAngle, zAngle);
}

Operator* parseSetupProjectionOperator(const QDomNode& node) {
	int axesSelector;
	Value texWidth;
	Value texHeight;

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			QString name = child.toElement().attribute("name");

			if (name == "axesSelector") {
				if (child.toElement().attribute("value") == "scope.xy") {
					axesSelector = AXES_SCOPE_XY;
				} else if (child.toElement().attribute("value") == "scope.xz") {
					axesSelector = AXES_SCOPE_XZ;
				} else {
				}
			} else if (name == "texWidth") {
				std::string type =  child.toElement().attribute("type").toUtf8().constData();
				std::string value =  child.toElement().attribute("value").toUtf8().constData();
				if (type == "absolute") {
					texWidth = Value(Value::TYPE_ABSOLUTE, value);
				} else if (type == "relative") {
					texWidth = Value(Value::TYPE_RELATIVE, value);
				} else {
					throw "type of texWidth for texture has to be either absolute or relative.";
				}
			} else if (name == "texHeight") {
				std::string type =  child.toElement().attribute("type").toUtf8().constData();
				std::string value =  child.toElement().attribute("value").toUtf8().constData();
				if (type == "absolute") {
					texHeight = Value(Value::TYPE_ABSOLUTE, value);
				} else if (type == "relative") {
					texHeight = Value(Value::TYPE_RELATIVE, value);
				} else {
					throw "type of texHeight for texture has to be either absolute or relative.";
				}
			}
		}

		child = child.nextSibling();
	}

	return new SetupProjectionOperator(axesSelector, texWidth, texHeight);
}

Operator* parseShapeLOperator(const QDomNode& node) {
	float frontWidth;
	float leftWidth;

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			QString name = child.toElement().attribute("name");

			if (name == "frontWidth") {
				frontWidth = child.toElement().attribute("value").toFloat();
			} else if (name == "leftWidth") {
				leftWidth = child.toElement().attribute("value").toFloat();
			}
		}

		child = child.nextSibling();
	}

	return new ShapeLOperator(frontWidth, leftWidth);
}

Operator* parseSizeOperator(const QDomNode& node) {
	Value xSize;
	Value ySize;
	Value zSize;

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			QString name = child.toElement().attribute("name");
			QString type;

			if (!child.toElement().hasAttribute("type")) {
				throw "param node under size node has to have type attribute.";
			}

			type = child.toElement().attribute("type");
			std::string value = child.toElement().attribute("value").toUtf8().constData();

			if (name == "xSize") {
				if (type == "relative") {
					xSize = Value(Value::TYPE_RELATIVE, value);
				} else if (type == "absolute") {
					xSize = Value(Value::TYPE_ABSOLUTE, value);
				} else {
					throw "type attribute under size node has to be either relative or absolute.";
				}
			} else if (name == "ySize") {
				if (type == "relative") {
					ySize = Value(Value::TYPE_RELATIVE, value);
				} else if (type == "absolute") {
					ySize = Value(Value::TYPE_ABSOLUTE, value);
				} else {
					throw "type attribute under size node has to be either relative or absolute.";
				}
			} else if (name == "zSize") {
				if (type == "relative") {
					zSize = Value(Value::TYPE_RELATIVE, value);
				} else if (type == "absolute") {
					zSize = Value(Value::TYPE_ABSOLUTE, value);
				} else {
					throw "type attribute under size node has to be either relative or absolute.";
				}
			}
		}

		child = child.nextSibling();
	}

	return new SizeOperator(xSize, ySize, zSize);
}

Operator* parseSplitOperator(const QDomNode& node) {
	int splitAxis;
	std::vector<Value> sizes;
	std::vector<std::string> names;

	if (!node.toElement().hasAttribute("splitAxis")) {
		throw "split node has to have splitAxis attribute.";
	}
	if (node.toElement().attribute("splitAxis") == "x") {
		splitAxis = DIRECTION_X;
	} else if (node.toElement().attribute("splitAxis") == "y") {
		splitAxis = DIRECTION_Y;
	} else {
		splitAxis = DIRECTION_Z;
	}

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			QString type = child.toElement().attribute("type");
			std::string value = child.toElement().attribute("value").toUtf8().constData();
			bool repeat = false;
			if (child.toElement().hasAttribute("repeat")) {
				repeat = true;
			}

			if (repeat) {
				if (type == "absolute") {
					sizes.push_back(Value(Value::TYPE_ABSOLUTE, value, true));
				} else if (type == "relative") {
					sizes.push_back(Value(Value::TYPE_RELATIVE, value, true));
				} else {
					sizes.push_back(Value(Value::TYPE_FLOATING, value, true));
				}
			} else {
				if (type == "absolute") {
					sizes.push_back(Value(Value::TYPE_ABSOLUTE, value));
				} else if (type == "relative") {
					sizes.push_back(Value(Value::TYPE_RELATIVE, value));
				} else {
					sizes.push_back(Value(Value::TYPE_FLOATING, value));
				}
			}

			names.push_back(child.toElement().attribute("name").toUtf8().constData());
		}

		child = child.nextSibling();
	}

	return new SplitOperator(splitAxis, sizes, names);
}

Operator* parseTaperOperator(const QDomNode& node) {
	if (!node.toElement().hasAttribute("height")) {
		throw "taper node has to have height attribute.";
	}

	std::string height = node.toElement().attribute("height").toUtf8().constData();

	std::string top_ratio;
	if (node.toElement().hasAttribute("top_ratio")) {
		top_ratio = node.toElement().attribute("top_ratio").toUtf8().constData();

	} else {
		top_ratio = "0.0";
	}

	return new TaperOperator(height, top_ratio);
}

Operator* parseTextureOperator(const QDomNode& node) {
	std::string texture;

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			QString name = child.toElement().attribute("name");

			if (name == "texture") {
				texture = child.toElement().attribute("value").toUtf8().constData();
			}
		}

		child = child.nextSibling();
	}

	return new TextureOperator(texture);
}

Operator* parseTranslateOperator(const QDomNode& node) {
	int mode;
	int coordSystem;
	Value x;
	Value y;
	Value z;

	if (!node.toElement().hasAttribute("mode")) {
		throw "translate node has to have mode attribute.";
	}
	if (node.toElement().attribute("mode") == "abs") {
		mode = MODE_ABSOLUTE;
	} else if (node.toElement().attribute("mode") == "rel") {
		mode = MODE_RELATIVE;
	} else {
		throw "mode has to be either abs or rel.";
	}

	if (!node.toElement().hasAttribute("coordSystem")) {
		throw "translate node has to have coordSystem attribute.";
	}
	if (node.toElement().attribute("coordSystem") == "world") {
		coordSystem = COORD_SYSTEM_WORLD;
	} else if (node.toElement().attribute("coordSystem") == "object") {
		coordSystem = COORD_SYSTEM_OBJECT;
	} else {
		throw "coordSystem has to be either world or object.";
	}

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "param") {
			if (!child.toElement().hasAttribute("name")) {
				throw "param has to have name attribute.";
			}
			std::string name = child.toElement().attribute("name").toUtf8().constData();
			if (!child.toElement().hasAttribute("value")) {
				throw "param has to have value attribute.";
			}
			std::string value = child.toElement().attribute("value").toUtf8().constData();
			if (!child.toElement().hasAttribute("type")) {
				throw "param has to have type attribute.";
			}
			std::string type = child.toElement().attribute("type").toUtf8().constData();

			if (name == "x") {
				if (type == "absolute") {
					x = Value(Value::TYPE_ABSOLUTE, value);
				} else if (type == "relative") {
					x = Value(Value::TYPE_RELATIVE, value);
				} else {
					throw "type of param for translate has to be either absolute or relative.";
				}
			} else if (name == "y") {
				if (type == "absolute") {
					y = Value(Value::TYPE_ABSOLUTE, value);
				} else if (type == "relative") {
					y = Value(Value::TYPE_RELATIVE, value);
				} else {
					throw "type of param for translate has to be either absolute or relative.";
				}
			} else if (name == "z") {
				if (type == "absolute") {
					z = Value(Value::TYPE_ABSOLUTE, value);
				} else if (type == "relative") {
					z = Value(Value::TYPE_RELATIVE, value);
				} else {
					throw "type of param for translate has to be either absolute or relative.";
				}
			}
		}

		child = child.nextSibling();
	}

	return new TranslateOperator(mode, coordSystem, x, y, z);
}

}
