/* Copyright (c) 2024 Lilian Buzer - All rights reserved - */
#pragma once

#include <string>
#include "Event.h"
#include "Model.h"
#include "ObjGeom.h"

using namespace std;

enum State { WAIT, INTERACT  };

string serializeScene(const Model& Data)
{
	stringstream ss;

	for (const auto& obj : Data.LObjets)
	{
		obj->serialize(ss);
	}

	return ss.str();
}

/////////////////////////////////////////////////////////////////////////
//
//		Save Scene pour Undo

void saveSceneState(Model& Data)
{
	Data.undoHistory.push_back(serializeScene(Data));
}


////////////////////////////////////////////////////////////////////

class Tool
{
protected :
	int currentState;

public:
	Tool() {   currentState = State::WAIT; }
	virtual void processEvent(const Event& E, Model & Data) {}

	virtual void draw(Graphics& G,const  Model& Data) {}
};

////////////////////////////////////////////////////////////////////


class ToolSelect : public Tool
{
public:

	ToolSelect() : Tool() {}
	void processEvent(const Event& E, Model& Data) override
	{
		if (E.Type == EventType::MouseDown && E.info == "0") // left mouse button pressed
		{


			V2 clickPos = Data.currentMousePos;
			// parcourt a l'envers pour selectionner l'objet "front"
			for (auto it = Data.LObjets.rbegin(); it != Data.LObjets.rend(); ++it)
			{
				auto& obj = *it;
				Hitbox box = obj->getHitbox();
				if (clickPos.isInside(box.P, box.size))
				{
					if (Data.selectedObject == obj)
					{
						Data.selectedObject = nullptr;
					}
					else
					{
						Data.selectedObject = obj;
					}
					return;
				}
			}
			Data.selectedObject = nullptr;
		}
	}

	void draw(Graphics& G, const Model& Data) override
	{
		if (Data.selectedObject)
		{
			Hitbox box = Data.selectedObject->getHitbox();
			G.drawRectangle(box.P, box.size, Color::Magenta, false, Data.drawingOptions.thickness_ + 3);
		}
	}
};

////////////////////////////////////////////////////////////////////


class ToolEditPoints : public Tool
{
private:

	shared_ptr<ObjGeom> currentMovingObject = nullptr;
	int pointIndexToMove = -1;

public:

	ToolEditPoints() : Tool() {}
	void processEvent(const Event& E, Model& Data) override
	{
		if (E.Type == EventType::MouseDown && E.info == "0") // left mouse button pressed
		{
			for (const auto& obj : Data.LObjets)
			{
				vector<V2*> points = obj->getPoints();
				for (size_t i = 0; i < points.size(); ++i)
				{
					V2 P = *points[i];

					if ((P - Data.currentMousePos).norm() < 5)
					{
						currentMovingObject = obj;
						pointIndexToMove = (int)i;
						currentState = State::INTERACT;
						return;
					}
				}
			}
		}

		if (E.Type == EventType::MouseUp && E.info == "0") // left mouse button released
		{
			if (currentState == State::INTERACT)
			{
				currentState = State::WAIT;
				currentMovingObject = nullptr;
				pointIndexToMove = -1;
				saveSceneState(Data);
				return;
			}
		}

		if (currentState == State::INTERACT && E.Type == EventType::MouseMove)
		{
			if (currentMovingObject && pointIndexToMove != -1)
			{
				vector<V2*> points = currentMovingObject->getPoints();
				if (pointIndexToMove < (int)points.size())
				{
					*points[pointIndexToMove] = Data.currentMousePos;
				}
				return;
			}
		}
	}

	void draw(Graphics& G, const Model& Data) override
	{
		for (const auto& obj : Data.LObjets)
		{
			vector<V2*> points = obj->getPoints();

			for (size_t i = 0; i < points.size(); ++i)
			{
				V2 P = *points[i];
				Color c = Color::White;

				if (obj == currentMovingObject && (int)i == pointIndexToMove) {
					c = Color::Gray;
				}

				G.drawCircle(P, 5, c, true);
				G.drawCircle(P, 5, Color::Black, false, 2);
			}
		}
	}
};


////////////////////////////////////////////////////////////////////


class ToolSegment : public Tool
{
	V2 Pstart;
public : 

	ToolSegment() : Tool() {}
	void processEvent(const Event& E, Model& Data) override
	{
		if (E.Type == EventType::MouseDown && E.info == "0") // left mouse button pressed
		{
			if (currentState == State::WAIT)
			{
				// interactive drawing
				Pstart = Data.currentMousePos;
				currentState = State::INTERACT;  
				return; // pour mettre fin immediatement au traitement de l'evenement
				// plus besoin de verifier les autres conditions
			}
		}

		if (E.Type == EventType::MouseUp && E.info == "0") // left mouse button released
		{
			if (currentState == State::INTERACT)
			{
				// add object in the scene
				V2 P2 = Data.currentMousePos;
				auto newObj = make_shared<ObjSegment>(Data.drawingOptions, Pstart, P2);
				Data.LObjets.push_back(newObj);
				currentState = State::WAIT;

				saveSceneState(Data);

				Data.currentTool = make_shared<ToolSelect>();
				return;
			}
		}
	}


