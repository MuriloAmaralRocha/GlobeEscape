/*
	-------------------------------------------------------------------------------------------------------

	Projeto: GLOBE ESCAPE
	Desenvolvido por: Ca�que de Campos Mendon�a
					  Elizabete Mitie Kirino
					  Kalil Nogueira Farias
					  Murilo do Amaral Rocha
					  Vin�cius Duarte Furukawa

	*�udios retirados dos sites www.epidemicsound.com e https://freesound.org
	*Imagens produzidas pelo pr�prio grupo.

	-------------------------------------------------------------------------------------------------------

	Parte dos dados das fases s�o carregados do arquivo fases.txt segundo a seguinte l�gica:
		1 -> virar no sentido hor�rio
		2 -> virar no sentido anti hor�rio
		3 -> afastar do centro
		4 -> aproximar do centro
		M�ltiplos de 10, mas n�o de 100 -> indica qual seta deve estar l� e cria uma chave na posi��o
		M�tiplos de 100 -> indica qual seta deve estar l� e cria um obst�culo na posi��o
		-1 -> passa para o pr�ximo anel
		-5 -> final da fase

	-------------------------------------------------------------------------------------------------------

	Como jogar:
		WASD -> move o cursor
		setas -> move o tabuleiro
		ESPA�O -> libera o personagem para se movimentar
		M -> mute/desmute
		R -> reiniciar a posi��o do boneco de neve e as chaves
		ESC -> pause

	Objetivo:
		O objetivo do jogo � arrumar o tabuleiro (s� � poss�vel mov�-lo com o personagem parado) que cont�m a setas
		(ou todas as setas de um anel ou todas as setas de uma fatia) para criar um caminho em que o boneco de neve possa passar.
		O boneco de neve n�o pode bater em nenhum obst�culo ou ele volta para o come�o e perde as chaves j� coletadas.
		Caso o boneco tenha coletado todas as chaves e chegado ao fim (c�rculo verde), o jogador passa de fase.

	-------------------------------------------------------------------------------------------------------
*/

//BIBLIOTECAS
#include<stdio.h>
#include<stdlib.h>
#include<graphics.h>
#include<conio.h>
#include<time.h>
#include<windows.h>


//CONSTANTES
#define TAMANHO 720            			//Tamanho da tela
#define TEMPO_DESENHO 200         		//Tempo em milisegundos entre cada atualiza��o na tela
#define TOTAL_DE_FASES 4				//Quantidade de fases
#define SPRITE_COMPLEMENTOS 64			//Tamanho dos sprites de objetos complementares
#define CICLO_DESENHO 4					//Vezes necess�rias para que a sprite do personagem parado pisque
#define TECLA_A 65
#define TECLA_D 68
#define TECLA_M 77
#define TECLA_R 82
#define TECLA_S 83
#define TECLA_W 87


//STRUCTS
struct Coordenadas{
	int anel, fatia;
};
struct Pixel{
	int x, y;
};

//VARI�VEIS GLOBAIS
Coordenadas total, posicaoPersonagem;				//Total de aneis e fatias na fase e posi��o atual do personagem no mapa
Coordenadas inicioEFim[TOTAL_DE_FASES][2];			//Guarda as posi��es iniciais e finais do personagem
Pixel **posicoes;									//Guarda as coordenadas em pixels para fazer os desenhos
int fase = 0;										//Fase atual do jogo
void *fundo;										//Imagem do fundo
void *roda, *mascaraRoda;							//Imagem da roda e sua m�scara
int **aneisEFatias;									//Dados dos an�is e fatias carregados do arquivo fases.txt
void **setas, **mascaraSetas;						//Imagens das setas e suas m�scaras
void ***personagem, ***mascaraPersonagem;			//Sprite do personagem e sua m�scara
void **objetos, **mascaraObjetos;					//0 -> obst�culo; 1 -> cursor; 2 -> in�cio; 3 -> fim; 4 -> chave
Coordenadas *obstaculos, *chaves;					//Posi��es em que h� obst�culos e chaves
bool *pegouChaves;									//Indica quais chaves foram pegas
int quantidadeDeObstaculos, quantidadeDeChaves;		//Total de obst�culos e de chaves em uma fase
bool venceu;										//Indica se o jogador venceu ou n�o
Pixel spritePersonagem;								//Usada para controlar qual sprite do personagem deve aparecer
void *menuPrincipal;								//Imagem menu principal
void **telaLoading;									//Imagem loado
void *textoLoading, *mascaraTextoLoading;			//Imagem do texto "Carregando" e sua m�scara
void *vitoria, *mascaraVitoria;						//Imagem do texto "Vit�ria" e sua m�scara
void *telaFinal;									//Imagem de fundo da tela da vit�ria
void *imagemPause;									//Imagem de pause


//FUN��ES
void liberarMemoria();
bool compararCoordenadas(Coordenadas primeira, Coordenadas segunda);
void jogar();
int menu();
bool pause();
void carregarFundo();
void desenharFundo();
void carregarAneisEFatias();
void carregarPosicoes();
void carregarSetas();
void desenharSetas();
void carregarPersonagem();
void desenharPersonagem(bool mostrarPersonagem);
Coordenadas moverCursor(Coordenadas posicao);
int moverCursorSentidoHorario(int fatia);
int moverCursorSentidoAntiHorario(int fatia);
int afastarCursorDoCentro(int anel);
int aproximarCursorDoCentro(int anel);
void desenharCursor(Coordenadas cursor);
void moverTabuleiro(Coordenadas referencia);
void moverTabuleiroSentidoHorario(int anel);
void moverTabuleiroSentidoAntiHorario(int anel);
void afastarTabuleiroDoCentro(int fatia);
void aproximarTabuleiroDoCentro(int fatia);
void embaralharTabuleiro();
void carregarIncioEFim();
bool moverPersonagem(bool movimentoPersonagem);
void moverPersonagemSentidoHorario();
void moverPersonagemSentidoAntiHorario();
void afastarPersonagemDoCentro();
void aproximarPersonagemDoCentro();
void carregarObjetos();
void carregarObstaculosEChaves();
void desenharChaves();
void desenharObstaculos();
void desenharInicio();
void desenharFim();
void carregarFase();
bool passarDeFase(bool movimentoPersonagem);
void mudarDeFase();
bool setVolume(bool volume);
void carregarMenuPrincipal();
void desenharMenuPrincipal();
void carregarTelaLoading();
void desenharTelaLoading();
void carregarTelaVitoria();
void desenharTelaVitoria();
void carregarPause();
void desenharPause();


//COME�O DO PROGRAMA
int main(){
	int opcao = 0;			//Op��o de tela
	bool tela = true;		//Vari�vel do loop

	initwindow(TAMANHO, TAMANHO, "Globe Escape");	//Inicializar tela do jogo
	waveOutSetVolume(0,0xFFFFFFFF);				 	//Altura dos sons

	//Carregar parte das telas do jogo
	setactivepage(2);     
	carregarMenuPrincipal();
	carregarTelaLoading();
	carregarTelaVitoria();
	carregarPause();
    setvisualpage(1);
	carregarIncioEFim();

	//Loop
	while(tela){
		switch(opcao){
			case 0:		//Menu Principal
				opcao = menu();
				break;
			case 1:		//Jogar
				jogar();
				opcao = 0;
				break;
			default:	//Sair do jogo
				tela = false;
		}
	}

	liberarMemoria();
	closegraph();
	return(0);
}


