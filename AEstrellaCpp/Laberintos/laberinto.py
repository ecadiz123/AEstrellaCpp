import random
from collections import deque

# Función para generar el laberinto con un camino garantizado
def generar_laberinto(filas, columnas):
    # Crear un laberinto lleno de obstáculos ('#')
    laberinto = [['#' for _ in range(columnas)] for _ in range(filas)]

    # Colocar el nodo inicial 'A' y el objetivo 'B'
    inicio = (random.randint(1, filas - 2), random.randint(1, columnas - 2))
    objetivo = (random.randint(1, filas - 2), random.randint(1, columnas - 2))
    while inicio == objetivo:
        objetivo = (random.randint(1, filas - 2), random.randint(1, columnas - 2))

    laberinto[inicio[0]][inicio[1]] = 'A'
    laberinto[objetivo[0]][objetivo[1]] = 'B'

    # Pila para realizar el backtracking y crear el laberinto
    pila = [(inicio[0], inicio[1])]

    while pila:
        fila, col = pila.pop()
        vecinos = [(fila - 2, col), (fila + 2, col), (fila, col - 2), (fila, col + 2)]
        random.shuffle(vecinos)

        for n_fila, n_col in vecinos:
            if 0 <= n_fila < filas and 0 <= n_col < columnas and laberinto[n_fila][n_col] == '#':
                laberinto[(fila + n_fila) // 2][(col + n_col) // 2] = '*'
                laberinto[n_fila][n_col] = '*'
                pila.append((n_fila, n_col))

    return laberinto, inicio, objetivo



# Función BFS para encontrar el camino mínimo
def bfs(laberinto, inicio, objetivo):
    filas, columnas = len(laberinto), len(laberinto[0])
    movimientos = [(-1, 0), (1, 0), (0, -1), (0, 1)]  # Arriba, abajo, izquierda, derecha
    
    visitado = [[False for _ in range(columnas)] for _ in range(filas)]
    cola = deque([(inicio[0], inicio[1], 0)])  # (fila, columna, distancia)
    visitado[inicio[0]][inicio[1]] = True
    
    while cola:
        fila, columna, distancia = cola.popleft()
        
        # Si llegamos al objetivo, retornamos la distancia
        if (fila, columna) == objetivo:
            return distancia
        
        # Explorar los vecinos
        for movimiento in movimientos:
            nueva_fila, nueva_columna = fila + movimiento[0], columna + movimiento[1]
            
            if 0 <= nueva_fila < filas and 0 <= nueva_columna < columnas:
                if not visitado[nueva_fila][nueva_columna] and laberinto[nueva_fila][nueva_columna] in ('*', 'B'):
                    visitado[nueva_fila][nueva_columna] = True
                    cola.append((nueva_fila, nueva_columna, distancia + 1))
    
    # Si no hay camino
    return -1

# Función para guardar el laberinto en un archivo
def guardar_laberinto(laberinto, camino_minimo, filas, columnas):
    archivo_salida = f"{camino_minimo}_laberinto_{filas}x{columnas}.txt"
    
    with open(archivo_salida, 'w') as archivo:
        for fila in laberinto:
            archivo.write(''.join(fila) + '\n')
    
    print(f"Laberinto guardado en: {archivo_salida}")

# Valida que exista un nodo inicio y final
def validar_laberinto(laberinto, inicio, objetivo):
    if laberinto[inicio[0]][inicio[1]] == 'A' and laberinto[objetivo[0]][objetivo[1]] == 'B':
        return True
    return False

# Función principal para generar y guardar el laberinto con el nombre basado en el camino mínimo
def generar_y_guardar_laberinto(filas, columnas):
    laberinto, inicio, objetivo = generar_laberinto(filas, columnas)

    camino_minimo = bfs(laberinto, inicio, objetivo)
    
    if camino_minimo == -1 or not validar_laberinto(laberinto, inicio, objetivo):
        print("No se encontró un camino válido.")
        generar_y_guardar_laberinto(filas, columnas)
    else:
        guardar_laberinto(laberinto, camino_minimo, filas, columnas)

# Ejemplo de uso:
generar_y_guardar_laberinto(15000,15000)
