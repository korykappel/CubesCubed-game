//=============================================================================
// Color Cube App.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//
//=============================================================================


//#include "C:\Users\KAPPELKR1\Desktop\Games 2\games code\Common\d3dApp.h"
#include "d3dApp.h"
#include "Box.h"
#include "GameObject.h"
#include "Line.h"
#include "Quad.h"
#include <d3dx9math.h>
#include "LineObject.h"
#include "QuadObject.h"
#include <ctime>
#include <sstream>
#include "audio.h"

#define Normalize D3DXVec3Normalize





class ColoredCubeApp : public D3DApp
{
public:
	ColoredCubeApp(HINSTANCE hInstance);
	~ColoredCubeApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 
	Vector3 moveCube();
	void initWalls();
	Vector3 randomPosition();

	void collisionVelocity(GameObject *, GameObject *);
	void reinitialize();

private:
	void buildFX();
	void buildVertexLayouts();

private:
	Audio *audio;

	Line lineX, lineY, lineZ, aim;
	Box target1, target2, target3, target4, target5, b;
	//GameObject gameObject1, gameObject2, gameObject3, gameObject4, gameObject5, gameObject6;
	GameObject bullet;
	Box c[NUM_CUBES];
	GameObject cubes[NUM_CUBES];
	LineObject xLine, yLine, zLine, aimingLine;

	Quad quad, darkGreyQuad, lightGreyQuad;
	QuadObject wallQuad[NUM_WALL_QUADS];

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	//my addition
	ID3D10EffectVariable* mfxFLIPVar;
	// additional effect variables for color changing
	ID3D10EffectVariable *mfxOverrideColorVar;
	ID3D10EffectVectorVariable *mfxObjectColorVar;


	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;

	//my edits
	D3DXMATRIX worldBox1, worldBox2;

	float mTheta;
	float mPhi;

	bool won, spaceWasPressed, bulletIn, patient;

	State state;

	D3DXVECTOR4 aimVec;

	D3DXVECTOR4 ColoredCubeApp::vectorTimesMatrix(D3DXVECTOR4 vec, Matrix mat);

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	ColoredCubeApp theApp(hInstance);

	theApp.initApp();

	return theApp.run();
}

ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance)
	: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
	mfxWVPVar(0), mTheta(0.0f), mPhi(PI*0.25f)
{
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 

	//won = true;
	spaceWasPressed = false;
	bulletIn = false;
	state = start;
	aimVec = D3DXVECTOR4(-15,0,0,0);
	patient = false;
}

ColoredCubeApp::~ColoredCubeApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);

	delete audio;
}

void ColoredCubeApp::initApp()
{
	D3DApp::initApp();

	// init sound system
    audio = new Audio();
	HRESULT hr;
    if (*WAVE_BANK != '\0' && *SOUND_BANK != '\0')  // if sound files defined
    {
        if( FAILED( hr = audio->initialize() ) )
        {
			// throwing strings since we don't have the GameError class from last semester
            if( hr == HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) )
                throw(std::string("Failed to initialize sound system because media file not found."));
            else
                throw(std::string("Failed to initialize sound system."));
        }
    }

	lineX.init(md3dDevice, 1.0f, GREEN);
	lineY.init(md3dDevice, 1.0f, BLACK);
	lineZ.init(md3dDevice, 1.0f, YELLOW);

	for(int i = 0; i<NUM_CUBES; i++)
		c[i].init(md3dDevice, scale, WHITE);




	for(int i = 0; i<NUM_CUBES; i++)
		cubes[i].init(&c[i], sqrt(2.0f), randomPosition(), Vector3(0,0,0), 10,scale);

	b.init(md3dDevice, 1.0f, BLACK);
	bullet.init(&b, sqrt(2.0f*1.4), Vector3(20,-3,0), Vector3(0,0,0), 10,scale);

