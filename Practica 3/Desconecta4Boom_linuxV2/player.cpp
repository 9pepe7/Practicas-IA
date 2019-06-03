#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include "player.h"
#include "environment.h"

using namespace std;

const double masinf=9999999999.0, menosinf=-9999999999.0;


// Constructor
Player::Player(int jug){
    jugador_=jug;
}

// Actualiza el estado del juego para el jugador
void Player::Perceive(const Environment & env){
    actual_=env;
}

double Puntuacion(int jugador, const Environment &estado){
    double suma=0;

    for (int i=0; i<7; i++)
      for (int j=0; j<7; j++){
         if (estado.See_Casilla(i,j)==jugador){
            if (j<3)
               suma += j;
            else
               suma += (6-j);
         }
      }

    return suma;
}


// Funcion de valoracion para testear Poda Alfabeta
double ValoracionTest(const Environment &estado, int jugador){
    int ganador = estado.RevisarTablero();

    if (ganador==jugador)
       return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
            return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
            return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else
          return Puntuacion(jugador,estado);
}

// ------------------- Los tres metodos anteriores no se pueden modificar
// Funciones auxiliares para calcular la heuristica
// Lo basaremos en cuantas fichas del jugador hay seguidas en el tablero
// En horizontal:
int Hor(const Environment &estado, int jug){
  int puntos=0, juntas=0, rival, bomb, riv_bomb;
  if(jug==1){ // Damos valores en funcion de See_Casilla
    rival=2;
    bomb=4;
    riv_bomb=5;
  } else{ // if jug==2
    rival=1;
    bomb=5;
    riv_bomb=4;
  }
  for(int i=0;i<7;++i){ // Recorremos
    for(int j=0; j<7; ++j){ // Me gustaría usar switches, pero no me deja
      if(estado.See_Casilla(i,j)==jug || estado.See_Casilla(i,j)==bomb){ // Si el jugador tiene una ficha en esa casilla
        ++juntas;
        switch(juntas){
          case 2: puntos+=4; break;
          case 3: puntos+=12; break;
          default: ++puntos; //1
        }
      } else if(estado.See_Casilla(i,j)==rival || estado.See_Casilla(i,j)==riv_bomb){ // si despues de varias seguidas, tiene el rival, no sirve esa racha, y se resta lo sumado
        switch(juntas){
          case 2: puntos-=4; break;
          case 3: puntos-=12; break;
          default: --puntos; //1
        }
        juntas=0; // Se resetea juntas
      } else
        juntas=0; // Si esta vacio
    }
    juntas=0;
  }
  return puntos;
}
// Para la transicion vertical, el algoritmo es el mismo que para la horizontal, cambiando i por j en seecasilla
int Ver(const Environment &estado, int jug){
  int puntos=0, juntas=0, rival, bomb, riv_bomb;
  if(jug==1){ // Damos valores en funcion de See_Casilla
    rival=2;
    bomb=4;
    riv_bomb=5;
  } else{ // if jug==2
    rival=1;
    bomb=5;
    riv_bomb=4;
  }
  for(int i=0;i<7;++i){ // Recorremos
    for(int j=0; j<7; ++j){ // Me gustaría usar switches, pero no me deja
      if(estado.See_Casilla(i,j)==jug || estado.See_Casilla(i,j)==bomb){ // Si el jugador tiene una ficha en esa casilla
        ++juntas;
        switch(juntas){
          case 2: puntos+=4; break;
          case 3: puntos+=12; break;
          default: ++puntos; //1
        }
      } else if(estado.See_Casilla(i,j)==rival || estado.See_Casilla(i,j)==riv_bomb){ // si despues de varias seguidas, tiene el rival, no sirve esa racha, y se resta lo sumado
        switch(juntas){
          case 2: puntos-=4; break;
          case 3: puntos-=12; break;
          default: --puntos; //1
        }
        juntas=0; // Se resetea juntas
      } else
        juntas=0; // Si esta vacio
    }
    juntas=0;
  }
  return puntos;
}
// La diagonal es similar, pero se hace en dos partes
int Dia(const Environment &estado, int jug){
  int puntos=0, juntas=0, rival, bomb, riv_bomb;
  if(jug==1){ // Damos valores en funcion de See_Casilla
    rival=2;
    bomb=4;
    riv_bomb=5;
  } else{ // if jug==2
    rival=1;
    bomb=5;
    riv_bomb=4;
  }
  for(int i=0; i<4; ++i){
    for(int j=3; j<7; ++j){
      for(int k=0; k<4; ++k){
        if(estado.See_Casilla(i+k,j-k)==jug || estado.See_Casilla(i+k,j-k)==bomb){ // Si el jugador tiene una ficha en esa casilla
          ++juntas;
          switch(juntas){
            case 2: puntos+=4; break;
            case 3: puntos+=12; break;
            default: ++puntos; //1
          }
        } else if(estado.See_Casilla(i+k,j-k)==rival || estado.See_Casilla(i+k,j-k)==riv_bomb){ // si despues de varias seguidas, tiene el rival, no sirve esa racha, y se resta lo sumado
          switch(juntas){
            case 2: puntos-=4; break;
            case 3: puntos-=12; break;
            default: --puntos; //1
          }
          juntas=0; // Se resetea juntas
        } else
          juntas=0; // Si esta vacio
      }
      juntas=0;
    }
  }
  for(int i=0; i<4; ++i){
    for(int j=0; j<4; ++j){
      for(int k=0; k<4; ++k){
        if(estado.See_Casilla(i+k,j+k)==jug || estado.See_Casilla(i+k,j+k)==bomb){ // Si el jugador tiene una ficha en esa casilla
          ++juntas;
          switch(juntas){
            case 2: puntos+=4; break;
            case 3: puntos+=12; break;
            default: ++puntos; //1
          }
        } else if(estado.See_Casilla(i+k,j+k)==rival || estado.See_Casilla(i+k,j+k)==riv_bomb){ // si despues de varias seguidas, tiene el rival, no sirve esa racha, y se resta lo sumado
          switch(juntas){
            case 2: puntos-=4; break;
            case 3: puntos-=12; break;
            default: --puntos; //1
          }
          juntas=0; // Se resetea juntas
        } else
          juntas=0; // Si esta vacio
      }
      juntas=0;
    }
  }
  return puntos;
}

// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)
// La primera parte es calcada a la test, la segunda es la que aplica nuestra heuristica
// Se usan 3 funciones auxiliares para ver el estado del tablero
double Valoracion(const Environment &estado, int jugador){
  int ganador = estado.RevisarTablero();

  if (ganador==jugador)
    return 99999999.0; // Gana el jugador que pide la valoracion
  else if (ganador!=0)
    return -99999999.0; // Pierde el jugador que pide la valoracion
  else if (estado.Get_Casillas_Libres()==0)
    return 0;  // Hay un empate global y se ha rellenado completamente el tablero
  else { // Aplicamos la heuristica
    int rival = (jugador==1)?2:1;
    int yo_hor = Hor(estado, jugador),  yo_ver = Ver(estado, jugador),  yo_dia = Dia(estado, jugador),
    rival_hor  = Hor(estado, rival), rival_ver = Ver(estado, rival), rival_dia = Dia(estado, rival);
    return ((rival_hor+rival_ver+rival_dia)-(yo_hor+yo_ver+yo_dia));
  } // Como las funciones suman en positivo las fichas seguidas, y nosotros buscamos que el rival las tenga,
}   // la heuristica hace rival-yo, para favorecer aquellos estado en los que el rival enlace fichas





// Esta funcion no se puede usar en la version entregable
// Aparece aqui solo para ILUSTRAR el comportamiento del juego
// ESTO NO IMPLEMENTA NI MINIMAX, NI PODA ALFABETA
void JuegoAleatorio(bool aplicables[], int opciones[], int &j){
    j=0;
    for (int i=0; i<8; i++){
        if (aplicables[i]){
           opciones[j]=i;
           j++;
        }
    }
}


