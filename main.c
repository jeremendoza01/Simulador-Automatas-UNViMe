#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ESTADOS 1000

int contador_nodos_ast = 0;

typedef struct Nodo
{
    int id;
    char tipo;
    struct Nodo *izquierdo;
    struct Nodo *derecho;
} Nodo;

Nodo *crearHoja(char simbolo)
{
    Nodo *n = (Nodo *)malloc(sizeof(Nodo));
    n->id = contador_nodos_ast++;
    n->tipo = simbolo;
    n->izquierdo = NULL;
    n->derecho = NULL;
    return n;
}

Nodo *crearNodoAST(char operador, Nodo *izquierdo, Nodo *derecho)
{
    Nodo *n = (Nodo *)malloc(sizeof(Nodo));
    n->id = contador_nodos_ast++;
    n->tipo = operador;
    n->izquierdo = izquierdo;
    n->derecho = derecho;
    return n;
}

const char *cursor;
Nodo *parsearExpresion();

Nodo *parsearFactor()
{
    if (*cursor == '(')
    {
        cursor++;
        Nodo *adentro = parsearExpresion();
        if (*cursor == ')')
            cursor++;
        return adentro;
    }
    if ((*cursor >= 'a' && *cursor <= 'z') || (*cursor >= '0' && *cursor <= '9') || *cursor == 'E')
    {
        Nodo *hoja = crearHoja(*cursor);
        cursor++;
        return hoja;
    }
    return NULL;
}

Nodo *parsearCerradura()
{
    Nodo *base = parsearFactor();
    while (*cursor == '*' || *cursor == '+') // Agregamos el '+'
    {
        char operador = *cursor;
        cursor++;
        base = crearNodoAST(operador, base, NULL);
    }
    return base;
}

Nodo *parsearTermino()
{
    Nodo *izquierdo = parsearCerradura();
    while (*cursor == '.' || (*cursor >= 'a' && *cursor <= 'z') || (*cursor >= '0' && *cursor <= '9') || *cursor == 'E' || *cursor == '(')
    {
        if (*cursor == '.')
            cursor++;
        Nodo *derecho = parsearCerradura();
        izquierdo = crearNodoAST('.', izquierdo, derecho);
    }
    return izquierdo;
}

Nodo *parsearExpresion()
{
    Nodo *izquierdo = parsearTermino();
    while (*cursor == '|')
    {
        cursor++;
        Nodo *derecho = parsearTermino();
        izquierdo = crearNodoAST('|', izquierdo, derecho);
    }
    return izquierdo;
}

void imprimirASTDot_recursivo(Nodo *raiz)
{
    if (raiz == NULL)
        return;
    printf("  N%d [label=\"%c\"];\n", raiz->id, raiz->tipo);
    if (raiz->izquierdo)
    {
        printf("  N%d -> N%d;\n", raiz->id, raiz->izquierdo->id);
        imprimirASTDot_recursivo(raiz->izquierdo);
    }
    if (raiz->derecho)
    {
        printf("  N%d -> N%d;\n", raiz->id, raiz->derecho->id);
        imprimirASTDot_recursivo(raiz->derecho);
    }
}

void generarDotAST(Nodo *raiz)
{
    printf("digraph G {\n");
    imprimirASTDot_recursivo(raiz);
    printf("}\n");
}

typedef struct Estado Estado;
typedef struct Transicion Transicion;

struct Transicion
{
    char simbolo;
    Estado *destino;
    Transicion *siguiente;
};

struct Estado
{
    int id;
    int es_final;
    Transicion *transiciones;
};

int contador_estados_afn = 0;
Estado *listaEstadosAFN[MAX_ESTADOS];

Estado *crearEstadoAFN(int es_final)
{
    Estado *e = (Estado *)malloc(sizeof(Estado));
    e->id = contador_estados_afn;
    e->es_final = es_final;
    e->transiciones = NULL;
    listaEstadosAFN[contador_estados_afn++] = e;
    return e;
}

void agregarTransicionAFN(Estado *origen, char simbolo, Estado *destino)
{
    Transicion *t = (Transicion *)malloc(sizeof(Transicion));
    t->simbolo = simbolo;
    t->destino = destino;
    t->siguiente = origen->transiciones;
    origen->transiciones = t;
}

typedef struct
{
    Estado *inicio;
    Estado *fin;
} FragmentoAFN;

FragmentoAFN baseAFN(char simbolo)
{
    Estado *ini = crearEstadoAFN(0);
    Estado *fin = crearEstadoAFN(1);
    agregarTransicionAFN(ini, simbolo, fin);
    FragmentoAFN f = {ini, fin};
    return f;
}

