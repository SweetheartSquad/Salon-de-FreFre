#pragma once

#include <MY_Palette.h>
#include <MY_ResourceManager.h>
#include <MY_Palette_Definition.h>

MY_Palette::MY_Palette(BulletWorld * _world, Shader * _shader):
	name("")
{
	std::vector<TriMesh *> & meshes = MY_ResourceManager::globalAssets->getMesh("palette")->meshes;

	base = new MeshEntity(meshes.at(0), _shader);

	childTransform->addChild(base);

	for(unsigned long int i = 1; i < meshes.size(); ++i){
		MY_SelectionTarget * colour = new MY_SelectionTarget(_world, meshes.at(i), _shader);
		colour->setColliderAsBoundingBox();
		colour->createRigidBody(0);
		childTransform->addChild(colour);
		options.push_back(colour);
	}
}

void MY_Palette::loadDefinition(MY_Palette_Definition * _def){
	name = _def->name;

	Texture * tex = _def->texture;
	base->mesh->replaceTextures(tex);

	for(unsigned long int i = 0; i < NUM_OPTIONS; ++i){
		MY_SelectionTarget * o = options.at(i);
		o->mesh->replaceTextures(tex);
		o->name = _def->options.at(i)->name;
		o->texture = _def->options.at(i)->texture;
	}
}

void MY_Palette::translateComponents(glm::vec3 _t, bool _relative){
	base->firstParent()->translate(_t, _relative);
	for(auto o : options){
		o->translatePhysical(_t, _relative);
	}
}