	void draw(Graphics& G, const Model& Data) override
	{
		if (currentState == State::INTERACT)
  		  G.drawLine(Pstart, Data.currentMousePos, Data.drawingOptions.borderColor_, Data.drawingOptions.thickness_);
	}

};


////////////////////////////////////////////////////////////////////


class ToolRectangle : public Tool
{
	V2 Pstart;
public:

	ToolRectangle() : Tool() {}
	void processEvent(const Event& E, Model& Data) override
	{
		if (E.Type == EventType::MouseDown && E.info == "0") // left mouse button pressed
		{
			if (currentState == State::WAIT)
			{
				// interactive drawing
				Pstart = Data.currentMousePos;
				currentState = State::INTERACT;
				return; // pour mettre fin immediatement au traitement de l'evenement
				// plus besoin de verifier les autres conditions
			}
		}

		if (E.Type == EventType::MouseUp && E.info == "0") // left mouse button released
		{
			if (currentState == State::INTERACT)
			{
				// add object in the scene
				V2 P2 = Data.currentMousePos;
				auto newObj = make_shared<ObjRectangle>(Data.drawingOptions, Pstart, P2);
				Data.LObjets.push_back(newObj);
				currentState = State::WAIT;

				saveSceneState(Data);

				Data.currentTool = make_shared<ToolSelect>();
				return;
			}
		}
	}

	void draw(Graphics& G, const Model& Data) override
	{
		V2 P, size;
		if (currentState == State::INTERACT)
		{
			getPLH(Pstart, Data.currentMousePos, P, size);

			if (Data.drawingOptions.isFilled_)
				G.drawRectangle(P, size, Data.drawingOptions.interiorColor_, true);

			G.drawRectangle(P, size, Data.drawingOptions.borderColor_, false, Data.drawingOptions.thickness_);
		}
	}
};


////////////////////////////////////////////////////////////////////


class ToolCircle : public Tool
{
	V2 Pstart;
public:

	ToolCircle() : Tool() {}
	void processEvent(const Event& E, Model& Data) override
	{
		if (E.Type == EventType::MouseDown && E.info == "0") // left mouse button pressed
		{
			if (currentState == State::WAIT)
			{
				// interactive drawing
				Pstart = Data.currentMousePos;
				currentState = State::INTERACT;
				return; // pour mettre fin immediatement au traitement de l'evenement
				// plus besoin de verifier les autres conditions
			}
		}

		if (E.Type == EventType::MouseUp && E.info == "0") // left mouse button released
		{
			if (currentState == State::INTERACT)
			{
				// add object in the scene
				V2 P2 = Data.currentMousePos;
				auto newObj = make_shared<ObjCircle>(Data.drawingOptions, Pstart, P2);
				Data.LObjets.push_back(newObj);
				currentState = State::WAIT;

				saveSceneState(Data);

				Data.currentTool = make_shared<ToolSelect>();
				return;
			}
		}
	}

	void draw(Graphics& G, const Model& Data) override
	{
		if (currentState == State::INTERACT)
		{
			if (Data.drawingOptions.isFilled_)
				//(V2 C, float r, Color c, bool fill, int thickness)
				G.drawCircle(Pstart, (Data.currentMousePos - Pstart).norm(), Data.drawingOptions.interiorColor_, true);

			G.drawCircle(Pstart, (Data.currentMousePos - Pstart).norm(), Data.drawingOptions.borderColor_, false, Data.drawingOptions.thickness_);
		}
	}
};


////////////////////////////////////////////////////////////////////


class ToolSegPolygone : public Tool
{
	vector<V2> Points;
public:

	ToolSegPolygone() : Tool() {}
	void processEvent(const Event& E, Model& Data) override
	{
		if (E.Type == EventType::MouseDown && E.info == "0") // left mouse button pressed
		{
			Points.push_back(Data.currentMousePos);
			currentState = State::INTERACT;
		}

		if (E.Type == EventType::MouseUp && E.info == "2") // right mouse button released
		{
			if (currentState == State::INTERACT)
			{
				if (Points.size() >= 2)
				{
					auto newObj = make_shared<ObjSegPolygone>(Data.drawingOptions, Points);
					Data.LObjets.push_back(newObj);
				}
				else
				{
					cout << "Polyline ignored (less than 2 points)." << endl;
				}

				Points.clear();
				currentState = State::WAIT;

				saveSceneState(Data);

				Data.currentTool = make_shared<ToolSelect>();
				return;
			}
		}
	}

	void draw(Graphics& G, const Model& Data) override
	{
		if (currentState == State::INTERACT)
		{
			if (Points.size() >= 2)
			{
				G.drawPolygon(Points, Data.drawingOptions.borderColor_, false, Data.drawingOptions.thickness_);
			}

			if (!Points.empty())
			{
				V2 lastPoint = Points.back();
				G.drawLine(lastPoint, Data.currentMousePos, getInverseColor(Data.drawingOptions.borderColor_), Data.drawingOptions.thickness_);
			}
		}
	}
};
