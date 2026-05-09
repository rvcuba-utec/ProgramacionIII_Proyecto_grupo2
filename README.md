# Proyecto Final – Plataforma de Streaming (Programación III)

## Integrantes
* Renzo Vladimir Cuba Zari
* Elías Emanuel Paz Raymondi
* Bricet Daniela Silvana Rodrigo Inocente

---

## 1. Descripción del proyecto (Avance – Semana 8)
En esta primera fase se implementa la carga y limpieza del catálogo de películas y la **búsqueda por título**.  

---

## 2. Pre‑procesamiento de los datos

### 2.1 Archivo de entrada
Se utiliza el dataset `wiki_movie_plots_deduped.csv` con la siguiente estructura:
```
Release Year | Title | Origin/Ethnicity | Director | Cast | Genre | Wiki Page | Plot
```

### 2.2 Limpieza y normalización de los títulos
Cada título pasa por un proceso de limpieza para ser indexado:

1. **Eliminación de acentos y caracteres especiales UTF‑8**  
   Se mapean secuencias multibyte (`á`, `ñ`, `ü`, etc.) a sus equivalentes ASCII, y los símbolos no reconocidos se convierten en espacios.

2. **Conversión a minúsculas y filtrado alfanumérico**  
   Solo se conservan letras `a‑z` y dígitos `0‑9`; cualquier otro carácter se reemplaza por un espacio.

3. **Colapso de espacios múltiples y recorte de extremos**  
   Se reducen las secuencias de varios espacios a uno solo y se eliminan los espacios inicial y final.

4. **Resultado**  
   Cada película almacena su título original (para mostrar en la interfaz) y su título normalizado (para construir el árbol de búsqueda).  
   **No se eliminan stop-words** (artículos, preposiciones) para no impedir búsquedas exactas como *“El Padrino”*.

### 2.3 Lectura del CSV
El archivo contiene sinopsis que ocupan varios renglones y campos entrecomillados. Se implementa una función que acumula líneas mientras el número de comillas dobles sea impar, asegurando que cada registro se parsea completo.  
Se descartan únicamente las películas cuyo título queda vacío después de la normalización.

---

## 3. Estructura de datos elegida: Suffix Trie

### 3.1 Requisitos que debe cumplir
- Los nodos del árbol deben almacenar caracteres (letras y números).
- Debe permitir buscar **cualquier subcadena** dentro de los títulos.
- Debe escalar a ≈35 000 películas con un consumo de memoria moderado.

### 3.2 Justificación de la elección
Un **Suffix Trie** (trie de sufijos) es un árbol donde cada cadena se inserta junto con todos sus sufijos.  

De este modo, **cualquier subcadena buscada (por ejemplo `"tan"` de tal vez `titanic`) es un prefijo de alguno de los sufijos almacenados**, y se encuentra recorriendo el trie carácter a carácter en tiempo O(m), donde m es la longitud de la consulta.

Además, el Suffix Trie permite añadir más adelante la búsqueda sobre sinopsis sin cambiar la estructura (agregando un **índice invertido**), mientras que con dos árboles separados se duplicaría la lógica de búsqueda y la complejidad del código.

Por tanto, el Suffix Trie es la opción más equilibrada para este proyecto: cumple los requisitos con un código limpio, mantenible y ampliable.

### 3.3 Diseño interno
**Nodo del árbol:**
```cpp
struct Node {
    unordered_map<char, Node*> children;   // conexiones a otros caracteres
    unordered_set<int> movieIds;           // IDs de películas cuyo título contiene la secuencia hasta aquí
};
```
- La raíz representa la cadena vacía.
- Cada nodo almacena los identificadores de todas las películas cuyo título contiene la subcadena formada por el camino desde la raíz hasta ese nodo.
- Al buscar una subcadena, si se llega al nodo correspondiente, los IDs de las películas coincidentes se obtienen directamente del conjunto, sin necesidad de recorrer el subárbol.

---

## 4. Algoritmos de inserción y búsqueda

### 4.1 Inserción de títulos en el Suffix Trie

**Pseudocódigo:**
```
procesar_pelicula(movieId, titulo_normalizado):
    insertar_titulo(titulo_normalizado, movieId)

insertar_titulo(titulo, movieId):
    para i desde 0 hasta longitud(titulo)-1:
        sufijo = titulo.substr(i)
        nodo = raiz
        nodo.movieIds.insertar(movieId)
        para cada caracter c en sufijo:
            si nodo.hijos no contiene c:
                nodo.hijos[c] = nuevo Nodo()
            nodo = nodo.hijos[c]
            nodo.movieIds.insertar(movieId)
```

**Complejidad:** O(n²) por título, donde n es la longitud del título.

### 4.2 Búsqueda de subcadena o frase

Para una consulta se normaliza de la misma forma que los títulos y se tokeniza por espacios:
- Si la consulta es una sola palabra o subcadena, se busca esa cadena directamente.
- Si contiene varias palabras, se realiza la unión (OR) de los resultados de cada token.

**Pseudocódigo:**
```
buscar(consulta_normalizada):
    tokens = tokenizar(consulta_normalizada)
    resultado = conjunto vacío
    para cada token en tokens:
        nodo = raiz
        para cada caracter c en token:
            si nodo.hijos contiene c:
                nodo = nodo.hijos[c]
            sino:
                salir del bucle (token no encontrado)
        si se recorrió todo el token:
            resultado.unir(nodo.movieIds)
    devolver resultado
```

**Complejidad:** O(n) por cada token, donde n es el número de tokens, independiente del número de películas. La unión final es proporcional al número de resultados.

**Comportamiento según el requisito:**  
- *“bar”* → encuentra todos los títulos que contienen esa subcadena (ej. *“Barco”*, *“Barbie”*, *“Zombies bar”*).  
- *“barco fantasma”* → encuentra todos los títulos que contienen *“barco”* **o** *“fantasma”* (u ambos).

---

## 5. Ejecución del programa
El código fuente en C++ se encuentra en este repositorio.  
**Compilación:**  
```bash
g++ -std=c++17 -O2 main.cpp -o programa
```
**Ejecución:**  
```bash
./programa
```
