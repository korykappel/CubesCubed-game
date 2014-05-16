/*
	Author:			Zac Stahl
	Date:			02/17/2014
	Description:	This file implements the class QuadObject
					and its functions.
*/

#include "QuadObject.h"

QuadObject::QuadObject()
{
	speed = 0;
	active = true;
	Identity(&world);
	rotX = 0;
	rotY = 0;
	rotZ = 0;
	visible = true;
}

QuadObject::~QuadObject()
{
	quad = NULL;
}

void QuadObject::draw()
{
	if (!active || !visible)
		return;
    D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
        quad->draw();
    }
}

void QuadObject::init(Quad *q, Vector3 pos, float s)
{
	quad = q;
	position = pos;
	scale = s;
	rotX = 0;
	rotY = 0;
	rotZ = 0;
}

void QuadObject::update(float dt)
{

	Matrix rotXM, rotYM, rotZM, transM;
	RotateX(&rotXM, rotX);
	RotateY(&rotYM, rotY);
	RotateZ(&rotZM, rotZ); 
	Translate(&transM, position.x, position.y, position.z);
	world = rotXM * rotYM * rotZM * transM;
}

