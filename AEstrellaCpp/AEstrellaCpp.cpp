#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <chrono>

using namespace std;

// Definición de la estructura Punto
struct Punto {
    short i;
    short j;

    Punto() : i(0), j(0) {}

    Punto(int i, int j) : i(static_cast<short>(i)), j(static_cast<short>(j)) {}

    bool operator==(const Punto& other) const {
        return i == other.i && j == other.j;
    }
};

// Especialización de hash para Punto
struct HashPunto {
    size_t operator()(const Punto& p) const {
        return hash<short>()(p.i) ^ hash<short>()(p.j);
    }
};

// Definición de las direcciones
enum class direcciones_t {
    UP, DOWN, LEFT, RIGHT
};


class Nodo {
public:
    Punto pto;
    float heuristica;
    float fTotal;
    int costoAcumulado;
    Nodo* padre;

    Nodo() : pto(), heuristica(0), fTotal(0), costoAcumulado(0), padre(nullptr) {}

    Nodo(Nodo* padre, const Punto& pto, float heuristica) :
        pto(pto),
        heuristica(heuristica),
        padre(padre),
        costoAcumulado(padre->costoAcumulado + 1),
        fTotal(static_cast<float>(costoAcumulado) + heuristica) {}

    Nodo(const Punto& p, float heuristica) :
        pto(p),
        heuristica(heuristica),
        fTotal(heuristica),
        costoAcumulado(0),
        padre(nullptr) {}
};

// Definición de la clase Laberinto
class Laberinto {
private:
    vector<vector<char>> laberinto;
    Punto inicio;
    Punto final;
    Nodo* inicial;
    unordered_map<Punto, Nodo*, HashPunto> entrada;
    unordered_map<Punto, Nodo*, HashPunto> salida;
    vector<Nodo*> solucion;
    int size;
    Nodo* llegada;

public:
    Laberinto(const string& path) {
        ifstream archivo(path);
        if (archivo.is_open()) {
            string linea;
            getline(archivo, linea);
            size = linea.length();
            laberinto.resize(size, vector<char>(size));
            for (int j = 0; j < size; j++) {
                laberinto[0][j] = linea[j];
            }

            int i = 1;
            while (getline(archivo, linea)) {
                for (int j = 0; j < size; j++) {
                    laberinto[i][j] = linea[j];
                }
                i++;
            }
            archivo.close();

            // Inicializar inicio y final
            for (int k = 0; k < size; k++) {
                for (int j = 0; j < size; j++) {
                    if (laberinto[k][j] == 'A') {
                        inicio = Punto(k, j);
                    }
                    if (laberinto[k][j] == 'B') {
                        final = Punto(k, j);
                    }
                }
            }
        }
        else {
            cerr << "No se pudo abrir el archivo: " << path << endl;
        }
    }

