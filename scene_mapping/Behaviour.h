#pragma once

#ifndef _BEHAVIOUR_H
#define _BEHAVIOUR_H



// forward declarations
class GameObject;


namespace gef {
	class Renderer3D;
	class SpriteRenderer;
}

enum BEHAVIOUR_NAME {
	BASE_BEHAVIOUR,
	FOLLOW_MARKER_BEHAVIOUR,
	GAME_CONTROLLER_BEHAVIOUR,
	CAMERA_FEED_BEHAVIOUR,
	PLAYER_MOVEMENT_BEHAVIOUR
};


// base behaviour class
// used to give behaviours to game objects
class Behaviour
{
public:
	Behaviour();
	virtual ~Behaviour();


	// Initialise methods called when behaviour is created
	// init called first assigns gameobject to behaviour and sets its behaviour name
	virtual bool Init(GameObject* newGameObject);  //

	// CleanUp method called to destroy resources used
	virtual bool CleanUp();

	// Awake called before start
	virtual bool Awake();  //

	// Start called last
	virtual bool Start();  //

	///////////////////////////////

	// Update methods called each frame

	// Update is the main method used each frame
	virtual bool Update(float dt);  //

	// Late Update always called before Update
	virtual bool EarlyUpdate(float dt);  //

	// Called straight after mesh transform's creation
	virtual bool OnUpdateMesh();  //

	////////////////////////////////

	// Render Methods called each frame

	virtual bool PreRenderGUI(gef::SpriteRenderer* sprite_renderer);

	// PreRender called before 3D render of gameobject
	virtual bool PreRender(gef::Renderer3D* renderer_3d); //

	// OnRender called during 3D render of gameobject
	virtual bool OnRender(gef::Renderer3D* renderer_3d); //

	// PostRender called after 3d render of gameobject
	virtual bool PostRender(gef::Renderer3D* renderer_3d); //

	// called when GUI is being drawn
	virtual bool OnGUI(gef::SpriteRenderer* sprire_renderer);  //


	////////////////////////////////////////

	// Collision methods ////

	// methods used when dealing with collision with other game objects
	// "other" references object being collided with

	// OnEnterCollision called on frame collision begins
	virtual bool OnEnterCollision(GameObject* other); //

	// OnCollision called each frame collision occurs with object
	virtual bool OnCollision(GameObject* other);  //

	// OnExitCOllision called on frame collision ends
	virtual bool OnExitCollision(GameObject* other); //

	////////////////////////////////////

	// OnEnable/OnDisable called on frame when behaviour becomes enabled or disabled respectively
	virtual bool OnEnable();
	virtual bool OnDisable();


	// OnDestroy called when object is destroyed
	virtual bool OnDestroy();  //

	// OnApplicationQuit called when the application is exitted (Probs never gonna use this one)
	virtual bool OnApplicationQuit();



	inline BEHAVIOUR_NAME Name() { return behaviour_name_; }
	inline bool IsEnabled() { return enabled_; }
	inline bool EnableChangingTo() { return enabledChangeTo_; }
	bool IsEnableChanging();
	inline void SetEnable(bool newValue) { enabledChangeTo_ = newValue; }

protected:

	// Name of behaviour, used to find on object so other behaviours can find it
	BEHAVIOUR_NAME behaviour_name_;

	// Reference to game object behaviour is attached to
	GameObject* gameobject_;

	// bool to tell whether behaviour is currently enabled
	bool enabled_;
	bool enabledChangeTo_;
};


#endif