/*
	xLine.init(&lineX, Vector3(0,0,0), 20);
	xLine.setPosition(Vector3(0,0,0));
	yLine.init(&lineY, Vector3(0,0,0), 20);
	yLine.setPosition(Vector3(0,0,0));
	yLine.setRotationZ(ToRadian(90));
	zLine.init(&lineZ, Vector3(0,0,0), 20);
	zLine.setPosition(Vector3(0,0,0));
	zLine.setRotationY(ToRadian(90));*/


	quad.init(md3dDevice, CUBE_RADIUS, GRAY);
	darkGreyQuad.init(md3dDevice, CUBE_RADIUS, DARK_GRAY);
	lightGreyQuad.init(md3dDevice, CUBE_RADIUS, LIGHT_GRAY);

	initWalls();



	aimingLine.init(&lineY, bullet.getPosition(), 50.0f);
	aimingLine.setRotationY(ToRadian(180));

	buildFX();
	buildVertexLayouts();

	// This stuff must be set AFTER buildFX()
	for(int i = 0; i<NUM_CUBES; i++)
	{
		cubes[i].setOverrideColorVar(mfxOverrideColorVar);
		cubes[i].setObjectColorVar(mfxObjectColorVar);
	}

	audio->playCue(SOUNDTRACK);
}

void ColoredCubeApp::reinitialize()
{
       spaceWasPressed = false;
	   patient = false;
       bulletIn = false;
       aimVec = D3DXVECTOR4(-15,0,0,0);

       aimingLine.init(&lineY, bullet.getPosition(), 50.0f);
       aimingLine.setRotationY(ToRadian(180));
       aimingLine.setActive();
       for(int i = 0; i<NUM_CUBES; i++)
       {
              cubes[i].init(&c[i], sqrt(2.0f), randomPosition(), Vector3(0,0,0), 10,scale);
              cubes[i].hit == false;
              cubes[i].setColor(WHITE);
			  cubes[i].setRotX(0);
			  cubes[i].setRotY(0);
			  cubes[i].setRotZ(0);
			  cubes[i].setRotXSpeed(0);
			  cubes[i].setRotYSpeed(0);
			  cubes[i].setRotZSpeed(0);
       }
       bullet.init(&b, sqrt(2.0f*1.4), Vector3(20,-3,0), Vector3(0,0,0), 10,scale);

}