    void printLaberinto() {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                cout << laberinto[i][j];
            }
            cout << endl;
        }
    }

    void printInicioFinal() {
        cout << "inicio: " << inicio.i << "," << inicio.j << endl;
        cout << "final: " << final.i << "," << final.j << endl;
    }

    bool MovValido(const Punto& punto) {
        if (punto.i >= size || punto.j >= size || punto.i < 0 || punto.j < 0 || laberinto[punto.i][punto.j] == '#') {
            return false;
        }
        return true;
    }

    Punto Arriba(const Punto& actual) { return Punto(actual.i - 1, actual.j); }
    Punto Abajo(const Punto& actual) { return Punto(actual.i + 1, actual.j); }
    Punto Izquierda(const Punto& actual) { return Punto(actual.i, actual.j - 1); }
    Punto Derecha(const Punto& actual) { return Punto(actual.i, actual.j + 1); }

    int Manhattan(const Punto& puntoInicial, const Punto& puntoFinal) {
        return abs(puntoInicial.i - puntoFinal.i) + abs(puntoInicial.j - puntoFinal.j);
    }

    int Alternativa(const Punto& puntoInicial, const Punto& puntoFinal) {
        return 1.6f * static_cast<float>(Manhattan(puntoInicial, puntoFinal));
    }


    vector<Nodo*> vecinosAArreglo(Nodo* actual) {
        vector<Nodo*> vecinos(4, nullptr);
        if (MovValido(Arriba(actual->pto))) {
            vecinos[0] = new Nodo(actual, Arriba(actual->pto), Manhattan(Arriba(actual->pto), this->final));
        }
        if (MovValido(Abajo(actual->pto))) {
            vecinos[1] = new Nodo(actual, Abajo(actual->pto), Manhattan(Abajo(actual->pto), this->final));
        }
        if (MovValido(Izquierda(actual->pto))) {
            vecinos[2] = new Nodo(actual, Izquierda(actual->pto), Manhattan(Izquierda(actual->pto), this->final));
        }
        if (MovValido(Derecha(actual->pto))) {
            vecinos[3] = new Nodo(actual, Derecha(actual->pto), Manhattan(Derecha(actual->pto), this->final));
        }
        return vecinos;
    }


    // Método para usar la heurística Manhattan

    void AestrellaManhattan() {

        // Se calcula la distancia usando Manhattan para priorizar 
        // los nodos que estén más cerca del objetivo, luego, la cola
        // de prioridad siempre devuelve el menor fTotal
        this->inicial = new Nodo(this->inicio, Manhattan(this->inicio, this->final));
        priority_queue<pair<double, Nodo*>, vector<pair<double, Nodo*> >, greater<pair<double, Nodo*> > > menorF;

        // Se inserta el nodo inicial en menorF
        // Se agrega el nodo inicial a entrada, almacenando
        // los nodos no explorados.
        menorF.push(make_pair(this->inicial->fTotal, this->inicial));
        entrada[this->inicial->pto] = this->inicial;

        // Blucle mediante el que se explora hasta que se encuentre el punto
        // final, luego elimina el menorF que ya se evaluó

        Nodo* actual = nullptr;
        while (!menorF.empty()) {
            actual = menorF.top().second;
            menorF.pop();

            // Se verifica si ya se llegó al destino
            // y rompe el bucle.
            if (actual->pto == this->final) 
            {
                break;
            }

            entrada.erase(actual->pto);
            salida[actual->pto] = actual;

            // Se obtienen los vecinos accesibles desde el nodo
            // actual, hace las comparaciones correspondientes y
            // recorre los vecinos.
            vector<Nodo*> vecinos = vecinosAArreglo(actual);
            for (Nodo* nodovecino : vecinos) 
            {
                if (nodovecino != nullptr)//Si el valor es null, nodovecino no es valido 
                {
                    //Se revisa si existe el mismo pto en listas entrada
                    //y salida y si es que tiene mayor costo, es remplazado por
                    //nodovecino
                    if (entrada.count(nodovecino->pto)) 
                    {
                        if (entrada[nodovecino->pto]->costoAcumulado > nodovecino->costoAcumulado)
                        {
                            entrada.erase(nodovecino->pto);
                        }
                    }
                    if (salida.count(nodovecino->pto)) {
                        if (salida[nodovecino->pto]->costoAcumulado > nodovecino->costoAcumulado)
                        {
                            salida.erase(nodovecino->pto);
                        }
                    }
                    //En el caso de que no este en ninguna lista, se añade
                    //a entrada para que sea procesado
                    if (!salida.count(nodovecino->pto) && !entrada.count(nodovecino->pto))
                    {
                        entrada[nodovecino->pto] = nodovecino;
                        menorF.push(make_pair(nodovecino->fTotal, nodovecino));
                    }
                }
            }
        }


        // Retrocede desde actual hasta llegar al inicio, almacena
        // los nodos y lo invierte para que sea el orden correcto, dando
        // el camino más óptimo.
        Nodo* aux = actual;
        while (aux->padre != nullptr) 
        {
            this->solucion.push_back(aux);
            aux = aux->padre;
        }
        this->solucion.push_back(aux);
        reverse(this->solucion.begin(), this->solucion.end());
    }

  
    //Funcion que usa H alternativa
    vector<Nodo*> vecinosAArregloAlt(Nodo* actual) 
    {
        vector<Nodo*> vecinos(4, nullptr);
        if (MovValido(Arriba(actual->pto))) {
            vecinos[0] = new Nodo(actual, Arriba(actual->pto), Alternativa(Arriba(actual->pto), this->final));
        }
        if (MovValido(Abajo(actual->pto))) {
            vecinos[1] = new Nodo(actual, Abajo(actual->pto), Alternativa(Abajo(actual->pto), this->final));
        }
        if (MovValido(Izquierda(actual->pto))) {
            vecinos[2] = new Nodo(actual, Izquierda(actual->pto), Alternativa(Izquierda(actual->pto), this->final));
        }
        if (MovValido(Derecha(actual->pto))) {
            vecinos[3] = new Nodo(actual, Derecha(actual->pto), Alternativa(Derecha(actual->pto), this->final));
        }
        return vecinos;
    }

    // Lo mismo que arriba pero se usa la heurística
    // alternativa en lugar de la de Manhattan normal.

    void AestrellaAlternativa() {
        this->inicial = new Nodo(this->inicio, Alternativa(this->inicio, this->final));
        priority_queue<pair<double, Nodo*>, vector<pair<double, Nodo*> >, greater<pair<double, Nodo*> > > menorF;

        menorF.push(make_pair(this->inicial->fTotal, this->inicial));
        entrada[this->inicial->pto] = this->inicial;

        Nodo* actual = nullptr;
        while (!menorF.empty()) {
            actual = menorF.top().second;
            menorF.pop();

            if (actual->pto == this->final) {
                break;
            }

            entrada.erase(actual->pto);
            salida[actual->pto] = actual;

            vector<Nodo*> vecinos = vecinosAArregloAlt(actual);
            for (Nodo* nodovecino : vecinos) {
                if (nodovecino != nullptr) {
                    if (entrada.count(nodovecino->pto)) {
                        if (entrada[nodovecino->pto]->costoAcumulado > nodovecino->costoAcumulado) {
                            entrada.erase(nodovecino->pto);
                        }
                    }
                    if (salida.count(nodovecino->pto)) {
                        if (salida[nodovecino->pto]->costoAcumulado > nodovecino->costoAcumulado) {
                            salida.erase(nodovecino->pto);
                        }
                    }
                    if (!salida.count(nodovecino->pto) && !entrada.count(nodovecino->pto)) {
                        entrada[nodovecino->pto] = nodovecino;
                        menorF.push(make_pair(nodovecino->fTotal, nodovecino));
                    }
                }
            }
        }



        Nodo* aux = actual;
        while (aux->padre != nullptr) {
            this->solucion.push_back(aux);
            aux = aux->padre;
        }
        this->solucion.push_back(aux);
        reverse(this->solucion.begin(), this->solucion.end());
    }

    void printNodosSol() {
        for (Nodo* nodo : solucion) {
            cout << "punto: " << nodo->pto.i << "," << nodo->pto.j << endl;
        }
    }

    void printNumNodos() {
        cout << "Nodos visitados: " << salida.size() << endl;
        cout << "Largo del camino: " << solucion.size() - 1 << endl;
    }

    void printLaberintoCamino() {
        vector<vector<char>> labaux = this->laberinto;
        for (Nodo* nodo : solucion) {
            if (!(nodo == solucion.back() || nodo == solucion.front())) {
                labaux[nodo->pto.i][nodo->pto.j] = 'x';
            }
        }
    }

    void printDirecciones() {
        // Se recorre el vector solución
        for (size_t i = 1; i < solucion.size(); i++) {
            Nodo* actual = solucion[i];
            Nodo* anterior = solucion[i - 1];

            // Se determina el movimiento
            if (actual->pto.i == anterior->pto.i - 1 && actual->pto.j == anterior->pto.j) {
                cout << "UP" << endl;
            }
            else if (actual->pto.i == anterior->pto.i + 1 && actual->pto.j == anterior->pto.j) {
                cout << "DOWN" << endl;
            }
            else if (actual->pto.i == anterior->pto.i && actual->pto.j == anterior->pto.j - 1) {
                cout << "LEFT" << endl;
            }
            else if (actual->pto.i == anterior->pto.i && actual->pto.j == anterior->pto.j + 1) {
                cout << "RIGHT" << endl;
            }
        }
        cout << "Nodos visitados: " << salida.size() << endl;
        cout << "Largo del camino: " << solucion.size() - 1 << endl;
    }



};

int main() {
    string pathrelativo = "Laberintos/";
    cout << "Ingresar nombre archivo" << endl;
    string name;
    cin >> name;
    pathrelativo += name;

    Laberinto labprueba(pathrelativo);

    cout << "Elija Heuristica a utilizar:" << endl;
    cout << "1. Manhattan" << endl;
    cout << "2. Alternativa" << endl;
    int x;
    cin >> x;

    auto start = chrono::high_resolution_clock::now();

    switch (x) 
    {
    case 1:
        labprueba.AestrellaManhattan();
        break;
    case 2:
        labprueba.AestrellaAlternativa();
        break;

    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end - start);   

    cout << "Tiempo " << duration.count() << " Segundos" << endl;


    


    return 0;
}