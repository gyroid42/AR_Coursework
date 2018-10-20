#pragma once


#ifndef _FONT_MANAGER_H
#define _FONT_MANAGER_H


#include <map>
#include <vector>


// Forward declarations
namespace gef {
	class Font;
	class Platform;
}


// Enum for names of fonts
enum FONTS {
	COMIC_SANS
};



// Font Manager class
// Loads and Unloads fonts for use in application
class FontManager
{
public:
	FontManager();
	~FontManager();

	// Init method called when font manager is created
	bool Init();

	// CleanUp called when font manager is deleted
	bool CleanUp();


	// Loads a font
	bool LoadFont(gef::Platform& platform, FONTS name, const char* file);

	// Gets a font from loaded fonts
	gef::Font* GetFont(FONTS fontName);

	// Unloads and removes font
	bool RemoveFont(FONTS fontName);
	bool ClearFonts();


	// static referencde to font manager
	inline static FontManager* GetFontManager() { return font_manager_; }

private:

	// Used to access font manager in rest of application
	static FontManager* font_manager_;


	// Map used to store loaded fonts
	std::map<FONTS, gef::Font*> fonts_;	
};

#endif