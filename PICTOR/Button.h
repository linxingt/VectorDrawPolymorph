/* Copyright (c) 2024 Lilian Buzer - All rights reserved - */
#pragma once

#include <string>
#include "V2.h"
#include "Graphics.h"
#include "Event.h"
#include "Model.h"
#include <functional>

using namespace std;

// button component in the GUI
enum ButtonType { IMAGE, COLOR_LINE, COLOR_FILL, THICKNESS_LINE, TOGGLE_FILL };

const vector<Color> COLOR_CHOICES = {
	Color::Red,
	Color::Green,
	Color::Blue,
	Color::Magenta,
	Color::Cyan,
	Color::Yellow
	//couleur white/black/gray avec la fonction getInverseColor cause ambiguite 
};

const vector<int> THICKNESS_CHOICES = {
	1, // Fin
	3, // Moyen
	6, // Epais
	10 // Tres epais
};

class Button
{
	string imageFile_;
	string myName_;
	ButtonType type_;
	V2 pos_;
	V2 size_;
	function<void(Model&)> storedFunction_; // when the button is clicked, call this function

 

public:

	V2 getPos()  { return pos_;  }
	V2 getSize() { return size_; }

	Button(string myName, V2 pos, V2 size, string imageFile, function<void(Model&)> callBack, ButtonType type = IMAGE) :
		myName_(myName), pos_(pos), size_(size), imageFile_(imageFile), storedFunction_(callBack), type_(type){}

	void manageEvent(const Event& Ev, Model& Ap)
	{
		if (Ev.Type == EventType::MouseDown)
		{
			cout << ">> Clic sur le bouton " << myName_ << endl;
			storedFunction_(Ap);
		}
	}

	void draw(Graphics & G, const Model& Data)
	{
        if (type_ == ButtonType::COLOR_LINE) {
            V2 center = pos_ + size_ / 2;
            int halfSize = size_.x / 3;

            Color c = COLOR_CHOICES[Data.currentBorderColorIndex];
            int t = 5;

            G.drawLine(center - V2(halfSize, 0), center + V2(halfSize, 0), c, t);

        }
        else if (type_ == ButtonType::COLOR_FILL) {
            V2 center = pos_ + size_ / 2;
            int halfSize = size_.x / 3;

            Color c = COLOR_CHOICES[Data.currentInteriorColorIndex];
            G.drawCircle(center, halfSize, c, true);

        }
        else if (type_ == ButtonType::THICKNESS_LINE) {
            V2 center = pos_ + size_ / 2;
            int halfSize = size_.x / 3;

            Color c = Color::White; 
            int t = THICKNESS_CHOICES[Data.currentThicknessIndex]; 

            G.drawLine(center - V2(halfSize, 0), center + V2(halfSize, 0), c, t);

        }
        else if (type_ == ButtonType::TOGGLE_FILL) {
            V2 P_rect = pos_ + V2(size_.x / 4, size_.y / 4);
            V2 size_rect = size_ / 2;

            Color c = Data.isFilled ? Color::Gray : Color::White;

            G.drawRectangle(P_rect, size_rect, c, Data.isFilled, 3);
            G.drawRectangle(P_rect, size_rect, Color::Black, false, 1);

        }
		else {
			G.drawRectWithTexture(imageFile_, pos_, size_);
		}
		G.drawRectangle(pos_, size_, Color::Gray, false,2);
		G.drawRectangle(pos_ + V2(2,2), size_-V2(4,4), Color::Black, false,2);
	}

	
};