void ColoredCubeApp::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void ColoredCubeApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);

	switch(state)
	{
	case start:
		if(GetAsyncKeyState(VK_RETURN) & 0x8000) 
		{
			state = game;
		}
		break;

	case game:
		{
			xLine.update(dt);
			yLine.update(dt);
			zLine.update(dt);
			aimingLine.update(dt);

			Vector3 oldPosition[NUM_CUBES];
			for(int i = 0; i<NUM_CUBES; i++) {
				oldPosition[i] = cubes[i].getPosition();
				cubes[i].update(dt);
			}
			Vector3 bulletOldPosition = bullet.getPosition();
			bullet.update(dt);

			for(int i=0; i<NUM_WALL_QUADS; i++) {
				wallQuad[i].update(dt);
			}

			if(!bulletIn)
			{
				if(bullet.getPosition().x < 8)
					bulletIn = true;

				if(bullet.collided(&wallQuad[3])) {
					Vector3 temp = bullet.getVelocity();
					Vector3 pos = bullet.getPosition();
					pos.y = -8;
					temp.y *= -1;
					bullet.setPosition(pos);
					bullet.setVelocity(temp);
				}
				else if(bullet.collided(&wallQuad[0])) { //left
					Vector3 temp = bullet.getVelocity();
					Vector3 pos = bullet.getPosition();
					pos.z = -8;
					temp.z *= -1;
					bullet.setPosition(pos);
					bullet.setVelocity(temp);
				}
				else if(bullet.collided(&wallQuad[2])) { //up
					Vector3 temp = bullet.getVelocity();
					Vector3 pos = bullet.getPosition();
					pos.y = 8;
					temp.y *= -1;
					bullet.setPosition(pos);
					bullet.setVelocity(temp);
				}
				else if(bullet.collided(&wallQuad[1])) { // right
					Vector3 temp = bullet.getVelocity();
					Vector3 pos = bullet.getPosition();
					pos.z = 8;
					temp.z *= -1;
					bullet.setPosition(pos);
					bullet.setVelocity(temp);
				}
			}

			//every few seconds, randomize velocities:
			for(int i = 0; i<NUM_CUBES; i++)
			{
				if(!cubes[i].hit)
				{
					if(rand()%500 == 0) cubes[i].randomizeVelocity();
				}
			}

			for(int i = 1; i<NUM_CUBES; i++)
			{
				for(int j = 0; j < i; j++)
				{
					if (cubes[i].collided(&cubes[j])) 
					{
						if(cubes[i].hit && !cubes[j].hit)
						{
							cubes[j].hit = true;
							if(cubes[j].getColor() != BLACK)
								audio->playCue(CHANGECOLOR);
							cubes[j].setColor(BLACK);
						}
						else if(!cubes[i].hit && cubes[j].hit)
						{
							cubes[i].hit = true;
							if(cubes[i].getColor() != BLACK)
								audio->playCue(CHANGECOLOR);
							cubes[i].setColor(BLACK);
						}

						if(cubes[i].hit && cubes[j].hit)
							collisionVelocity(&cubes[i], &cubes[j]);

						else if(!cubes[i].hit && !cubes[j].hit)
						{
							cubes[i].randomizeVelocity();
							cubes[j].randomizeVelocity();
						}
					}
				}
			}

			// check for cubes collisions with wall
			for(int i=0; i<NUM_CUBES; i++) {
				for(int j=0; j<NUM_WALL_QUADS; j++) {
					if(cubes[i].collided(&wallQuad[j])) {
						cubes[i].setPosition(oldPosition[i]);
						audio->playCue(WALLBOUNCE);
					}
				}
			}

			//check for bullet collide with wall
			for(int j=0; j<NUM_WALL_QUADS; j++) {
				if(bulletIn && bullet.collided(&wallQuad[j])) {
					bullet.setPosition(bulletOldPosition);
					audio->playCue(WALLBOUNCE);
				}
			}

			for(int i=0; i<NUM_CUBES; i++) {
				if(bullet.collided(&cubes[i])) {
					aimingLine.setInActive();
					//cubes[i].setPosition(oldPosition[i]);
					//bullet.setPosition(bulletOldPosition);
					// comment out when collisionVelocity() works!
					/*cubes[i].setVelocity(bullet.getVelocity()*.66);
					bullet.setVelocity(bullet.getVelocity()*.33);*/
					collisionVelocity(&bullet, &cubes[i]);
					cubes[i].hit = true;
					if(cubes[i].getColor() != BLACK)
							audio->playCue(CHANGECOLOR);
					cubes[i].setColor(BLACK);
				}
			}

			for(int i=0; i<NUM_CUBES; i++) {
				if(cubes[i].hit)
				{
					Vector3 temp = cubes[i].getVelocity();
					float v = temp.x + temp.y + temp.z;
					if(abs(v) < .15)
						cubes[i].setVelocity(cubes[i].getVelocity() * 0);
					else 
					{
						cubes[i].setVelocity(cubes[i].getVelocity() * .9999);
						cubes[i].setRotXSpeed(cubes[i].getRotXSpeed() * .9999);
						cubes[i].setRotYSpeed(cubes[i].getRotYSpeed() * .9999);
						cubes[i].setRotZSpeed(cubes[i].getRotZSpeed() * .9999);
					}
				}
			}


			Vector3 temp = bullet.getVelocity();
			float v = temp.x + temp.y + temp.z;
			if(bulletIn && abs(v) < .15)
				bullet.setVelocity(bullet.getVelocity() * 0);
			else 
				bullet.setVelocity(bullet.getVelocity() * .9999);


			//WINNING TEST LOOP
			bool testWin = true;
			for(int i = 0; i<NUM_CUBES; i++)
			{
				if(cubes[i].hit && (cubes[i].getVelocity().x != 0 || cubes[i].getVelocity().y != 0 || cubes[i].getVelocity().z != 0))
					testWin = false;
			}
			if(!bulletIn || (bullet.getVelocity().x != 0 || bullet.getVelocity().y != 0 || bullet.getVelocity().z != 0)) testWin = false;
			if(testWin) state = end;


			// check for bullets collision with the wall
			// need to add code for bullet to go through front wall
			// if implementing collisions with walls
			/*if(bullet.collided(&wallQuad[j])) {
			bullet.setPosition(oldPosition[i]);
			}*/

			//static float maxSpinSpeed = 

			//// Begin Spin Section

			for(int i = 0; i<NUM_CUBES; i++)
			{
				if (cubes[i].collided(&bullet))
				{
					//cubes[i].hit = true; 
					//cubes[i].setColor(BLUE);

					Vector3 endingPos = cubes[i].getPosition();
					Vector3 startingPos = bullet.getPosition();
					float zDist = startingPos.z - endingPos.z;
					float xDist = startingPos.x - endingPos.x;
					float yDist = startingPos.y - endingPos.y;
					//Vector3 posVec(endingPos - startingPos);
					//Vector3 velVec(bullet.getVelocity());
					//double posMag = sqrt(posVec.x*posVec.x + posVec.y*posVec.y + posVec.z*posVec.z);
					//double velMag = sqrt(velVec.x*velVec.x + velVec.y*velVec.y + velVec.z*velVec.z);
					//double angle = acos((posVec.x*velVec.x + posVec.y*velVec.y + posVec.z*velVec.z)/(posMag*velMag));
					//set speed proportional to angle between position and velocity vectors

					/*if(startingPos.z > endingPos.z)
					cubes[i].setRotYSpeed(angle/(PI/7));
					else if(startingPos.z < endingPos.z)
					cubes[i].setRotYSpeed(-angle/(PI/7));*/

					//make sure these work when bullet is coming from other direction
					//TAKE VELOCIY VECTORS INTO ACCOUNT?? (coming from an angle)
					if((abs(zDist) != 0) && (abs(zDist) < 2*scale-.05))
					{
						//if it clips it on the x axis
						if(abs(yDist) < 2*scale)
						{
							if(bullet.getVelocity().x > 0)
								cubes[i].setRotYSpeed(zDist); //maxSpindSpeed*zDist/(2*scale)
							else 
								cubes[i].setRotYSpeed(-zDist);
						}
						//if it clips on y-axis
						if(abs(xDist) < 2*scale) 
						{
							if(bullet.getVelocity().y > 0)
								cubes[i].setRotXSpeed(zDist); 
							else 
								cubes[i].setRotXSpeed(-zDist);
						}
					}

					if((abs(yDist) != 0) && (abs(yDist) < 2*scale-.05))
					{
						//if it clips it on the x axis
						if(abs(zDist) < 2*scale)
						{
							if(bullet.getVelocity().x > 0)
								cubes[i].setRotZSpeed(-yDist); //maxSpindSpeed*zDist/(2*scale)
							else 
								cubes[i].setRotZSpeed(yDist);
						}
						//if it clips on z-axis
						if(abs(xDist) < 2*scale)
						{
							if(bullet.getVelocity().z > 0)
								cubes[i].setRotXSpeed(yDist); 
							else 
								cubes[i].setRotXSpeed(-yDist);
						}
					}

					if((abs(xDist) != 0) && (abs(xDist) < 2*scale-.05))
					{
						//if it clips it on the y axis
						if(abs(zDist) < 2*scale)
						{
							if(bullet.getVelocity().y > 0)
								cubes[i].setRotZSpeed(-xDist); //maxSpindSpeed*zDist/(2*scale)
							else 
								cubes[i].setRotZSpeed(xDist);
						}
						//if it clips on z-axis
						if(abs(yDist) < 2*scale)
						{
							if(bullet.getVelocity().z > 0)
								cubes[i].setRotYSpeed(xDist); 
							else 
								cubes[i].setRotYSpeed(-xDist);
						}
					}



					//if(!cubes[i].getCollided()) //if they didn't collide last frame
					//{
					//	cubes[i].setVelocity(bullet.getVelocity()*.75); //FOR SOME REASON YOU CAN'T MULTIPLY BY FRACTION LIKE (3/4)
					//	bullet.setVelocity(bullet.getVelocity()/4);
					//	cubes[i].setCollided(true);
					//}
				}
			//	else cubes[i].setCollided(false); 
			}
			//// End Epin Section

			if(!spaceWasPressed)
			{
				Matrix temp;
				Identity(&temp);

				if(GetAsyncKeyState('W') & 0x8000)
				{
					RotateZ(&temp, -ToRadian(.05));
					aimingLine.setRotationZ(aimingLine.getRotationZ()-ToRadian(.05));
				}
				else if(GetAsyncKeyState('A') & 0x8000) 
				{
					RotateY(&temp, -ToRadian(.05));
					aimingLine.setRotationY(aimingLine.getRotationY()-ToRadian(.05));
				}
				else if(GetAsyncKeyState('S') & 0x8000) 
				{
					RotateZ(&temp, ToRadian(.05));
					aimingLine.setRotationZ(aimingLine.getRotationZ()+ToRadian(.05));
				}
				else if(GetAsyncKeyState('D') & 0x8000) 
				{
					RotateY(&temp, ToRadian(.05));
					aimingLine.setRotationY(aimingLine.getRotationY()+ToRadian(.05));
				}

				if(GetAsyncKeyState(VK_RETURN) & 0x8000) 
				{
					reinitialize();
				}

				aimVec = vectorTimesMatrix(aimVec, temp);
				//converting vec4 to vec3:
				Vector3 aimVector;
				aimVector.x = aimVec.x;
				aimVector.y = aimVec.y;
				aimVector.z = aimVec.z;

				Normalize(&aimVector, &aimVector);


				if(GetAsyncKeyState(' ') & 0x8000) 
				{
					bullet.setVelocity(aimVector*55);
					patient = true;
					spaceWasPressed = true;
				}
			}


			// Build the view matrix.
			D3DXVECTOR3 pos(40.0f,0.0f,0.0f);
			//D3DXVECTOR3 pos(0.0f,4.0f,12.0f);
			D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
			D3DXMatrixLookAtLH(&mView, &pos, &target, &up);

			break;
		}
	case end:
		if(GetAsyncKeyState(VK_RETURN) & 0x8000) 
		{
			reinitialize();
			state = game;
		}
		break;
	}
}

