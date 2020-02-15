//
// Created by vin on 2/14/20.
//

#include "Pathfinder.h"

#include <ctgmath>
#include <fstream>
#include <list>
#include <sstream>

Pathfinder::Pathfinder() {
  srand(time(nullptr));

  resetMaze();
}

string Pathfinder::toString() const {
  ostringstream mazeStream;

  for (int x = 0; x < CUBE_SIDE_LENGTH; x++) {
    for (int y = 0; y < CUBE_SIDE_LENGTH; y++) {
      for (int z = 0; z < CUBE_SIDE_LENGTH; z++) {
        int cell = maze[x][y][z];
        mazeStream << cell;

        if (z < CUBE_SIDE_LENGTH - 1) {
          mazeStream << " ";
        }
      }
      mazeStream << endl;
    }
    if (x < CUBE_SIDE_LENGTH - 1) {
      mazeStream << endl;
    }
  }

  return mazeStream.str();
}

void Pathfinder::createRandomMaze() {
  for (auto& slice : maze) {
    for (auto& row : slice) {
      for (auto& cell : row) {
        cell = rand() % 2;
      }
    }
  }

  // Make valid maze
  maze[0][0][0] = 1;
  maze[CUBE_SIDE_LENGTH - 1][CUBE_SIDE_LENGTH - 1][CUBE_SIDE_LENGTH - 1] = 1;
}

bool Pathfinder::importMaze(string file_name) {
  ifstream fileIn(file_name);

  if (!fileIn.is_open()) {
    // File doesn't exist
    return false;
  }

  list<int> cellsFlat;

  string token;
  while (fileIn >> token) {
    if (token.size() > 1 || !isdigit(token[0])) {
      // Not valid, return
      return false;
    }

    cellsFlat.push_back(stoi(token));
  }

  // We could make 3 a constant, but in order to change it, we'd also have to
  // change every nested for loop and array declaration.
  // No `numpy.ndarray` here!
  int cubeVolume = pow(CUBE_SIDE_LENGTH, 3);

  if (cellsFlat.size() != cubeVolume || cellsFlat.front() != 1 ||
      cellsFlat.back() != 1) {
    // Too many cells or top left and/or bottom right cells aren't passable
    return false;
  }

  for (auto& slice : maze) {
    for (auto& row : slice) {
      for (auto& cell : row) {
        cell = cellsFlat.front();
        cellsFlat.pop_front();
      }
    }
  }
}

vector<string> Pathfinder::solveMaze() {
  int originalMaze[CUBE_SIDE_LENGTH][CUBE_SIDE_LENGTH][CUBE_SIDE_LENGTH];

  path.clear();
  for (int i = 0; i < CUBE_SIDE_LENGTH; i++) {
    for (int j = 0; j < CUBE_SIDE_LENGTH; j++) {
      for (int k = 0; k < CUBE_SIDE_LENGTH; k++) {
        originalMaze[i][j][k] = maze[i][j][k];
      }
    }
  }

  findPath(0, 0, 0);


  for (int i = 0; i < CUBE_SIDE_LENGTH; i++) {
    for (int j = 0; j < CUBE_SIDE_LENGTH; j++) {
      for (int k = 0; k < CUBE_SIDE_LENGTH; k++) {
        maze[i][j][k] = originalMaze[i][j][k];
      }
    }
  }

  return path;
}

string Pathfinder::getCellString(int x, int y, int z) {
  ostringstream cellStream;
  cellStream << "(" << z << ", " << y << ", " << x << ")";
  return cellStream.str();
}

bool Pathfinder::isCoordInsideMaze(int x, int y, int z) {
  bool xInsideMaze = x >= 0 && x < CUBE_SIDE_LENGTH;
  bool yInsideMaze = y >= 0 && y < CUBE_SIDE_LENGTH;
  bool zInsideMaze = z >= 0 && z < CUBE_SIDE_LENGTH;

  return xInsideMaze && yInsideMaze && zInsideMaze;
}

bool Pathfinder::findPath(int x, int y, int z) {
  string cellString = getCellString(x, y, z);

  path.push_back(cellString);

  if (!isCoordInsideMaze(x, y, z) || maze[x][y][z] == 0 || maze[x][y][z] == 2) {
    path.pop_back();
    return false;
  }

  // We're the first to make it here
  maze[x][y][z] = 2;

  if (x == CUBE_SIDE_LENGTH - 1 && y == CUBE_SIDE_LENGTH - 1 &&
      z == CUBE_SIDE_LENGTH - 1) {
    // reached end of maze
    return true;
  }


  bool up = findPath(x, y + 1, z);
  bool down = findPath(x, y - 1, z);
  bool left = findPath(x + 1, y, z);
  bool right = findPath(x - 1, y, z);
  bool in = findPath(x, y, z + 1);
  bool out = findPath(x, y, z - 1);

  if (up || down || left || right || in || out) {
    return true;
  } else {
    path.pop_back();
    return false;
  }
}

void Pathfinder::resetMaze() {
  for (auto& slice : maze) {
    for (auto& row : slice) {
      for (auto& cell : row) {
        cell = DEFAULT_CELL_VALUE;
      }
    }
  }
}
