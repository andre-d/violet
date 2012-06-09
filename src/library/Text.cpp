#include "Text.h"
#include <stdexcept>

Text::Text(std::string filename) {
	font = new FTGLPixmapFont(filename.c_str());

	if(font->Error()) {
	    throw std::runtime_error("Failed to load font: " + filename);
	}

	if(!font->CharMap(ft_encoding_unicode)) {
		 throw std::runtime_error("Failed init font: " + filename);
	}

	wchar_t buf[0x10000];
	for(size_t i = 1; i < 0x10000; i++)
	{
	    buf[i] = (wchar_t)i;
	}
	buf[0xFFFF] = '\0';

	font->Advance(buf);
}

void Text::draw(std::string text, int size) {
	font->FaceSize(size);
	font->Render(text.c_str());
}

Text::~Text() {
	delete font;
}

