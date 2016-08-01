

struct LuzDirecionada
{
	float4 Ambiente;
	float4 Difuso;
	float4 Especular;
	float3 Direcao;
	float pad;
};


struct PontoLuz
{
	float4 Ambiente;
	float4 Difuso;
	float4 Especular;

	float3 Posicao;
	float Alcance;

	float3 Att;
	float pad;
};


struct LuzFocada
{
	float4 Ambiente;
	float4 Difuso;
	float4 Especular;

	float3 Posicao;
	float Alcance;

	float3 Direcao;
	float ConeExp;

	float3 Att;
	float pad;
};


struct Material
{
	float4 Ambiente;
	float4 Difuso;
	float4 Especular; // w = Exponencial de especulacao
	float4 Refletir;
};









//
//	Iluminacao
//


///Luz do tipo direcional (ex.: SOL)
void ComputarLuzDirecionada(Material mat, LuzDirecionada L, float3 normal, float3 toEye,
	out float4 ambiente, out float4 difuso, out float4 espec) {

	//
	//	Inicializando a saida
	//
	ambiente = float4(0.0f, 0.0f, 0.0f, 0.0f);
	difuso = float4(0.0f, 0.0f, 0.0f, 0.0f);
	espec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//
	//	O Vetor da Luz (L) eh o oposto ao vetor em que o raio de luz viaja
	//
	float3 luzVec = -L.Direcao;


	//
	//	Adicionando a luz do tipo ambiente
	//
	ambiente = mat.Ambiente * L.Ambiente;


	//
	//	Adiciona termos de difuso e especular, desde que a superficie esteja visivel 
	//
	float fatorDifusao = dot(luzVec, normal);


	//
	//	Retornar da funcao caso o fator seja negativo
	//
	[flatten]
	if (fatorDifusao > 0.0f) {

		float3 v = reflect(-luzVec, normal);
		float especValor = pow(max(dot(v, toEye), 0.0f), mat.Especular.w);

		difuso = fatorDifusao * mat.Difuso * L.Difuso;
		espec = especValor * mat.Especular * L.Especular;

	}
}






///Luz do tipo ponto (ex.: Lampada)
void ComputarPontoLuz(Material mat, PontoLuz L, float3 pos, float3 normal, float3 toEye,
	out float4 ambiente, out float4 difuso, out float4 espec) {



	//
	//	Inicializando a saida
	//
	ambiente = float4(0.0f, 0.0f, 0.0f, 0.0f);
	difuso = float4(0.0f, 0.0f, 0.0f, 0.0f);
	espec = float4(0.0f, 0.0f, 0.0f, 0.0f);


	//
	//	Vetor da superficie para a luz
	//
	float3 luzVec = L.Posicao - pos;



	//
	//	Distancia da superficie para a luz
	//
	float d = length(luzVec);


	if (d > L.Alcance)
		return;

	//
	//	Normalizando o vetor da luz
	//
	luzVec /= d;


	//
	//	Adicionando luz ambiente
	//
	ambiente = mat.Ambiente * L.Ambiente;


	//
	//	Adicionando luz difusa e especular
	//
	float fatorDifusao = dot(luzVec, normal);



	//
	// Flatten
	//
	[flatten]
	if (fatorDifusao > 0.0f) {

		float3 v = reflect(-luzVec, normal);
		float valorEspec = pow(max(dot(v, toEye), 0), mat.Especular.w);

		difuso = fatorDifusao * mat.Difuso * L.Difuso;
		espec = valorEspec * mat.Especular * L.Especular;
	}

	//
	//	Atenuar
	//
	float att = 1.0f / dot(L.Att, float3(1.0f, d, d*d));

	difuso *= att;
	espec *= att;
}





///Luz do tipo foco (ex.: Lanterna)
void ComputarFocoLuz(Material mat, LuzFocada L, float3 pos, float3 normal, float3 toEye,
	out float4 ambiente, out float4 difuso, out float4 espec) {


	//
	//	Inicializando a saida
	//
	ambiente = float4(0.0f, 0.0f, 0.0f, 0.0f);
	difuso = float4(0.0f, 0.0f, 0.0f, 0.0f);
	espec = float4(0.0f, 0.0f, 0.0f, 0.0f);


	//
	//	Vetor da superficie para a luz
	//
	float3 luzVec = L.Posicao - pos;


	//
	//	Distancia da superficie para a luz
	//
	float d = length(luzVec);


	if (d > L.Alcance)
		return;

	//
	//	Normalizando o vetor da luz
	//
	luzVec /= d;



	//
	//	Adicionando luz ambiente
	//
	ambiente = mat.Ambiente * L.Ambiente;



	//
	//	Adicionando luz difusa e especular
	//
	float fatorDifusao = dot(luzVec, normal);


	//
	// Flatten
	//
	[flatten]
	if (fatorDifusao > 0.0f) {

		float3 v = reflect(-luzVec, normal);
		float valorEspec = pow(max(dot(v, toEye), 0), mat.Especular.w);

		difuso = fatorDifusao * mat.Difuso * L.Difuso;
		espec = valorEspec * mat.Especular * L.Especular;
	}

	//
	//	Escalar pelo fator de "foco" e atenuar
	//
	float spot = pow(max(dot(-luzVec, L.Direcao), 0), L.ConeExp);

	float att = spot / dot(L.Att, float3(1.0f, d, d*d));
	ambiente *= spot;
	difuso *= att;
	espec *= att;
}


















cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
	///
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	Material gMaterial;
	///
};


cbuffer cbPerFrame : register(b1)
{
	LuzDirecionada gLuzDir;
	PontoLuz gLuzPt;
	LuzFocada gLuzFoco;
	float3 gPosOlhoW;
};


struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
};


//
//	Main Shaders
//

float4 PS(VertexOut pin) : SV_Target
{
    //Interpolando a normal pode desnormaliza-la, entao iremos normalizar aqui
	pin.NormalW = normalize(pin.NormalW);

	
	float3 toEyeW = normalize(gPosOlhoW - pin.PosW);

	float4 ambiente = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 difuso = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 especular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 A, D, S;

	ComputarLuzDirecionada(gMaterial, gLuzDir, pin.NormalW, toEyeW, A, D, S);
	ambiente += A;
	difuso += D;
	especular += S;

	ComputarPontoLuz(gMaterial, gLuzPt, pin.PosW, pin.NormalW, toEyeW, A, D, S);
	ambiente += A;
	difuso += D;
	especular += S;

	ComputarFocoLuz(gMaterial, gLuzFoco, pin.PosW, pin.NormalW, toEyeW, A, D, S);
	ambiente += A;
	difuso += D;
	especular += S;


	float4 litColor = ambiente + difuso + especular;
	litColor.a = gMaterial.Difuso.a;

	return litColor;
}











