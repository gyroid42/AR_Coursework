#include "MeshManager.h"

#include <graphics\mesh.h>
#include <graphics\scene.h>
#include "primitive_builder.h"

MeshManager::MeshManager()
{
}
MeshManager::~MeshManager()
{
}


// Set singleton to nullptr
MeshManager* MeshManager::mesh_manager_ = nullptr;



// Init and CleanUp ///////////

bool MeshManager::Init(gef::Platform& platform) {


	// Create Singleton
	if (!mesh_manager_) {

		mesh_manager_ = this;
	}
	else if (mesh_manager_ != this) {

		return false;
	}

	return true;
}


bool MeshManager::CleanUp() {


	ClearMeshes();

	ClearScenes();


	// set singleton to null
	mesh_manager_ = nullptr;

	return true;
}


bool MeshManager::ClearScenes() {

	for (auto& iterator : scenes_) {

		if (iterator.second) {

			delete iterator.second;
			iterator.second = nullptr;
		}
	}

	scenes_.clear();

	return true;
}


bool MeshManager::ClearMeshes() {

	for (auto& iterator : meshes_) {

		if (iterator.second) {

			delete iterator.second;
			iterator.second = nullptr;
		}
	}

	meshes_.clear();

	return true;
}



// Loads first Mesh from Scene
bool MeshManager::LoadMesh(gef::Platform& platform, MESHES meshName, SCENES sceneName, gef::Sphere* newBoundingSphere) {


	// Get Scene from scene list
	gef::Scene* scene = GetScene(sceneName);

	if (scene) {
		if (LoadMesh(platform, meshName, scene, newBoundingSphere)) {

			return true;
		}
	}

	return false;
}

bool MeshManager::LoadMesh(gef::Platform& platform, MESHES meshName, gef::Scene* scene, gef::Sphere* newBoundingSphere) {

	if (!scene) {

		return false;
	}

	gef::Mesh* newMesh = GetFirstMesh(platform, scene);

	if (newBoundingSphere) {

		newMesh->set_bounding_sphere(*newBoundingSphere);
	}

	meshes_.insert(std::make_pair(meshName, newMesh));

	return true;
}



// Adds mesh to loaded mesh list

bool MeshManager::AddMesh(gef::Mesh* newMesh, MESHES meshName, gef::Sphere* newBoundingSphere) {

	if (newBoundingSphere) {

		newMesh->set_bounding_sphere(*newBoundingSphere);
	}

	meshes_.insert(std::make_pair(meshName, newMesh));

	return true;
}



// Returns mesh of meshName
gef::Mesh* MeshManager::GetMesh(MESHES meshName) {

	// Find mesh and return mesh
	auto iterator = meshes_.find(meshName);

	if (iterator != meshes_.end()) {

		return iterator->second;
	}

	// No mesh found so return null
	return nullptr;
}


// Unloads mesh from application
bool MeshManager::RemoveMesh(MESHES meshName) {

	auto iterator = meshes_.find(meshName);

	if (iterator != meshes_.end()) {

		if (iterator->second) {

			delete iterator->second;
			iterator->second = nullptr;
		}

		meshes_.erase(iterator);
	}

	return true;
}


// Returns first mesh in a scene
gef::Mesh* MeshManager::GetFirstMesh(gef::Platform& platform, gef::Scene* scene) {

	gef::Mesh* mesh = nullptr;

	if (scene) {

		if (scene->mesh_data.size() > 0) {

			mesh = scene->CreateMesh(platform, scene->mesh_data.front());
		}
	}

	return mesh;
}




// Loads scene from file
bool MeshManager::LoadScene(gef::Platform& platform, SCENES sceneName, const char* file) {

	gef::Scene* newScene = new gef::Scene();

	if (!newScene->ReadSceneFromFile(platform, file)) {

		return false;
	}


	newScene->CreateMaterials(platform);

	scenes_.insert(std::make_pair(sceneName, newScene));

	return true;

}


// Unloads scene from manager
bool MeshManager::RemoveScene(SCENES sceneName) {

	auto iterator = scenes_.find(sceneName);

	if (iterator != scenes_.end()) {

		if (iterator->second) {

			delete iterator->second;
			iterator->second = nullptr;
		}

		scenes_.erase(iterator);
	}

	return true;
}



// Returns scene with name 'sceneName'
gef::Scene* MeshManager::GetScene(SCENES sceneName) {

	auto iterator = scenes_.find(sceneName);

	if (iterator != scenes_.end()) {

		return iterator->second;
	}

	return nullptr;
}
