# ICG - Implementação de um pipeline gráfico com linguagem C++
Aqui temos uma breve explicação sobre a realização da segunda atividade da disciplina Introdução a computação gráfica, ministrada pelo professor Christan Azambuja Pagot. O objetivo da atividade era implementar um pipeline gráfico que converte os vértices desde o espaço do objeto, até o espaço de tela, e posteriormente podemos desenhar as primitivas utilizando as funções que construímos na primeira etapa.

Endereço da primeira parte do trabalho: https://github.com/joseolivio/CG_01

### Entendendo o pipeline gráfico
O pipeline gráfico, é um modelo conceitual que descreve quais etapas um sistema gráfico precisa executar para renderizar uma cena 3D para uma tela 2D. Uma vez que um modelo 3D tenha sido criado, por exemplo, em um videogame ou qualquer outra animação 3D, o pipeline gráfico é o processo de transformar esse modelo 3D no que o computador exibe na tela. Ao todo são 5 transformações, como pode ser visto na figura abaixo. As transformações de um espaço para outro são feitas através de matrizes, que multiplicam os vértices do objeto, as matrizes podem ser combinadas para otimizar este processo. O trabalho foi feito em línguagem C++ e utilizamos a biblioteca glm para operações matemáticas envolvendo matrizes e vetores.

