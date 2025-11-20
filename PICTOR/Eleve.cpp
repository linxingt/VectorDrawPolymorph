/* Copyright (c) 2024 Lilian Buzer - All rights reserved - */

#include <string>
#include "V2.h"
#include "Graphics.h"
#include "Event.h" 
#include "Model.h"
#include "Button.h"
#include "Tool.h"
#include <algorithm> // for remove_if

using namespace std;

// ESC : close window

const string SAVE_FILE = "scene_data.txt";
shared_ptr<Tool> previousTool = nullptr;

void updateDrawingOptions(Model& Data) {
	Data.drawingOptions.borderColor_ = COLOR_CHOICES[Data.currentBorderColorIndex];
	Data.drawingOptions.interiorColor_ = COLOR_CHOICES[Data.currentInteriorColorIndex];
	Data.drawingOptions.thickness_ = THICKNESS_CHOICES[Data.currentThicknessIndex];
	Data.drawingOptions.isFilled_ = Data.isFilled;
}

void deserializeScene(Model& Data, const string& serializedState)
{
	Data.LObjets.clear();
	Data.selectedObject = nullptr;

	stringstream ss(serializedState);
	string line;

	while (getline(ss, line))
	{
		if (line.empty()) continue;

		stringstream line_ss(line); 
		string item;

		getline(line_ss, item, ',');
		string type = item;

		float b_r, b_g, b_b; // Border RGB
		float i_r, i_g, i_b; // Interior RGB
		getline(line_ss, item, ','); b_r = stof(item);
		getline(line_ss, item, ','); b_g = stof(item);
		getline(line_ss, item, ','); b_b = stof(item);
		getline(line_ss, item, ','); i_r = stof(item);
		getline(line_ss, item, ','); i_g = stof(item);
		getline(line_ss, item, ','); i_b = stof(item);

		getline(line_ss, item, ',');
		bool isFilled = (stoi(item) == 1);
		getline(line_ss, item, ',');
		int thickness = stoi(item);

		ObjAttr attr(Color(b_r, b_g, b_b), isFilled, Color(i_r, i_g, i_b), thickness);
		shared_ptr<ObjGeom> newObj;

		if (type == "ObjRectangle" || type == "ObjSegment" || type == "ObjCircle")
		{
			V2 P1, P2;
			getline(line_ss, item, ','); P1.x = stof(item);
			getline(line_ss, item, ','); P1.y = stof(item);
			getline(line_ss, item, ','); P2.x = stof(item);
			getline(line_ss, item, '\n'); P2.y = stof(item);

			if (type == "ObjRectangle")
				newObj = make_shared<ObjRectangle>(attr, P1, P2);
			else if (type == "ObjSegment")
				newObj = make_shared<ObjSegment>(attr, P1, P2);
			else if (type == "ObjCircle")
				newObj = make_shared<ObjCircle>(attr, P1, P2);
		}
		else if (type == "ObjSegPolygone")
		{
			vector<V2> Points;
			getline(line_ss, item, ',');
			int size = stoi(item);

			for (int i = 0; i < size; ++i) {
				V2 P;
				getline(line_ss, item, ','); P.x = stof(item);
				if (i == size - 1) {
					getline(line_ss, item, '\n');
				}
				else {
					getline(line_ss, item, ',');
				}
				P.y = stof(item);
				Points.push_back(P);
			}
			newObj = make_shared<ObjSegPolygone>(attr, Points);
		}

		if (newObj) {
			Data.LObjets.push_back(newObj);
		}

		else {
			cout << "Type d'objet inconnu lors du chargement : " << type << endl;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//		setup screen

int main(int argc, char* argv[])
{
	cout << "Press ESC to abort" << endl;
	Graphics::initMainWindow("Pictor", V2(1200, 800), V2(200, 200));
}

void bntToolSegmentClick(Model& Data)   { Data.currentTool = make_shared<ToolSegment>(); }
void bntToolRectangleClick(Model& Data) { Data.currentTool = make_shared<ToolRectangle>(); }
void bntToolCircleClick(Model& Data) { Data.currentTool = make_shared<ToolCircle>(); }
void bntRAZClick(Model& Data) { 
	if (Data.selectedObject) {
		auto& L = Data.LObjets;
	// remove_if: ne supprime pas reellement d'elements du vecteur. 
	// Il deplace tous les elements qui ne repondent pas a la condition au debut du vecteur.
		L.erase(remove_if(L.begin(), L.end(),
			[&](const auto& obj) { return obj == Data.selectedObject; }),
			L.end());
		Data.selectedObject = nullptr;
	}else Data.LObjets.clear();
	saveSceneState(Data);
}
void bntUpClick(Model& Data) {
	if (Data.selectedObject) {
		auto& L = Data.LObjets;
		auto it = find(L.begin(), L.end(), Data.selectedObject);

		if (it != L.end() && next(it) != L.end()) {
			iter_swap(it, next(it));
			saveSceneState(Data);
		}
	}
}
void bntDownClick(Model& Data) {
	if (Data.selectedObject) {
		auto& L = Data.LObjets;
		auto it = find(L.begin(), L.end(), Data.selectedObject);

		if (it != L.begin() && it != L.end()) {
			iter_swap(it, prev(it));
			saveSceneState(Data);
		}
	}
}
void bntBorderColorClick(Model& Data) {
	Data.currentBorderColorIndex = (Data.currentBorderColorIndex + 1) % COLOR_CHOICES.size();
	updateDrawingOptions(Data);
}
void bntInteriorColorClick(Model& Data) {
	Data.currentInteriorColorIndex = (Data.currentInteriorColorIndex + 1) % COLOR_CHOICES.size();
	updateDrawingOptions(Data);
}
void bntThicknessClick(Model& Data) {
	Data.currentThicknessIndex = (Data.currentThicknessIndex + 1) % THICKNESS_CHOICES.size();
	updateDrawingOptions(Data);
}
void bntFilledClick(Model& Data) {
	Data.isFilled = !Data.isFilled;
	updateDrawingOptions(Data);
}
void bntToolSegPolygoneClick(Model& Data) { Data.currentTool = make_shared<ToolSegPolygone>(); }
void btnSaveClick(Model& Data)
{
	ofstream file(SAVE_FILE);

	if (file.is_open())
	{
		file << serializeScene(Data);
		file.close();
		cout << "Scene saved to " << SAVE_FILE << endl;
	}
	else
	{
		cout << "Unable to open file " << SAVE_FILE << " for saving." << endl;
	}
}
void btnLoadClick(Model& Data)
{
	ifstream file(SAVE_FILE);

	if (file.is_open())
	{
		// Lire le contenu complet du fichier dans une seule chaine

		// string prend (ite, ite) pour definir la plage de caracteres a copier dans la nouvelle chaine
		string content((istreambuf_iterator<char>(file)),
		// pointe vers le tout premier caractere disponible dans le fichier associe 
			istreambuf_iterator<char>());
		// iterateur par defaut, cree sans argument, qui represente la position apres le dernier caractere du flux.

		file.close();

		deserializeScene(Data, content);

		saveSceneState(Data);

		cout << "Scene loaded from " << SAVE_FILE << endl;
	}
	else
	{
		cout << "Unable to open file " << SAVE_FILE << " for loading." << endl;
	}
}
void btnUndoClick(Model& Data)
{
	if (Data.undoHistory.size() > 1)
	{
		Data.undoHistory.pop_back();

		const string& previousState = Data.undoHistory.back();

		deserializeScene(Data, previousState);

		cout << "Action annulee." << endl;
	}
}
void bntToolEditPointsClick(Model& Data) 
{
	if (dynamic_pointer_cast<ToolEditPoints>(Data.currentTool))
	{
		Data.currentTool = previousTool ? previousTool : make_shared<ToolSelect>();
		previousTool = nullptr;
		cout << "Mode Edition des Points DESACTIVE." << endl;
	}
	else
	{
		previousTool = Data.currentTool;
		Data.currentTool = make_shared<ToolEditPoints>();
		cout << "Mode Edition des Points ACTIVE." << endl;
	}
}

void initApp(Model& App)
{
	// choose default tool
	App.currentTool = make_shared<ToolSelect>();

	// create buttons
	int x = 0;
	int s = 80; // size

	// button Segment
	auto B1 = make_shared<Button>("Outil Segment", V2(x, 0), V2(s, s), "outil_segment.png", bntToolSegmentClick);
	App.LButtons.push_back(B1);
	x += s;

	// button Rectangle
	auto B2 = make_shared<Button>("Outil Rectangle",V2(x, 0), V2(s, s), "outil_rectangle.png", bntToolRectangleClick);
	App.LButtons.push_back(B2);
	x += s;

	// button Circle
	auto B3 = make_shared<Button>("Outil Circle", V2(x, 0), V2(s, s), "outil_ellipse.png", bntToolCircleClick);
	App.LButtons.push_back(B3);
	x += s;

	// button RAZ
	auto B4 = make_shared<Button>("Delete", V2(x, 0), V2(s, s), "outil_delete.png", bntRAZClick);
	App.LButtons.push_back(B4);
	x += s;

	// button Up
	auto B5 = make_shared<Button>("Up", V2(x, 0), V2(s, s), "outil_up.png", bntUpClick);
	App.LButtons.push_back(B5);
	x += s;

	// button Down
	auto B6 = make_shared<Button>("Down", V2(x, 0), V2(s, s), "outil_down.png", bntDownClick);
	App.LButtons.push_back(B6);
	x += s;

	// button BorderColor
	auto B7 = make_shared<Button>("BorderColor", V2(x, 0), V2(s, s), "", bntBorderColorClick, COLOR_LINE);
	App.LButtons.push_back(B7);
	x += s;

	// button InteriorColor
	auto B8 = make_shared<Button>("InteriorColor", V2(x, 0), V2(s, s), "", bntInteriorColorClick, COLOR_FILL);
	App.LButtons.push_back(B8);
	x += s;

	// button Thickness
	auto B9 = make_shared<Button>("Thickness", V2(x, 0), V2(s, s), "", bntThicknessClick, THICKNESS_LINE);
	App.LButtons.push_back(B9);
	x += s;

	// button Filled
	auto B10 = make_shared<Button>("Filled", V2(x, 0), V2(s, s), "", bntFilledClick, TOGGLE_FILL);
	App.LButtons.push_back(B10);
	x += s;

	// button SegPolygone
	auto B11 = make_shared<Button>("Outil SegPolygone", V2(x, 0), V2(s, s), "outil_polygone.png", bntToolSegPolygoneClick);
	App.LButtons.push_back(B11);
	x += s;

	// button Save
	auto B12 = make_shared<Button>("Save", V2(x, 0), V2(s, s), "outil_save.png", btnSaveClick);
	App.LButtons.push_back(B12);
	x += s;

	// button Load
	auto B13 = make_shared<Button>("Load", V2(x, 0), V2(s, s), "outil_load.png", btnLoadClick);
	App.LButtons.push_back(B13);
	x += s;

	// button Undo
	auto B14 = make_shared<Button>("Undo", V2(x, 0), V2(s, s), "outil_undo.png", btnUndoClick);
	App.LButtons.push_back(B14);
	x += s;

	// button bntEditPointsClick
	auto B15 = make_shared<Button>("Outil EditPointsClick", V2(x, 0), V2(s, s), "outil_corner.png", bntToolEditPointsClick);
	App.LButtons.push_back(B15);
	x += s;

	// put two objets in the scene

	ObjAttr DrawOpt1 = ObjAttr(Color::Cyan, true, Color::Green, 6);
	auto newObj1 = make_shared<ObjRectangle>(DrawOpt1, V2(100, 100), V2(300, 200));
	App.LObjets.push_back(newObj1);

	ObjAttr DrawOpt2 = ObjAttr(Color::Red, true, Color::Blue, 5);
	auto newObj2 = make_shared<ObjRectangle>(DrawOpt2, V2(500, 300), V2(600, 600));
	App.LObjets.push_back(newObj2);

	ObjAttr DrawOpt3 = ObjAttr(Color::Blue, true, Color::Magenta, 4);
	auto newObj3 = make_shared<ObjCircle>(DrawOpt3, V2(300, 500), V2(400, 400));
	App.LObjets.push_back(newObj3);

	saveSceneState(App);
}

/////////////////////////////////////////////////////////////////////////
//
//		Event management

 
void processEvent(const Event& Ev, Model & Data)
{
	Ev.print(); // Debug

	// MouseMove event updates x,y coordinates
	if (Ev.Type == EventType::MouseMove ) Data.currentMousePos = V2(Ev.x, Ev.y);
	 

	// detect a mouse click on the tools icons

	V2 P = Data.currentMousePos;
	for (auto B : Data.LButtons)
		if (Ev.Type == EventType::MouseDown && P.isInside(B->getPos(),B->getSize()) )
		{
			B->manageEvent(Ev,Data);
			return;
		}


	// send event to the activated tool
	Data.currentTool->processEvent(Ev,Data);
	
	 
}
 
/////////////////////////////////////////////////////////////////////////
//
//     Drawing elements
 
 
void drawCursor(Graphics& G, const Model& D)
{
 

	V2 P = D.currentMousePos;
	int r = 7;
	
	Color c = Color::Black;
	G.drawLine(P + V2(r, 1), P + V2(-r, 1), c);
	G.drawLine(P + V2(r,-1), P + V2(-r,-1), c);
	G.drawLine(P + V2( 1,-r), P + V2( 1, r), c);
	G.drawLine(P + V2(-1,-r), P + V2(-1, r), c);

	Color cc = Color::White;
	G.drawLine(P - V2(r, 0), P + V2(r, 0), cc);
	G.drawLine(P - V2(0, r), P + V2(0, r), cc);

	G.drawStringFontMono(P + V2(20, 0), "Hello", 20, 1, Color::Yellow);
}
 

void drawApp(Graphics& G, const Model & D)
{
	// reset with a black background
	G.clearWindow(Color::Black);

	// draw all geometric objects
	for (auto& Obj : D.LObjets)
	{
		bool isSelected = (D.selectedObject == Obj);
		Obj->draw(G, isSelected);
	}

	// draw the app menu
	for (auto& myButton : D.LButtons)
		myButton->draw(G, D);

	// draw current tool and interface (if active)
	D.currentTool->draw(G, D);

	// draw cursor
	drawCursor(G, D);
}

 




