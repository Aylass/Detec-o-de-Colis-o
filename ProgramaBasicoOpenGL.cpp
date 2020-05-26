// **********************************************************************
// PUCRS/FACIN
// COMPUTAÇÃO GRÁFICA
//
// Teste de colisão em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@inf.pucrs.br
// **********************************************************************
#include <iostream>
#include <iomanip>
#include <cmath>
#include <ctime>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <glut.h>
#endif

#include "Ponto.h"
#include "Linha.h"

const int MAX = 300;//máximo de linhas verdes
bool devoTestar = true;
bool devoExibir = true;
bool devoImprimirFPS = false;

float numDIVS = 20; // 10

float numDIVSVERTICAL = 2; // 13

Linha Linhas[MAX];
Linha Veiculo;

typedef struct{
    float inicio;//guarda o x inicial
    float fim;//guarda o x final
    float alturainicio;
    float alturafim;
    int veiculo;//0 se o veiculo não estiver    1 se o veiculo estiver
}Divisoes;

Linha divs[50];//guarda o posicionamento das lihas divisórias
Divisoes divisoes[100];
float coordveiculo[4]; //guardar as coordenadas do veículo

float tx, ty, alfa;
void PrintMenu()
{
    cout << "f - imprime FPS." << endl;
    cout << "ESPACO - liga/desliga teste de colisao." << endl;
}
// ***********************************************
// imprime a matriz de transformaçao da OpenGL
// ***********************************************
void imprimeMatrizGL()
{

    GLfloat matriz_gl[4][4];

    glGetFloatv(GL_MODELVIEW_MATRIX,&matriz_gl[0][0]);

    for(int i = 0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            cout << setw(5) << matriz_gl[i][j] << " ";
        }
        cout << endl;
    }
}
// ***********************************************
//  void InstanciaPonto(Ponto p, Ponto &out)
//
//  Esta função calcula as coordenadas
//  de um ponto no sistema de referência do
//  universo (SRU), ou seja, aplica as rotações,
//  escalas e translações a um ponto no sistema
//  de referência do objeto SRO.
// ***********************************************
void InstanciaPonto(Ponto p, Ponto &out)
{
    GLfloat ponto_novo[4];
    GLfloat matriz_gl[4][4];
    int  i;

    glGetFloatv(GL_MODELVIEW_MATRIX,&matriz_gl[0][0]);

    for(i=0; i<4; i++)
    {
        ponto_novo[i] = matriz_gl[0][i] * p.x +
                        matriz_gl[1][i] * p.y +
                        matriz_gl[2][i] * p.z +
                        matriz_gl[3][i];
    }
    out.x = ponto_novo[0];
    out.y = ponto_novo[1];
    out.z = ponto_novo[2];

    //imprimeMatrizGL();

}

// **********************************************************************
//      Organiza o cenário em subdivisoes do mesmo
//
// **********************************************************************
void SubDivide(int n, int ny){//recebe o número de subdivisoes escolhidas na horizontal e na vertical
    float tamdv = 10.0/n;//tamaho de cada subdivisao em x
    printf("oi %f",tamdv);//ok
    float x = 0;//representa o x do universo lógico
    float y = 0;//representa o y do universo lógico
    int i;
    for(i = 0; i<n;i++){ //cria as linhas divisórias em x
        //inicio da linha
        divs[i].x1 = x;
        divs[i].y1 = 0;
        //fim da linha
        divs[i].x2 = x;
        divs[i].y2 = 10;

        x = x + tamdv;
        //printf("Linha X %d: %f\n",i,divs[i].x1);
    }
    float tamdvy = 10.0/ny;//tamaho de cada subdivisao em y
    printf("%f",tamdvy);//ok
    for(int p = 0; p<ny;p++){ //cria as linhas divisórias em y
        //inicio da linha
        divs[i+p].x1 = 0;
        divs[i+p].y1 = y;
        //fim da linha
        divs[i+p].x2 = 10;
        divs[i+p].y2 = y;
        //printf("%f",y);//ok
        y = y + tamdvy;
       // printf("Linha Y %d: %f\n",i,divs[i+p].y1);
    }
    //cria as divisões
    int aux = 0;
     y = 0;
        for(int yy = 0; yy<ny;yy++){
            divisoes[aux].inicio = divs[0].x1;
            divisoes[aux].fim = divs[1].x1;
            divisoes[aux].veiculo = 0;
            divisoes[aux].alturainicio = y;
            divisoes[aux].alturafim = y+tamdvy;
            for(int xx = 1; xx<n;xx++){
                aux++;
                divisoes[aux].inicio = divs[xx].x1;
                divisoes[aux].fim = divs[xx+1].x1;
                divisoes[aux].veiculo = 0;
                divisoes[aux].alturainicio = y;
                divisoes[aux].alturafim = y+tamdvy;
            }
            divisoes[aux].fim = 10;
            aux++;
            y = y + tamdvy;
        }

    /*for(int i = 0; i<(n*ny);i++){
        printf("Inicia: %f",divisoes[i].inicio);
        printf("   AlturaInicio: %f",divisoes[i].alturainicio);
        printf("   AlturaFim: %f",divisoes[i].alturafim);
        printf("   Termina: %f \n",divisoes[i].fim);
    }*/
}

