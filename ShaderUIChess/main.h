#ifndef _MAIN_H_
#define _MAIN_H_

//-----------------------------------------------------------------------------
// Main Application Includes
//-----------------------------------------------------------------------------
//#include <tchar.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <D3DX9.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
//#include "resource.h"
#include "rendering/d3d.h"

//#define RANDOM_COLOR 0xFF000000 | ((rand() * 0xFFFFFF) / RAND_MAX)

// typedef struct _VOLUME_INFO     // Stores information about our object volume
// {
// 	D3DXVECTOR3 Min;            // Minimum object space extents of the volume
// 	D3DXVECTOR3 Max;            // Maximum object space extents of the volume
// 
// } VOLUME_INFO;

struct CALLBACK_FUNC   // Stores details for a callback
{
	LPVOID  pFunction;          // Function Pointer
	LPVOID  pContext;           // Context to pass to the function

};

#endif // _MAIN_H_