FragmentoAFN unionAFN(FragmentoAFN a, FragmentoAFN b)
{
    Estado *ini = crearEstadoAFN(0);
    Estado *fin = crearEstadoAFN(1);
    a.fin->es_final = 0;
    b.fin->es_final = 0;
    agregarTransicionAFN(ini, 'E', a.inicio);
    agregarTransicionAFN(ini, 'E', b.inicio);
    agregarTransicionAFN(a.fin, 'E', fin);
    agregarTransicionAFN(b.fin, 'E', fin);
    FragmentoAFN f = {ini, fin};
    return f;
}

FragmentoAFN concatAFN(FragmentoAFN a, FragmentoAFN b)
{
    a.fin->es_final = 0;
    agregarTransicionAFN(a.fin, 'E', b.inicio);
    FragmentoAFN f = {a.inicio, b.fin};
    return f;
}

FragmentoAFN kleeneAFN(FragmentoAFN a)
{
    Estado *ini = crearEstadoAFN(0);
    Estado *fin = crearEstadoAFN(1);
    a.fin->es_final = 0;
    agregarTransicionAFN(ini, 'E', a.inicio);
    agregarTransicionAFN(ini, 'E', fin);
    agregarTransicionAFN(a.fin, 'E', a.inicio);
    agregarTransicionAFN(a.fin, 'E', fin);
    FragmentoAFN f = {ini, fin};
    return f;
}

FragmentoAFN positivaAFN(FragmentoAFN a)
{
    Estado *ini = crearEstadoAFN(0);
    Estado *fin = crearEstadoAFN(1);
    a.fin->es_final = 0;

    agregarTransicionAFN(ini, 'E', a.inicio);
    agregarTransicionAFN(a.fin, 'E', a.inicio); // Loop back (repite 1 o más)
    agregarTransicionAFN(a.fin, 'E', fin);

    FragmentoAFN f = {ini, fin};
    return f;
}

void generarDotAFN(FragmentoAFN afn)
{
    printf("digraph {\n");
    printf("  inic [shape=point];\n");
    printf("  inic->q%d;\n", afn.inicio->id);
    for (int i = 0; i < contador_estados_afn; i++)
    {
        Estado *e = listaEstadosAFN[i];
        if (e->es_final)
        {
            printf("  q%d [shape=doublecircle];\n", e->id);
        }
        for (Transicion *t = e->transiciones; t; t = t->siguiente)
        {
            printf("  q%d->q%d [label=\"%c\"];\n", e->id, t->destino->id, t->simbolo);
        }
    }
    printf("}\n");
}

FragmentoAFN ast_a_AFN(Nodo *raiz)
{
    if (!raiz)
        return baseAFN('E');

    if (raiz->tipo == '|')
    {
        return unionAFN(ast_a_AFN(raiz->izquierdo), ast_a_AFN(raiz->derecho));
    }
    else if (raiz->tipo == '.')
    {
        return concatAFN(ast_a_AFN(raiz->izquierdo), ast_a_AFN(raiz->derecho));
    }
    else if (raiz->tipo == '*')
    {
        return kleeneAFN(ast_a_AFN(raiz->izquierdo));
    }
    else if (raiz->tipo == '+')
    {
        return positivaAFN(ast_a_AFN(raiz->izquierdo));
    }
    else
    {
        return baseAFN(raiz->tipo);
    }
}

typedef struct
{
    int id;
    int estados_afn[MAX_ESTADOS];
    int marcado;
    int es_final;
} EstadoAFD;

EstadoAFD listaAFD[MAX_ESTADOS];
int contador_estados_afd = 0;
int tabla_transiciones_afd[MAX_ESTADOS][256];

void clausuraEpsilon(int id_estado_afn, int *conjunto_res)
{
    if (conjunto_res[id_estado_afn])
        return;
    conjunto_res[id_estado_afn] = 1;

    Estado *e = listaEstadosAFN[id_estado_afn];
    for (Transicion *t = e->transiciones; t; t = t->siguiente)
    {
        if (t->simbolo == 'E')
        {
            clausuraEpsilon(t->destino->id, conjunto_res);
        }
    }
}

void operacionMover(int *conjunto_origen, char simbolo, int *conjunto_res)
{
    for (int i = 0; i < contador_estados_afn; i++)
    {
        if (conjunto_origen[i])
        {
            Estado *e = listaEstadosAFN[i];
            for (Transicion *t = e->transiciones; t; t = t->siguiente)
            {
                if (t->simbolo == simbolo)
                {
                    conjunto_res[t->destino->id] = 1;
                }
            }
        }
    }
}

