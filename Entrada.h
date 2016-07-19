#pragma once
class Entrada
{
public:

	Entrada()
	{
		for (int i = 0; i < 256; ++i)
			teclas[i] = false;
	}

	~Entrada()
	{
	}

	bool TeclaPressionada(unsigned int tecla) { return teclas[tecla]; }

	bool TeclaSolta(unsigned int tecla) { return teclas[tecla]; }

	void SetTeclaPressionada(unsigned int tecla) {
		teclas[tecla] = true;
	}

	void SetTeclaSolta(unsigned int tecla) {
		teclas[tecla] = false;
	}

private:

	bool teclas[256];


};

