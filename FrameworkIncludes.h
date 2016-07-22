#ifndef _FRAME_WORK_INCLUDES_H
#define _FRAME_WORK_INCLUDES_H
#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <WindowsX.h>
#include <d3dx11.h>
#include <xnamath.h>
//#include <dxerr.h>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include "Model.h"
#include "Shader.h"
#include "Entrada.h"
#include "Timer.h"
#include "Camera.h"
#include "Mouse.h"
#include "Geometrias.h"

#endif