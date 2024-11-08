import os
from collections import deque

def getValidSolutions(labyrinth_dir, solution_dir):
    labyrinths = []
    # Listar todos los archivos en el directorio del laberinto
    for labyrinth_filename in os.listdir(labyrinth_dir):
        if labyrinth_filename.endswith(".txt"):
            solution_filename = f"solution_{labyrinth_filename}"
            
            # Verificar si existe el archivo en el directorio de soluciones
            solution_path = os.path.join(solution_dir, solution_filename)
            if os.path.exists(solution_path):
                print(f"Solución encontrada para {labyrinth_filename}: {solution_filename}")
                labyrinths.append(labyrinth_filename)
            else:
                print(f"Solución NO encontrada para {labyrinth_filename}")
    return labyrinths

def getStartAndEnd(maze):
    start = None
    end = None

    for i in range(len(maze)):
        for j in range(len(maze[i])):
            if maze[i][j] == 'A':
                start = (i,j)
            if maze[i][j] == 'B':
                end = (i,j)
    
    return [start, end]

def getMinAndInstructions(dir, file_name):
    file_path = os.path.join(dir, "solution_"+file_name)
    min = None
    explore = None
    instructions = []
    with open(file_path, 'r') as file:
        for line in file:
            if not min:
                min = line.strip()
                continue
            if not explore:
                explore = line.strip()
                continue
            instructions.append(line.strip())
            
    return [min, instructions]

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

# Verfica si el set de instrucciones proporcionado es un camino valido para llegar al objetivo
def getIsAWay(maze, start, end, instructions):
    moves = { 
        "UP": (-1, 0),
        "DOWN": (1, 0),
        "LEFT": (0, -1),
        "RIGHT": (0, 1)
    }
    node = start
    for instruction in instructions:
        node = tuple(a + b for a, b in zip(node, moves[instruction]))
        if maze[node[0]][node[1]] != '*':
            break

    return node == end

def printSummary(i, min, realMin, instructions, isAWay, size):
    fileName = f"Grupo {i}.txt"
    
    # Verificar si el archivo ya existe
    file_mode = 'a' if os.path.exists(fileName) else 'w'
    
    # Abrir el archivo en modo 'append' si ya existe, o 'write' si no existe
    with open(fileName, file_mode) as file:
        file.write('Laberinto ' + size + '\n')
        if int(min) == int(realMin) and int(min) == len(instructions):
            file.write('\tEl costo del camino minimo encontrado es el correcto' + '\n')
        else:
            file.write('\tEl costo del camino minimo encontrado NO es el correcto' + '\n')
        
        if isAWay:
            file.write('\tLa solución entregada permite llegar al objetivo' + '\n')
        else:
            file.write('\tLa solución entregada NO logro llegar al objetivo' + '\n')
        file.write('\n')


# Directorios donde se encuentran los archivos de laberintos y soluciones
labyrinth_dir = "sets"
solution_dir = "solutions"

# Validar la existencia de soluciones
valid_solutions = getValidSolutions(labyrinth_dir, solution_dir)

n = 20 #cantidad de grupos

for i in range(n):
    set_name = "set"+str(i)
    sizes = ["10x10", "20x20","1000x1000", "20000x20000"]
    for size in sizes:
        file_name = set_name + "_laberinto_" + size + ".txt"
        if file_name in valid_solutions:
            file_path = os.path.join("sets", file_name)
            with open(file_path, 'r') as file:
                print(size)
                maze = [list(line.strip()) for line in file if line.strip()]
                [start, end] = getStartAndEnd(maze)
                [min, instructions] = getMinAndInstructions(solution_dir, file_name)
                realMin = bfs(maze, start, end)
                isAWay = getIsAWay(maze, start, end, instructions)
                printSummary(i, min, realMin, instructions, isAWay, size)
                
        else:
            print(file_name, "No existe")

