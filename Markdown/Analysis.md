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

## Caso I: Dos nodos envían paquetes

> El nodo 0 y nodo 2 envían paquetes al nodo 5

## Caso II: Todos envían paquetes

> Todos los paquetes menos el 5 envían paquetes al nodo 5

## Caso III: Random

> Cada paquete puede enviar paquetes a cualquier otro siguiendo una distribución uniforme

## Conclusión

# Algoritmo 1: Short Flood

## Suposiciones

- Topología de anillo.
- La topología del anillo no se modifica.
- Los nodos pueden estar en cualquier orden.
- **Se sabe la cantidad de nodos en el anillo.**

## Pasos del algoritmo

1. Cuando un `nodo x` *(nodo de origen)* necesita mandar un paquete a un destino `nodo y`, se inicializa el contador de saltos (`hoops`) en 0 y envía el paquete a sus 2 vecinos, generando así un paquete a la izquierda y otro a la derecha.

2. Por cada nodo que se recibe el paquete, si no es el destino, el hoops aumenta en uno y se compara el valor de este:
    - Si `hoops < 4` => se pasa al siguiente nodo.
    - Si `hoops >= 4` => se descarta ese paquete.
    > `Cantidad máxima de hoops = cantidad de nodos / 2 = 4`

Este proceso ocurre sucesivamente hasta encontrar el nodo de destino `y`.

3. Cuando el paquete llega al nodo de destino `y`.


La *cantidad de máxima de hoops* permite que la inundación utilice los mínimos recursos posibles mientras también se garantiza que un paquete llegue a cualquier destino del anillo.

# Análisis
## Caso I: Dos nodos envían paquetes

> El nodo 0 y nodo 2 envían paquetes al nodo 5

## Caso II: Todos envían paquetes

> Todos los paquetes menos el 5 envían paquetes al nodo 5

## Caso III: Random

> Cada paquete puede enviar paquetes a cualquier otro siguiendo una distribución uniforme

## Conclusión

- Mayor utilización del ancho de banda.
- Mayor velocidad en el envío de paquetes.
- Problemas cuando la cantidad de nodos es par porque el nodo de destino recibe dos paquetes en vez de uno.
  - No reconoce acks duplicados.

# Algoritmo 2: Chained Hello

## Suposiciones

- Topología de anillo.
- La topología del anillo no se modifica.
- Los nodos pueden estar fuera de orden.
- El anillo podría tener cantidad arbitraria de nodos.
- **Se sabe la cantidad de nodos en el anillo.**

## Pasos del algoritmo

1. Existe un `nodo x` que es el encargado de iniciar la cadena de HELLO, (en nuestro caso lo definimos como el nodo 0). Este `nodo x` envía quién es (su index) y se lo pasa al vecino que tiene a su derecha con un *mensaje hello de listas enlazadas*.

> `hello = [nodo[x]]`

2. Luego el vecino a la derecha del `nodo x`, el `nodo a`, anota quién es y se lo pasa a su vez a *su* vecino de la derecha, `nodo b`.
    - Este proceso ocurre sucesivamente.

> `hello = [nodo[x], nodo[a], nodo[b], ...]`

1. Cuando el `nodo x` *(nodo origen de los mensajes hello)* recibe el mensaje, vuelve a anotarse, se da cuenta que él es el inicio y fin de la lista, lo cual indica que la lista está finalizada (se cerró la red de anillo).

> `hello = [nodo[x], nodo[a], nodo[b], ..., nodo[x]]`

4. El `nodo x` debe inundar la red con la lista finalizada para informar la topología de la misma. Inunda la red enviando un paquete con dicha información en ambas direcciones (ambos vecinos del `nodo x` reciben el paquete).

5. Cada nodo recibe la información de la topología de la red y calcula la mejor ruta para cada nodo. Si ya tiene la topología guardada, ignora el paquete y no lo retransmite (esto corta la inundación cuando ya todos tengan la información).

## Caso I: Dos nodos envían paquetes

> El nodo 0 y nodo 2 envían paquetes al nodo 5

## Caso II: Todos envían paquetes

> Todos los paquetes menos el 5 envían paquetes al nodo 5

## Caso III: Random

> Cada paquete puede enviar paquetes a cualquier otro siguiendo una distribución uniforme

## Conclusión

- Una vez conocida la topología de la red siempre se envía por la mejor ruta.
- No se presenta el problema si la cantidad de nodos es impar o par a diferencia del algoritmo "Short Flood".

## Mejoras posibles

- Poder responder a cambios en la topología de la red: cuando se cae un nodo.

---

# Punto Estrella

Si se deseara implementar el punto estrella, el algoritmo de enrutamiento debería funcionar en cualquier topología de red. En nuestro caso ninguno de nuestros algoritmos de enrutamiento funcionaría porque dependenmos fuertemente de la topología inicial (anillo) y también de que esta topología no se altere durante la vida de la red.

# Referencias

- [1] Tanenbaum A. S. (2012). Redes de computadores (5ta ed.) Pearson.
- [2] https://es.wikipedia.org/wiki/Red_en_anillo

---

**[ENUNCIADO](../Markdown/Assignment.md) | [README](../README.md)**

---