![p1](https://user-images.githubusercontent.com/31492509/56624879-1ab41f00-6611-11e9-8cbb-1f28a1e1dfdb.png)

### 1. Transformação: Espaço do Objeto → Espaço do Universo
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
### 2. Transformação: Espaço do Universo → Espaço da Câmera
Esta etapa do pipeline é responsável por transformar vértices do espaço do universo para o espaço da câmera. A "camêra" é o ponto de vista da cena, a camêra pode ser posicionada em várias posições durante uma cena, para fazer esta transformação vamos realizar uma mudança de base, ou seja, transformar as coordenadas para um novo sistema. Este novo sistema é definido através de três informações:
- A posição da camêra, que indica onde a camera está posicionada no espaço de universo.
- A direção da camêra, que mostra para onde a camera está apontando.
- O vetor "up", que indica a parte de cima da camêra, nos dando a posição correta no eixo.
A matriz que realiza esta mudaná é chama de matriz de visualização, ou view matrix. Ela é composta por uma translação e uma rotação, seus valores podem ser definidos por operações matemáticas a partir dos três valores de entrada descritos acima.

![3](https://user-images.githubusercontent.com/31492509/56625247-d0cc3880-6612-11e9-9e02-d3712026b545.png)

Trecho do código que descreve a matriz de visualização:
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
### 3. Transformação: Espaço da Câmera → Espaço de Recorte
Esta etapa do pipeline é responsável por transformar vértices do espaço da câmera para o espaço de recorte (ou projetivo). Aqui podemos adicionar a distorção perpectiva, que faz com que objetos distantes da camêra sofram uma deformação aparentando serem menores e objetos próximos à camêra aparentam ser maiores. É o efeito que simula como nosso cérebro interpreta objetos distantes, você pode observar esta distorção em rodovias onde as pontas do acostamento parecem se tocar no "infinito".

![GO-118-trecho-Divisa-GO-DF-Campos-Belos](https://user-images.githubusercontent.com/31492509/56627143-f3624f80-661a-11e9-9f9b-98c8d22d480c.jpg)

A matriz que realiza esta transformação é chamada de matriz de projeção, ou projection matrix, ela é definida a partir da distância da camêra para o plano de visualização que chamamos geralmente de "d". Aqui nossa coordenada homogênea, que vou entrar em mais detalhes a frente, provavelmente terá seu valor alterado de 1.

![5](https://user-images.githubusercontent.com/31492509/56627139-f3624f80-661a-11e9-8d8d-3b32ce5ac447.png)

Trecho do código que descreve a matriz de projeção:
```
	// PROJECTION MATRIX ESP. CAMERA -> ESP. DE RECORTE (CLIPING SPACE)
	double d = 3; // distância do centro de projeção para o viewplane
	glm::mat4 projection_matrix = glm::mat4(
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, 1, d),
		glm::vec4(0, 0, -1 / d, 0));
	// matriz model view proj, que leva do espaço de objeto para o esp. de recorte
	glm::mat4 mvp = modeling_matrix * view_matrix * projection_matrix;
```

### 4. Transformação: Espaço de Recorte → Espaço “Canônico"
Esta etapa do pipeline é responsável por transformar pontos do espaço de recorte para o espaço canônico. Isto é feito em duas etapas: 
Primeiro, vamos dividir as coordenadas dos vértices no espaço de recorte pela sua coordenada homogênea, ou seja, o quarto valor do vetor. Esta transformação gera uma alteração da geometria da cena, tornando menores os objetos que estiverem mais distantes da câmera.
```
	//Divide pela coordenada homogênea
	for (unsigned int i = 0; i < objeto.size(); i++) {
		objeto[i] = objeto[i] / objeto[i].w;
	}
```
E vamos aplicar a combinação das três matrizes definidas nos passos 1, 2 e 3 nos vértices, transformando-os para o espaço canônico, onde o hexahedro anterior se torna um cubo de coordenadas unitárias e centrado na origem.
``` 
	// aplicação da matriz mvp
	for (unsigned int i = 0; i < objeto.size(); i++) {
		objeto[i] = objeto[i] * mvp;
	}
```

![6](https://user-images.githubusercontent.com/31492509/56627141-f3624f80-661a-11e9-8994-30b211f12455.png)

### 5. Transformação: Espaço Canônico → Espaço de Tela"
Por fim, vamos transformar as coordenadas no espaço canônico para o espaço da tela, já falamos sobre o espaço na tela no primeiro trabalho da disciplina, mas sabemos que ele é bidimensional, ou seja, temos apenas coordenadas x e y. 

![7](https://user-images.githubusercontent.com/31492509/56627142-f3624f80-661a-11e9-908f-c80ea3f6b716.png)


A matriz que realiza esta transformação é chamada de ViewPort, e pode ser quebrada em três partes.
- Primeiro vamos inverter o eixo y de coordenadas, pois sabemos que na tela o eixo y parte de cima para baixo.
```
	// inverte o eixo y
	glm::mat4 s1 = glm::mat4(
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, -1, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(0, 0, 0, 1));
```

- Depois vamos fazer uma escala nos eixos x e y para que se adequem as coordenadas da tela em questão, por isso recebe como entrada valores w (width) e h (heigth) que são os valores de largura e altura da tela respectivamente.
```
	// ajusta o tamanho para a tela (escala)
	glm::mat4 t = glm::mat4(
		glm::vec4(width / 2, 0, 0, 0),
		glm::vec4(0, height / 2, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(1, 1, 0, 1));
```
- E vamos por fim transladar o nosso bloco para que existam apenas valores "positivos" diferente do espaço canônico onde o centro de projeção era na origem.
```
	// translação para a parte "positiva"
	glm::mat4 s2 = glm::mat4(
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4((width - 1) / 2, (height - 1) / 2, 0, 1));
```
A matriz viewport de fato é a combinação destas três matrizes, e podemos aplicar nos vértices, encerrando o pipeline gráfico.
```
	glm::mat4 viewport_matrix = s2 * t * s1;

	// aplica a viewport nos vértices
	for (unsigned int i = 0; i < objeto.size(); i++)
	{
		objeto[i] = round(viewport_matrix * objeto[i]);
	}
```
Essas dimensões são configuráveis para cada tela, no nosso trabalho a janela possui dimensão 512x512, ou seja o valor de largura e altura eram iguais, ambos 512.

![8](https://user-images.githubusercontent.com/31492509/56627153-fd844e00-661a-11e9-9995-33a0ccadf593.png)

### 6. Rasterização
Esta etapa gera a rasterização dos modelos no espaço de tela, ou seja, "desenha" a cena na tela de fato. Nós já fizemos um algoritmo de rasterização na primeira etapa do trabalho, então reaproveitamos o código aqui, foi necessário apenas pegar os valores de coordenadas geradas pelo pipeline, criar um objeto vértice com tais coordenadas e rasterizar as primitivas (triângulos) na tela.
Uma coisa importante é limpar o buffer do objeto, se não as rasterizações iriam se sobrepor.
```
	// limpa o buffer
	memset(FBptr, 0, width*height * 4);

	// rasteriza na tela, usando a função do primeiro trabalho
	for (unsigned int i = 0; i < objeto.size(); i += 3) {

		Vertice v1(objeto[i][0], objeto[i][1], 255, 255, 255, 255);
		Vertice v2(objeto[i + 1][0], objeto[i + 1][1], 255, 255, 255, 255);
		Vertice v3(objeto[i + 2][0], objeto[i + 2][1], 255, 255, 255, 255);

		DrawTriangle(v1, v2, v3);
	}
```

### Coordenadas homogêneas
Você deve ter observado que durante nosso pipeline utilizamos matrizes de dimensão 4x4, mesmo trabalhando com objetos 3d (3x3) isso acontence pois utilizamos as chamadas coordenadas homogêneas, que nos dão diversas vantagens, por exemplo, não conseguimos representar a transformação de rotação 3d em uma matriz 3x3, pois não ficaria linear, mas se utilizarmos coordenadas homogêneas isso se torna possível, geralmente a coordenada homogênea tem valor 1, mas durante o pipeline esse valor pode ser alterado.

![ch](https://user-images.githubusercontent.com/31492509/56627154-fd844e00-661a-11e9-8b82-a252f6ff0342.png)

### Resultados
Para verificar os resultados do nosso pipeline, utilizamos uma biblioteca do professor que carrega modelos 3d, o modelo escolhido foi a cabeça de um macaco. Os valores foram lidos através de uma função e armazenados em um vetor de coordenadas, e após passar por nosso pipeline e rasterizar na tela temos o seguinte resultado:

![WhatsApp Image 2019-04-23 at 23 05 18](https://user-images.githubusercontent.com/31492509/56627530-936ca880-661c-11e9-860c-59fca392af34.jpeg)

Para comparar e verificar realmente o funcionamento do pipeline, fizemos a mesma rasterização do modelo 3 do macaco só que utilizando um código em OpenGL, e ambos sistemas geraram os mesmos resultados.

![WhatsApp Image 2019-04-23 at 12 59 04](https://user-images.githubusercontent.com/31492509/56627298-92874700-661b-11e9-83ce-38a77c32da0a.jpeg)

### Considerações:
Ficamos satisfeitos com o resultado final da atividade, é muito gratificante pegar todo esse conhecimento teórico aplicar na prática e ver que realmente funciona, as maiores dificuldades que tivemos no trabalho não foi a implementação do pipeline em si, mas sim nas estruturas de dados que iriamos utilizar para salvar os valores do objeto 3d e na utilização da biblioteca GLM.

### Referências:
Algumas fontes que nos auxiliaram durante o desenvolvimento do trabalho;

Fundamentals of Computer Graphics - Peter Shirley, Michael Ashikhmin, Steve Marschner

Notas de Aula do Prof. Christian

https://en.wikipedia.org/wiki/Graphics_pipeline



  
  