int buscarEstadoAFD(int *conjunto)
{
    for (int i = 0; i < contador_estados_afd; i++)
    {
        int iguales = 1;
        for (int j = 0; j < contador_estados_afn; j++)
        {
            if (listaAFD[i].estados_afn[j] != conjunto[j])
            {
                iguales = 0;
                break;
            }
        }
        if (iguales)
            return i;
    }
    return -1;
}

void convertir_AFN_a_AFD(FragmentoAFN afn, const char *alfabeto)
{
    contador_estados_afd = 0;

    for (int i = 0; i < MAX_ESTADOS; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            tabla_transiciones_afd[i][j] = -1;
        }
    }

    EstadoAFD inicioAFD;
    memset(inicioAFD.estados_afn, 0, sizeof(inicioAFD.estados_afn));
    inicioAFD.id = contador_estados_afd;
    inicioAFD.marcado = 0;
    inicioAFD.es_final = 0;

    clausuraEpsilon(afn.inicio->id, inicioAFD.estados_afn);

    for (int j = 0; j < contador_estados_afn; j++)
    {
        if (inicioAFD.estados_afn[j] && listaEstadosAFN[j]->es_final)
        {
            inicioAFD.es_final = 1;
            break;
        }
    }

    listaAFD[contador_estados_afd++] = inicioAFD;

    int hay_sin_marcar = 1;
    while (hay_sin_marcar)
    {
        hay_sin_marcar = 0;
        for (int i = 0; i < contador_estados_afd; i++)
        {
            if (!listaAFD[i].marcado)
            {
                listaAFD[i].marcado = 1;
                hay_sin_marcar = 1;

                for (int c = 0; alfabeto[c] != '\0'; c++)
                {
                    int conjunto_mover[MAX_ESTADOS] = {0};
                    operacionMover(listaAFD[i].estados_afn, alfabeto[c], conjunto_mover);

                    int conjunto_nuevo[MAX_ESTADOS] = {0};
                    for (int j = 0; j < contador_estados_afn; j++)
                    {
                        if (conjunto_mover[j])
                            clausuraEpsilon(j, conjunto_nuevo);
                    }

                    int vacio = 1;
                    for (int j = 0; j < contador_estados_afn; j++)
                    {
                        if (conjunto_nuevo[j])
                        {
                            vacio = 0;
                            break;
                        }
                    }

                    if (!vacio)
                    {
                        int id_existente = buscarEstadoAFD(conjunto_nuevo);
                        if (id_existente == -1)
                        {
                            EstadoAFD nuevo_estado;
                            memcpy(nuevo_estado.estados_afn, conjunto_nuevo, sizeof(nuevo_estado.estados_afn));
                            nuevo_estado.id = contador_estados_afd;
                            nuevo_estado.marcado = 0;
                            nuevo_estado.es_final = 0;

                            for (int j = 0; j < contador_estados_afn; j++)
                            {
                                if (nuevo_estado.estados_afn[j] && listaEstadosAFN[j]->es_final)
                                {
                                    nuevo_estado.es_final = 1;
                                    break;
                                }
                            }

                            listaAFD[contador_estados_afd] = nuevo_estado;
                            tabla_transiciones_afd[i][(unsigned char)alfabeto[c]] = nuevo_estado.id;
                            contador_estados_afd++;
                        }
                        else
                        {
                            tabla_transiciones_afd[i][(unsigned char)alfabeto[c]] = id_existente;
                        }
                    }
                }
                break;
            }
        }
    }
}

// funcion auxiliar para imprimir las transiciones del AFD
void imprimirTransicionesAFD(const char *alfabeto)
{
    printf("digraph {\n");
    printf("  inic [shape=point];\n");
    printf("  inic->AFD_q0;\n");

    for (int i = 0; i < contador_estados_afd; i++)
    {
        if (listaAFD[i].es_final)
        {
            printf("  AFD_q%d [shape=doublecircle];\n", i);
        }
        for (int c = 0; alfabeto[c] != '\0'; c++)
        {
            int destino = tabla_transiciones_afd[i][(unsigned char)alfabeto[c]];
            if (destino != -1)
            {
                printf("  AFD_q%d -> AFD_q%d [label=\"%c\"];\n", i, destino, alfabeto[c]);
            }
        }
    }
    printf("}\n");
}

int simularAFD_iterativo(int estado_inicial, const char *cadena)
{
    int estado_actual = estado_inicial;
    for (int i = 0; cadena[i] != '\0'; i++)
    {
        unsigned char c = cadena[i];
        estado_actual = tabla_transiciones_afd[estado_actual][c];
        if (estado_actual == -1)
            return 0;
    }
    return listaAFD[estado_actual].es_final;
}

