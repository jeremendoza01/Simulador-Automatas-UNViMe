#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**************************
Gramática (con Sigma={a,b,c}):

E  -> T E'
E' -> '|' T E' 
E' -> $               // $ representa a lambda
T  -> F T' 
T' -> '.' F T' 
T' -> $ 
F  -> P F' 
F' -> '*' F' 
F' -> $
P  -> '(' E ')' 
P  -> 'a' 
P  -> 'b'
P  -> 'c'

**************************/

int contador_nodos_ast = 0;

typedef struct Nodo {
    int id;
    char tipo;
    struct Nodo *izquierdo;
    struct Nodo *derecho;
} Nodo;

Nodo* crearHoja(char simbolo) {
    Nodo* n = (Nodo*)malloc(sizeof(Nodo));
    n->id = contador_nodos_ast++;
    n->tipo = simbolo;
    n->izquierdo = NULL;
    n->derecho = NULL;
    return n;
}

Nodo* crearNodo(char operador, Nodo* izquierdo, Nodo* derecho) {
    Nodo* n = (Nodo*)malloc(sizeof(Nodo));
    n->id = contador_nodos_ast++;
    n->tipo = operador;
    n->izquierdo = izquierdo;
    n->derecho = derecho;
    return n;
}


Nodo* E();
Nodo* E_prima(Nodo* izquierdo);
Nodo* T();
Nodo* T_prima(Nodo* izquierdo);
Nodo* F();
Nodo* F_prima(Nodo* izquierdo);
Nodo* P();

const char *cursor;
char cadena_entrada[256];

// Funcion auxiliar para imprimir el DOT y validar el arbol
void imprimirASTDot_recursivo(Nodo* raiz) {
    if (raiz == NULL) return;
    
    printf("  N%d [label=\"%c\"];\n", raiz->id, raiz->tipo);
    
    if (raiz->izquierdo) {
        printf("  N%d -> N%d;\n", raiz->id, raiz->izquierdo->id);
        imprimirASTDot_recursivo(raiz->izquierdo);
    }
    if (raiz->derecho) {
        printf("  N%d -> N%d;\n", raiz->id, raiz->derecho->id);
        imprimirASTDot_recursivo(raiz->derecho);
    }
}


// LOGICA DEL PARSER
// Regla gramatical: E -> T E'
Nodo* E(){
    if ((*cursor >= 'a' && *cursor <= 'z') || (*cursor >= '0' && *cursor <= '9') || *cursor == 'E' || *cursor == '(') {
        printf("%-16s E -> T E'\n", cursor);
        Nodo* nodo_t = T();
        if (nodo_t) {
            return E_prima(nodo_t);
        }
    }
    return NULL;
}

// Regla gramatical: E' -> '|' T E' | $
Nodo* E_prima(Nodo* izquierdo){
    if (*cursor == '|') {
        printf("%-16s E' -> | T E'\n", cursor);
        cursor++; // Consumir '|'
        Nodo* nodo_t = T();
        if (nodo_t) {
            Nodo* nuevo_izquierdo = crearNodo('|', izquierdo, nodo_t);
            return E_prima(nuevo_izquierdo); // llamada recursiva arrastrando el subárbol
        }
        return NULL;
    } else if (*cursor == ')' || *cursor == '\0') {
        printf("%-16s E' -> $\n", cursor);
        return izquierdo; //devuelve el subárbol intacto (lambda)
    }
    return NULL;
}

// Regla gramatical: T -> F T' 
Nodo* T(){
    if ((*cursor >= 'a' && *cursor <= 'z') || (*cursor >= '0' && *cursor <= '9') || *cursor == 'E' || *cursor == '(') {
        printf("%-16s T -> F T'\n", cursor);
        Nodo* nodo_f = F();
        if (nodo_f) {
            return T_prima(nodo_f);
        }
    }
    return NULL;
}

// Regla gramatical: T' -> '.' F T' | $
Nodo* T_prima(Nodo* izquierdo){
    if (*cursor == '.') {
        printf("%-16s T' -> . F T'\n", cursor);
        cursor++; // Consumir '.'
        Nodo* nodo_f = F();
        if (nodo_f) {
            Nodo* nuevo_izquierdo = crearNodo('.', izquierdo, nodo_f);
            return T_prima(nuevo_izquierdo);
        }
        return NULL;
    } else if (*cursor == '|' || *cursor == ')' || *cursor == '\0') {
        printf("%-16s T' -> $\n", cursor);
        return izquierdo;
    }
    return NULL;
}

// Regla gramatical: F -> P F' 
Nodo* F(){
    if ((*cursor >= 'a' && *cursor <= 'z') || (*cursor >= '0' && *cursor <= '9') || *cursor == 'E' || *cursor == '(') {
        printf("%-16s F -> P F'\n", cursor);
        Nodo* nodo_p = P();
        if (nodo_p) {
            return F_prima(nodo_p);
        }
    }
    return NULL;
}

// Regla gramatical: F' -> '*' F' | $
Nodo* F_prima(Nodo* izquierdo){
    if (*cursor == '*') {
        printf("%-16s F' -> * F' \n", cursor);
        cursor++; // Consumir '*'
        Nodo* nuevo_izquierdo = crearNodo('*', izquierdo, NULL);
        return F_prima(nuevo_izquierdo);
    } else if (*cursor == '.' || *cursor == '|' || *cursor == ')' || *cursor == '\0') {
        printf("%-16s F' -> $\n", cursor);
        return izquierdo;
    }
    return NULL;
}

// Regla gramatical: P -> '(' E ')' | a | b | c ...
Nodo* P(){
    if (*cursor == '(') {
        printf("%-16s P -> ( E )\n", cursor);
        cursor++; // Consumir '('
        Nodo* nodo_e = E();
        if (nodo_e && *cursor == ')') {
            cursor++; // Consumir ')'
            return nodo_e;
        }
        return NULL;
    } else if ((*cursor >= 'a' && *cursor <= 'z') || (*cursor >= '0' && *cursor <= '9') || *cursor == 'E') {
        printf("%-16s P -> %c\n", cursor, *cursor);
        Nodo* hoja = crearHoja(*cursor);
        cursor++; // Consumir el carácter
        return hoja;
    }
    return NULL;
}


// MAIN
// ==========================================
int main(){
    puts("Ingresa la expresion regular:");
    scanf("%255s", cadena_entrada); 
    cursor = cadena_entrada;
    contador_nodos_ast = 0;
    
    puts("\nEntrada          Accion");
    puts("--------------------------------");

    Nodo* arbol_sintactico = E();

    if (arbol_sintactico != NULL && *cursor == '\0'){ 
        puts("--------------------------------");
        puts("Expresion parseada con exito.\n");
        
        puts("--- Codigo DOT del Arbol Sintactico ---");
        printf("digraph G {\n");
        imprimirASTDot_recursivo(arbol_sintactico);
        printf("}\n");
        
        return 0;
    } else {
        puts("--------------------------------");
        puts("Error parseando la expresion regular.");
        return 1;
    }
}