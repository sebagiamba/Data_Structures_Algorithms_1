#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <chrono>
#include <set>
#include <map>

using namespace std;

int ZERO = 0;
int CANT_NEGOCIOS, LIMITE_CONTAGIO;

struct negocio {
    int b;
    int c;
};
vector<negocio> negocios;

// Función auxiliar: Suma el beneficio de los negocios pasados por parámetro.
// Si hay negocios contiguos o el contagio supera el límite permitido retorna 0.
int sumarBeneficio(vector<int> &indices) {
    int beneficio = 0;
    int contagio = 0;
    for (int i = 0; i < indices.size(); i++) {
        beneficio += negocios[indices[i]].b;
        contagio += negocios[indices[i]].c;
        if ((i > 0 && indices[i] <= indices[i - 1] + 1) || contagio > LIMITE_CONTAGIO)
            return 0;
    }
    return beneficio;
}

// Fuerza Bruta: Crea el conjunto de partes de {1..n} y chequea para cada uno si se cumplen las
// condiciones de no contigüidad y contagio menor a LIMITE_CONTAGIO. Devuelve la mejor opción.
// i: índice negocio actual
// indices: índices de negocios a considerar
int FB(int i, vector<int> &indices) {
    // Caso Base
    if (i >= CANT_NEGOCIOS)
        return sumarBeneficio(indices);

    // Caso Recursivo
    indices.push_back(i);
    int rama1 = FB(i + 1, indices);
    indices.pop_back();
    int rama2 = FB(i + 1, indices);
    return max(rama1, rama2);
}

bool poda_factibilidad = true;
bool poda_optimalidad = true;
int maxActual = 0;

bool hayMas(int beneficio, int i) {
    int sumaParcial = beneficio;

    for (; i >= 0; i--)
        sumaParcial += negocios[i].b;

    return sumaParcial >= maxActual;
}

// Backtracking: Crea el árbol solo con las soluciones que cumplen la condición de no contigüidad.
// Se realiza una poda por FACTIBILIDAD evitando así recorrer las ramas que tienen nodos que superan el
// límite de contagio.
// Se realiza una poda por OPTIMALIDAD evitando recorrer ramas que contienen nodos cuyo posible beneficio es menor
// al mejor encontrado hasta el momento.
int BT(int i, int beneficio, int contagio, bool verifica, int ultimoAgregado) {
    // Caso base
    if (i == negocios.size()) {
        if (contagio <= LIMITE_CONTAGIO) {
            if (verifica && maxActual < beneficio) {
                maxActual = beneficio;
                return beneficio;
            } else
                return ZERO;
        } else
            return ZERO;
    }

    // Poda por factibilidad
    if (poda_factibilidad && contagio > LIMITE_CONTAGIO)
        return ZERO;

    // Poda por optimalidad
    if (poda_optimalidad && !hayMas(beneficio, i))
        return ZERO;


    // Sin agregar el i-esimo local:
    int sinActual = BT(i + 1, beneficio, contagio, verifica, ultimoAgregado);

    // Agregando el i-esimo local:
    beneficio += negocios[i].b;
    contagio += negocios[i].c;
    verifica = verifica && abs(ultimoAgregado - i) >= 2;
    ultimoAgregado = i;
    int conActual = BT(i + 1, beneficio, contagio, verifica, ultimoAgregado);

    return max(sinActual, conActual);
}

vector<vector<int>> tbl_memoization;
const int UNDEFINED = -1;

// Programación Dinámica
// i: índice negocio actual
// c: contagio acumulado
int PD(int i, int c) {
    if (tbl_memoization[i][c] == UNDEFINED) {
        if (i >= CANT_NEGOCIOS || c + negocios[i].c > LIMITE_CONTAGIO)
            tbl_memoization[i][c] = 0;
        else {
            int rama1 = PD(i + 2, c + negocios[i].c) + negocios[i].b;
            int rama2 = PD(i + 1, c);
            tbl_memoization[i][c] = max(rama1, rama2);
        }
    }
    return tbl_memoization[i][c];
}

int generarCaso(int caso) {
    switch (caso) {
        case 0:
            CANT_NEGOCIOS = 4;
            LIMITE_CONTAGIO = 30;
            negocios.assign(CANT_NEGOCIOS, {0, 0});
            negocios[0] = {100, 31};
            negocios[1] = {101, 32};
            negocios[2] = {102, 33};
            negocios[3] = {103, 34};
            return 0;
        case 1:
            CANT_NEGOCIOS = 6;
            LIMITE_CONTAGIO = 35;
            negocios.assign(CANT_NEGOCIOS, {0, 0});
            negocios[0] = {10, 0};
            negocios[1] = {10, 0};
            negocios[2] = {10, 0};
            negocios[3] = {10, 0};
            negocios[4] = {10, 0};
            negocios[5] = {10, 0};
            return 30;
        case 2:
            CANT_NEGOCIOS = 5;
            LIMITE_CONTAGIO = 40;
            negocios.assign(CANT_NEGOCIOS, {0, 0});
            negocios[0] = {50, 10};
            negocios[1] = {25, 10};
            negocios[2] = {10, 20};
            negocios[3] = {20, 30};
            negocios[4] = {15, 20};
            return 70;
    }
}