void ColoredCubeApp::drawScene()
{

	D3DApp::drawScene();

	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
	md3dDevice->IASetInputLayout(mVertexLayout);

	// set some variables for the shader
	int foo[1];
	foo[0] = 0;
	// set the point to the shader technique
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	//setting the color flip variable in the shader
	mfxFLIPVar->SetRawValue(&foo[0], 0, sizeof(int));

	switch(state) {

	case start:
		{
			std::wstringstream outs;   
			//outs.precision(2);
			outs << L"Hello and welcome to Cubes Cubed. You are about to embark on the journey of a lifetime.\n\n"
				<< "You will laugh             You will cry             You will aim               You will shoot\n\n"
				<< "You will leave a different person.\n"
				<< "\nBut most of all...\n\nYou will cube."
				<< "\n\n\n\n\n\n\n\n\n\n"
				<< "Controls:\nWASD to aim\n\tSpace to fire\n\tEnter to 'shake' things up a bit";
			//time = outs.str();
			RECT S = {20, 20, 0, 0};
			mFont->DrawText(0, outs.str().c_str(), -1, &S, DT_NOCLIP, WHITE);
			mSwapChain->Present(0, 0);

			std::wstringstream outs2;   
			//outs.precision(2);
			outs2 << L"        Cubes Cubed\nPress Enter to continue";
			//time = outs.str();
			RECT S2 = {310, 260, 0, 0};
			mFont->DrawText(0, outs2.str().c_str(), -1, &S2, DT_NOCLIP, GRAY);
			mSwapChain->Present(0, 0);
		}
		break;

	case game:
		//draw the lines
	/*	mWVP = xLine.getWorldMatrix()*mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		xLine.setMTech(mTech);
		xLine.draw();

		mWVP = yLine.getWorldMatrix() *mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		yLine.setMTech(mTech);
		yLine.draw();

		mWVP = zLine.getWorldMatrix() *mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		zLine.setMTech(mTech);
		zLine.draw();*/

		mWVP = aimingLine.getWorldMatrix() *mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		aimingLine.setMTech(mTech);
		aimingLine.draw();

		//draw the cubes
		for(int i = 0; i<NUM_CUBES; i++)
		{
			mWVP = cubes[i].getWorldMatrix()  *mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);  //sets matrix to matrix variable in the shader
			cubes[i].setMTech(mTech);
			cubes[i].draw();
		}

		//draw the bullet
		mWVP = bullet.getWorldMatrix()  *mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);  //sets matrix to matrix variable in the shader
		bullet.setMTech(mTech);
		bullet.draw();


		for(int i=0; i<NUM_WALL_QUADS;i++) {
			mWVP = wallQuad[i].getWorldMatrix() *mView*mProj;
			mfxWVPVar->SetMatrix((float*)&mWVP);
			wallQuad[i].setMTech(mTech);
			wallQuad[i].draw();
		}

		if(patient)
        {
                std::wstringstream outs;   
                //outs.precision(2);
                outs << L"Now be patient as your choice unfolds...";
                //time = outs.str();
                RECT S = {250, 520, 0, 0};
                mFont->DrawText(0, outs.str().c_str(), -1, &S, DT_NOCLIP, WHITE);
        }
        else
        {
                std::wstringstream outs;   
                //outs.precision(2);
                outs << L"Choose carefully...";
                //time = outs.str();
                RECT S = {330, 520, 0, 0};
                mFont->DrawText(0, outs.str().c_str(), -1, &S, DT_NOCLIP, WHITE);
        }


		mSwapChain->Present(0, 0);
		break;

	case end:
		int score = 0;
		for(int i = 0; i<NUM_CUBES; i++)
		{
			if(cubes[i].hit)
				score++;
		}
		std::wstringstream outs;   
		//outs.precision(2);
		outs << score << L" of the " << NUM_CUBES <<" cubes have come to know salvation because of your valiant efforts"
			<< "\n\nPress Enter to start anew";
		//time = outs.str();
		RECT S = {100, 250, 0, 0};
		mFont->DrawText(0, outs.str().c_str(), -1, &S, DT_NOCLIP, WHITE);
		mSwapChain->Present(0, 0);

		break;

	}


}

void ColoredCubeApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"color.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("ColorTech");

	mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxFLIPVar = mFX->GetVariableByName("flip");

	mfxOverrideColorVar = mFX->GetVariableByName("overrideColor");
	mfxObjectColorVar = mFX->GetVariableByName("objectColor")->AsVector();


}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}


//PLACE THIS SOMEWHERE ELSE LATER
D3DXVECTOR4 ColoredCubeApp::vectorTimesMatrix(D3DXVECTOR4 vec, Matrix mat)
{
	D3DXVECTOR4 final;
	final.x = vec.x*mat._11 + vec.y*mat._21 + vec.z*mat._31 + vec.w*mat._41;
	final.y = vec.x*mat._12 + vec.y*mat._22 + vec.z*mat._32 + vec.w*mat._42;
	final.z = vec.x*mat._13 + vec.y*mat._23 + vec.z*mat._33 + vec.w*mat._43;
	final.w = vec.x*mat._14 + vec.y*mat._24 + vec.z*mat._34 + vec.w*mat._44;

	return final;
}

void ColoredCubeApp::initWalls() {
	wallQuad[0].init(&quad,Vector3(0,0,0),1);
	wallQuad[0].setRotationX(ToRadian(90));
	wallQuad[0].setRotationZ(ToRadian(45));
	wallQuad[0].setPosition(LEFT_WALL);

	wallQuad[1].init(&quad,Vector3(0,0,0),1);
	wallQuad[1].setRotationX(ToRadian(-90));
	wallQuad[1].setRotationZ(ToRadian(45));
	wallQuad[1].setPosition(RIGHT_WALL);

	wallQuad[2].init(&lightGreyQuad,Vector3(0,0,0),1);
	wallQuad[2].setRotationY(ToRadian(45));
	wallQuad[2].setRotationZ(ToRadian(180));
	wallQuad[2].setPosition(TOP_WALL);

	wallQuad[3].init(&lightGreyQuad,Vector3(0,0,0),1);
	wallQuad[3].setRotationY(ToRadian(45));
	wallQuad[3].setPosition(BOTTOM_WALL);

	wallQuad[4].init(&darkGreyQuad,Vector3(0,0,0),1);
	wallQuad[4].setRotationY(ToRadian(45));
	wallQuad[4].setRotationZ(ToRadian(-90));
	wallQuad[4].setPosition(BACK_WALL);

	wallQuad[5].init(&quad,Vector3(0,0,0),1);
	wallQuad[5].setRotationY(ToRadian(45));
	wallQuad[5].setRotationZ(ToRadian(90));
	wallQuad[5].setPosition(FRONT_WALL);
	wallQuad[5].setInVisible();
}

