#include "ofxNanoSVG.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

bool ofxNanoSVG::load(const string& filepath)
{
	clear();
	
	NSVGimage* image;
	image = nsvgParseFromFile(ofToDataPath(filepath).c_str(), "px", 72.0f);
	
	if (image == NULL) return false;
	
	width = image->width;
	height = image->height;
	
	std::string name;
	
	for (NSVGshape* shape = image->shapes; shape != NULL; shape = shape->next)
	{
		auto p = std::make_shared<ofPath>();
		
		for (NSVGpath* path = shape->paths; path != NULL; path = path->next)
		{
			ofVec2f p0(path->pts[0], path->pts[1]);
			p->moveTo(p0);
			
			for (int i = 0; i < path->npts - 1; i += 3)
			{
				float* pts = &path->pts[i * 2] + 2;
				
				ofVec2f cp0(pts[0], pts[1]);
				ofVec2f cp1(pts[2], pts[3]);
				ofVec2f p1(pts[4], pts[5]);
				
				ofVec2f v0 = p0 - cp0;
				ofVec2f v1 = cp1 - p1;
				
				bool is_line = v0.isAligned(v1, 0.01f);
				
				if (is_line)
					p->lineTo(p1);
				else
					p->bezierTo(cp0, cp1, p1);
				
				p0 = p1;
			}
			
			if (path->closed)
				p->close();
			
			if (shape->fill.type == NSVG_PAINT_COLOR)
			{
				p->setFilled(true);
				
				ofColor c;
				c.r = shape->fill.color & 0x000000FF;
				c.g = (shape->fill.color >> 8) & 0x000000FF;
				c.b = (shape->fill.color >> 16) & 0x000000FF;
				p->setFillColor(c);
			}
			else
			{
				p->setFilled(false);
			}
			
			if (shape->stroke.type == NSVG_PAINT_COLOR)
			{
				p->setStrokeWidth(shape->strokeWidth);
				
				ofColor c;
				c.r = shape->stroke.color & 0x000000FF;
				c.g = (shape->stroke.color >> 8) & 0x000000FF;
				c.b = (shape->stroke.color >> 16) & 0x000000FF;
				p->setStrokeColor(c);
			}
			else
			{
				p->setStrokeWidth(0);
			}
		}
		
		if (name != shape->id)
		{
			layer_arr.emplace_back();
		}
		
		layer_arr.back().emplace_back(p);
		layer_map[shape->id].emplace_back(p);
	}
	
	for (const auto& it : layer_map)
		layer_names.emplace_back(it.first);
	
	nsvgDelete(image);
	
	return true;
}

void ofxNanoSVG::clear()
{
	layer_arr.clear();
	layer_map.clear();
	layer_names.clear();
}

float ofxNanoSVG::getWidth() const { return width; }

float ofxNanoSVG::getHeight() const { return height; }

int ofxNanoSVG::getNumLayer() const
{
	return layer_arr.size();
}

const ofxNanoSVG::Layer& ofxNanoSVG::getLayer(int index) const
{
	return layer_arr[index];
}

const std::vector<std::string>& ofxNanoSVG::getLayerNames() const
{
	return layer_names;
}

void ofxNanoSVG::dumpLayerNames() const
{
	for (const auto& name : getLayerNames())
		cout << name << endl;
}

const ofxNanoSVG::Layer& ofxNanoSVG::getLayer(const std::string& layer_name) const
{
	return layer_map.at(layer_name);
}

void ofxNanoSVG::draw(float x, float y) const
{
	ofPushMatrix();
	ofTranslate(x, y);
	for (const auto& layer : layer_arr)
		layer.draw();
	ofPopMatrix();
}
