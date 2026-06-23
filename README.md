# Simulador de Autómatas Finitos y Expresiones Regulares en C

Este proyecto implementa un simulador completo de autómatas finitos desarrollado como Trabajo Práctico Obligatorio para la asignatura Teoría de la Computación (2026) en la Universidad Nacional de Villa Mercedes (UNViMe). 

El programa procesa expresiones regulares, construye su representación en memoria a través de distintos algoritmos formales y permite evaluar cadenas de texto para determinar su aceptación o rechazo.

## 🚀 Características Principales

El simulador implementa un flujo completo de transformación y evaluación:

1. Parser Descendente Recursivo LL(1) a AST: Modificación de un parser LL(1) para convertir una expresión regular plana en un Árbol Sintáctico Abstracto (AST).
2. Algoritmo de Thompson (ER a AFN): Construcción de un Autómata Finito No Determinista (AFN) con transiciones épsilon (ε) a partir del AST generado.
3. Algoritmo de Subconjuntos (AFN a AFD): Conversión del AFN en un Autómata Finito Determinista (AFD) eliminando la ambigüedad y calculando las clausuras épsilon y la operación mover.
4. Simulador Iterativo AFD: Evaluación eficiente de cadenas carácter por carácter utilizando una matriz de transiciones determinista.
5. Función Minigrep: Búsqueda de subcadenas dentro de una línea de texto mayor que coincidan con el patrón de la expresión regular ingresada.
6. Exportación a Graphviz (DOT): Generación de código en formato `.dot` para visualizar gráficamente el AST y los Autómatas (AFN y AFD).

## ⚙️ Operadores y Alfabeto Soportados

El simulador admite el alfabeto alfanumérico (`a-z`, `0-9`) y el símbolo `E` para representar la cadena vacía (Lambda/Épsilon).

Los operadores matemáticos soportados son:
* **Unión (`|`)**
* **Concatenación explícita e implícita (`.` y adyacencia)**
* **Clausura de Kleene (`*`)**
* **Cerradura Positiva (`+`)**
* **Agrupamiento (`()`)**

## 🛠️ Cómo compilar y ejecutar

El proyecto está escrito en C estándar y no requiere dependencias externas más allá de los compiladores habituales (GCC/Clang).

**1. Compilación:**
Desde la terminal, ubicado en la raíz del repositorio, ejecutá:
```bash
gcc main.c -o simulador_automatas
