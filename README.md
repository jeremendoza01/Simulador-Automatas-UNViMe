<div align="center">
  <h1>⚙️ Simulador de Autómatas Finitos y Expresiones Regulares</h1>
  <p>
    <strong>Un motor de evaluación de expresiones regulares construido desde cero en C, implementando parsing sintáctico y algoritmos formales de teoría de la computación.</strong>
  </p>

  <img src="https://img.shields.io/badge/Language-C-blue.svg" alt="C" />
  <img src="https://img.shields.io/badge/Algorithm-Thompson-orange.svg" alt="Thompson Algorithm" />
  <img src="https://img.shields.io/badge/Algorithm-Subset_Construction-success.svg" alt="Subset Construction" />
  <img src="https://img.shields.io/badge/Output-Graphviz_DOT-yellow.svg" alt="Graphviz DOT" />
  <img src="https://img.shields.io/badge/Status-Completed-brightgreen.svg" alt="Status" />
</div>

<br />

## 📖 Sobre el Proyecto

Este proyecto fue desarrollado como Trabajo Práctico Obligatorio para la cátedra de **Teoría de la Computación (2026)** en la **Universidad Nacional de Villa Mercedes (UNViMe)**. 

El objetivo principal es demostrar el entendimiento profundo de la teoría de lenguajes formales mediante la construcción de un motor capaz de interpretar una expresión regular plana, estructurarla en memoria y compilarla en máquinas de estados abstractas (Autómatas) para la validación de cadenas.

## ✨ Arquitectura y Módulos

El sistema está diseñado bajo un modelo de procesamiento en cascada (Pipeline), compuesto por 4 capas fundamentales:

1. **Parser Descendente Recursivo LL(1):** Procesa la expresión regular de entrada (Infix) y construye un Árbol Sintáctico Abstracto (AST) respetando la jerarquía matemática de los operadores.
2. **Generador AFN (Algoritmo de Thompson):** Recorre el AST y construye un Autómata Finito No Determinista con transiciones épsilon (ε-NFA).
3. **Generador AFD (Construcción de Subconjuntos):** Elimina el no determinismo y las transiciones vacías, calculando la "Clausura Épsilon" y la operación "Mover" para generar un Autómata Finito Determinista (DFA) optimizado.
4. **Motor de Simulación Iterativa:** Evalúa cadenas de texto contra la matriz de transiciones del AFD en tiempo lineal $O(n)$. Incluye funcionalidad **Minigrep** para búsqueda de patrones de subcadenas.

## 🛠️ Especificaciones Técnicas

### Operadores Soportados
| Operador | Descripción | Símbolo |
| :---: | :--- | :---: |
| **Unión** | Alternativa entre dos expresiones | `|` |
| **Concatenación** | Secuencia (Explícita e implícita) | `.` |
| **Clausura de Kleene** | Cero o más repeticiones | `*` |
| **Cerradura Positiva** | Una o más repeticiones | `+` |
| **Agrupamiento** | Precedencia de operaciones | `()` |

* **Alfabeto soportado:** `a-z`, `0-9`.
* **Cadena Vacía (Lambda/Épsilon):** Representada por el carácter `E`.

## 🚀 Instalación y Despliegue

El proyecto está escrito en **C estándar** y no posee dependencias externas complejas, asegurando una alta portabilidad.

**1. Clonar el repositorio:**
```bash
git clone [https://github.com/jeremendoza01/Simulador-Automatas-UNViMe.git](https://github.com/jeremendoza01/Simulador-Automatas-UNViMe.git)
cd Simulador-Automatas-UNViMe

📊 Integración con Graphviz
Las opciones 2, 3 y 4 del menú generan el código nativo en formato .dot. Este código puede ser copiado directamente en herramientas como WebGraphviz o Edotor para visualizar los grafos generados por los algoritmos.

