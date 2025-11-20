/* Copyright (c) 2024 Lilian Buzer - All rights reserved - */
#pragma once

#include "Color.h"
#include "V2.h"
#include "ObjAttr.h"
#include "Graphics.h"
#include <fstream>
#include <sstream>
#include <string>

struct Hitbox { V2 P, size; };

class ObjGeom
{
public :
	ObjAttr drawInfo_;

	ObjGeom() {}
	ObjGeom(ObjAttr  drawInfo) : drawInfo_(drawInfo)   {  }

	virtual void draw(Graphics& G, bool isSelected) {}

	// Notez le = 0; pour forcer l'implementation dans les classes derivees
	virtual Hitbox getHitbox() const = 0;
	virtual void serialize(stringstream& ss) const = 0;
	virtual vector<V2*> getPoints() { return {}; }
};


class ObjRectangle : public ObjGeom
{
public :
	V2 P1_;
	V2 P2_;

	ObjRectangle(ObjAttr  drawInfo, V2 P1, V2 P2) : ObjGeom(drawInfo)
	{
		P1_ = P1;
		P2_ = P2;
	}

	void draw(Graphics& G, bool isSelected) override
	{
		V2 P, size;
		getPLH(P1_, P2_, P, size);

		int thickness = drawInfo_.thickness_;
		Color interiorColor = drawInfo_.interiorColor_;
		if (isSelected)
		{
			thickness += 3;
			interiorColor = getInverseColor(interiorColor);
		}

		if ( drawInfo_.isFilled_ )
  		   G.drawRectangle(P,size, interiorColor, true);

		G.drawRectangle(P, size, drawInfo_.borderColor_, false, thickness);
	}

	Hitbox getHitbox() const override {
		V2 P, size;
		getPLH(P1_, P2_, P, size);
		return Hitbox{ P, size };
	}

	void serialize(stringstream& ss) const override
	{
		ss << "ObjRectangle,";

		ss << (float)drawInfo_.borderColor_.R << "," << (float)drawInfo_.borderColor_.G << "," << (float)drawInfo_.borderColor_.B << ",";
		ss << (float)drawInfo_.interiorColor_.R << "," << (float)drawInfo_.interiorColor_.G << "," << (float)drawInfo_.interiorColor_.B << ",";
		ss << (drawInfo_.isFilled_ ? 1 : 0) << ",";
		ss << drawInfo_.thickness_ << ",";

		ss << P1_.x << "," << P1_.y << "," << P2_.x << "," << P2_.y << "\n";
	}

	vector<V2*> getPoints() override {
		return { &P1_, &P2_ }; 
	}
};


class ObjSegment : public ObjGeom
{
public:
	V2 P1_;
	V2 P2_;

	ObjSegment(ObjAttr  drawInfo, V2 P1, V2 P2) : ObjGeom(drawInfo)
	{
		P1_ = P1;
		P2_ = P2;
	}

	void draw(Graphics& G, bool isSelected) override
	{
		V2 P, size;
	 
		int thickness = drawInfo_.thickness_;
		Color borderColor = drawInfo_.borderColor_;
		if (isSelected)
		{
			thickness += 3;
			borderColor = getInverseColor(borderColor);
		}

		G.drawLine(P1_, P2_, borderColor, thickness);
	}

	Hitbox getHitbox() const override {
		V2 P, size;
		getPLH(P1_, P2_, P, size);
		return Hitbox{ P, size };
	}

	void serialize(stringstream& ss) const override
	{
		ss << "ObjSegment,";

		ss << (float)drawInfo_.borderColor_.R << "," << (float)drawInfo_.borderColor_.G << "," << (float)drawInfo_.borderColor_.B << ",";
		ss << (float)drawInfo_.interiorColor_.R << "," << (float)drawInfo_.interiorColor_.G << "," << (float)drawInfo_.interiorColor_.B << ",";
		ss << (drawInfo_.isFilled_ ? 1 : 0) << ",";
		ss << drawInfo_.thickness_ << ",";

		ss << P1_.x << "," << P1_.y << "," << P2_.x << "," << P2_.y << "\n";
	}

	vector<V2*> getPoints() override {
		return { &P1_, &P2_ };
	}
};


