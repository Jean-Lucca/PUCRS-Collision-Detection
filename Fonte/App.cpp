#include <iostream>
#include <iomanip>
#include <cmath>
#include <ctime>


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
#include "Faixas.h"

#include "Temporizador.h"
#include <vector>
#include <math.h>
#include <algorithm>

const int N_LINHAS = 100;
int N_SUBDIVISOES = 5;
const int MAX_X = 100;

ConjuntoDeFaixas cnj;

int menu;
long int ContadorInt=0;
long int ContChamadas=0;


Linha Linhas[N_LINHAS];

//Estrutura do envelope
struct AABB
{
    Ponto centro;
    Ponto meiaLarg;
};

AABB AABBs[N_LINHAS];

double Abs(double a)
{
    return std::fabs(a);
}
// **********************************************************************
//  void collisionCK()
//  Verifica se existe colisao entre os envelopes
// **********************************************************************
bool collisionCK(AABB &E1, AABB &E2)
{
    if(Abs(E1.centro.x - E2.centro.x) > (E1.meiaLarg.x + E2.meiaLarg.x)){return false; }
    if(Abs(E1.centro.y - E2.centro.y) > (E1.meiaLarg.y + E2.meiaLarg.y)){return false; }
    else {  return true; }
};
// **********************************************************************
//  void buildAABB()
//  Constroi os envelopes
// **********************************************************************

void buildFaixas()
{
    cnj.CriaFaixas(N_SUBDIVISOES);
    int auxx = MAX_X/N_SUBDIVISOES;
    for(int i=0;i<N_LINHAS;i++)
    {
        int iniciox = Linhas[i].minx/auxx;
        int fimx = Linhas[i].maxx/auxx;

        if(iniciox < 0) { iniciox = 0; }
        if(fimx >= N_SUBDIVISOES) { fimx = N_SUBDIVISOES-1; }

        while(iniciox <= fimx)
        {
            cnj.CadastraArestaNaFaixa((iniciox), i);
            iniciox++;
        }
    }
}

// **********************************************************************
//  void init(void)
//  Inicializa os parâmetros globais de OpenGL
// **********************************************************************
void init(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    srand(unsigned(time(NULL)));
    Ponto PA,PB;
    for(int i=0; i< N_LINHAS; i++)
    {
        Linhas[i].geraLinha(MAX_X, 5);
        PA.set(Linhas[i].x1, Linhas[i].y1);
        PB.set(Linhas[i].x2, Linhas[i].y2);
        Linhas[i].maxx = ObtemMaximo(PA,PB).x;
        Linhas[i].maxy = ObtemMaximo(PA,PB).y;
        Linhas[i].minx = ObtemMinimo(PA,PB).x;
        Linhas[i].miny = ObtemMinimo(PA,PB).y;
        AABBs[i].centro = Ponto(((Linhas[i].x1 + Linhas[i].x2)/2), ((Linhas[i].y1 + Linhas[i].y2)/2));
        AABBs[i].meiaLarg = Ponto(Abs((Linhas[i].minx - Linhas[i].maxx)/2), Abs((Linhas[i].miny - Linhas[i].maxy)/2));
    }

    cout<< "Selecione o tipo de teste de colisao " <<endl;
    cout<< "1: Testar colisoes " <<endl;
    cout<< "2: Testar colisoes usando envelopes " <<endl;
    cout<< "3: Testar colisoes usando faixas " <<endl;
    cin >> menu;

    if(menu==3)
    {
        cout<< "Insira o numero de subdivisoes: " <<endl;
        cin >> N_SUBDIVISOES;
        if(N_SUBDIVISOES > 1 && N_SUBDIVISOES <=50)
        {
            buildFaixas();
            cnj.MostraFaixas();
        }
        else
        {
            init();
        }
    }
}

// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
//
// **********************************************************************
void reshape( int w, int h )
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,100,0,100,0,1);

    glViewport(0, 0, w, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

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

// **********************************************************************
//
// **********************************************************************
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
// **********************************************************************
//
// **********************************************************************
void DesenhaLinhas()
{
    glColor3f(0,1,0);
    for(int i=0; i< N_LINHAS; i++)
    {
        Linhas[i].desenhaLinha();
    }
}

void TestarTodasContraTodas()
{
    Ponto PA, PB, PC, PD;
    ContChamadas = ContadorInt = 0;

    glLineWidth(2);
    glColor3f(1,0,0);

    for(int i=0; i< N_LINHAS; i++)
    {
        PA.set(Linhas[i].x1, Linhas[i].y1);
        PB.set(Linhas[i].x2, Linhas[i].y2);
        for(int j=0; j< N_LINHAS; j++)
        {
            PC.set(Linhas[j].x1, Linhas[j].y1);
            PD.set(Linhas[j].x2, Linhas[j].y2);

            ContChamadas++;
            if(HaInterseccao(PA,PB,PC,PD))
            {
                ContadorInt ++;
                Linhas[i].desenhaLinha();
                Linhas[j].desenhaLinha();
            }
        }
    }
}

void TestarComFaixas()
{
    Ponto PA, PB, PC, PD;
    ContChamadas = ContadorInt = 0;

    glLineWidth(2);
    glColor3f(1,0,0);

    int auxx = MAX_X/N_SUBDIVISOES;

    for(int i=0;i<N_LINHAS;i++)
    {
        PA.set(Linhas[i].x1, Linhas[i].y1);
        PB.set(Linhas[i].x2, Linhas[i].y2);

        //Encontra quais faixas a linha esta
        int iniciox = Linhas[i].minx/auxx;
        int fimx = Linhas[i].maxx/auxx;

        if(iniciox < 0) { iniciox = 0; }
        if(fimx >= N_SUBDIVISOES) { fimx = N_SUBDIVISOES-1; }

        while(iniciox <= fimx)
        {
            //Verifica se existe interseccao com as outras linhas que estao nas mesmas faixas
            for(int j=0;j<cnj.getFaixa(iniciox).ArestasNaFaixa.size();j++)
            {
                PC.set(Linhas[cnj.getFaixa(iniciox).ArestasNaFaixa[j]].x1, Linhas[cnj.getFaixa(iniciox).ArestasNaFaixa[j]].y1);
                PD.set(Linhas[cnj.getFaixa(iniciox).ArestasNaFaixa[j]].x2, Linhas[cnj.getFaixa(iniciox).ArestasNaFaixa[j]].y2);

                if(i!=cnj.getFaixa(iniciox).ArestasNaFaixa[j])
                {
                    ContChamadas++;
                    if(HaInterseccao(PA,PB,PC,PD))
                    {
                        ContadorInt ++;
                        Linhas[i].desenhaLinha();
                        Linhas[cnj.getFaixa(iniciox).ArestasNaFaixa[j]].desenhaLinha();
                    }
                }
            }
            iniciox++;
        }
    }
}
void TestarComEnvelope()
{
    Ponto PA, PB, PC, PD;
    ContChamadas = ContadorInt = 0;

    glLineWidth(2);
    glColor3f(1,0,0);

    for(int i=0; i< N_LINHAS; i++)
    {
        PA.set(Linhas[i].x1, Linhas[i].y1);
        PB.set(Linhas[i].x2, Linhas[i].y2);
        for(int j=0; j< N_LINHAS; j++)
        {
            PC.set(Linhas[j].x1, Linhas[j].y1);
            PD.set(Linhas[j].x2, Linhas[j].y2);
            if(i!=j)
            {
                //se os envelopes colitem testa
                if(collisionCK(AABBs[i],AABBs[j]))
                {
                    ContChamadas++;
                    if(HaInterseccao(PA,PB,PC,PD))
                    {
                        ContadorInt ++;
                        Linhas[i].desenhaLinha();
                        Linhas[j].desenhaLinha();
                    }
                }
            }
        }
    }
}

// **********************************************************************
//  void display( void )
// **********************************************************************
void display( void )
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    DesenhaLinhas();

    switch ( menu )
    {
    case 1:
        TestarTodasContraTodas();
        break;
    case 2:
        TestarComEnvelope();
        break;
    case 3:
            glLineWidth(1);
            glColor3f(0,0,1);
            for(int i=0;i<MAX_X;)
            {
                glBegin(GL_LINES);
                    glVertex2f(i,0);
                    glVertex2f(i,MAX_X);
                glEnd();
                i = i + MAX_X/N_SUBDIVISOES;
            }
        TestarComFaixas();
        break;
    default:
        break;
    }
    glutSwapBuffers();
}

double nFrames=0;
double TempoTotal=0;
Temporizador T;
double AccumDeltaT=0;
// **********************************************************************
//
// **********************************************************************
void animate()
{
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;

    if (AccumDeltaT > 1.0/30) // fixa a atualização da tela em 30
    {
        AccumDeltaT = 0;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0)
    {
        cout << "Tempo Acumulado: "  << TempoTotal << " segundos. " ;
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames/TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
        cout << "Contador de Intersecoes Existentes:" << ContadorInt/2.0 << endl;
        cout << "Contador de Chamadas:" << ContChamadas << endl;
    }
}
// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
//
// **********************************************************************
void keyboard ( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27:
        exit ( 0 );
        break;
    case' ':
        init();
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

    switch ( a_keys )
    {
    case GLUT_KEY_UP:

        break;
    case GLUT_KEY_DOWN:

        break;
    case GLUT_KEY_LEFT:

        break;
    case GLUT_KEY_RIGHT:

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
// **********************************************************************
int  main ( int argc, char** argv )
{
    cout << "ARGC: " << argc << endl;
    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    glutInitWindowSize  ( 500, 500);

    glutCreateWindow    ( "Algorimos de Calculo de Colisao" );

    init ();

    glutDisplayFunc ( display );
    glutIdleFunc ( animate );

    glutReshapeFunc ( reshape );

    glutKeyboardFunc ( keyboard );

    glutSpecialFunc ( arrow_keys );

    glutMainLoop ( );

    return 0;
}
