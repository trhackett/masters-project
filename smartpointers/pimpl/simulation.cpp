
#include "simulation.h"
#include <cstdlib>
#include <queue>
#include <iostream>

double manhattenDistance(int row, int col, Simulation* sim) {
	return abs(row - sim->getEndRow()) +
       abs(col - sim->getEndCol());
}

double avoidTraffic(int row, int col, Simulation* sim) {
	return sim->getIntersections()[row][col]->getNumCars();


	/*
	int r = car.row;
	int c = car.col;

	  // car can always move either upDir or downDir
	bool upDirOk = r > 0;
	bool rightDirOk = c < sim.getMaxCols()-1;
	bool downDirOk = r < sim.getMaxCols()-1;
	bool leftDirOk = c > 0;

	int minDirection;

	  // if upDir isn't ok, downDir definitely is
	if (upDirOk) {
		minDirection = sim.getIntersections()[r-1][c];
	} else {
		minDirection = sim.getIntersections()[r+1][c];
	}

	if (rightDirOk) {
		int rightDirSize = sim.getIntersections()[r][c+1]
		if (minDirection > rightDirSize) {
			minDirection = rightDirSize;
		}
	}

	if (downDirOk) {
		int downDirSize = sim.getIntersections()[r+1][c]
		if (minDirection > downDirSize) {
			minDirection = downDirSize;
		}
	}

	if (leftDirOk) {
		int leftDirSize = sim.getIntersections()[r][c-1]
		if (minDirection > leftDirSize) {
			minDirection = leftDirSize;
		}
	}

	return minDirection;
	*/
}

Simulation::Simulation(int rows, int cols)
 : maxRows(rows), maxCols(cols), startRow(0), startCol(0),
   endRow(maxRows-1), endCol(maxCols-1), carFrequency(5)
{
	  // intersections is currently empty - we need to set upDir the
	  // environment by creating a bunch of intersection with their
	  // delete functions
	for (int i = 0; i < maxRows; i++) {
		intersections.push_back(
			vector<SmartPtr<Intersection, UniquePtr>>(maxCols)
		);
	}

	for (int i = 0; i < maxRows; i++) {
		for (int j = 0; j < maxCols; j++) {
			intersections[i][j].reset(new Intersection,
				[] (Intersection* i) { delete i; });
		}
	}
}




void Simulation::runSimulation(int numIterations) {

	  // each run of the for loop is an iteration
	for (int i = 0; i < numIterations; i++) {

		  // potentially insert another car
		if (i % carFrequency == 0) {
			insertNewCar();
		}

		  // each car has the chance to move
		list<SmartPtr<Car, SharedPtr>>::iterator s = allCars.begin();
		list<SmartPtr<Car, SharedPtr>>::iterator e = allCars.end();
		for (; s != e; ++s) {
			moveCarInDirection(*s, getNextMove(*s));
		}
	}
}


void Simulation::insertNewCar() {
	SmartPtr<Car, SharedPtr> temp(
		[] () { return new Car; }, 
		[] (Car* c) { delete c; });

	temp->row = startRow;
	temp->col = startCol;

	  // VARY THIS
	temp->hFunc = &manhattenDistance;

	  // allCars needs to track it for movement
	allCars.push_back(temp);

	  // it is located in an intersection
	intersections[startRow][startCol]->cars.insert(temp);
}

int Simulation::getRowInDirection(int row, Direction d) const {
	  // left or right is ok
	if (d == rightDir || d == leftDir) {
		return row;
	}

	if (d == upDir && row > 0) {
		return row-1;
	}

	if (d == downDir && row < maxRows-1) {
		return row+1;
	}

	return -1;
}
int Simulation::getColInDirection(int col, Direction d) const {
	  // up or down is ok
	if (d == upDir || d == downDir) {
		return col;
	}

	if (d == Direction::rightDir && col < maxCols-1) {
		return col+1;
	}

	if (d == Direction::leftDir && col > 0) {
		return col-1;
	}

	return -1;
}


  // going into this function, you KNOW that the direction is a valid
  // one or it wouldn't even have been considered by getNextMove()
void Simulation::moveCarInDirection(SmartPtr<Car, SharedPtr> carPtr, Direction d) {

	  // direction MUST be valid or else it wouldn't have been chosen
	int currRow = carPtr->row;
	int currCol = carPtr->col;
	int nextRow = getRowInDirection(currRow, d);
	int nextCol = getColInDirection(currCol, d);

	  // I guess I'll check anyway
	if (nextRow != -1 && nextCol != -1) {
		intersections[nextRow][nextCol]->addCar(carPtr);
		intersections[currRow][currCol]->removeCar(carPtr);
	}
}


  // this is where A* happens
Direction Simulation::getNextMove(SmartPtr<Car, SharedPtr> car) {

	  // init empty
	priority_queue<node, vector<node>, decltype(cmpNodes)> openList(cmpNodes);
	map<pair<int, int>, node> closedList;
	map<pair<int, int>, double> closedFVals;
	map<pair<int, int>, double> openFVals;

	  // starting point goes in open list, zero f
	openList.push(node(car->row, car->col, -1, -1, 0, 0));

	while (!openList.empty()) {
		  // get the node with the smallest f and pop off open list
		node parent = openList.top();
		openList.pop();

		  // try each direction
		for (int i = 0; i < 4; i++) {
			int nextRow = getRowInDirection(parent.row, (Direction)i);
			int nextCol = getColInDirection(parent.col, (Direction)i);

			  // if it's a valid move, add it in
			if (nextRow != -1 && nextCol != -1) {

				  // if you're there, stop
				if (nextRow == endRow && nextCol == endCol) {
					return backtrack(parent.row, parent.col, closedList);
				}

				  // make the new node
				double hVal = car->hFunc(nextRow, nextCol, this);
				node child(nextRow, nextCol, parent.row, parent.col, parent.g + 1, hVal);

				  // add it to the list as long as there isn't already
				  // a cheaper way to get there
				if (!betterOptionIn(child.row, child.col, child.f, openFVals) &&
					!betterOptionIn(child.row, child.col, child.f, closedFVals))
				{
					openList.push(child);
				}
			}
		}

		  // add it to the closed vals
		closedList[pair<int, int>(parent.row, parent.col)] = parent;
		closedFVals[pair<int, int>(parent.row, parent.col)] = parent.f;
	}

	return upDir;
}

Direction Simulation::evalDirection(int fromR, int fromC, int toR, int toC) {
	  // coming from row above, going down
	if (toR == fromR + 1) {
		return downDir;
	}

	if (toR == fromR - 1) {
		return upDir;
	}

	  // coming from left, going right
	if (toC == fromC + 1) {
		return rightDir;
	}

	if (toC == fromC - 1) {
		return leftDir;
	}

	return badDir;
}

Direction Simulation::backtrack(int r, int c, map<pair<int, int>, node>& closedList) {
	int parentRow = closedList[pair<int, int>(r, c)].parentRow;
	int parentCol = closedList[pair<int, int>(r, c)].parentCol;

	  // if you're back at the beginning, return the direction it should go
	if (parentRow == startRow && parentCol == startCol) {
		return evalDirection(r, c, parentRow, parentCol);
	}

	  // if not, then keep following the parent route
	return backtrack(parentRow, parentCol, closedList);
}

bool Simulation::betterOptionIn(int r, int c, double f, map<pair<int, int>, double>& fVals) {
	map<pair<int, int>, double>::iterator it = fVals.find(pair<int, int>(r, c));

	  // if there's an f value in there and it's better than the one we have, return
	  // true
	if (it != fVals.end() && it->second < f) {
		return true;
	}

	return false;
}