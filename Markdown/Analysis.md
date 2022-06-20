# Informe

> Informe de la tarea de análisis del laboratorio 4 de Redes y Sistemas Distribuidos - Primer cuatrimestre, FAMaF 2022.

# Integrantes
- Carrizo, Ernesto.
- Domínguez, Agustín.
- Vispo, Valentina.

---

**[ENUNCIADO](../Markdown/Assignment.md) | [README](../README.md)**

---

## Contenido

<!-- Tabla de contenido -->

# Abstract

En este laboratorio se estudia el comportamiento de una red representada en un modelo de anillo de 8 nodos, unidos con dos posibles vecinos por el modelo full-dúplex. Cada nodo está formado por dos capas de enlace, una de red y una de aplicación.

La capa de enlace y la capa de aplicación tienen generadores de tráfico y bufferes respectivamente.

Un escenario se envía desde 2 nodos con igual distancia al nodo de destino y en otro todos los nodos, exceptuando el destinatario, envían paquetes, resultando así en diferentes distancias al nodo de destino.

Posteriormente se realizó dos diseños e implementaciones de algoritmos de enrutamiento basados en esta red de anillo y se estudió la efectividad en dichos escenarios.

# Modelo de anillo

Una red de anillo es una topología de red en la que cada nodo se conecta exactamente con dos nodos, formando así una única ruta continua.

# Algoritmo original

## Caso I

## Caso II

## Conclusión

# Algoritmo 1:

## Suposiciones

- Topología de anillo.
- La topología del anillo no se modifica.
- Los nodos pueden estar fuera de orden.
- `Cantidad máxima de hoops = cantidad de nodos / 2` (la red no debería saber esto pero es a modo de analizar los 3 algoritmos)

## Pasos del algoritmo

1. En el `nodo x` se inicializa el contador de saltos (`hoops`) en 0 y envía el paquete a sus 2 vecinos, generando así un paquete a la izquierda y otro a la derecha.

2. Por cada nodo que se recibe el paquete, el hoops aunmenta en uno y se compara el valor de este:
    - Si `hoops < 4` => se pasa al siguiente nodo.
    - Si `hoops >= 4` => se descarta ese paquete.
    > 4 es la cantidad máxima de hoops.

Este proceso ocurre sucesivamente hasta encontrar el nodo de destino.

3. Cuando el paquete llega al nodo de destino, descarta el siguiente paquete recibido de ese origen.

## Caso I

## Caso II

## Conclusión

- Mayor utilización del ancho de banda.
- Mayor velocidad en el envío de paquetes.
- No reconoce acks duplicados.

# Algoritmo 2: Chained Hello 

## Suposiciones

- Topología de anillo.
- La topología del anillo no se modifica.
- Los nodos pueden estar fuera de orden.
- El anillo podría tener cantidad arbitraria de nodos.

## Pasos del algoritmo

1. El `nodo x` envía quién es (su index) y se lo pasa al vecino que tiene a su derecha con un mensaje hello de listas enlazadas.

> `hello = [nodo[x]]`

2. Luego el vecino del `nodo x`, `nodo a`, anota quién es y se lo pasa a su vecino de la derecha, `nodo b`.
    - Este proceso ocurre sucesivamente.

> `hello = [nodo[x], nodo[a], nodo[b], ...]`

3. Cuando el `nodo x` *(nodo origen de los mensajes hello)* recibe el mensaje, vuelve a anotarse, se da cuenta que él es el inicio y fin de la lista. Entonces, esto indica que la lista está finalizada (se cerró la red de anillo).

> `hello = [nodo[x], nodo[a], nodo[b], ..., nodo[x]]`

4. El `nodo x` debe inundar la red con la lista finalizada para informar la topología de la red.

5. Cada nodo recibe la información de la topología de la red.

## Caso I

## Caso II

# Referencias

- [1] Tanenbaum A. S. (2012). Redes de computadores (5ta ed.) Pearson.
- [2] https://es.wikipedia.org/wiki/Red_en_anillo

---

**[ENUNCIADO](../Markdown/Assignment.md) | [README](../README.md)**

---