# ICG - Implementação de um pipeline gráfico com linguagem C++
Aqui temos uma breve explicação sobre a realização da segunda atividade da disciplina Introdução a computação gráfica, ministrada pelo professor Christan Azambuja Pagot. O objetivo da atividade era implementar um pipeline gráfico que converte os vértices desde o espaço do objeto, até o espaço de tela, e posteriormente podemos desenhar as primitivas utilizando as funções que construímos na primeira etapa.

Endereço da primeira parte do trabalho: https://github.com/joseolivio/CG_01

## Entendendo o pipeline gráfico
O pipeline gráfico, é um modelo conceitual que descreve quais etapas um sistema gráfico precisa executar para renderizar uma cena 3D para uma tela 2D. Uma vez que um modelo 3D tenha sido criado, por exemplo, em um videogame ou qualquer outra animação 3D, o pipeline gráfico é o processo de transformar esse modelo 3D no que o computador exibe na tela. Ao todo são 5 transformações, como pode ser visto na figura abaixo. As transformações de um espaço para outro são feitas através de matrizes, que multiplicam os vértices do objeto, as matrizes podem ser combinadas para otimizar este processo.

--- FIGURA P1 ---

## 1. Transformação: Espaço do Objeto → Espaço do Universo
A descrição do modelo é feita no sistema de coordenadas do objeto, ou simplismente espaço do objeto. O objeto é descrito no centro do espaço de coordenadas. No próximo espaço, o espaço de mundo, ou espaço de universo, os objetos estão organizados em uma cena com outros objetos e podem estar deslocados da origem. A matriz que realiza esta transformação é denominada matriz de modelagem (ou modeling matrix). A matriz de modelagem é composta por uma sequência de transformações geométricas que posicionam o modelo no universo. As transformações que podem compor a matriz de modelagem são a rotação, translação, escala e o cisalhamento (shear). No nosso pipeline a transformação que faremos é de uma translação no eixo y, a matriz de translação é multiplicada da matriz identidade formando a nossa model matrix.


figura 2

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
