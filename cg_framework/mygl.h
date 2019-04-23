#ifndef _MYGL_H_
#define _MYGL_H_

#include "definitions.h"
#include "math.h"
#include <vector>

using namespace std;

class Vertice
{
  int posX;
  int posY;
  int red;
  int green;
  int blue;
  int alfa;

  public:
  	void setPosX(int x);
  	void setPosY(int y);
  	void setR(int r);
  	void setG(int g);
  	void setB(int b);
  	void setA(int a);

  	int getPosX();
  	int getPosY();
  	int getR();
  	int getG();
  	int getB();
  	int getA();
  	Vertice(int x, int y, int r, int g, int b, int a);
};

Vertice::Vertice(int x, int y, int r, int g, int b, int a){
	posX = x;
	posY = y;
	red = r;
	green = g;
	blue = b;
	alfa = a;
}

void Vertice::setPosX(int x){
	posX = x;
}

void Vertice::setPosY(int y){
	posY = y;
}

void Vertice::setR(int r){
	red = r;
}

void Vertice::setG(int g){
	green = g;
}

void Vertice::setB(int b){
	blue = b;
}

void Vertice::setA(int a){
	alfa = a;
}

int Vertice::getPosX(){
	return posX;
}

int Vertice::getPosY(){
	return posY;
}

int Vertice::getR(){
	return red;
}

int Vertice::getG(){
	return green;
}

int Vertice::getB(){
	return blue;
}

int Vertice::getA(){
	return alfa;
}
//*****************************************************************************
// Defina aqui as suas funções gráficas
//*****************************************************************************

void PutPixel(Vertice pixel){
	int posicao;

	posicao = 4 * (pixel.getPosX() + (pixel.getPosY()*512));

	FBptr[posicao] = pixel.getR(); // componente R
	FBptr[posicao+1] = pixel.getG(); // componente G
	FBptr[posicao+2] = pixel.getB(); // componente B
	FBptr[posicao+3] = pixel.getA(); // componente A

}
void InverteVertice(Vertice &pixel1, Vertice &pixel2){
	Vertice troca(pixel1.getPosX(),pixel1.getPosY(),pixel1.getR(),pixel1.getG(),pixel1.getB(),pixel1.getA());
	pixel1.setPosX(pixel2.getPosX());
	pixel1.setPosY(pixel2.getPosY());
	pixel1.setR(pixel2.getR());
	pixel1.setG(pixel2.getG());
	pixel1.setB(pixel2.getB());
	pixel1.setA(pixel2.getA());

	pixel2.setPosX(troca.getPosX());
	pixel2.setPosY(troca.getPosY());
	pixel2.setR(troca.getR());
	pixel2.setG(troca.getG());
	pixel2.setB(troca.getB());
	pixel2.setA(troca.getA());
	
}

void Interpolacao(Vertice aux, Vertice pixel1, Vertice pixel2){
	Vertice inter(aux.getPosX(),aux.getPosY(), 0, 0, 0, 0);

	float dCor[4];
	int variacao;
	float incCor[4];
	int dx = pixel2.getPosX() - pixel1.getPosX();
	int dy = pixel2.getPosY() - pixel1.getPosY();
	
	dCor[0] = pixel2.getR() - pixel1.getR();
	dCor[1] = pixel2.getG() - pixel1.getG();
	dCor[2] = pixel2.getB() - pixel1.getB();
	dCor[3] = pixel2.getA() - pixel1.getA();

	if(dx == 0 || (abs(dy) > abs(dx))){
		variacao = aux.getPosY() - pixel1.getPosY();
		for (int i = 0; i < 4; i++)
		{
			incCor[i] = dCor[i]/dy;
		}
	} else if((abs(dx) == abs(dy) ) || (abs(dx) > abs(dy)) ){
		variacao = aux.getPosX() - pixel1.getPosX();
		for (int i = 0; i < 4; i++)
		{
			incCor[i] = dCor[i]/dx;
		}
	}

	inter.setR(pixel1.getR() + incCor[0]*variacao);
	inter.setG(pixel1.getG() + incCor[1]*variacao);
	inter.setB(pixel1.getB() + incCor[2]*variacao);
	inter.setA(pixel1.getA() + incCor[3]*variacao);

	PutPixel(inter);
}


