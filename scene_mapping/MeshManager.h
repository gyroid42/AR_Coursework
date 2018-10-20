#pragma once

#ifndef _MESH_MANAGER_H
#define _MESH_MANAGER_H

#include <map>

#include <system\platform.h>


// Forward declarations
class PrimitiveBuilder;

namespace gef {
	class Mesh;
	class Scene;
	class Sphere;
}


// enum for names of meshes
enum MESHES {

	BALL1,
	BALL2
};


// enum for names of scenes
enum SCENES {

	BALL1_SCENE,
	BALL2_SCENE
};




// Mesh Manager
// Loads/Unloads meshes and scnees to/from application
class MeshManager
{
public:
	MeshManager();
	~MeshManager();

	// Initialise and CleanUp methods
	bool Init(gef::Platform& platform);
	bool CleanUp();

	/////////////////////

	// Loading Methods /////////

	// Loads a scene from a .scn file
	bool LoadScene(gef::Platform& platform, SCENES sceneName, const char* file);

	// Loads a mesh from a scene
	bool LoadMesh(gef::Platform& platform, MESHES meshName, SCENES sceneName, gef::Sphere* newBoundingSphere = nullptr);
	bool LoadMesh(gef::Platform& platform, MESHES meshName, gef::Scene* scene, gef::Sphere* newBoundingSphere = nullptr);
	
	// Adds mesh to loaded meshes
	bool AddMesh(gef::Mesh* mesh, MESHES meshName, gef::Sphere* newBoundingSphere = nullptr);

	// Unloads and Removes scenes/meshes from manager
	bool RemoveScene(SCENES sceneName);
	bool RemoveMesh(MESHES meshName);

	bool ClearScenes();
	bool ClearMeshes();

	////////////////////////////////////

	// Getters //

	gef::Scene* GetScene(SCENES sceneName);
	gef::Mesh* GetMesh(MESHES meshName);


	/////////////////////////////////////


	// static reference to mesh manager
	inline static MeshManager* GetMeshManager() { return mesh_manager_; }

private:


	// Used to access mesh manager in rest of application
	static MeshManager* mesh_manager_;


	// maps containing all loaded scenes and meshes
	std::map<SCENES, gef::Scene*> scenes_;
	std::map<MESHES, gef::Mesh*> meshes_;


	// Gets the first mesh in a scene file
	gef::Mesh* GetFirstMesh(gef::Platform& platform, gef::Scene* scene);

};

#endif