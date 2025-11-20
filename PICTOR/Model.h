/* Copyright (c) 2024 Lilian Buzer - All rights reserved - */
#pragma once

#include "Color.h"
#include "ObjGeom.h"
#include "V2.h"
#include "ObjAttr.h"
#include <vector>
#include <string>
#include <memory>
using namespace std;

// declaration used to avoid cyclic dependency
class Tool; 
class Button;
class Model;
void initApp(Model& Data);


class Model
{
  public :

	shared_ptr<Tool> currentTool;

	V2 currentMousePos;

	ObjAttr drawingOptions;

	int currentBorderColorIndex = 0;
	int currentInteriorColorIndex = 0;
	int currentThicknessIndex = 0;
	bool isFilled = true;

	shared_ptr<ObjGeom> selectedObject = nullptr;

	vector< shared_ptr<ObjGeom> > LObjets;

	vector< shared_ptr<Button> > LButtons;

	vector<string> undoHistory;

	Model()
	{
		initApp(*this);
	}
};