void DrawLine(Vertice pixel1, Vertice pixel2){
	if (pixel2.getPosX() < pixel1.getPosX())
	{
		InverteVertice(pixel1, pixel2);
		cout << "INVERTIDO" << endl;
	}

	int dx = pixel2.getPosX() - pixel1.getPosX();
	int dy = pixel2.getPosY() - pixel1.getPosY();
	cout << "DX: "<< dx <<"		DY: " << dy << endl;
	PutPixel(pixel1);

	if(dx > 0 && dy > 0 && abs(dx) > abs(dy)){ // 1º e 5º OCTANTE
		int d = 2 * dy - dx;
		int incr_e = 2 * dy;
		int incr_ne = 2 * (dy - dx);
		int x = pixel1.getPosX();
		int y = pixel1.getPosY();


		Vertice aux(x, y, pixel1.getR(), pixel1.getG(), pixel1.getB(), pixel1.getA());

		while(aux.getPosX() < pixel2.getPosX()){
			Interpolacao(aux, pixel1, pixel2);
			if (d <= 0){
				d += incr_e;
				aux.setPosX(aux.getPosX()+1);
			} else{
				d += incr_ne;
				aux.setPosX(aux.getPosX()+1);
				aux.setPosY(aux.getPosY()+1);
			}
		}	
	} else if(dx > 0 && dy > 0 && abs(dy) > abs(dx)){ // 2º e 6º OCTANTE
		cout << "OCTANTE 2 OU 6" << endl;
		int d = 2 * dx - dy;
		int incr_e = 2 * dx;
		int incr_ne = 2 * (dx - dy);
		int x = pixel1.getPosX();
		int y = pixel1.getPosY();

		Vertice aux(x, y, pixel1.getR(), pixel1.getG(), pixel1.getB(), pixel1.getA());

		while(aux.getPosY() < pixel2.getPosY()){
			Interpolacao(aux, pixel1, pixel2);
			if (d <= 0){
				d += incr_e;
				aux.setPosY(aux.getPosY()+1);
			} else{
				d += incr_ne;
				aux.setPosY(aux.getPosY()+1);
				aux.setPosX(aux.getPosX()+1);
			}
		}	

	} else if(dx > 0 && dy < 0 && abs(dx) > abs(dy)){ // 4º e 8º OCTANTE
		cout << "OCTANTE 4 OU 8" << endl;
		int d = 2 * dy + dx;
		int incr_e = 2 * dy;
		int incr_ne = 2 * (dy + dx);
		int x = pixel1.getPosX();
		int y = pixel1.getPosY();

		Vertice aux(x, y, pixel1.getR(), pixel1.getG(), pixel1.getB(), pixel1.getA());

		while(aux.getPosX() < pixel2.getPosX()){
			Interpolacao(aux, pixel1, pixel2);
			if (d >= 0){
				d += incr_e;
				aux.setPosX(aux.getPosX()+1);
			} else{
				d += incr_ne;
				aux.setPosX(aux.getPosX()+1);
				aux.setPosY(aux.getPosY()-1);
			}
		}	

	} else if(dx > 0 && dy < 0 && abs(dy) > abs(dx)){ // 3º e 7º OCTANTE
		cout << "OCTANTE 3 OU 7" << endl;
		int d = 2 * dy + dx;
		int incr_e = 2 * dx;
		int incr_ne = 2 * (dy + dx);
		int x = pixel1.getPosX();
		int y = pixel1.getPosY();

		Vertice aux(x, y, pixel1.getR(), pixel1.getG(), pixel1.getB(), pixel1.getA());

		while(aux.getPosY() > pixel2.getPosY()){
			Interpolacao(aux, pixel1, pixel2);
			if (d <= 0){
				d += incr_e;
				aux.setPosY(aux.getPosY()-1);
			} else{
				d += incr_ne;
				aux.setPosX(aux.getPosX()+1);
				aux.setPosY(aux.getPosY()-1);
			}
		}	

	} else if(dx == 0){
		int x = pixel1.getPosX();
		int y = pixel1.getPosY();

		Vertice aux(x, y, pixel1.getR(), pixel1.getG(), pixel1.getB(), pixel1.getA());

		if(dy > 0){
			while(aux.getPosY() < pixel2.getPosY()){
				Interpolacao(aux, pixel1, pixel2);
				aux.setPosY(aux.getPosY()+1);
			}	
		} else if(dy < 0){
			while(aux.getPosY() > pixel2.getPosY()){
				Interpolacao(aux, pixel1, pixel2);
				aux.setPosY(aux.getPosY()-1);
			}
		}
		
	} else if(dy == 0){
		int x = pixel1.getPosX();
		int y = pixel1.getPosY();

		Vertice aux(x, y, pixel1.getR(), pixel1.getG(), pixel1.getB(), pixel1.getA());

		while(aux.getPosX() < pixel2.getPosX()){
			Interpolacao(aux, pixel1, pixel2);
			aux.setPosX(aux.getPosX()+1);
		}

	} else if(dx == dy){
		int x = pixel1.getPosX();
		int y = pixel1.getPosY();

		Vertice aux(x, y, pixel1.getR(), pixel1.getG(), pixel1.getB(), pixel1.getA());

		while(aux.getPosX() < pixel2.getPosX()){
			Interpolacao(aux, pixel1, pixel2);
			aux.setPosX(aux.getPosX()+1);
			aux.setPosY(aux.getPosY()+1);
		}
	}
	PutPixel(pixel2);
}


void DrawTriangle(Vertice pixel1, Vertice pixel2, Vertice pixel3){
	DrawLine(pixel1, pixel2);
	DrawLine(pixel2, pixel3);
	DrawLine(pixel1, pixel3);
}


/*void DesenhaPixels(void)
{
	// Escreve um pixel vermelho na posicao (0,0) da tela:
	FBptr[0] = 255; // componente R
	FBptr[1] = 0; // componente G
	FBptr[2] = 0; // componente B
	FBptr[3] = 255; // componente A
	// Escreve um pixel verde na posicao (1,0) da tela:
	FBptr[4] = 0; // componente R
	FBptr[5] = 255; // componente G
	FBptr[6] = 0; // componente B
	FBptr[7] = 255; // componente A
	// Escreve um pixel azul na posicao (2,0) da tela:
	FBptr[8] = 0; // componente R
	FBptr[9] = 0; // componente G
	FBptr[10] = 255; // componente B
	FBptr[11] = 255; // componente A
}

void DesenhaLinha(void)
{
	for (unsigned int i=0; i<250; i++)
	{
	FBptr[4*i + 4*i*IMAGE_WIDTH + 0] = 255;
	FBptr[4*i + 4*i*IMAGE_WIDTH + 1] = 0;
	FBptr[4*i + 4*i*IMAGE_WIDTH + 2] = 255;
	FBptr[4*i + 4*i*IMAGE_WIDTH + 3] = 255;
	}
}*/
#endif // _MYGL_H_

