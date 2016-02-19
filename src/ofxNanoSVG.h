#pragma once

#include "nanosvg.h"

#include "ofMain.h"

class ofxNanoSVG
{
public:
	
	using Path = std::shared_ptr<ofPath>;
	
	struct Layer : public std::vector<Path>
	{
		void draw() const {
			for (const auto& it : *this) {
				it->draw();
			}
		}
	};

	bool load(const string& filepath);
	void clear();
	
	float getWidth() const;
	float getHeight() const;
	
	int getNumLayer() const;
	const Layer& getLayer(int index) const;

	const std::vector<std::string>& getLayerNames() const;
	const Layer& getLayer(const std::string& layer_name) const;
	
	void dumpLayerNames() const;

public:
	
	void draw(float x, float y) const;
	
private:
	
	float width, height;
	std::vector<Layer> layer_arr;
	std::unordered_map<std::string, Layer> layer_map;
	std::vector<std::string> layer_names;
};
