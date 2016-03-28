#pragma once

#include <MY_Scene_Menu.h>
#include <shader/ComponentShaderText.h>
#include <sweet/UI.h>

#include <MY_Scene_Main.h>
#include <MY_Scene_Path.h>

MY_Scene_Menu::MY_Scene_Menu(Game * _game) :
	MY_Scene_Base(_game)
{
	// Create a linear layout to contain all of our menu items
	// remember that these elements are all going to exist in the uiLayer, so we pass in its physics world in the constructors
	VerticalLinearLayout * layout = new VerticalLinearLayout(uiLayer->world);
	layout->horizontalAlignment = kCENTER;
	layout->verticalAlignment = kMIDDLE;
	// set the layout's size to 100% of its parent
	layout->setRationalHeight(1.f, uiLayer);
	layout->setRationalWidth(1.f, uiLayer);

	// Create some text labels
	TextLabel * label6 = new TextLabel(uiLayer->world, font, textShader);
	TextLabel * label7 = new TextLabel(uiLayer->world, font, textShader);
	TextLabel * label8 = new TextLabel(uiLayer->world, font, textShader);

	// set the text on the labels
	label6->setText("Quit Game");
	label7->setText("Play Game");
	label8->setText("Path Thing");

	// make the labels' background visible (by default both the scene's clear colour and the text colour will be black)
	label6->setBackgroundColour(1,1,1,1);
	label7->setBackgroundColour(1,1,1,1);
	label8->setBackgroundColour(1,1,1,1);

	// make the labels clickable
	label6->setMouseEnabled(true);
	label7->setMouseEnabled(true);
	label8->setMouseEnabled(true);

	// add listeners to each label, making them buttons that take the player to different scenes
	label6->eventManager->addEventListener("click", [&](sweet::Event * _event){
		game->exit();
	});
	label7->eventManager->addEventListener("click", [&](sweet::Event * _event){
		if(game->scenes.count("main") == 0){
			game->scenes["main"] = new MY_Scene_Main(game);
		}
		game->switchScene("main", false);
	});
	label8->eventManager->addEventListener("click", [&](sweet::Event * _event){
		if(game->scenes.count("path") == 0){
			game->scenes["path"] = new MY_Scene_Path(game);
		}
		game->switchScene("path", false);
	});


	// add the labels to the layout
	layout->addChild(label6);
	layout->addChild(label7);
	layout->addChild(label8);

	// add the layout to the uiLayer
	uiLayer->addChild(layout);

	uiLayer->invalidateLayout();

	// add a mouse indicator (AKA a cursor) to the uiLayer so that the user can see what they're doing
	uiLayer->addMouseIndicator();
}