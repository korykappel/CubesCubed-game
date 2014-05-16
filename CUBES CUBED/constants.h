#ifndef Constants_H
#define Constants_H

// Note: to avoid issues mixing the Audio class (designed for DX9) with the rest of the DX10 stuff,
// #define DX9_AUDIO_HACK in Audio.cpp (but not anywhere else!) to selectively include just stuff for Audio there.
#ifndef DX9_AUDIO_HACK

//Defines to make life easier

#define Vector3 D3DXVECTOR3
#define Matrix D3DXMATRIX
#define Identity D3DXMatrixIdentity
#define Translate D3DXMatrixTranslation
#define RotateX D3DXMatrixRotationX
#define RotateY D3DXMatrixRotationY
#define RotateZ D3DXMatrixRotationZ
#define ToRadian D3DXToRadian
#define ToDegree D3DXToDegree
#define Scale D3DXMatrixScaling
#define Normalize D3DXVec3Normalize
#define Normalize4 D3DXVec4Normalize

const int NUM_WALL_QUADS = 6;
//const int NUM_CUBES = 4;
const int CUBE_RADIUS = 20;
const Vector3 TOP_WALL = Vector3(0,CUBE_RADIUS/2,0);
const Vector3 BOTTOM_WALL = Vector3(0,-CUBE_RADIUS/2,0);
const Vector3 LEFT_WALL = Vector3(0,0,-CUBE_RADIUS/2);
const Vector3 RIGHT_WALL = Vector3(0,0,CUBE_RADIUS/2);
const Vector3 FRONT_WALL = Vector3(CUBE_RADIUS/2,0,0);
const Vector3 BACK_WALL = Vector3(-CUBE_RADIUS/2,0,0);

#define NUM_CUBES 20
const float scale = 1.0f;

enum State {start, game, end};

#endif

//// Audio stuff
// audio files
const char WAVE_BANK[]  = "audio\\Win\\Wave Bank.xwb";
// SOUND_BANK must be location of .xsb file.
const char SOUND_BANK[] = "audio\\Win\\Sound Bank.xsb";

//audio cues
const char SOUNDTRACK[] = "dark-piano-and-violin";
const char CHANGECOLOR[] = "change color";
const char WALLBOUNCE[] = "wall bounce";

#endif