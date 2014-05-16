
#ifndef GameObject_H
#define GameObject_H

#include "d3dUtil.h"
//#include "C:\Users\KAPPELKR1\Desktop\Games 2\games code\Common\d3dUtil.h"
#include "Box.h"
#include "constants.h"
#include <ctime>
#include "QuadObject.h"



class GameObject
{
public:

	GameObject();
	~GameObject();

	void init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float s);
	void draw();
	void update(float dt);

	void setPosition (Vector3 pos) {position = pos;}
	Vector3 getPosition() {return position;}
	void setVelocity (Vector3 vel) {velocity = vel;}
	Vector3 getVelocity() {return velocity;}
	void setSpeed(float s) {speed = s;}
	float getSpeed() {return speed;}
	void setRadius(float r) {radius = r; radiusSquared = (scale*r)*(scale*r);}
	float getRadiusSquare() {return radiusSquared;}
	float getRadius() {return radius;}
	Matrix getWorldMatrix() {return world;}
	void setScale(float s) {scale = s; radiusSquared = (s*radius)*(s*radius);}
	float getScale() {return scale;}
	void setActive() {active = true;}
	void setInActive() {active = false;}
	bool getActiveState() {return active;}
	void setMTech(ID3D10EffectTechnique* m){ mTech = m;}
	bool collided(GameObject *gameObject);
	void collisionVelocity(GameObject *gameObject);
	bool collided(QuadObject *quadObject);
	
	//used for good collision detection:
	Vector3 getOldPosition() {return oldPosition;}
	void setOldPosition(Vector3 pos) {oldPosition = pos;}

	void randomizeVelocity();

	void setRotXSpeed(float r) {spinXSpeed = r;}
	void setRotYSpeed(float r) {spinYSpeed = r;}
	void setRotZSpeed(float r) {spinZSpeed = r;}
	float getRotXSpeed() {return spinXSpeed;}
	float getRotYSpeed() {return spinYSpeed;}
	float getRotZSpeed() {return spinZSpeed;}
	//float getRotX() {return rotX;}
	//float getRotY() {return rotY;}
	//float getRotZ() {return rotZ;}
	void setRotX(float r) {rotX = r;}
	void setRotY(float r) {rotY = r;}
	void setRotZ(float r) {rotZ = r;}

	float spinAmountX, spinAmountY, spinAmountZ;
	float spinXSpeed, spinYSpeed, spinZSpeed;

	bool collidedLastFrame;
	bool getCollided() {return collidedLastFrame;}
	void setCollided(bool c) {collidedLastFrame = c;}

	bool hit; //used for its color and updating velocity

	void setColor(D3DXCOLOR c) { overrideColor = true; color = c; }
    D3DXCOLOR getColor() { return color; }
    void setOverrideColorVar(ID3D10EffectVariable *v) { mfxOverrideColorVar = v; } // both of these must be set if overriding color
    void setObjectColorVar(ID3D10EffectVectorVariable *v) { mfxObjectColorVar = v; }


private:
	Box *box; 
	Vector3 position;
	Vector3 oldPosition;
	Vector3 velocity;
	float speed;
	float radius;
	float radiusSquared;
	bool active;
	Matrix world;
	float scale;
	ID3D10EffectTechnique* mTech;
	float rotX, rotY, rotZ;

	bool overrideColor;
    D3DXCOLOR color;
    ID3D10EffectVariable *mfxOverrideColorVar;
    ID3D10EffectVectorVariable *mfxObjectColorVar;

};


#endif