void test(string algoritmo, int res_esperado, int caso) {
    int res;

    if (algoritmo == "FB") {
        vector<int> indices = {};
        res = FB(0, indices);
    }

    if (algoritmo == "BT")
        res = BT(0, 0, 0, true, -2);

    if (algoritmo == "PD") {
        tbl_memoization = vector<vector<int >>(CANT_NEGOCIOS + 2, vector<int>(LIMITE_CONTAGIO + 2, UNDEFINED));
        res = PD(0, 0);
    }

    if (res != res_esperado) {
        cerr << "Error " << algoritmo << " caso " << caso << ": \nEsperado: " << res_esperado << "\nResultado: "
             << res << "\n";
        return;
    }
    cout << "Pasó caso " << caso << ", algoritmo " << algoritmo << "\n";
}

void correrTests() {
    for (int i = 0; i < 3; i++) {
        int res_esperado = generarCaso(i);
        test("FB", res_esperado, i);
        test("BT", res_esperado, i);
        test("PD", res_esperado, i);
    }
}

// Recibe por parámetro qué algoritmos utilizar para la ejecución separados por espacios.
// Imprime por clog la información de ejecución de los algoritmos.
// Imprime por cout el resultado de algun algoritmo ejecutado.
int main(int argc, char **argv) {
    // Leemos el parametro que indica el algoritmo a ejecutar.
    map<string, string> algoritmos_implementados = {
            {"FB",    "Fuerza Bruta"},
            {"BT",    "Backtracking con podas"},
            {"BT-F",  "Backtracking con poda por factibilidad"},
            {"BT-O",  "Backtracking con poda por optimalidad"},
            {"DP",    "Programacion dinámica (Top-Down)"},
            {"TESTS", "Ejecuta los tests unitarios"},
    };

    // Verificar que el algoritmo pedido exista.
    if (argc < 2 || algoritmos_implementados.find(argv[1]) == algoritmos_implementados.end()) {
        cerr << "Algoritmo no encontrado: " << argv[1] << endl;
        cerr << "Los algoritmos existentes son: " << endl;
        for (auto &alg_desc: algoritmos_implementados)
            cerr << "\t- " << alg_desc.first << ": " << alg_desc.second << endl;
        return 0;
    }

    string algoritmo = argv[1];

    if (algoritmo != "TESTS") {
        cin >> CANT_NEGOCIOS >> LIMITE_CONTAGIO;
        negocios.assign(CANT_NEGOCIOS, {0, 0});
        for (int i = 0; i < CANT_NEGOCIOS; ++i) cin >> negocios[i].b >> negocios[i].c;
    }

    // Ejecutamos el algoritmo y obtenemos su tiempo de ejecución.
    int optimum;
    optimum = 0;
    vector<int> indices = {};

    auto start = chrono::steady_clock::now();
    if (algoritmo == "FB") {
        optimum = FB(0, indices);
    } else if (algoritmo == "BT") {
        poda_optimalidad = poda_factibilidad = true;
        optimum = BT(0, 0, 0, true, -2);
    } else if (algoritmo == "BT-F") {
        poda_optimalidad = false;
        poda_factibilidad = true;
        optimum = BT(0, 0, 0, true, -2);
    } else if (algoritmo == "BT-O") {
        poda_optimalidad = true;
        poda_factibilidad = false;
        optimum = BT(0, 0, 0, true, -2);
    } else if (algoritmo == "DP") {
        // Inicializa la tabla de memoización con UNDEFINED: tbl_memoization[CANT_NEGOCIOS+2][LIMITE_CONTAGIO+2].
        tbl_memoization = vector<vector<int >>(CANT_NEGOCIOS + 2, vector<int>(LIMITE_CONTAGIO + 2, UNDEFINED));
        // Obtenemos la solucion optima.
        optimum = PD(0, 0);
    } else if (algoritmo == "TESTS") {
        correrTests();
    }

    auto end = chrono::steady_clock::now();
    double total_time = chrono::duration<double, milli>(end - start).count();

    // Imprimimos el tiempo de ejecución por stderr.
    clog << total_time << endl;

    // Imprimimos el resultado por stdout.
    cout << optimum << endl;
    return 0;
}