class ObjCircle : public ObjGeom
{
public:
	V2 P1_;
	V2 P2_;

	ObjCircle(ObjAttr  drawInfo, V2 P1, V2 P2) : ObjGeom(drawInfo)
	{
		P1_ = P1;
		P2_ = P2;
	}

	void draw(Graphics& G, bool isSelected) override
	{
		int thickness = drawInfo_.thickness_;
		Color interiorColor = drawInfo_.interiorColor_;
		if (isSelected)
		{
			thickness += 3;
			interiorColor = getInverseColor(interiorColor);
		}

		if (drawInfo_.isFilled_)
			//(V2 C, float r, Color c, bool fill, int thickness)
			G.drawCircle(P1_, (P2_ - P1_).norm(), interiorColor, true);

		G.drawCircle(P1_, (P2_ - P1_).norm(), drawInfo_.borderColor_, false, thickness);
	}

	Hitbox getHitbox() const override {
		int width = (P2_ - P1_).norm() * 2;
		return Hitbox{ P1_ - V2{(P2_ - P1_).norm(),(P2_ - P1_).norm()}, V2{width,width} };
	}

	void serialize(stringstream& ss) const override
	{
		ss << "ObjCircle,";

		ss << (float)drawInfo_.borderColor_.R << "," << (float)drawInfo_.borderColor_.G << "," << (float)drawInfo_.borderColor_.B << ",";
		ss << (float)drawInfo_.interiorColor_.R << "," << (float)drawInfo_.interiorColor_.G << "," << (float)drawInfo_.interiorColor_.B << ",";
		ss << (drawInfo_.isFilled_ ? 1 : 0) << ",";
		ss << drawInfo_.thickness_ << ",";

		ss << P1_.x << "," << P1_.y << "," << P2_.x << "," << P2_.y << "\n";
	}

	vector<V2*> getPoints() override {
		return { &P1_, &P2_ };
	}
};


class ObjSegPolygone : public ObjGeom
{
public:
	vector<V2> Points_;

	ObjSegPolygone(ObjAttr  drawInfo, vector<V2>& Points) : ObjGeom(drawInfo)
	{
		Points_ = Points;
	}

	void draw(Graphics& G, bool isSelected) override
	{
		if (Points_.size() < 2) return;

		int thickness = drawInfo_.thickness_;
		Color borderColor = drawInfo_.borderColor_;
		if (isSelected)
		{
			thickness += 3;
			borderColor = getInverseColor(borderColor);
		}

		// false car une ligne brisee ne peut pas être remplie
		G.drawPolygon(Points_, borderColor, false, thickness);
	}

	Hitbox getHitbox() const override {
		if (Points_.empty()) {
			return Hitbox{ V2(0, 0), V2(0, 0) };
		}
		V2 Pmin = Points_[0];
		V2 Pmax = Points_[0];
		for (const auto& P : Points_)
		{
			if (P.x < Pmin.x) Pmin.x = P.x;
			if (P.y < Pmin.y) Pmin.y = P.y;
			if (P.x > Pmax.x) Pmax.x = P.x;
			if (P.y > Pmax.y) Pmax.y = P.y;
		}

		V2 P = Pmin;
		V2 size = Pmax - Pmin;

		return Hitbox{ P, size };
	}

	void serialize(stringstream& ss) const override
	{
		ss << "ObjSegPolygone,";

		ss << (float)drawInfo_.borderColor_.R << "," << (float)drawInfo_.borderColor_.G << "," << (float)drawInfo_.borderColor_.B << ",";
		ss << (float)drawInfo_.interiorColor_.R << "," << (float)drawInfo_.interiorColor_.G << "," << (float)drawInfo_.interiorColor_.B << ",";
		ss << (drawInfo_.isFilled_ ? 1 : 0) << ",";
		ss << drawInfo_.thickness_ << ",";

		ss << Points_.size();
		for (const V2& p : Points_) { ss << "," << p.x << "," << p.y; } ss << "\n";
	}

	vector<V2*> getPoints() override {
		vector<V2*> pointPointers;

		for (V2& P : Points_) {
			pointPointers.push_back(&P);
		}

		return pointPointers;
	}
};
