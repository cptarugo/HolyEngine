#include "Framework.h"

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int iCmdShow){

	Framework *framework = new Framework();

	if (framework->Iniciar()){
		framework->Executar();
	}

	return 0;
}