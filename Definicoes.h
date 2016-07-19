#ifndef _DEF_H
#define _DEF_H


//Nao pode ser constante, derp
//#define JANELA_WIDTH 800 
//#define JANELA_HEIGHT 600
#define JANELA_X 100
#define JANELA_Y 100
#define JANELA_NOME L"Holy Engine"



//Macros de Utilidade
#define ReleaseCOM(x){ if(x){ x->Release(); x = 0; } }
#define SafeDelete(x){ delete x; x = 0; }

static float JANELA_WIDTH = 800;
static float JANELA_HEIGHT = 600;

const float CLIP_LONGE = 1000.0f; //Far clipping plane
const float CLIP_PERTO = 0.1f; //Near clipping plane

const bool USE_FULLSCREEN = false;
const bool VSYNC_ENABLED = false;

#endif