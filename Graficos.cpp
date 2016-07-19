#include "Graficos.h"



Graficos::Graficos()
{
	graficoObjetos = new std::list<GraficoObject>();
}


Graficos::~Graficos()
{
	SafeDelete(graficoObjetos);
}

void Graficos::CriarModelo(char * file_name, ID3D11Device * device)
{
	if (!jaExisteFileName(file_name)) {
		GraficoObject novoObjeto;

		Model *novoModelo = new Model();
		novoModelo->OpenTXT(file_name, device);

		novoObjeto.modelo = novoModelo;
		novoObjeto.modelo_file_name = file_name;

		graficoObjetos->push_back(novoObjeto);
	}
}



bool Graficos::jaExiste(Model * obj)
{
	for (auto it = graficoObjetos->begin(); it != graficoObjetos->end(); ++it)
		if ((*it).modelo == obj)
			return true;
	return false;
}

bool Graficos::jaExisteFileName(char * file_name)
{
	for (auto it = graficoObjetos->begin(); it != graficoObjetos->end(); ++it)
		if ((*it).modelo_file_name == file_name)
			return true;
	return false;
}