//CORPO DAS FUN��ES
void jogar(){
	/*
		Fun��o onde ocorre toda a l�gica do jogo, executada enquanto o jogador est� jogando
	*/

	//Vari�veis locais
	bool sair = false, movimentoPersonagem = false, volume = true, mostrarPersonagem = true;
  	unsigned long long gt1, gt2;
  	int pagina = 1, contadorMostrarPersonagem = 0;
  	Coordenadas cursor;
  	cursor.anel = 0;	cursor.fatia = 0;
  	
  	//set de vari�veis globais
  	venceu = false;

  	//Tocar trilha sonora do jogo
  	mciSendString("open .\\audios\\trilha.mp3 type MPEGVideo alias trilha", NULL, 0, 0); 
    mciSendString("play trilha repeat", NULL, 0, 0);

  	carregarFase();

  	srand(time(NULL));
  	embaralharTabuleiro();

  	gt1 = GetTickCount();

  	//La�o principal
  	while(!sair){
  		gt2 = GetTickCount();

  		if(gt2 - gt1 > TEMPO_DESENHO){
  			gt1 = gt2;

  			if(pagina == 1)
	        	pagina = 2;
	      	else
	        	pagina = 1;
	      
	      	setactivepage(pagina);
	        cleardevice();

	        cursor = moverCursor(cursor);
	        movimentoPersonagem = moverPersonagem(movimentoPersonagem);
	        if(!movimentoPersonagem)	//S� � poss�vel movimentar o tabuleiro com o personagem parado
		        moverTabuleiro(cursor);

	        desenharFundo();
	        desenharObstaculos();
	        desenharInicio();
	        desenharSetas();
	        desenharFim();
	        desenharChaves();
	        desenharPersonagem(mostrarPersonagem);
	        desenharCursor(cursor);

	        setvisualpage(pagina);

	        movimentoPersonagem = passarDeFase(movimentoPersonagem);

	        //Piscar personagem caso ele esteja parado
	        if(spritePersonagem.y == 0){
	        	contadorMostrarPersonagem++;
	        	if(contadorMostrarPersonagem >= CICLO_DESENHO){
	        		contadorMostrarPersonagem = 0;
	        		mostrarPersonagem = !mostrarPersonagem;
	        	}
	        } else{
	        	contadorMostrarPersonagem = 0;
	        	mostrarPersonagem = true;
	        }

	        if(GetKeyState(TECLA_M)&0x80)
	        	volume = setVolume(volume);
	        if(GetKeyState(VK_ESCAPE)&0x80)
      			sair = pause();
      		if(venceu){
      			desenharTelaVitoria();
      			delay(2000);
      			sair = true;
      			fase = 0;
      		}
  		}
  	}
  	mciSendString("close trilha", NULL, 0, 0); //Encerrar trilha sonora
}

int menu(){
	/*
		Fun��o onde ocorre toda l�gica do menu principal, executada enquanto o jogador est� nele
		Retorna uma das op��es poss�veis do jogo
	*/

  	int pagina = 1;

  	while(true){ //Loop
  		if(pagina == 1)
	       	pagina = 2;
	    else
	       	pagina = 1;
	      
	    setactivepage(pagina);
	    cleardevice();

	    desenharMenuPrincipal();

       setvisualpage(pagina);

        if(GetKeyState(VK_SPACE)&0x80){				//Jogar
        	while(GetKeyState(VK_ESCAPE)&0x80);
        	return(1);
        }
        if(GetKeyState(VK_ESCAPE)&0x80){			//Sair do jogo
        	while(GetKeyState(VK_ESCAPE)&0x80);
        	return(2);
        }
	}
}

bool compararCoordenadas(Coordenadas primeira, Coordenadas segunda){
	/*
		Fun��o que compara se duas vari�veis do tipo Coordenadas s�o iguais
		Retorna o valor dessa compara��o
	*/

	return((primeira.anel == segunda.anel) && (primeira.fatia == segunda.fatia));
}

void liberarMemoria(){
	/*
		Libera a mem�ria alocada pelos ponteiros
	*/

	free(fundo);
	free(roda);
	free(mascaraRoda);
	free(aneisEFatias);
	free(posicoes);
	free(setas);
	free(mascaraSetas);
	free(personagem);
	free(mascaraPersonagem);
	free(objetos);
	free(mascaraObjetos);
	free(obstaculos);
	free(chaves);
	free(pegouChaves);
	free(menuPrincipal);
	free(telaLoading);
	free(textoLoading);
	free(mascaraTextoLoading);
	free(vitoria);
	free(mascaraVitoria);
	free(telaFinal);
	free(imagemPause);
}

bool pause(){
	/*
		Tela de pause e suas mec�nicas
		Retorna se o jogador continuar� jogano ou n�o
	*/

	while(GetKeyState(VK_ESCAPE)&0x80);
	desenharPause();

	while(true){
		if(GetKeyState(VK_ESCAPE)&0x80){		//Continuar jogando
			while(GetKeyState(VK_ESCAPE)&0x80);
			return(false);
		}
		else if(GetKeyState(VK_BACK)&0x80){		//Voltar para o menu principal
			while(GetKeyState(VK_BACK)&0x80);
			return(true);
		}
	}
}

