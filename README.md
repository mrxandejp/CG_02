# ICG - Implementação de um pipeline gráfico com linguagem C++
Aqui temos uma breve explicação sobre a realização da segunda atividade da disciplina Introdução a computação gráfica, ministrada pelo professor Christan Azambuja Pagot. O objetivo da atividade era implementar um pipeline gráfico que converte os vértices desde o espaço do objeto, até o espaço de tela, e posteriormente podemos desenhar as primitivas utilizando as funções que construímos na primeira etapa.

Endereço da primeira parte do trabalho: https://github.com/joseolivio/CG_01

## Entendendo o pipeline gráfico
O pipeline gráfico, é um modelo conceitual que descreve quais etapas um sistema gráfico precisa executar para renderizar uma cena 3D para uma tela 2D. Uma vez que um modelo 3D tenha sido criado, por exemplo, em um videogame ou qualquer outra animação 3D, o pipeline gráfico é o processo de transformar esse modelo 3D no que o computador exibe na tela. Ao todo são 5 transformações, como pode ser visto na figura abaixo. As transformações de um espaço para outro são feitas através de matrizes, que multiplicam os vértices do objeto, as matrizes podem ser combinadas para otimizar este processo. O trabalho foi feito em línguagem C++ e utilizamos a biblioteca glm para operações matemáticas envolvendo matrizes e vetores.
![p1](https://user-images.githubusercontent.com/31492509/56624879-1ab41f00-6611-11e9-8cbb-1f28a1e1dfdb.png)


## 1. Transformação: Espaço do Objeto → Espaço do Universo
A descrição do modelo é feita no sistema de coordenadas do objeto, ou simplismente espaço do objeto. O objeto é descrito no centro do espaço de coordenadas. No próximo espaço, o espaço de mundo, ou espaço de universo, os objetos estão organizados em uma cena com outros objetos e podem estar deslocados da origem. A matriz que realiza esta transformação é denominada matriz de modelagem (ou modeling matrix). A matriz de modelagem é composta por uma sequência de transformações geométricas que posicionam o modelo no universo. As transformações que podem compor a matriz de modelagem são a rotação, translação, escala e o cisalhamento (shear). No nosso pipeline a transformação que faremos é de uma translação no eixo y, a matriz de translação é multiplicada da matriz identidade formando a nossa model matrix.


![2](https://user-images.githubusercontent.com/31492509/56624875-1982f200-6611-11e9-8f46-03cbfd66981c.png)

Trecho do código que descreve a matriz de modelagem:
```
	// MODEL MATRIX (ESP. OBJETO -> ESP. UNIV):
	glm::mat4 i4 = glm::mat4( // matriz identsidade 4x4
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(0, 0, 0, 1));
	// Transformação aplicada -> rotação no eixo y
	glm::mat4 rotation = glm::mat4(
		glm::vec4(cos(rotation_angle), 0, -sin(rotation_angle), 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(sin(rotation_angle), 0, cos(rotation_angle), 0),
		glm::vec4(0, 0, 0, 1));

	glm::mat4 modeling_matrix = i4 * rotation; // vamos aplicar uma rotação no objeto.
  ```
  
  ## 2. Transformação: Espaço do Universo → Espaço da Câmera
Esta etapa do pipeline é responsável por transformar vértices do espaço do universo para o espaço da câmera. A "camêra" é o ponto de vista da cena, a camêra pode ser posicionada em várias posições durante uma cena, para fazer esta transformação vamos realizar uma mudança de base, ou seja, transformar as coordenadas para um novo sistema. Este novo sistema é definido através de três informações:
- A posição da camêra, que indica onde a camera está posicionada no espaço de universo.
- A direção da camêra, que mostra para onde a camera está apontando.
- O vetor "up", que indica a parte de cima da camêra, nos dando a posição correta no eixo.
A matriz que realiza esta mudaná é chama de matriz de visualização, ou view matrix. E seus valores podem ser definidos por operações matemáticas a partir dos três valores de entrada descritos acima.

figura 3 

```
	// VIEW MATRIX (ESP. UNIV -> ESP. CAMERA):
	glm::vec3 pos = glm::vec3(0, 0, 5);
	glm::vec3 lookat = glm::vec3(0, 0, 0);
	glm::vec3 up = glm::vec3(0, 1, 0);

	//Base ortonormal da camera, coordenadas x y e z.
	glm::vec3 zc = -(lookat - pos) / glm::l1Norm(lookat - pos);
	glm::vec3 xc = glm::cross(up, zc) / glm::l1Norm(glm::cross(up, zc));
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
```




  
  
