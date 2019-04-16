#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>
#include <vector>

// Este es el método principal que debe contener los 4 Comportamientos_Jugador
// que se piden en la práctica. Tiene como entrada la información de los
// sensores y devuelve la acción a realizar.
Action ComportamientoJugador::think(Sensores sensores) {
	if (sensores.nivel != 4){ // Estoy en el nivel 1
		if (sensores.mensajeF != -1){ // Primer movimiento
			fil = sensores.mensajeF; // Set donde estoy
			col = sensores.mensajeC;
			brujula = 0; // 0 corresponde con Norte
			ultimaAccion=actIDLE;

			actual.fila = fil; // Set donde estoy vol.2
			actual.columna = col;
			actual.orientacion = brujula;

			destino.fila = sensores.destinoF; //  Set a donde voy
			destino.columna = sensores.destinoC;
		}
	}
	else { // Estoy en el nivel 2
		cout << "Aún no implementado el nivel 2" << endl;
	}
	switch(ultimaAccion){ // Corregir fil y brujula con respecto a la ultima accion
		case actTURN_R: brujula=(brujula+1)%4; break;
		case actTURN_L: brujula=(brujula+3)%4; break;
		case actFORWARD:
			switch(brujula){
				case 0: fil--; break;
				case 1: col++; break;
				case 2: fil++; break;
				case 3: col--; break;
			}
			break;
	}
	cout << "Fila: " << fil << " Col: " << col << " Or: " << brujula << endl;

	//Ha cambiado el destino?
	if(sensores.destinoF != destino.fila || sensores.destinoC != destino.columna){
		destino.fila=sensores.destinoF; // Actualizamos destino
		destino.columna=sensores.destinoC;
		hayPlan=false;
	}

	if(!hayPlan){
		actual.fila=fil;
		actual.columna=col;
		actual.orientacion=brujula;
		hayPlan=pathFinding(sensores.nivel,actual,destino,plan);
	}
	Action sigAccion;
	if(hayPlan && plan.size()>0){
		sigAccion=plan.front();
		plan.erase(plan.begin());
	} else{
		// Sistema reactivo
		cout << "No hay plan. Problema.";
	}

	ultimaAccion=sigAccion;
  return sigAccion;
}


// Llama al algoritmo de busqueda que se usará en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding (int level, const estado &origen, const estado &destino, list<Action> &plan){
	switch (level){
		case 1: cout << "Busqueda en profundad\n";
			      return pathFinding_Profundidad(origen,destino,plan);
						break;
		case 2: cout << "Busqueda en Anchura\n";
			      return pathFinding_Anchura(origen,destino,plan);
						break;
		case 3: cout << "Busqueda Costo Uniforme\n";
						pathFinding_CostoUniforme(origen, destino, plan);
						break;
		case 4: cout << "Busqueda para el reto\n";
						// Incluir aqui la llamada al algoritmo de búsqueda usado en el nivel 2
						break;
	}
	cout << "Comportamiento sin implementar\n";
	return false;
}


//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el código en carácter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla){
	if (casilla=='P' or casilla=='M' or casilla =='D')
		return true;
	else
	  return false;
}

// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st){
	int fil=st.fila, col=st.columna;

  // calculo cual es la casilla de delante del agente
	switch (st.orientacion) {
		case 0: fil--; break;
		case 1: col++; break;
		case 2: fil++; break;
		case 3: col--; break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil<0 or fil>=mapaResultado.size()) return true;
	if (col<0 or col>=mapaResultado[0].size()) return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col])){
		// No hay obstaculo, actualizo el parámetro st poniendo la casilla de delante.
    st.fila = fil;
		st.columna = col;
		return false;
	} else{
	  return true;
	}
}

struct nodo{
	estado st;
	list<Action> secuencia;
	int coste;
	bool operator < (const nodo &n) const{
		return (this->coste < n.coste);
	}
};

struct ComparaEstados{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};

// Implementación de la búsqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	stack<nodo> pila;											// Lista de Abiertos

  nodo current;
	current.st = origen;
	current.secuencia.empty();

	pila.push(current);

  while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		pila.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			pila.push(hijoTurnR);

		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			pila.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				pila.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la pila
		if (!pila.empty()){
			current = pila.top();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}


	return false;
}


//---------------------- Implementación de la busqueda en anchura ---------------------------

// Implementación de la búsqueda en anchura.
// Entran los puntos origen y destino y devuelve la secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Anchura (const estado &origen,const estado &destino,list<Action> &plan) {
	cout << "Calculando plan de busqueda en anchura\n";
	plan.clear(); 												// Borro la lista
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	queue<nodo> cola;											// Lista de Abiertos
	nodo padre;
	padre.st = origen;
	padre.secuencia.empty();
	cola.push(padre);

  while ( !cola.empty() && (padre.st.fila!=destino.fila || padre.st.columna!=destino.columna) ){
		cola.pop();
		generados.insert(padre.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = padre;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			cola.push(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = padre;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			cola.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = padre;
		if (!HayObstaculoDelante(hijoForward.st)){ // Comprueba si puede avanzar, y si sí, avanza hijoForward
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				cola.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la pila
		if (!cola.empty()){
			padre = cola.front();
		}
	}

  cout << "Terminada la busqueda\n";

	if (padre.st.fila == destino.fila && padre.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = padre.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		cout << "Rumbo a " << padre.st.fila << " " << padre.st.columna << endl;
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}


	return false;
}


//---------------------- Implementación de la busqueda en costo uniforme ---------------------------

struct ComparaNodos{
	bool operator() (const nodo &n1, const nodo &n2) const{
		return n2 < n1;
	}
};

void aniade_modifica (priority_queue<nodo> &abiertos, const nodo &hijo){
	queue<nodo> aux;
	nodo n;
	bool encontrado=false;
	while( !abiertos.empty() && !encontrado ){ 	// Recorre abiertos
		n=abiertos.top();
		abiertos.pop();
		if( n.st == hijo.st ){
			encontrado=true;
			if(n.coste>hijo.coste){
				n.coste=hijo.coste;
			}
		}
		aux.push(n);
	}
	while(!aux.empty()){ // Devuelve los elementos a abiertos
		n=aux.front();
		aux.pop();
		abiertos.push(n);
	}
}

bool ComportamientoJugador::pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan){
	cout << "Calculando plan de busqueda de costo uniforme" << endl;
	plan.clear(); 																// Limpio la lista
	set<estado,ComparaEstados> cerrados; 					// set de Cerrados
	priority_queue<nodo> abiertos;								// cola de Abiertos
	nodo padre;
	padre.st = origen;
	padre.secuencia.empty();
	padre.coste=mapaResultado[origen.fila][origen.columna];
	cout << "Nodo raiz: x=" << padre.st.fila << " y=" << padre.st.columna
	<< " or=" << padre.st.orientacion << " coste=" << padre.coste << endl;
	abiertos.push(padre);

	while (!abiertos.empty() && (padre.st.fila!=destino.fila||padre.st.columna!=destino.columna) ){
		abiertos.pop();
		cerrados.insert(padre.st);

		nodo hijo=padre;
		hijo.st.orientacion=(hijo.st.orientacion +1)%4; // Hijo girar derecha
		if(cerrados.find(hijo.st)!=cerrados.end()){
			hijo.secuencia.push_back(actTURN_R);
			hijo.coste++;
			aniade_modifica(abiertos,hijo); // Si no esta, lo añade, si sí, lo modifica
		}

	padre=abiertos.top();
	}
}

// Sacar por la términal la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}



void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}


// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
}



int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
