#pragma once

#include <MY_Palette.h>
#include <MY_ResourceManager.h>

MY_Palette::MY_Palette(BulletWorld * _world, Shader * _shader){
	std::vector<TriMesh *> & meshes = MY_ResourceManager::globalAssets->getMesh("palette")->meshes;
	Texture * tex = MY_ResourceManager::globalAssets->getTexture("palette")->texture;

	for(auto m : meshes){
		m->pushTexture2D(tex);
	}

	base = new MeshEntity(meshes.at(0), _shader);

	childTransform->addChild(base);

	for(unsigned long int i = 1; i < meshes.size(); ++i){
		MY_SelectionTarget * colour = new MY_SelectionTarget(_world, meshes.at(i), _shader);
		colour->setColliderAsBoundingBox();
		colour->createRigidBody(0);
		colour->name = "test_" + std::to_string(i);
		childTransform->addChild(colour);
		options.push_back(colour);
	}
}

void MY_Palette::translateComponents(glm::vec3 _t, bool _relative){
	base->firstParent()->translate(_t, _relative);
	for(auto o : options){
		o->translatePhysical(_t, _relative);
	}
}