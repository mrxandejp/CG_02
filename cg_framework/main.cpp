#include "main.h"
#include "objLoader.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

using namespace std;
/*
// COORDENADAS ESPAÇO DO OBJETO
Vertice v1(IMAGE_WIDTH/2 -150, IMAGE_HEIGHT/2 + 150, 255, 0, 0, 255);
Vertice v2(IMAGE_WIDTH/2, IMAGE_HEIGHT/2 -150, 0, 255, 0, 255);
Vertice v3(IMAGE_WIDTH/2 +150, IMAGE_HEIGHT/2 + 150, 0, 0, 255, 255);
*/
/*
// Iniciação da variavel de rotação
float y_rotacao = 0.0;
*/

objLoader* objData; 
char arquivo[17] = "monkey_head2.obj";
float rotation_angle = 0.0; // ângulo delta de rotação.
//-----------------------------------------------------------------------------
void MyGlDraw(void) {
	
		objData = new objLoader();
		objData->load(arquivo);
		vector<glm::vec4> vec_array;
		for (int n = 0; n < objData->faceCount; n++) {
			obj_face* obj = objData->faceList[n];

			for (unsigned int i = 0; i < 3; i++) {
				vec_array.push_back(glm::vec4(objData->vertexList[obj->vertex_index[i]]->e[0], objData->vertexList[obj->vertex_index[i]]->e[1], objData->vertexList[obj->vertex_index[i]]->e[2], 1.0));
			}
		}

		vector<glm::vec4> objeto = vec_array;

		// PIPELINE GRÁFICO:

		// MODEL MATRIX (ESP. OBJETO -> ESP. UNIV):
		glm::mat4 i4 = glm::mat4( // matriz identsidade 4x4
			glm::vec4(1, 0, 0, 0),
			glm::vec4(0, 1, 0, 0),
			glm::vec4(0, 0, 1, 0),
			glm::vec4(0, 0, 0, 1)); 
		// Transformação aplicada -> rotação no eixo y
		glm::mat4 rotation = glm::mat4(
			glm::vec4(cos(rotation_angle), 0, -sin(rotation_angle), 0),
			glm::vec4(0,                   1,         0,            0),
			glm::vec4(sin(rotation_angle), 0, cos(rotation_angle),  0),
			glm::vec4(0,                   0,              0,       1));

		rotation_angle += 0.05; // vai girando o objeto em 0.05 grau por atualização.
		glm::mat4 modeling_matrix = i4 * rotation; // vamos aplicar uma rotação no objeto.

		// VIEW MATRIX (ESP. UNIV -> ESP. CAMERA):
		glm::vec3 pos = glm::vec3(0, 0, 5);
		glm::vec3 lookat = glm::vec3(0, 0, 0);
		glm::vec3 up = glm::vec3(0, 1, 0);
		
		//Base ortonormal da camera, coordenadas x y e z.
		glm::vec3 zc = -(lookat - pos)/glm::l1Norm(lookat - pos);
		glm::vec3 xc = glm::cross(up, zc)/glm::l1Norm(glm::cross(up, zc));
		glm::vec3 yc = glm::cross(zc, xc);

		glm::mat4 c_b = glm::mat4( 
			glm::vec4(xc, 0), 
			glm::vec4(yc, 0),
			glm::vec4(zc, 0),
			glm::vec4(0, 0, 0, 1));
		
		glm::mat4 c_t = glm::mat4(
			glm::vec4(1, 0, 0, -pos.x), // matriz T para formar a view matriz (fórmula no slide)
			glm::vec4(0, 1, 0, -pos.y),
			glm::vec4(0, 0, 1, -pos.z),
			glm::vec4(0, 0, 0, 1));
		
		glm::mat4 view_matrix = c_b * c_t;

		// PROJECTION MATRIX ESP. CAMERA -> ESP. DE RECORTE (CLIPING SPACE)
		double d = 3; // distância do centro de projeção para o viewplane
		glm::mat4 projection_matrix = glm::mat4(
			glm::vec4(1, 0, 0, 0),
			glm::vec4(0, 1, 0, 0),
			glm::vec4(0, 0, 1, d),
			glm::vec4(0, 0, -1/d, 0));
		// matriz model view proj, que leva do espaço de objeto para o esp. de recorte
		glm::mat4 mvp = modeling_matrix * view_matrix * projection_matrix; 

		// ESP. DE RECORTE -> ESP. CANÔNICO
		// aplicação da matriz mvp
		for (unsigned int i = 0; i < objeto.size(); i++) {
			objeto[i] = objeto[i] * mvp;
		}

		//Divide pela coordenada homogênea
		for (unsigned int i = 0; i < objeto.size(); i++) {
			objeto[i] = objeto[i] / objeto[i].w;
		}

		// VIEWPORT MATRIX (ESP.CANÔNICO -> ESPAÇO DA TELA (SCREEN SPACE))

		int width = 512;
		int height = 512;
		// inverte o eixo y
		glm::mat4 s1 = glm::mat4(
			glm::vec4(1, 0, 0, 0),
			glm::vec4(0, -1, 0, 0),
			glm::vec4(0, 0, 1, 0),
			glm::vec4(0, 0, 0, 1));
		// translação para a parte "positiva"
		glm::mat4 s2 = glm::mat4(
			glm::vec4(1, 0, 0, 0),
			glm::vec4(0, 1, 0, 0),
			glm::vec4(0, 0, 1, 0),
			glm::vec4((width - 1) / 2, (height - 1) / 2, 0, 1));
		// ajusta o tamanho para a tela (escala)
		glm::mat4 t = glm::mat4(
			glm::vec4(width / 2, 0, 0, 0),
			glm::vec4(0, height / 2, 0, 0),
			glm::vec4(0, 0, 1, 0),
			glm::vec4(1, 1, 0, 1));

		glm::mat4 viewport_matrix = s2 * t * s1;

		// aplica a viewport nos vértices
		for (unsigned int i = 0; i < objeto.size(); i++)
		{
			objeto[i] = round(viewport_matrix * objeto[i]);
		}
		// FIM DO PIPELINE GRÁFICO
		
		// limpa o buffer
		memset(FBptr, 0, width*height * 4);
	
		// rasteriza na tela, usando a função do primeiro trabalho
		for (unsigned int i = 0; i < objeto.size(); i += 3) {

			Vertice v1(objeto[i][0],	 objeto[i][1],	   255, 255, 255, 255);
			Vertice v2(objeto[i + 1][0], objeto[i + 1][1], 255, 255, 255, 255);
			Vertice v3(objeto[i + 2][0], objeto[i + 2][1], 255, 255, 255, 255);

			DrawTriangle(v1, v2, v3);
		}
	

} // fim GLDRAW

//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	// Inicializações.
	InitOpenGL(&argc, argv);
	InitCallBacks();
	InitDataStructures();

	// Ajusta a função que chama as funções do mygl.h
	DrawFunc = MyGlDraw;	

	// Framebuffer scan loop.
	glutMainLoop();

	return 0;
}

