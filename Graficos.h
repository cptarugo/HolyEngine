#ifndef _GRAFICOS_H
#define _GRAFICOS_H

#pragma once


#include "Model.h"
#include <list>

struct GraficoObject {
	Model* modelo;
	char* modelo_file_name;
	//Shader
	//Modelo name
	//.....
};

class Graficos
{
public:
	Graficos();
	~Graficos();

	void CriarModelo(char *file_name, ID3D11Device *device);

private:
	std::list<GraficoObject> *graficoObjetos;


	bool jaExiste(Model *obj);
	bool jaExisteFileName(char* file_name);
};

#endif