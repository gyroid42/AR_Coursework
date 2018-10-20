#ifndef _RENDER_TARGET_APP_H
#define _RENDER_TARGET_APP_H

#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include <platform/vita/graphics/texture_vita.h>


//#include "Text.h"

// Vita AR includes
#include <camera.h>
#include <gxm.h>
#include <motion.h>
#include <libdbg.h>
#include <libsmart.h>
#include <smart/scene_mapping.h>



class Camera;
class MeshManager;
//class GameObject;
class GameObjectManager;
class FontManager;
//class CameraFeed;
class PrimitiveBuilder;
class PrimitiveRenderer;
class Input;
class LevelManager;


// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	//class Font;
	class Renderer3D;
	//class Mesh;
	class RenderTarget;
	class TextureVita;
	//class InputManager;
}


class ARApp : public gef::Application
{
public:
	ARApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();

	void DrawSceneMappingDebugLines();

private:
	//void InitFont();
	//void CleanUpFont();
	void DrawHUD();

	void RenderOverlay();
	void SetupLights();

	void DrawAxes(PrimitiveRenderer* primitive_renderer, const gef::Matrix44& transform, const float size);
	void DrawLandmark(PrimitiveRenderer* primitive_renderer, int landmark_num);

	//gef::InputManager* input_manager_;
	Input* input_;
	FontManager* font_manager_;
	GameObjectManager* gameobject_manager_;
	MeshManager* mesh_manager_;
	LevelManager* level_manager_;

	gef::SpriteRenderer* sprite_renderer_;

	//float fps_;
	//Text fps_text_;
	//Text marker_detected_text_;
	//Text scene_map_init_mode_;
	//Text scene_map_state_;

	class gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* primitive_builder_;
	PrimitiveRenderer* primitive_renderer_;



	Camera* camera_;
	//CameraFeed* camera_feed_;



};




#endif // _RENDER_TARGET_APP_H