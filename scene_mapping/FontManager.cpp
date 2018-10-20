#include "FontManager.h"

#include <graphics\font.h>


FontManager::FontManager()
{
}
FontManager::~FontManager()
{
}


// set singleton to null
FontManager* FontManager::font_manager_ = nullptr;


// Initialise
bool FontManager::Init() {

	// Create singleton
	if (!font_manager_) {

		font_manager_ = this;
	}
	else if (font_manager_ != this) {

		return false;
	}

	return true;
}


// Deletes resources used
bool FontManager::CleanUp() {


	ClearFonts();

	font_manager_ = nullptr;

	return true;
}


// loads font from file and gives it a name
bool FontManager::LoadFont(gef::Platform& platform, FONTS name, const char* file) {


	gef::Font* newFont = new gef::Font(platform);
	if (!newFont->Load(file)) {

		return false;
	}

	// Add new font to font list
	fonts_.insert(std::make_pair(name, newFont));
	return true;
}


// returns a font from name passed
gef::Font* FontManager::GetFont(FONTS fontName) {

	// Check if font exists
	auto iterator = fonts_.find(fontName);

	if (iterator != fonts_.end()) {

		// if font exists return font
		return iterator->second;
	}


	// no font found return nullptr
	return nullptr;
}


// Removes font from loaded fonts
bool FontManager::RemoveFont(FONTS fontName) {

	// Check if font exists
	auto iterator = fonts_.find(fontName);

	if (iterator != fonts_.end()) {

		// If font exists delete and remove from font list
		if (iterator->second) {

			delete iterator->second;
			iterator->second = nullptr;
		}

		fonts_.erase(iterator);
	}

	return true;
}



bool FontManager::ClearFonts() {

	// Loop for each font stored and delete it
	auto iterator = fonts_.begin();

	while (iterator != fonts_.end()) {

		if (iterator->second) {

			delete iterator->second;
			iterator->second = nullptr;
		}

		iterator++;
	}

	fonts_.clear();

	return true;
}