void carregarFundo(){
	/*
		Fun��o que carrega a imagem de fundo dependendo da fase em que o jogador est�, tamb�m carrega as imagens da roda
	*/

	int tamanho = imagesize(0, 0, TAMANHO - 1, TAMANHO - 1);  //Quantidade de mem�ria necess�ria

	fundo = NULL;
	fundo = realloc(fundo, tamanho);

	switch(fase){
		case 0:
			readimagefile(".\\imagens\\cenarios\\iglu.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
			break;
		case 1:
			readimagefile(".\\imagens\\cenarios\\empire.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
			break;
		case 2:
			readimagefile(".\\imagens\\cenarios\\torre.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
			break;
		default:
			readimagefile(".\\imagens\\cenarios\\fuji.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
	}
	getimage(0, 0, TAMANHO - 1, TAMANHO - 1, fundo);

	roda = NULL;
	mascaraRoda = NULL;
	roda = realloc(roda, tamanho);
	mascaraRoda = realloc(mascaraRoda, tamanho);

	switch(fase){
		case 0:
			readimagefile(".\\imagens\\cenarios\\rodas\\roda1.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
			getimage(0, 0, TAMANHO - 1, TAMANHO - 1, roda);
			readimagefile(".\\imagens\\cenarios\\rodas\\rodamasc1.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
			getimage(0, 0, TAMANHO - 1, TAMANHO - 1, mascaraRoda);
			break;
		case 1:
			readimagefile(".\\imagens\\cenarios\\rodas\\roda2.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
			getimage(0, 0, TAMANHO - 1, TAMANHO - 1, roda);
			readimagefile(".\\imagens\\cenarios\\rodas\\rodamasc2.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
			getimage(0, 0, TAMANHO - 1, TAMANHO - 1, mascaraRoda);
			break;
		case 2:
			readimagefile(".\\imagens\\cenarios\\rodas\\roda3.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
			getimage(0, 0, TAMANHO - 1, TAMANHO - 1, roda);
			readimagefile(".\\imagens\\cenarios\\rodas\\rodamasc3.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
			getimage(0, 0, TAMANHO - 1, TAMANHO - 1, mascaraRoda);
			break;
		default:
			readimagefile(".\\imagens\\cenarios\\rodas\\roda4.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
			getimage(0, 0, TAMANHO - 1, TAMANHO - 1, roda);
			readimagefile(".\\imagens\\cenarios\\rodas\\rodamasc4.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
			getimage(0, 0, TAMANHO - 1, TAMANHO - 1, mascaraRoda);
	}
}

void desenharFundo(){
	/*
		Fun��o que desenha o fundo como uma combina��o entre a imagem de fundo e as rodas
	*/

	putimage(0, 0, fundo, COPY_PUT);
	putimage(0, 0, mascaraRoda, AND_PUT);
	putimage(0, 0, roda, OR_PUT);
}

void carregarAneisEFatias(){
	/*
		Fun��o que carrega os dados das setas de cada fase e define o total de aneis e fatias nela usando o arquivo fases.txt
	*/

	FILE *dados;
	int numeroArquivo, contador = 0;
	Coordenadas posicao;
	posicao.anel = 0;	posicao.fatia = 0;

	switch(fase){  //Quantidade total de anies e fatias em cada fase
		case 0:
			total.anel = 3;		total.fatia = 4;
			break;
		case 1:
			total.anel = 4;		total.fatia = 4;
			break;
		case 2:
			total.anel = 3;		total.fatia = 8;
			break;
		default:
			total.anel = 4;		total.fatia = 8;
	}

	aneisEFatias = NULL;
	aneisEFatias = (int **)realloc(aneisEFatias, sizeof(int *) * total.anel);
	for(int i = 0; i < total.anel; i++){
		aneisEFatias[i] = NULL;
		aneisEFatias[i] = (int *)realloc(aneisEFatias[i], sizeof(int) * total.fatia);
	}

	dados = fopen(".\\arquivos\\fases.txt", "r");
	if(dados == NULL)
		return;

	while(fscanf(dados, "%d", &numeroArquivo) != EOF){
		if(contador == fase){			//Verifica se est� carregando a fase correta
			if(numeroArquivo == -5)		//Sai do la�o principal, pois come�ou uma nova fase
				break;
			else if(numeroArquivo == -1){  //Passa para o pr�ximo anel
				posicao.anel++;
				posicao.fatia = 0;
			} else{
				if(numeroArquivo % 100 == 0)
					numeroArquivo /= 100;
				else if(numeroArquivo % 10 == 0)
					numeroArquivo /= 10;
				aneisEFatias[posicao.anel][posicao.fatia] = numeroArquivo;
				posicao.fatia++;
			}
		} else if(numeroArquivo == -5)	//Caso n�o esteja na fase correta soma o contador enquanto "procura a fase correta"
			contador++;
	}
	fclose(dados);
}

void carregarPosicoes(){
	/*
		Fun��o de define onde est� cada posi��o do tabuleiro (para os desenhos) em pixels dependendo da fase
	*/

	posicoes = NULL;
	posicoes = (Pixel **)realloc(posicoes, sizeof(Pixel *) * total.anel);
	for(int i = 0; i < total.anel; i++){
		posicoes[i] = NULL;
		posicoes[i] = (Pixel *)realloc(posicoes[i], sizeof(Pixel) * total.fatia);
	}

	if(fase == 0){
		posicoes[0][0].x = TAMANHO/2;	posicoes[0][0].y = 11.81 * TAMANHO/100;
		posicoes[1][0].x = TAMANHO/2;	posicoes[1][0].y = 21.3 * TAMANHO/100;
		posicoes[2][0].x = TAMANHO/2;	posicoes[2][0].y = 30.56 * TAMANHO/100;

		for(int i = 0; i < 3; i++){
			posicoes[i][1].x = TAMANHO - posicoes[i][0].y;	posicoes[i][1].y = posicoes[i][0].x;
			posicoes[i][2].x = TAMANHO - posicoes[i][0].x;	posicoes[i][2].y = TAMANHO - posicoes[i][0].y;
			posicoes[i][3].x = posicoes[i][0].y;			posicoes[i][3].y = posicoes[i][0].x;
		}
	} else if(fase == 1){
		posicoes[0][0].x = TAMANHO/2;	posicoes[0][0].y = 6.95 * TAMANHO/100;
		posicoes[1][0].x = TAMANHO/2;	posicoes[1][0].y = 17.59 * TAMANHO/100;
		posicoes[2][0].x = TAMANHO/2;	posicoes[2][0].y = 26.85 * TAMANHO/100;
		posicoes[3][0].x = TAMANHO/2;	posicoes[3][0].y = 35.19 * TAMANHO/100;

		for(int i = 0; i < 4; i++){
			posicoes[i][1].x = TAMANHO - posicoes[i][0].y;	posicoes[i][1].y = posicoes[i][0].x;
			posicoes[i][2].x = TAMANHO - posicoes[i][0].x;	posicoes[i][2].y = TAMANHO - posicoes[i][0].y;
			posicoes[i][3].x = posicoes[i][0].y;			posicoes[i][3].y = posicoes[i][0].x;
		}
	} else if(fase == 2){
		posicoes[0][0].x = TAMANHO/2;				posicoes[0][0].y = 11.85 * TAMANHO/100;
		posicoes[1][0].x = TAMANHO/2;				posicoes[1][0].y = 20.93 * TAMANHO/100;
		posicoes[2][0].x = TAMANHO/2;				posicoes[2][0].y = 30.93 * TAMANHO/100;

		posicoes[0][1].x = 77.5 * TAMANHO/100;		posicoes[0][1].y = 23.15 * TAMANHO/100;
		posicoes[1][1].x = 70.37 * TAMANHO/100;		posicoes[1][1].y = 30.09 * TAMANHO/100;
		posicoes[2][1].x = 63.89 * TAMANHO/100;		posicoes[2][1].y = 36.39 * TAMANHO/100;

		for(int i = 0; i < 3; i++){
			posicoes[i][2].x = TAMANHO - posicoes[i][0].y; 		posicoes[i][2].y = posicoes[i][0].x;
			posicoes[i][4].x = posicoes[i][0].x; 				posicoes[i][4].y = TAMANHO - posicoes[i][0].y;
			posicoes[i][6].x = posicoes[i][0].y;	 			posicoes[i][6].y = posicoes[i][0].x;

			posicoes[i][3].x = posicoes[i][1].x;  				posicoes[i][3].y = TAMANHO - posicoes[i][1].y;
			posicoes[i][5].x = TAMANHO - posicoes[i][1].x;		posicoes[i][5].y = TAMANHO - posicoes[i][1].y;
			posicoes[i][7].x = TAMANHO - posicoes[i][1].x; 		posicoes[i][7].y = posicoes[i][1].y;
		}
	} else if(fase == 3){
		posicoes[0][0].x = TAMANHO/2;				posicoes[0][0].y = 7.41 * TAMANHO/100;
		posicoes[1][0].x = TAMANHO/2;				posicoes[1][0].y = 18.33 * TAMANHO/100;
		posicoes[2][0].x = TAMANHO/2;				posicoes[2][0].y = 27.31 * TAMANHO/100;
		posicoes[3][0].x = TAMANHO/2;				posicoes[3][0].y = 34.72 * TAMANHO/100;

		posicoes[0][1].x = 81.2 * TAMANHO/100;		posicoes[0][1].y = 20.37 * TAMANHO/100;
		posicoes[1][1].x = 72.87 * TAMANHO/100;		posicoes[1][1].y = 27.78 * TAMANHO/100;
		posicoes[2][1].x = 67.13 * TAMANHO/100;		posicoes[2][1].y = 35.19 * TAMANHO/100;
		posicoes[3][1].x = 60.65 * TAMANHO/100;		posicoes[3][1].y = 39.54 * TAMANHO/100;

		for(int i = 0; i < 4; i++){
			posicoes[i][2].x = TAMANHO - posicoes[i][0].y; 		posicoes[i][2].y = posicoes[i][0].x;
			posicoes[i][4].x = posicoes[i][0].x; 				posicoes[i][4].y = TAMANHO - posicoes[i][0].y;
			posicoes[i][6].x = posicoes[i][0].y; 				posicoes[i][6].y = posicoes[i][0].x;

			posicoes[i][3].x = posicoes[i][1].x;  				posicoes[i][3].y = TAMANHO - posicoes[i][1].y;
			posicoes[i][5].x = TAMANHO - posicoes[i][1].x;		posicoes[i][5].y = TAMANHO - posicoes[i][1].y;
			posicoes[i][7].x = TAMANHO - posicoes[i][1].x; 		posicoes[i][7].y = posicoes[i][1].y;
		}
	}
}

void carregarSetas(){
	/*
		Fun��o que carrega as imagens do sprite de setas e sua m�cara
	*/

	int quantidadeDeSetas = 8, sprite = SPRITE_COMPLEMENTOS;	//Total de setas e tamanho de cada imagem final
	int tamanho = imagesize(0, 0, sprite - 1, sprite - 1);		//Quantidade de mem�ria necess�ria

	setas = NULL;
	mascaraSetas = NULL;
	setas = (void **)realloc(setas, sizeof(void *) * quantidadeDeSetas);
	mascaraSetas = (void **)realloc(mascaraSetas, sizeof(void *) * quantidadeDeSetas);

	readimagefile(".\\imagens\\complementos\\setas.jpg", 0, 0, sprite * quantidadeDeSetas - 1, sprite - 1);
	for(int i = 0; i < quantidadeDeSetas; i++){
		setas[i] = NULL;
		setas[i] = realloc(setas[i], tamanho);
		getimage(sprite * i, 0, sprite * (i+1) - 1, sprite - 1, setas[i]);
	}

	readimagefile(".\\imagens\\complementos\\setasmasc.jpg", 0, 0, sprite * quantidadeDeSetas - 1, sprite - 1);
	for(int i = 0; i < quantidadeDeSetas; i++){
		mascaraSetas[i] = NULL;
		mascaraSetas[i] = realloc(mascaraSetas[i], tamanho);
		getimage(sprite * i, 0, sprite * (i+1) - 1, sprite - 1, mascaraSetas[i]);
	}
}

void desenharSetas(){
	/*
		Fun��o para desenhar as setas levando em considera��o a quantidade fatias na fase,
		a posi��o da seta e para onde ela deve apontar.
	*/

	int deslocamento = SPRITE_COMPLEMENTOS/4, direcaoSeta;		//Deslocamento para desenhar a sera no lugar correto e indicador de para qual dire��o a seta aponta

	//La�os que passam por todas as setas
	for(int i = 0; i < total.anel; i++)
		for(int j = 0; j < total.fatia; j++){
			if(total.fatia == 4){			//Quantidade total de fatias na fase = 4
				switch(aneisEFatias[i][j]){
					case 1: //Hor�rio
						if(j == 0)			direcaoSeta = 2;
						else if(j == 1)		direcaoSeta = 0;
						else if(j == 2)		direcaoSeta = 6;
						else if(j == 3)		direcaoSeta = 4;
						break;
					case 2: //Anti hor�rio
						if(j == 0)			direcaoSeta = 6;
						else if(j == 1)		direcaoSeta = 4;
						else if(j == 2)		direcaoSeta = 2;
						else if(j == 3)		direcaoSeta = 0;
						break;
					case 3: //Afastar
						if(j == 0)			direcaoSeta = 4;
						else if(j == 1)		direcaoSeta = 2;
						else if(j == 2)		direcaoSeta = 0;
						else if(j == 3)		direcaoSeta = 6;
						break;
					default: //Aproximar
						if(j == 0)			direcaoSeta = 0;
						else if(j == 1)		direcaoSeta = 6;
						else if(j == 2)		direcaoSeta = 4;
						else if(j == 3)		direcaoSeta = 2;
				}
			} else if(total.fatia == 8){	//Quantidade total de fatias na fase = 8
				switch(aneisEFatias[i][j]){
					case 1: //Hor�rio
						if(j == 0)			direcaoSeta = 2;
						else if(j == 1)		direcaoSeta = 1;
						else if(j == 2)		direcaoSeta = 0;
						else if(j == 3)		direcaoSeta = 7;
						else if(j == 4)		direcaoSeta = 6;
						else if(j == 5)		direcaoSeta = 5;
						else if(j == 6)		direcaoSeta = 4;
						else if(j == 7)		direcaoSeta = 3;
						break;
					case 2: //Anti hor�rio
						if(j == 0)			direcaoSeta = 6;
						else if(j == 1)		direcaoSeta = 5;
						else if(j == 2)		direcaoSeta = 4;
						else if(j == 3)		direcaoSeta = 3;
						else if(j == 4)		direcaoSeta = 2;
						else if(j == 5)		direcaoSeta = 1;
						else if(j == 6)		direcaoSeta = 0;
						else if(j == 7)		direcaoSeta = 7;
						break;
					case 3: //Afastar
						if(j == 0)			direcaoSeta = 4;
						else if(j == 1)		direcaoSeta = 3;
						else if(j == 2)		direcaoSeta = 2;
						else if(j == 3)		direcaoSeta = 1;
						else if(j == 4)		direcaoSeta = 0;
						else if(j == 5)		direcaoSeta = 7;
						else if(j == 6)		direcaoSeta = 6;
						else if(j == 7)		direcaoSeta = 5;
						break;
					default: //Aproximar
						if(j == 0)			direcaoSeta = 0;
						else if(j == 1)		direcaoSeta = 7;
						else if(j == 2)		direcaoSeta = 6;
						else if(j == 3)		direcaoSeta = 5;
						else if(j == 4)		direcaoSeta = 4;
						else if(j == 5)		direcaoSeta = 3;
						else if(j == 6)		direcaoSeta = 2;
						else if(j == 7)		direcaoSeta = 1;
				}
			}		
			putimage(posicoes[i][j].x - deslocamento, posicoes[i][j].y - deslocamento, mascaraSetas[direcaoSeta], AND_PUT);
			putimage(posicoes[i][j].x - deslocamento, posicoes[i][j].y - deslocamento, setas[direcaoSeta], OR_PUT);
		}
}

void carregarPersonagem(){
	/*
		Fun��o que carrega o sprite do personagem e sua m�scara
	*/

	int sprite = TAMANHO/10, linha = 4, coluna = 3;			//Tamanho de cada trecho do sprite, quantidade de linhas e colunas nele
  	int tamanho = imagesize(0, 0, sprite - 1, sprite - 1);	//Quantidade de mem�ria para a imagem

  	personagem = NULL;
  	mascaraPersonagem = NULL;
  	personagem = (void ***)realloc(personagem, sizeof(void **) * linha);
  	mascaraPersonagem = (void ***)realloc(mascaraPersonagem, sizeof(void **) * linha);

  	for(int i = 0; i < linha; i ++){
		personagem[i] = NULL;
		mascaraPersonagem[i] = NULL;
	    personagem[i] = (void **)realloc(personagem[i], sizeof(void *) * coluna);
	    mascaraPersonagem[i] = (void **)realloc(mascaraPersonagem[i], sizeof(void *) * coluna);
	}

	readimagefile(".\\imagens\\personagem\\boneco.jpg", 0, 0, (sprite * coluna) - 1, (sprite * linha) - 1);
	for(int i = 0; i < linha; i++)
		for(int j = 0; j < coluna; j++){
			personagem[i][j] = NULL;
			personagem[i][j] = realloc(personagem[i][j], tamanho);
			getimage(sprite * j, sprite * i, sprite * (j+1) -1, sprite * (i+1) -1, personagem[i][j]);
		}

	readimagefile(".\\imagens\\personagem\\bonecomasc.jpg", 0, 0, (sprite * coluna) - 1, (sprite * linha) - 1);
	for(int i = 0; i < linha; i++)
		for(int j = 0; j < coluna; j++){
			mascaraPersonagem[i][j] = NULL;
			mascaraPersonagem[i][j] = realloc(mascaraPersonagem[i][j], tamanho);
			getimage(sprite * j, sprite * i, sprite * (j+1) -1, sprite * (i+1) -1, mascaraPersonagem[i][j]);
		}
}

void desenharPersonagem(bool mostrarPersonagem){
	/*
		Fun��o que desenha o personagem
		Par�metro mostra ou n�o o personagem
	*/

	int deslocamento = TAMANHO/20;		//Deslocamento necess�rio para desenhar o personagem na posi��o correta

	//Desenha o personagem se o par�metro de entrada for true
	if(mostrarPersonagem){
		putimage(posicoes[posicaoPersonagem.anel][posicaoPersonagem.fatia].x - deslocamento,
			posicoes[posicaoPersonagem.anel][posicaoPersonagem.fatia].y - deslocamento,
			mascaraPersonagem[spritePersonagem.x][spritePersonagem.y],AND_PUT);
		putimage(posicoes[posicaoPersonagem.anel][posicaoPersonagem.fatia].x - deslocamento,
			posicoes[posicaoPersonagem.anel][posicaoPersonagem.fatia].y - deslocamento,
			personagem[spritePersonagem.x][spritePersonagem.y], OR_PUT);
	}
}

Coordenadas moverCursor(Coordenadas posicao){
	/*
		Fun��o respons�vel por mover o cursor
		Par�metro posi��o atual do cursor
		Retorna a nova posi��o do cursor
	*/

	if(total.fatia == 4){			//Movimenta��o em fases que h� 4 fatias
		if(GetKeyState(TECLA_A)&0x80){
			if(posicao.fatia == 0)			posicao.fatia = moverCursorSentidoAntiHorario(posicao.fatia);
			else if(posicao.fatia == 1)		posicao.anel = aproximarCursorDoCentro(posicao.anel);
			else if(posicao.fatia == 2)		posicao.fatia = moverCursorSentidoHorario(posicao.fatia);
			else if(posicao.fatia == 3)		posicao.anel = afastarCursorDoCentro(posicao.anel);
		}
		if(GetKeyState(TECLA_D)&0x80){
			if(posicao.fatia == 0)			posicao.fatia = moverCursorSentidoHorario(posicao.fatia);
			else if(posicao.fatia == 1)		posicao.anel = afastarCursorDoCentro(posicao.anel);
			else if(posicao.fatia == 2)		posicao.fatia = moverCursorSentidoAntiHorario(posicao.fatia);
			else if(posicao.fatia == 3)		posicao.anel = aproximarCursorDoCentro(posicao.anel);
		}
		if(GetKeyState(TECLA_S)&0x80){
			if(posicao.fatia == 0)			posicao.anel = aproximarCursorDoCentro(posicao.anel);
			else if(posicao.fatia == 1)		posicao.fatia = moverCursorSentidoHorario(posicao.fatia);
			else if(posicao.fatia == 2)		posicao.anel = afastarCursorDoCentro(posicao.anel);
			else if(posicao.fatia == 3)		posicao.fatia = moverCursorSentidoAntiHorario(posicao.fatia);
		}
		if(GetKeyState(TECLA_W)&0x80){
			if(posicao.fatia == 0)			posicao.anel = afastarCursorDoCentro(posicao.anel);
			else if(posicao.fatia == 1)		posicao.fatia = moverCursorSentidoAntiHorario(posicao.fatia);
			else if(posicao.fatia == 2)		posicao.anel = aproximarCursorDoCentro(posicao.anel);
			else if(posicao.fatia == 3)		posicao.fatia = moverCursorSentidoHorario(posicao.fatia);
		}
	} else if(total.fatia == 8){	//Movimenta��o em fases que h� 8 fatias
		if(GetKeyState(TECLA_A)&0x80){
			if((posicao.fatia == 0) || (posicao.fatia == 1) || (posicao.fatia == 7))
				posicao.fatia = moverCursorSentidoAntiHorario(posicao.fatia);
			else if(posicao.fatia == 2)		posicao.anel = aproximarCursorDoCentro(posicao.anel);
			else if((posicao.fatia == 3) || (posicao.fatia == 4) || (posicao.fatia == 5))
				posicao.fatia = moverCursorSentidoHorario(posicao.fatia);
			else if(posicao.fatia == 6)		posicao.anel = afastarCursorDoCentro(posicao.anel);
		}
		if(GetKeyState(TECLA_D)&0x80){
			if((posicao.fatia == 0) || (posicao.fatia == 1) || (posicao.fatia == 7))
				posicao.fatia = moverCursorSentidoHorario(posicao.fatia);
			else if(posicao.fatia == 2)		posicao.anel = afastarCursorDoCentro(posicao.anel);
			else if((posicao.fatia == 3) || (posicao.fatia == 4) || (posicao.fatia == 5))
				posicao.fatia = moverCursorSentidoAntiHorario(posicao.fatia);
			else if(posicao.fatia == 6)		posicao.anel = aproximarCursorDoCentro(posicao.anel);
		}
		if(GetKeyState(TECLA_S)&0x80){
			if((posicao.fatia == 0) || (posicao.fatia == 1) || (posicao.fatia == 7))
				posicao.anel = aproximarCursorDoCentro(posicao.anel);
			else if(posicao.fatia == 2)		posicao.fatia = moverCursorSentidoHorario(posicao.fatia);
			else if((posicao.fatia == 3) || (posicao.fatia == 4) || (posicao.fatia == 5))
				posicao.anel = afastarCursorDoCentro(posicao.anel);
			else if(posicao.fatia == 6)		posicao.fatia = moverCursorSentidoAntiHorario(posicao.fatia);
		}
		if(GetKeyState(TECLA_W)&0x80){
			if((posicao.fatia == 0) || (posicao.fatia == 1) || (posicao.fatia == 7))
				posicao.anel = afastarCursorDoCentro(posicao.anel);
			else if(posicao.fatia == 2)		posicao.fatia = moverCursorSentidoAntiHorario(posicao.fatia);
			else if((posicao.fatia == 3) || (posicao.fatia == 4) || (posicao.fatia == 5))
				posicao.anel = aproximarCursorDoCentro(posicao.anel);
			else if(posicao.fatia == 6)		posicao.fatia = moverCursorSentidoHorario(posicao.fatia);
		}
	}
	return(posicao);
}

int moverCursorSentidoHorario(int fatia){
	/*
		Fun��o que move o cursor no sentido hor�rio
		Par�metro fatia atual do cursor
		Retona a nova fatia do cursor
	*/

	fatia++;
	if(fatia >= total.fatia)
		fatia = 0;
	return(fatia);
}

int moverCursorSentidoAntiHorario(int fatia){
	/*
		Fun��o que move o cursor no sentido anti hor�rio
		Par�metro fatia atual do cursor
		Retona a nova fatia do cursor
	*/

	fatia--;
	if(fatia < 0)
		fatia = total.fatia - 1;
	return(fatia);
}

int afastarCursorDoCentro(int anel){
	/*
		Fun��o que afasta o cursor do centro
		Par�metro anel atual do cursor
		Sa�da novo anel do cursor
	*/

	anel--;
	if(anel < 0)
		anel = total.anel - 1;
	return(anel);
}

int aproximarCursorDoCentro(int anel){
	/*
		Fun��o que aproxima o cursor do centro
		Par�metro anel atual do cursor
		Sa�da novo anel do cursor
	*/

	anel++;
	if(anel >= total.anel)
		anel = 0;
	return(anel);
}

void desenharCursor(Coordenadas cursor){
	/*
		Fun��o que desenha o cursor na tela
	*/

	int deslocamento = SPRITE_COMPLEMENTOS/2;	//Deslocamento necess�rio para desenhar no local correto

	putimage(posicoes[cursor.anel][cursor.fatia].x - deslocamento,
		posicoes[cursor.anel][cursor.fatia].y - deslocamento, mascaraObjetos[1], AND_PUT);
	putimage(posicoes[cursor.anel][cursor.fatia].x - deslocamento,
		posicoes[cursor.anel][cursor.fatia].y - deslocamento, objetos[1], OR_PUT);
}

void moverTabuleiro(Coordenadas referencia){
	/*
		Fun��o que move todas as setas no tabuleiro
		Par�mettro posi��o de refer�ncia do cursor para indicar aonde deve ocorrer movimento
	*/

	if(total.fatia == 4){				//Movimento em fases que h� 4 fatias
		if(GetKeyState(VK_LEFT)&0x80){
			if(referencia.fatia == 0)			moverTabuleiroSentidoAntiHorario(referencia.anel);
			else if(referencia.fatia == 1)		aproximarTabuleiroDoCentro(referencia.fatia);
			else if(referencia.fatia == 2)		moverTabuleiroSentidoHorario(referencia.anel);
			else if(referencia.fatia == 3)		afastarTabuleiroDoCentro(referencia.fatia);
		}
		if(GetKeyState(VK_RIGHT)&0x80){
			if(referencia.fatia == 0)			moverTabuleiroSentidoHorario(referencia.anel);
			else if(referencia.fatia == 1)		afastarTabuleiroDoCentro(referencia.fatia);
			else if(referencia.fatia == 2)		moverTabuleiroSentidoAntiHorario(referencia.anel);
			else if(referencia.fatia == 3)		aproximarTabuleiroDoCentro(referencia.fatia);
		}
		if(GetKeyState(VK_DOWN)&0x80){
			if(referencia.fatia == 0)			aproximarTabuleiroDoCentro(referencia.fatia);
			else if(referencia.fatia == 1)		moverTabuleiroSentidoHorario(referencia.anel);
			else if(referencia.fatia == 2)		afastarTabuleiroDoCentro(referencia.fatia);
			else if(referencia.fatia == 3)		moverTabuleiroSentidoAntiHorario(referencia.anel);
		}
		if(GetKeyState(VK_UP)&0x80){
			if(referencia.fatia == 0)			afastarTabuleiroDoCentro(referencia.fatia);
			else if(referencia.fatia == 1)		moverTabuleiroSentidoAntiHorario(referencia.anel);
			else if(referencia.fatia == 2)		aproximarTabuleiroDoCentro(referencia.fatia);
			else if(referencia.fatia == 3)		moverTabuleiroSentidoHorario(referencia.anel);
		}
	} else if(total.fatia == 8){		//Movimento em fases que h� 8 fatias
		if(GetKeyState(VK_LEFT)&0x80){
			if((referencia.fatia == 0) || (referencia.fatia == 1) || (referencia.fatia == 7))
				moverTabuleiroSentidoAntiHorario(referencia.anel);
			else if(referencia.fatia == 2)		aproximarTabuleiroDoCentro(referencia.fatia);
			else if((referencia.fatia == 3) || (referencia.fatia == 4) || (referencia.fatia == 5))
				moverTabuleiroSentidoHorario(referencia.anel);
			else if(referencia.fatia == 6)		afastarTabuleiroDoCentro(referencia.fatia);
		}
		if(GetKeyState(VK_RIGHT)&0x80){
			if((referencia.fatia == 0) || (referencia.fatia == 1) || (referencia.fatia == 7))
				moverTabuleiroSentidoHorario(referencia.anel);
			else if(referencia.fatia == 2)		afastarTabuleiroDoCentro(referencia.fatia);
			else if((referencia.fatia == 3) || (referencia.fatia == 4) || (referencia.fatia == 5))
				moverTabuleiroSentidoAntiHorario(referencia.anel);
			else if(referencia.fatia == 6)		aproximarTabuleiroDoCentro(referencia.fatia);
		}
		if(GetKeyState(VK_DOWN)&0x80){
			if((referencia.fatia == 0) || (referencia.fatia == 1) || (referencia.fatia == 7))
				aproximarTabuleiroDoCentro(referencia.fatia);
			else if(referencia.fatia == 2)		moverTabuleiroSentidoHorario(referencia.anel);
			else if((referencia.fatia == 3) || (referencia.fatia == 4) || (referencia.fatia == 5))
				afastarTabuleiroDoCentro(referencia.fatia);
			else if(referencia.fatia == 6)		moverTabuleiroSentidoAntiHorario(referencia.anel);
		}
		if(GetKeyState(VK_UP)&0x80){
			if((referencia.fatia == 0) || (referencia.fatia == 1) || (referencia.fatia == 7))
				afastarTabuleiroDoCentro(referencia.fatia);
			else if(referencia.fatia == 2)		moverTabuleiroSentidoAntiHorario(referencia.anel);
			else if((referencia.fatia == 3) || (referencia.fatia == 4) || (referencia.fatia == 5))
				aproximarTabuleiroDoCentro(referencia.fatia);
			else if(referencia.fatia == 6)		moverTabuleiroSentidoHorario(referencia.anel);
		}
	}
}

void moverTabuleiroSentidoHorario(int anel){
	/*
		Fun��o que move as setas no tabuleiro no sentido hor�rio
		Par�metro que indica em qual anel mover
	*/

	int temporario[total.fatia];	//Vetor que auxilia em fazer o movimento

  	for(int i = 0; i < total.fatia - 1; i++)
    	temporario[i + 1] = aneisEFatias[anel][i];

  	temporario[0] = aneisEFatias[anel][total.fatia - 1];
  	for(int i = 0; i < total.fatia; i++)
    	aneisEFatias[anel][i] = temporario[i];
}

void moverTabuleiroSentidoAntiHorario(int anel){
	/*
		Fun��o que move as setas no tabuleiro no sentido anti hor�rio
		Par�metro que indica em qual anel mover
	*/

	int temporario[total.fatia];	//Vetor que auxilia em fazer o movimento

  	for(int i = total.fatia - 1; i > 0; i--)
    	temporario[i - 1] = aneisEFatias[anel][i];

  	temporario[total.fatia - 1] = aneisEFatias[anel][0];
  	for(int i = 0; i < total.fatia; i++)
    	aneisEFatias[anel][i] = temporario[i];
}

void afastarTabuleiroDoCentro(int fatia){
	/*
		Fun��o que afasta as setas no tabuleiro do centro
		Par�metro que indica em qual anel mover
	*/

	int temporario[total.anel];	//Vetor que auxilia em fazer o movimento
  	
  	for(int i = total.anel - 1; i > 0; i--)
    	temporario[i - 1] = aneisEFatias[i][fatia];
  	
  	temporario[total.anel - 1] = aneisEFatias[0][fatia];
  	for(int i = 0; i < total.anel; i++)
    	aneisEFatias[i][fatia] = temporario[i];
}

void aproximarTabuleiroDoCentro(int fatia){
	/*
		Fun��o que afasta as setas no tabuleiro do centro
		Par�metro que indica em qual anel mover
	*/

	int temporario[total.anel];	//Vetor que auxilia em fazer o movimento
  
  	for(int i = 0; i < total.anel - 1; i++)
    	temporario[i + 1] = aneisEFatias[i][fatia];
  
  	temporario[0] = aneisEFatias[total.anel - 1][fatia];
  	for(int i = 0; i < total.anel; i++)
    	aneisEFatias[i][fatia] = temporario[i];
}

void embaralharTabuleiro(){
	return;
	/*
		Fun��o que embaralha as setas no tabuleiro
	*/

	int quantidadeDeMovimentos, opcaoEmbaralhar; //N�mero de movimentos e tipo de movimento (gerados aleat�rimante)
  	
  	//La�os que passam por todas as setas
  	for(int anel = 0; anel < total.anel; anel++)
    	for(int fatia = 0; fatia < total.fatia; fatia++){
      		quantidadeDeMovimentos = rand() % 6 + 1;	//Escolhe quantas vezes mover a pe�a (entre 1 e 6 vezes)
      		opcaoEmbaralhar = rand() % 4;				//Escolhe qual movimento fazer (entre 0 e 4)

		    for(int i = 0; i < quantidadeDeMovimentos; i++){
		        switch(opcaoEmbaralhar){
		        	case 0:
		            	moverTabuleiroSentidoAntiHorario(anel);
		            	break;
		          	case 1:
		          		moverTabuleiroSentidoHorario(anel);
		            	break;
		          	case 2:
		            	aproximarTabuleiroDoCentro(fatia);
		            	break;
		          	default:
		            	afastarTabuleiroDoCentro(fatia);
        		}
      		}
    	}
}

void carregarIncioEFim(){
	/*
		Fun��o que carrega as posi��es de in�cio e fim em cada fase
	*/

	inicioEFim[0][0].anel = 0; inicioEFim[0][0].fatia = 2;		inicioEFim[0][1].anel = 0; inicioEFim[0][1].fatia = 1;
	inicioEFim[1][0].anel = 1; inicioEFim[1][0].fatia = 2;		inicioEFim[1][1].anel = 3; inicioEFim[1][1].fatia = 0;
	inicioEFim[2][0].anel = 1; inicioEFim[2][0].fatia = 0;		inicioEFim[2][1].anel = 0; inicioEFim[2][1].fatia = 0;
	inicioEFim[3][0].anel = 0; inicioEFim[3][0].fatia = 6;		inicioEFim[3][1].anel = 3; inicioEFim[3][1].fatia = 5;
}

bool moverPersonagem(bool movimentoPersonagem){
	/*
		Fun��o que habilita/desabilita a movimenta��o do personagem
		Par�metro que indica se o personagem est� habilitado ou n�o a se mover
		Retona se o personagem est� habilitado ou n�o a se mover
	*/

	if(GetKeyState(VK_SPACE)&0x80)
		movimentoPersonagem = true;
	else if(GetKeyState(TECLA_R)&0x80){
		sndPlaySound(".\\audios\\reset.wav", SND_ASYNC);
		movimentoPersonagem = false;
		posicaoPersonagem = inicioEFim[fase][0];
	}

	if(movimentoPersonagem){
		if(spritePersonagem.y == 1)
			spritePersonagem.y = 2;
		else
			spritePersonagem.y = 1;

		switch(aneisEFatias[posicaoPersonagem.anel][posicaoPersonagem.fatia]){
			case 1:
				if(total.fatia == 4){
					if(posicaoPersonagem.fatia == 0)			spritePersonagem.x = 1;
					else if(posicaoPersonagem.fatia == 1)		spritePersonagem.x = 2;
					else if(posicaoPersonagem.fatia == 2)		spritePersonagem.x = 3;
					else if(posicaoPersonagem.fatia == 3)		spritePersonagem.x = 0;
				} else if(total.fatia == 8){
					if((posicaoPersonagem.fatia == 0) || (posicaoPersonagem.fatia == 1) || (posicaoPersonagem.fatia == 7))
						spritePersonagem.x = 1;
					else if(posicaoPersonagem.fatia == 2)		spritePersonagem.x = 2;
					else if((posicaoPersonagem.fatia == 3) || (posicaoPersonagem.fatia == 4) || (posicaoPersonagem.fatia == 5))
						spritePersonagem.x = 3;
					else if(posicaoPersonagem.fatia == 6)		spritePersonagem.x = 0;
				}
				moverPersonagemSentidoHorario();
				break;
			case 2:
				if(total.fatia == 4){
					if(posicaoPersonagem.fatia == 0)			spritePersonagem.x = 3;
					else if(posicaoPersonagem.fatia == 1)		spritePersonagem.x = 0;
					else if(posicaoPersonagem.fatia == 2)		spritePersonagem.x = 1;
					else if(posicaoPersonagem.fatia == 3)		spritePersonagem.x = 2;
				} else if(total.fatia == 8){
					if((posicaoPersonagem.fatia == 0) || (posicaoPersonagem.fatia == 1) || (posicaoPersonagem.fatia == 7))
						spritePersonagem.x = 3;
					else if(posicaoPersonagem.fatia == 2)		spritePersonagem.x = 0;
					else if((posicaoPersonagem.fatia == 3) || (posicaoPersonagem.fatia == 4) || (posicaoPersonagem.fatia == 5))
						spritePersonagem.x = 1;
					else if(posicaoPersonagem.fatia == 6)		spritePersonagem.x = 2;
				}
				moverPersonagemSentidoAntiHorario();
				break;
			case 3:
				if(total.fatia == 4){
					if(posicaoPersonagem.fatia == 0)			spritePersonagem.x = 0;
					else if(posicaoPersonagem.fatia == 1)		spritePersonagem.x = 1;
					else if(posicaoPersonagem.fatia == 2)		spritePersonagem.x = 2;
					else if(posicaoPersonagem.fatia == 3)		spritePersonagem.x = 3;
				} else if(total.fatia == 8){
					if((posicaoPersonagem.fatia == 0) || (posicaoPersonagem.fatia == 1) || (posicaoPersonagem.fatia == 7))
						spritePersonagem.x = 0;
					else if(posicaoPersonagem.fatia == 2)		spritePersonagem.x = 1;
					else if((posicaoPersonagem.fatia == 3) || (posicaoPersonagem.fatia == 4) || (posicaoPersonagem.fatia == 5))
						spritePersonagem.x = 2;
					else if(posicaoPersonagem.fatia == 6)		spritePersonagem.x = 3;
				}
				afastarPersonagemDoCentro();
				break;
			default:
				if(total.fatia == 4){
					if(posicaoPersonagem.fatia == 0)			spritePersonagem.x = 2;
					else if(posicaoPersonagem.fatia == 1)		spritePersonagem.x = 3;
					else if(posicaoPersonagem.fatia == 2)		spritePersonagem.x = 0;
					else if(posicaoPersonagem.fatia == 3)		spritePersonagem.x = 1;
				} else if(total.fatia == 8){
					if((posicaoPersonagem.fatia == 0) || (posicaoPersonagem.fatia == 1) || (posicaoPersonagem.fatia == 7))
						spritePersonagem.x = 2;
					else if(posicaoPersonagem.fatia == 2)		spritePersonagem.x = 3;
					else if((posicaoPersonagem.fatia == 3) || (posicaoPersonagem.fatia == 4) || (posicaoPersonagem.fatia == 5))
						spritePersonagem.x = 0;
					else if(posicaoPersonagem.fatia == 6)		spritePersonagem.x = 1;
				}
				aproximarPersonagemDoCentro();
		}
	} else{
		spritePersonagem.y = 0;
		for(int i = 0; i < quantidadeDeChaves; i++)
			pegouChaves[i] = false;
	}

	return(movimentoPersonagem);
}

void moverPersonagemSentidoHorario(){
	/*
		Fun��o que move o personagem no sentido hor�rio
	*/

	posicaoPersonagem.fatia++;
	if(posicaoPersonagem.fatia >= total.fatia)
		posicaoPersonagem.fatia = 0;
}

void moverPersonagemSentidoAntiHorario(){
	/*
		Fun��o que move o personagem no sentido anti hor�rio
	*/

	posicaoPersonagem.fatia--;
	if(posicaoPersonagem.fatia < 0)
		posicaoPersonagem.fatia = total.fatia - 1;
}

void afastarPersonagemDoCentro(){
	/*
		Fun��o que afasta o personagem do centro
	*/

	posicaoPersonagem.anel--;
	if(posicaoPersonagem.anel < 0)
		posicaoPersonagem.anel = 0;
}

void aproximarPersonagemDoCentro(){
	/*
		Fun��o que aproxima o personagem do centro
	*/

	posicaoPersonagem.anel++;
	if(posicaoPersonagem.anel >= total.anel)
		posicaoPersonagem.anel = total.anel - 1;
}

void carregarObjetos(){
	/*
		Fun��o que carrega as imagens dos objetos complementares e suas m�scaras
		(obst�culo, cursor, in�cio, fim e chave)
	*/

	int quantidadeDeObjetos = 5, sprite = SPRITE_COMPLEMENTOS;		//Quantidade de elementos e tamanho da sprite
	int tamanho = imagesize(0, 0, sprite - 1, sprite - 1);			//Quantidade de mem�ria necess�ria

	objetos = NULL;
	mascaraObjetos = NULL;
	objetos = (void **)realloc(objetos, sizeof(void *) * quantidadeDeObjetos);
	mascaraObjetos = (void **)realloc(mascaraObjetos, sizeof(void *) * quantidadeDeObjetos);

	readimagefile(".\\imagens\\complementos\\objetos.jpg", 0, 0, sprite * quantidadeDeObjetos - 1, sprite - 1);
	for(int i = 0; i < quantidadeDeObjetos; i++){
		objetos[i] = NULL;
		objetos[i] = realloc(objetos[i], tamanho);
		getimage(sprite * i, 0, sprite * (i+1) - 1, sprite - 1, objetos[i]);
	}

	readimagefile(".\\imagens\\complementos\\objetosmasc.jpg", 0, 0, sprite * quantidadeDeObjetos - 1, sprite - 1);
	for(int i = 0; i < quantidadeDeObjetos; i++){
		mascaraObjetos[i] = NULL;
		mascaraObjetos[i] = realloc(mascaraObjetos[i], tamanho);
		getimage(sprite * i, 0, sprite * (i+1) - 1, sprite - 1, mascaraObjetos[i]);
	}
}

void carregarObstaculosEChaves(){
	/*
		Fun��o que carrega as posi��es dos obst�culos e as chaves de acordo com o arquivo fases.txt
		(funcionamento muito semelhante a fun��o cerregarAneisEFatias())
	*/

	FILE *dados;
	int numeroArquivo, contador = 0;
	Coordenadas posicao;
	posicao.anel = 0;	posicao.fatia = 0;
	quantidadeDeObstaculos = 0;
	quantidadeDeChaves = 0;

	obstaculos = NULL;
	chaves = NULL;
	pegouChaves = NULL;

	dados = fopen(".\\arquivos\\fases.txt", "r");
	if(dados == NULL)
		return;

	while(fscanf(dados, "%d", &numeroArquivo) != EOF){
		if(contador == fase){		//Confere se � a fase correta
			if(numeroArquivo == -5)		//Encerra o la�o pois a fase acabou
				break;
			else if(numeroArquivo == -1){	//Troca de anel
				posicao.anel++;
				posicao.fatia = 0;
			} else{
				if(numeroArquivo % 100 == 0){ //Encontrou um obst�culo
					quantidadeDeObstaculos++;
					obstaculos = (Coordenadas *)realloc(obstaculos, sizeof(Coordenadas) * quantidadeDeObstaculos);
					obstaculos[quantidadeDeObstaculos - 1].anel = posicao.anel;
					obstaculos[quantidadeDeObstaculos - 1].fatia = posicao.fatia;
				}
				else if(numeroArquivo % 10 == 0){	//Encontrou uma chave
					quantidadeDeChaves++;
					chaves = (Coordenadas *)realloc(chaves, sizeof(Coordenadas) * quantidadeDeChaves);
					chaves[quantidadeDeChaves - 1].anel = posicao.anel;
					chaves[quantidadeDeChaves - 1].fatia = posicao.fatia;
				}
				posicao.fatia++;
			}
		} else if(numeroArquivo == -5) //Tenta encontrar a fase atual no arquivo
			contador++;
	}
	fclose(dados);

	pegouChaves = (bool *)realloc(pegouChaves, sizeof(bool) * quantidadeDeChaves);
	//Preenche o vetor que indica se as chaves foram pegas com false em todas as posi��es para todas as chaves
	for(int i = 0; i < quantidadeDeChaves; i++)
		pegouChaves[i] = false;
}

void desenharChaves(){
	/*
		Fun��o que desenha as chaves que ainda n�o foram pegas
	*/

	int deslocamento = SPRITE_COMPLEMENTOS/4;	//Deslocamento neces�rio para desenhar a chave no lugar correto

	for(int i = 0; i < quantidadeDeChaves; i++)
		if(!pegouChaves[i]){	//S� desenha as chaves que n�o foram pegas
			putimage(posicoes[chaves[i].anel][chaves[i].fatia].x - deslocamento,
				posicoes[chaves[i].anel][chaves[i].fatia].y - deslocamento, mascaraObjetos[4], AND_PUT);
			putimage(posicoes[chaves[i].anel][chaves[i].fatia].x - deslocamento,
				posicoes[chaves[i].anel][chaves[i].fatia].y - deslocamento, objetos[4], OR_PUT);
		}
}

void desenharObstaculos(){
	/*
		Fun��o que desenha os obst�culos da fase
	*/

	int deslocamento = SPRITE_COMPLEMENTOS/2;	//Deslocamento necess�rio para desenhar no lugar correto

	for(int i = 0; i < quantidadeDeObstaculos; i++){
		putimage(posicoes[obstaculos[i].anel][obstaculos[i].fatia].x - deslocamento,
			posicoes[obstaculos[i].anel][obstaculos[i].fatia].y - deslocamento, mascaraObjetos[0], AND_PUT);
		putimage(posicoes[obstaculos[i].anel][obstaculos[i].fatia].x - deslocamento,
			posicoes[obstaculos[i].anel][obstaculos[i].fatia].y - deslocamento, objetos[0], OR_PUT);
	}
}

void desenharInicio(){
	/*
		Fun��o que desenha o in�cio da fase
	*/

	int deslocamento = SPRITE_COMPLEMENTOS/2; //Deslocamento necess�rio para desenhar no lugar correto

	putimage(posicoes[inicioEFim[fase][0].anel][inicioEFim[fase][0].fatia].x - deslocamento,
		posicoes[inicioEFim[fase][0].anel][inicioEFim[fase][0].fatia].y - deslocamento, mascaraObjetos[2], AND_PUT);
	putimage(posicoes[inicioEFim[fase][0].anel][inicioEFim[fase][0].fatia].x - deslocamento,
		posicoes[inicioEFim[fase][0].anel][inicioEFim[fase][0].fatia].y - deslocamento, objetos[2], OR_PUT);
}

void desenharFim(){
	/*
		Fun��o que desenha o in�cio da fase
	*/

	int deslocamento = SPRITE_COMPLEMENTOS/2; //Deslocamento necess�rio para desenhar no lugar correto

	putimage(posicoes[inicioEFim[fase][1].anel][inicioEFim[fase][1].fatia].x - deslocamento,
		posicoes[inicioEFim[fase][1].anel][inicioEFim[fase][1].fatia].y - deslocamento, mascaraObjetos[3], AND_PUT);
	putimage(posicoes[inicioEFim[fase][1].anel][inicioEFim[fase][1].fatia].x - deslocamento,
		posicoes[inicioEFim[fase][1].anel][inicioEFim[fase][1].fatia].y - deslocamento, objetos[3], OR_PUT);
}

void carregarFase(){
	/*
		Fun��o que carrega uma nova fase salva as vari�veis no lugar correto
	*/

	posicaoPersonagem = inicioEFim[fase][0];
	spritePersonagem.x = 0;		spritePersonagem.y = 0;

	setactivepage(1);
	desenharTelaLoading();      
    setvisualpage(1);
    
    setactivepage(2);
    carregarFundo();
	carregarAneisEFatias();
	carregarPosicoes();
	carregarSetas();
	carregarPersonagem();
	carregarObjetos();
	carregarObstaculosEChaves();
}

bool passarDeFase(bool movimentoPersonagem){
	/*
		Fun��o que checa se o jogador conseguiu passar de fase ou n�o
		Recebe como par�metro se o personagem est� se movimentando ou n�o
		Retorna se o personagem est� em movimento ou n�o
	*/

	if(movimentoPersonagem){  //S� funciona se o personagem estiver em movimento
		//Checa se o personagem bateu em algum obst�culo
		for(int i = 0; i < quantidadeDeObstaculos; i++)
			if(compararCoordenadas(posicaoPersonagem, obstaculos[i])){
				sndPlaySound(".\\audios\\obstaculo.wav", SND_ASYNC);
				posicaoPersonagem = inicioEFim[fase][0];
				return(false);
			}

		//Checa se o personagem pegou uma chave
		for(int i = 0; i < quantidadeDeChaves; i++)
			if(compararCoordenadas(posicaoPersonagem, chaves[i]) && (!pegouChaves[i])){
				sndPlaySound(".\\audios\\chave.wav", SND_ASYNC);
				pegouChaves[i] = true;
				break;
			}

		//Checa se o personagem pegou todas as chaves antes de tentar sair da fase
		if(compararCoordenadas(posicaoPersonagem, inicioEFim[fase][1])){
			for(int i = 0; i < quantidadeDeChaves; i++)
				if(!pegouChaves[i]){
					sndPlaySound(".\\audios\\porta_trancada.wav", SND_ASYNC);
					posicaoPersonagem = inicioEFim[fase][0];
					return(false);
				}

			mciSendString("open .\\audios\\abrir_porta.mp3 type MPEGVideo alias abrir_porta", NULL, 0, 0); 
    		mciSendString("play abrir_porta", NULL, 0, 0);
			sndPlaySound(".\\audios\\passar.wav", SND_ASYNC);
			mudarDeFase();
			return(false);
		}	
	}
	return(movimentoPersonagem);
}

void mudarDeFase(){
	/*
		Fun��o que passa de fase e indica se o jogador venceu
	*/

	fase++;
	if(fase >= TOTAL_DE_FASES){
		venceu = true;
		return;
	}
	carregarFase();
}

bool setVolume(bool volume){
	/*
		Fun��o que liga/desliga o volume da fase
		Par�metro que indica se o volume est� ligado ou desligado
		Retorna se o volume est� ligado ou desligado
	*/

	while(GetKeyState(TECLA_M)&0x80);
	if(volume)
		waveOutSetVolume(0,0x00000000);
	else
		waveOutSetVolume(0,0xFFFFFFFF);
	return(!volume);
}

void carregarMenuPrincipal(){
	/*
		Fun��o que carrega as imagens do menu principal
	*/

	int tamanho = imagesize(0, 0, TAMANHO - 1, TAMANHO - 1);	//Quantidade de mem�ria necess�ria para a imagem

	menuPrincipal = NULL;
	menuPrincipal = realloc(menuPrincipal, tamanho);

	readimagefile(".\\imagens\\introducao\\menu.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
	getimage(0, 0, TAMANHO - 1, TAMANHO - 1, menuPrincipal);
}

void desenharMenuPrincipal(){
	/*
		Fun��o que desenha o menu principal
	*/

	putimage(0, 0, menuPrincipal, COPY_PUT);
}

void carregarTelaLoading(){
	/*
		Fun��o que carrega as telas de loading
	*/
	int tamanho = imagesize(0, 0, TAMANHO - 1, TAMANHO - 1); //Quantidade de mem�ria necess�rio para cada imagem

	textoLoading = malloc(tamanho);
	mascaraTextoLoading = malloc(tamanho);
	telaLoading = (void **)malloc(sizeof(void *) * TOTAL_DE_FASES);
	for(int i = 0; i < TOTAL_DE_FASES; i++)
		telaLoading[i] = malloc(tamanho);

	readimagefile(".\\imagens\\loading\\carregando.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
	getimage(0, 0, TAMANHO - 1, TAMANHO - 1, textoLoading);
	readimagefile(".\\imagens\\loading\\carregandomasc.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
	getimage(0, 0, TAMANHO - 1, TAMANHO - 1, mascaraTextoLoading);

	readimagefile(".\\imagens\\loading\\fase0.jpg", 0, 0, TAMANHO -1, TAMANHO - 1);
	getimage(0, 0, TAMANHO - 1, TAMANHO - 1, telaLoading[0]);
	readimagefile(".\\imagens\\loading\\fase1.jpg", 0, 0, TAMANHO -1, TAMANHO - 1);
	getimage(0, 0, TAMANHO - 1, TAMANHO - 1, telaLoading[1]);
	readimagefile(".\\imagens\\loading\\fase2.jpg", 0, 0, TAMANHO -1, TAMANHO - 1);
	getimage(0, 0, TAMANHO - 1, TAMANHO - 1, telaLoading[2]);
	readimagefile(".\\imagens\\loading\\fase3.jpg", 0, 0, TAMANHO -1, TAMANHO - 1);
	getimage(0, 0, TAMANHO - 1, TAMANHO - 1, telaLoading[3]);
}

void desenharTelaLoading(){
	/*
		Fun��o que desenha a tela de loading levando em considera��o em qual fase o jogador est�
	*/

	putimage(0, 0, telaLoading[fase], COPY_PUT);
	putimage(0, 0, mascaraTextoLoading, AND_PUT);
	putimage(0, 0, textoLoading, OR_PUT);
}

void carregarTelaVitoria(){
	/*
		Fun��o que carrega a imagem da tela de vit�ria
	*/

	int tamanho = imagesize(0, 0, TAMANHO - 1, TAMANHO - 1);	//Quantidade de mem�ria necess�ria para guardar as imagens

	vitoria = malloc(tamanho);
	mascaraVitoria = malloc(tamanho);
	telaFinal = malloc(tamanho);

	readimagefile(".\\imagens\\venceu\\vitoria.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
	getimage(0, 0, TAMANHO - 1, TAMANHO - 1, vitoria);
	readimagefile(".\\imagens\\venceu\\vitoriamasc.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
	getimage(0, 0, TAMANHO - 1, TAMANHO - 1, mascaraVitoria);
	readimagefile(".\\imagens\\venceu\\final.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
	getimage(0, 0, TAMANHO - 1, TAMANHO - 1, telaFinal);
}

void desenharTelaVitoria(){
	/*
		Fun��o que desenha a tela de vit�ria
	*/

	putimage(0, 0, telaFinal, COPY_PUT);
	putimage(0, 0, mascaraVitoria, AND_PUT);
	putimage(0, 0, vitoria, OR_PUT);
}

void carregarPause(){
	/*
		Fun��o que carrega a imagem da tela de pause
	*/

	int tamanho = imagesize(0, 0, TAMANHO - 1, TAMANHO - 1);

	imagemPause = malloc(tamanho);
	readimagefile(".\\imagens\\pause\\pausa.jpg", 0, 0, TAMANHO - 1, TAMANHO - 1);
	getimage(0, 0, TAMANHO - 1, TAMANHO - 1, imagemPause);
}

void desenharPause(){
	/*
		Fun��o que desenha a tela de pause
	*/

	putimage(0, 0, imagemPause, COPY_PUT);
}