// **********************************************************************
//
//
// **********************************************************************

// **********************************************************************
//  void init(void)
//  Inicializa os parâmetros globais de OpenGL
//
// **********************************************************************
void init(void)
{
    // Define a cor do fundo da tela (BRANCO)
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    srand(unsigned(time(NULL)));
    for(int i=0; i<MAX; i++)
        //Linhas[i].geraLinha();
        Linhas[i].geraLinhaCurta();
    // Define as coordenadas da linha que representa o veículo
    Veiculo.x1 = 0;
    Veiculo.y1 = 1.0;
    Veiculo.x2 = 0;
    Veiculo.y2 = -1.0;

    // Define a posição inicial da linha que representa o veículo
    tx = 5;
    ty = 5;
    alfa = 0.0;

    //subdivide o espaço
    SubDivide(numDIVS,numDIVSVERTICAL);

}

// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
//
// **********************************************************************
void reshape( int w, int h )
{
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define os limites lógicos da área OpenGL dentro da Janela
    glOrtho(0,10,0,10,0,1);

    // Define a área a ser ocupada pela área OpenGL dentro da Janela
    glViewport(0, 0, w, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

/* ********************************************************************** */
/*                                                                        */
/*  Calcula a interseccao entre 2 retas (no plano "XY" Z = 0)             */
/*                                                                        */
/* k : ponto inicial da reta 1                                            */
/* l : ponto final da reta 1                                              */
/* m : ponto inicial da reta 2                                            */
/* n : ponto final da reta 2                                              */
/*                                                                        */
/* s: valor do parâmetro no ponto de interseção (sobre a reta KL)         */
/* t: valor do parâmetro no ponto de interseção (sobre a reta MN)         */
/*                                                                        */
/* ********************************************************************** */
int intersec2d(Ponto k, Ponto l, Ponto m, Ponto n, double &s, double &t)
{
    double det;

    det = (n.x - m.x) * (l.y - k.y)  -  (n.y - m.y) * (l.x - k.x);

    if (det == 0.0)
        return 0 ; // não há intersecção

    s = ((n.x - m.x) * (m.y - k.y) - (n.y - m.y) * (m.x - k.x))/ det ;
    t = ((l.x - k.x) * (m.y - k.y) - (l.y - k.y) * (m.x - k.x))/ det ;

    return 1; // há intersecção
}

bool HaInterseccao(Ponto k, Ponto l, Ponto m, Ponto n)
{
    int ret;
    double s,t;
    ret = intersec2d( k,  l,  m,  n, s, t);
    if (!ret) return false;
    if (s>=0.0 && s <=1.0 && t>=0.0 && t<=1.0)
        return true;
    else return false;

}
void Redesenha(int i)
{
    glutPostRedisplay();
}
void DesenhaCenario()
{
    //desenha linhas subdivisórias em X
    int p;
    for(p = 0; p<numDIVS;p++){
        glColor3f(0,0,1);
        glLineWidth(1);
        divs[p].desenhaLinha();
    }

    //desenha linhas subdivisórias em Y
    for(int i = 0; i<numDIVSVERTICAL;i++){
        glColor3f(0,0,1);
        glLineWidth(1);
        divs[i+p].desenhaLinha();
    }


    Ponto P1, P2, PA, PB, temp;
    // Calcula e armazena as coordenadas da linha que representa o "veículo"
    glPushMatrix();
    {
        glTranslatef(tx, ty, 0);
        glRotatef(alfa,0,0,1);
        // guarda as coordenadas do primeiro ponto da linha
        temp.set(Veiculo.x1, Veiculo.y1);
        InstanciaPonto(temp, P1);
        temp.set(Veiculo.x2, Veiculo.y2);
        //GuardaCoodenadasDoVeiculo();
        InstanciaPonto(temp, P2);
        coordveiculo[0] = P1.x;
         coordveiculo[1] = P1.y;
          coordveiculo[2] = P2.x;
           coordveiculo[3] = P2.y;
    }
    glPopMatrix();

    // Desenha as linhas do cenário
    glLineWidth(1);
    glColor3f(1,1,0);

    //Lógica força Bruta
   // for(int i=0; i<MAX; i++)
   // {
       // if((Linhas[i].x1>=divisoes[p].inicio)&&(Linhas[i].x1<divisoes[p].fim))

      //  if (devoTestar)   // Esta variável é controlada pela "tecla de espaço"
      //  {
         //   temp.set(Linhas[i].x1, Linhas[i].y1);
        //    InstanciaPonto(temp, PA);
        //    temp.set(Linhas[i].x2, Linhas[i].y2);
       //     InstanciaPonto(temp, PB);
       //     if (HaInterseccao(PA, PB, P1, P2))
          //      glColor3f(1,0,0);
       //     else glColor3f(0,1,0);
      //  }
     //   else glColor3f(0,1,0);
      //  if (devoExibir) // Esta variável é controlada pela 'e'
     //       Linhas[i].desenhaLinha();
  //  }
    //printf("tx%f    ",tx);//ok

    //Lógica Sub Divisões

    //se o veículo se mexeu
    //qual ou quais div ele ta

    //printf("Veiculo se encontra: %d",divisoes[0].veiculo);//ok

    for(int p = 0; p<(numDIVS*numDIVSVERTICAL);p++){//para cada divisao
        if(((coordveiculo[0]>=divisoes[p].inicio)&&(coordveiculo[0]<=divisoes[p].fim)) || ((coordveiculo[2]>=divisoes[p].inicio)&&(coordveiculo[2]<=divisoes[p].fim))){ //define qual horizontal o veículo esta
            if(((coordveiculo[1]>=divisoes[p].alturainicio)&&(coordveiculo[1]<=divisoes[p].alturafim)) || ((coordveiculo[3]>=divisoes[p].alturainicio)&&(coordveiculo[3]<=divisoes[p].alturafim))){ //define qual vertical o veículo esta
                divisoes[p].veiculo = 1;
                //printf("Veiculo se encontra em %d",p);//ok
            }else{
                divisoes[p].veiculo = 0;
            }
        }else{
            divisoes[p].veiculo = 0;
        }
    }

    for(int i=0; i<MAX; i++){
        if (devoTestar)   // Esta variável é controlada pela "tecla de espaço"
        {
            temp.set(Linhas[i].x1, Linhas[i].y1);
            InstanciaPonto(temp, PA);
            temp.set(Linhas[i].x2, Linhas[i].y2);
            InstanciaPonto(temp, PB);
            glColor3f(0,1,0);
        }
        if (devoExibir) // Esta variável é controlada pela 'e'
           Linhas[i].desenhaLinha();
    }

    for(int p = 0; p<(numDIVS*numDIVSVERTICAL);p++){//para cada divisao
        if(divisoes[p].veiculo == 1){
            for(int i=0; i<MAX; i++) {//para cada linha
                 if (((Linhas[i].x1>=divisoes[p].inicio)&&(Linhas[i].x1<divisoes[p].fim)) || ((Linhas[i].x2>=divisoes[p].inicio)&&(Linhas[i].x2<divisoes[p].fim)) ) {
                    if (((Linhas[i].y1>=divisoes[p].alturainicio)&&(Linhas[i].y1<divisoes[p].alturafim)) || ((Linhas[i].y2>=divisoes[p].alturainicio)&&(Linhas[i].y2<divisoes[p].alturafim))) {
                             //ta na divisao
                            if (devoTestar)   // Esta variável é controlada pela "tecla de espaço"
                            {
                                temp.set(Linhas[i].x1, Linhas[i].y1);
                                InstanciaPonto(temp, PA);
                                temp.set(Linhas[i].x2, Linhas[i].y2);
                                InstanciaPonto(temp, PB);
                                if (HaInterseccao(PA, PB, P1, P2))
                                    glColor3f(1,0,0);
                                else glColor3f(0,1,0);
                            }  else glColor3f(0,1,0);
                            if (devoExibir) // Esta variável é controlada pela 'e'
                                Linhas[i].desenhaLinha();
                    }else glColor3f(0,1,0);//nao ta na divisao
                 }else glColor3f(0,1,0);//nao ta na divisao
            }
        }
    }

    // Desenha o veículo de novo
    glColor3f(1,0,1);
    glLineWidth(3);
    glPushMatrix();
    {
        glTranslatef(tx, ty, 0);
        glRotatef(alfa,0,0,1);
        Veiculo.desenhaLinha();
    }
    glPopMatrix();
}
// **********************************************************************
//  void display( void )
//
// **********************************************************************
void display( void )
{
    float new_time,base_time;
    // Limpa a tela com  a cor de fundo
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

#define QTD_FRAMES 500.0

    if (devoImprimirFPS) // Pressione f para imprimir
    {
        base_time = glutGet(GLUT_ELAPSED_TIME);

        for (int i=0; i< QTD_FRAMES;i++) // Repete o desenho do cenario
            DesenhaCenario();
        new_time = glutGet(GLUT_ELAPSED_TIME);

        float fps;
        fps = QTD_FRAMES/(new_time - base_time);

        cout << fps << " FPS." << endl;

        devoImprimirFPS = false;
    }
    else DesenhaCenario();

    glutSwapBuffers();

}


// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
//
// **********************************************************************

void keyboard ( unsigned char key, int x, int y )
{

    switch ( key )
    {
    case 27:        // Termina o programa qdo
        exit ( 0 );   // a tecla ESC for pressionada
        break;
    case 'e':
        devoExibir = !devoExibir;
        break;
    case'r':
        alfa = alfa + 3;
        break;
    case'R':
        alfa = alfa - 3;
        break;
    case' ':
        devoTestar = !devoTestar;
        if (devoTestar)
            cout << "Interseccao LIGADA." << endl;
        else cout << "Interseccao DESLIGADA." << endl;
        break;
    case 'f':
        devoImprimirFPS = true;
        cout << "Comecou a contar..." << endl;
        break;
    case 'p':
        /*for(int p = 0; p<(numDIVS*numDIVSVERTICAL);p++){//para cada divisao
            if(divisoes[p].veiculo==1){
                printf("Veiculo se encontra em %d, %f",p,divisoes[p].inicio);//ok
            }
        }
        printf("\n");*/
        break;
    default:
        break;
    }
    glutPostRedisplay();
}


// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
//
//
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )
{

    float incremento=0.2;

    switch ( a_keys )
    {
    case GLUT_KEY_UP:       // Se pressionar UP
        ty += incremento;
        break;
    case GLUT_KEY_DOWN:     // Se pressionar DOWN
        ty -= incremento;
        break;
    case GLUT_KEY_LEFT:       // Se pressionar LEFT
        tx -= incremento;
        break;
    case GLUT_KEY_RIGHT:     // Se pressionar RIGHT
        tx += incremento;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}
void mouse(int button, int state, int x, int y)
{
    glutPostRedisplay();
}
void mouseMove(int x, int y)
{
    glutPostRedisplay();
}
// **********************************************************************
//  void main ( int argc, char** argv )
//
//
// **********************************************************************
int  main ( int argc, char** argv )
{
    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  ( 650, 500);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de título da janela.
    glutCreateWindow    ( "Cálculo de colisão." );

    // executa algumas inicializações
    init ();


    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // será chamada automaticamente quando
    // for necessário redesenhar a janela
    glutDisplayFunc ( display );
    glutIdleFunc ( display );

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // será chamada automaticamente quando
    // o usuário alterar o tamanho da janela
    glutReshapeFunc ( reshape );

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // será chamada automaticamente sempre
    // o usuário pressionar uma tecla comum
    glutKeyboardFunc ( keyboard );

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" será chamada
    // automaticamente sempre o usuário
    // pressionar uma tecla especial
    glutSpecialFunc ( arrow_keys );
    //glutMouseFunc(mouse);
    //glutMotionFunc(mouseMove);

    PrintMenu();
    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}
