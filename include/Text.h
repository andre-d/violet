#pragma once
#include <FTGL/ftgl.h>
#include <string>

class Text {
	FTGLPixmapFont* font;
	public:
		Text(std::string filename);
		void draw(std::string text, int size);
		virtual ~Text();
};
