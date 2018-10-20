#include "Text.h"


#include <maths\vector4.h>
#include <graphics\font.h>
#include <graphics\sprite_renderer.h>

Text::Text() :
	font_(nullptr)
{
}
Text::~Text()
{
}


// Initialise method
bool Text::Init(gef::Vector4 position, float scale, UInt32 colour, gef::TextJustification justification, gef::Font* font, const char* message) {

	// Set all text attributes
	position_ = position;
	scale_ = scale;
	colour_ = colour;
	justification_ = justification;
	message_ = message;

	font_ = font;

	return true;
}

// Updates message
bool Text::SetMessage(const char* message) {

	message_ = message;

	return true;
}


// Draw text
bool Text::Draw(gef::SpriteRenderer* sprite_renderer, ...) {

	va_list args;

	va_start(args, sprite_renderer);

	font_->RenderText(sprite_renderer, position_, scale_, colour_, justification_, message_, args);

	va_end(args);

	return true;
}


