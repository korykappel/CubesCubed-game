/*
	Author:			Zac Stahl
	Date:			02/17/2014
	Description:	This file defines the class QuadObject
					and its functions.
*/

#ifndef QuadObject_H
#define QuadObject_H


#include "d3dUtil.h"
#include "Quad.h"
#include "constants.h"

class QuadObject
{
public:

	QuadObject();
	~QuadObject();

	void init(Quad *q,  Vector3 pos, float s);
	void draw();
	void update(float dt);

	void setPosition (Vector3 pos) {position = pos;}
	Vector3 getPosition() {return position;}
	void setVelocity (Vector3 vel) {velocity = vel;}
	Vector3 getVelocity() {return velocity;}
	void setSpeed(float s) {speed = s;}
	float getSpeed() {return speed;}
	Matrix getWorldMatrix() {return world;}
	void setScale(float s) {scale = s;}
	float getScale() {return scale;}
	void setActive() {active = true;}
	void setInActive() {active = false;}
	bool getActiveState() {return active;}
	void setMTech(ID3D10EffectTechnique* m){ mTech = m;}
	void setRotationX(float r) {rotX = r;}
	void setRotationY(float r) {rotY = r;}
	void setRotationZ(float r) {rotZ = r;}
	void setVisible() {visible = true;}
	void setInVisible() {visible = false;}
	bool getVisible() {return visible;}
	//void setColor(D3DXCOLOR); 

private:
	Quad *quad; 
	Vector3 position;
	Vector3 velocity;
	float speed;
	bool active;
	Matrix world;
	float scale;
	ID3D10EffectTechnique* mTech;
	float rotX, rotY, rotZ;
	bool visible;
};

#endif