double Poda_AlfaBeta(const Environment &estado, int jug, bool min_max, int prof, Environment::ActionType &accion, double alpha, double beta){
  if(prof==0 || estado.JuegoTerminado()) // Si termina el juego o la profundidad maxima se ha alcanzado (va bajando)
    return Valoracion(estado, jug);

  int u_accion = -1;
  double valor;
  Environment::ActionType ant_accion;
  Environment hijo = estado.GenerateNextMove(u_accion);
  bool VecAct[8];
  int n_hijos = estado.possible_actions(VecAct); // Posibles futuras acciones

  if(min_max){ // Si min_max es true, alpha
    for(int i=0; i<n_hijos; ++i){
      valor=Poda_AlfaBeta(hijo,jug,!min_max,prof-1,ant_accion,alpha,beta);
      if(valor>alpha){
        alpha=valor;
        accion= static_cast<Environment::ActionType>(u_accion);
      }
      if(beta<=alpha) // malo
        break;
      hijo=estado.GenerateNextMove(u_accion);
    }
    return alpha;
  } else{ // false, beta
    for(int i=0; i<n_hijos; ++i){
      valor=Poda_AlfaBeta(hijo,jug,!min_max,prof-1,ant_accion,alpha,beta);
      if(valor<beta){
        beta=valor;
        accion= static_cast<Environment::ActionType>(u_accion);
      }
      if(beta<=alpha) // malo
        break;
      hijo=estado.GenerateNextMove(u_accion);
    }
    return beta;
  }
}



// Invoca el siguiente movimiento del jugador
Environment::ActionType Player::Think(){
    const int PROFUNDIDAD_MINIMAX = 6;  // Umbral maximo de profundidad para el metodo MiniMax
    const int PROFUNDIDAD_ALFABETA = 8; // Umbral maximo de profundidad para la poda Alfa_Beta

    Environment::ActionType accion; // acci�n que se va a devolver
    bool aplicables[8]; // Vector bool usado para obtener las acciones que son aplicables en el estado actual. La interpretacion es
                        // aplicables[0]==true si PUT1 es aplicable
                        // aplicables[1]==true si PUT2 es aplicable
                        // aplicables[2]==true si PUT3 es aplicable
                        // aplicables[3]==true si PUT4 es aplicable
                        // aplicables[4]==true si PUT5 es aplicable
                        // aplicables[5]==true si PUT6 es aplicable
                        // aplicables[6]==true si PUT7 es aplicable
                        // aplicables[7]==true si BOOM es aplicable



    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha, beta; // Cotas de la poda AlfaBeta

    int n_act; //Acciones posibles en el estado actual


    n_act = actual_.possible_actions(aplicables); // Obtengo las acciones aplicables al estado actual en "aplicables"
    int opciones[10];

    // Muestra por la consola las acciones aplicable para el jugador activo
    //actual_.PintaTablero();
    cout << " Acciones aplicables ";
    (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
    for (int t=0; t<8; t++)
      if (aplicables[t])
         cout << " " << actual_.ActionStr( static_cast< Environment::ActionType > (t)  );
    cout << endl;

    /*
    //--------------------- COMENTAR Desde aqui
    cout << "\n\t";
    int n_opciones=0;
    JuegoAleatorio(aplicables, opciones, n_opciones);

    if (n_act==0){
      (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
      cout << " No puede realizar ninguna accion!!!\n";
      //accion = Environment::actIDLE;
    }
    else if (n_act==1){
           (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
            cout << " Solo se puede realizar la accion "
                 << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[0])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[0]);

         }
         else { // Hay que elegir entre varias posibles acciones
            int aleatorio = rand()%n_opciones;
            cout << " -> " << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[aleatorio])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[aleatorio]);
         }

    //--------------------- COMENTAR Hasta aqui
    */


    //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------


    // Opcion: Poda AlfaBeta
    // NOTA: La parametrizacion es solo orientativa
    alpha=menosinf; beta=masinf;
    valor = Poda_AlfaBeta(actual_, jugador_, true, PROFUNDIDAD_ALFABETA, accion, alpha, beta);
    cout << "Valor AlfaBeta: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;
    return accion;
}
