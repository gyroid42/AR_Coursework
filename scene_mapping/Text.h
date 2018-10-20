#pragma once


#ifndef _TEXT_H
#define _TEXT_H

#include <stdarg.h>
#include <system\platform.h>
#include <maths\vector4.h>
#include <graphics\font.h>



// Forward declaration
namespace gef {
	class Platform;
	class SpriteRenderer;
}




// Text Class
// Used to contain data for displaying text
class Text
{
public:
	Text();
	~Text();

	// Initialise method, creates all the information for drawing text
	bool Init(gef::Vector4 position, float scale, UInt32 colour, gef::TextJustification justification, gef::Font* font, const char* message);


	// Method used to draw text
	bool Draw(gef::SpriteRenderer* sprite_renderer, ...);


	// Getters //
	inline gef::Vector4 Position() { return position_; }
	inline float Scale() { return scale_; }
	inline UInt32 Colour() { return colour_; }
	inline gef::TextJustification Justification() { return justification_; }
	inline gef::Font* Font() { return font_; }


	// Setters //
	bool SetMessage(const char* message);

	inline void SetPosition(gef::Vector4 newPosition) { position_ = newPosition; }
	inline void SetScale(float newScale) { scale_ = newScale; }
	inline void SetColour(UInt32 newColour) { colour_ = newColour; }
	inline void SetJustification(gef::TextJustification newJustification) { justification_ = newJustification; }
	inline void SetFont(gef::Font* newFont) { font_ = newFont; }


protected:


	// Attributes of Text //

	// Position of text on screen
	gef::Vector4 position_;

	// Scale of text
	float scale_;

	// COlour of text
	UInt32 colour_;

	// Justification of text i.e. left-aligned, centre-aligned
	gef::TextJustification justification_;

	// Message the text displays
	const char* message_;

	// Font text uses
	gef::Font* font_;

};

#endif