Vector3 ColoredCubeApp::randomPosition()
{
	float x = (rand()%90)/10;
	if(rand()%2 == 0) x *= -1;
	int y = (rand()%90)/10;
	if(rand()%2 == 0) y *= -1;
	int z = (rand()%90)/10;
	if(rand()%2 == 0) z *= -1;

	Vector3 temp(x,y,z);
	return temp;
}

void ColoredCubeApp::collisionVelocity(GameObject *gameObject1, GameObject *gameObject2) {
	//Vector3 pos = gameObject->getPosition();
	//float rad = gameObject->getRadius();
	//if(position.x - pos.x 
	Vector3 vel1 = gameObject1->getVelocity();
	Vector3 vel2 = gameObject2->getVelocity();
	float x1, y1, z1, x2, y2, z2; 

	//x component
	//if opposite directions
	float temp = abs(vel1.x - vel2.x)/2.0f;
	if(vel1.x < 0 && vel2.x > 0)
	{
		x1 = temp;
		x2 = temp * -1;
	}
	else if(vel1.x > 0 && vel2.x < 0)
	{
		x1 = temp * -1;
		x2 = temp;
	}
	//same directions
	else if((vel1.x >= 0 && vel2.x >= 0) || (vel1.x <= 0 && vel2.x <= 0))
	{
		temp = (vel1.x + vel2.x)/2.0f;
		x1 = temp;
		x2 = temp;
	}

	//y component
	//if opposite directions
	temp = abs(vel1.y - vel2.y)/2.0f;
	if(vel1.y < 0 && vel2.y > 0)
	{
		y1 = temp;
		y2 = temp * -1;
	}
	else if(vel1.y > 0 && vel2.y < 0)
	{
		y1 = temp * -1;
		y2 = temp;
	}
	//same directions
	else if((vel1.y >= 0 && vel2.y >= 0) || (vel1.y <= 0 && vel2.y <= 0))
	{
		temp = (vel1.y + vel2.y)/2.0f;
		y1 = temp;
		y2 = temp;
	}
	//else -- 0's??

	//z component
	//if opposite directions
	temp = abs(vel1.z - vel2.z)/2.0f;
	if(vel1.z < 0 && vel2.z > 0)
	{
		z1 = temp;
		z2 = temp * -1;
	}
	else if(vel1.z > 0 && vel2.z < 0)
	{
		z1 = temp * -1;
		z2 = temp;
	}
	//same directions
	else if((vel1.z >= 0 && vel2.z >= 0) || (vel1.z <= 0 && vel2.z <= 0))
	{
		temp = (vel1.z + vel2.z)/2.0f;
		z1 = temp;
		z2 = temp;
	}

	Vector3 tempVec1(x1, y1, z1);
	Vector3 tempVec2(x2, y2, z2);
	gameObject1->setVelocity(tempVec1);
	gameObject2->setVelocity(tempVec2);
}