int minigrep(int estado_inicial, const char *linea)
{
    for (int inicio = 0; linea[inicio] != '\0'; inicio++)
    {
        int estado_actual = estado_inicial;

        for (int i = inicio; linea[i] != '\0'; i++)
        {
            unsigned char c = linea[i];
            estado_actual = tabla_transiciones_afd[estado_actual][c];

            // si el camino se rompe, salimos de este bucle y probamos desde la siguiente letra
            if (estado_actual == -1)
                break;

            // si en pleno recorrido caemos en un estado final, hay coincidencia
            if (listaAFD[estado_actual].es_final)
                return 1;
        }
    }
    return 0; // no se encontro la expresion regular en ninguna parte de la línea
}

// MAIN
int main()
{
    int opcion;
    char input[100];
    char alfabeto[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    char cadena[256];
    int regex_cargada = 0;

    Nodo *ast = NULL;
    FragmentoAFN afn;

    printf("=== CONSTRUCTOR Y SIMULADOR DE AUTOMATAS ===\n");

    do
    {
        printf("\n--- MENU PRINCIPAL ---\n");
        printf("1. Ingresar / Cambiar Expresion Regular\n");
        printf("2. Mostrar Arbol Sintactico (DOT)\n");
        printf("3. Mostrar AFN (DOT)\n");
        printf("4. Mostrar AFD (Transiciones DOT)\n");
        printf("5. Evaluar cadena (Simulador AFD iterativo)\n");
        printf("6. Buscar coincidencia en texto (Minigrep)\n");
        printf("7. Salir\n");
        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1)
        {
            while (getchar() != '\n')
                ; // Limpiar el buffer si mete cualquier cosa
            continue;
        }
        getchar(); // limpiar el salto de linea del buffer

        switch (opcion)
        {
        case 1:
            printf("\nIngrese la expresion regular: ");
            scanf("%99s", input);
            getchar(); // limpiar buffer

            // resetear todos los contadores por si ingresa una segunda vez
            contador_nodos_ast = 0;
            contador_estados_afn = 0;
            contador_estados_afd = 0;

            cursor = input;
            ast = parsearExpresion();
            afn = ast_a_AFN(ast);
            convertir_AFN_a_AFD(afn, alfabeto);

            regex_cargada = 1;
            printf(" -> Expresion cargada y automatas generados por debajo con exito.\n");
            break;

        case 2:
            if (!regex_cargada)
            {
                printf("\nChe, carga una expresion primero (Opcion 1).\n");
                break;
            }
            printf("\n--- Codigo DOT del Arbol Sintactico ---\n");
            generarDotAST(ast);
            break;

        case 3:
            if (!regex_cargada)
            {
                printf("\nChe, carga una expresion primero (Opcion 1).\n");
                break;
            }
            printf("\n--- Codigo DOT del AFN ---\n");
            generarDotAFN(afn);
            break;

        case 4:
            if (!regex_cargada)
            {
                printf("\nChe, carga una expresion primero (Opcion 1).\n");
                break;
            }
            printf("\n--- Codigo DOT del AFD (Subconjuntos) ---\n");
            imprimirTransicionesAFD(alfabeto);
            break;

        case 5:
            if (!regex_cargada)
            {
                printf("\nChe, carga una expresion primero (Opcion 1).\n");
                break;
            }
            printf("\nIngrese cadena para evaluar en el AFD: ");
            fgets(cadena, sizeof(cadena), stdin);
            cadena[strcspn(cadena, "\n")] = 0;

            if (simularAFD_iterativo(0, cadena))
            {
                printf(" -> ACEPTADA\n");
            }
            else
            {
                printf(" -> RECHAZADA\n");
            }
            break;

        case 6:
            if (!regex_cargada)
            {
                printf("\ncarga una expresion primero\n");
                break;
            }
            printf("\nIngrese linea de texto para buscar (minigrep): ");
            fgets(cadena, sizeof(cadena), stdin);
            cadena[strcspn(cadena, "\n")] = 0;

            if (minigrep(0, cadena))
            {
                printf(" -> COINCIDENCIA ENCONTRADA (Minigrep: True)\n");
            }
            else
            {
                printf(" -> NO HAY COINCIDENCIA (Minigrep: False)\n");
            }
            break;

        case 7:
            printf("\nSaliendo...\n");
            break;

        default:
            printf("\nOpcion incorrecta. Ingresa un numero del 1 al 7.\n");
        }
    } while (opcion != 7);

    return 0;
}