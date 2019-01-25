
#include "simulation.h"
#include <cstdlib>
#include <queue>
#include <iostream>

/* ============================================================================
	Potential heuristic functions
   ============================================================================ */ 
double manhattenDistance(int row, int col, Simulation* sim) {
	return abs(row - sim->getEndRow()) +
       abs(col - sim->getEndCol());
}

double avoidTraffic(int row, int col, Simulation* sim) {
	return sim->getIntersectionAt(row, col)->getNumCars();
}


void Car::init(int row, int col, double (*hFunc) (int, int, Simulation*))
{
	m_row = row;
	m_col = col;
	m_heuristicFunc = hFunc;
	m_timer.start();
}

double Car::getHValue(int row, int col, Simulation* sim) {
	return m_heuristicFunc(row, col, sim);
}


  // cars leave intersections
void Intersection::removeCar(SmartPtr<Car, SharedPtr> carPtr) {
	set<SmartPtr<Car, SharedPtr>>::iterator it = cars.find(carPtr);

	  // if it's in there, remove it
	if (it != cars.end()) {
		cars.erase(it);
	}
}

  // and cars come into intersections
void Intersection::addCar(SmartPtr<Car, SharedPtr> carPtr) {
	cars.insert(carPtr);
}


Simulation::Simulation(int rows, int cols)
 : maxRows(rows), maxCols(cols), startRow(0), startCol(0),
   endRow(maxRows-1), endCol(maxCols-1), carFrequency(5)
{
	  // intersections is currently empty - we need to set Up the
	  // environment by creating a bunch of intersection with their
	  // delete functions
	for (int i = 0; i < maxRows; i++) {
		iGrid.push_back(
			vector<SmartPtr<Intersection, UniquePtr>>(maxCols)
		);
	}

	for (int i = 0; i < maxRows; i++) {
		for (int j = 0; j < maxCols; j++) {
			iGrid[i][j].reset(
				new Intersection,
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
			RouteComputer rc;
			if (moveCarInDirection(*s, rc.getNextMove(*s, *this))) {
				s = allCars.erase(s);
			}
		}
	}
}


void Simulation::insertNewCar() {
	SmartPtr<Car, SharedPtr> temp(
		[] () { return new Car; }, 
		[] (Car* c) { delete c; });

	  // to test out different heuristics, pass different
	  // functions into here
	temp->init(startRow, startCol, &manhattenDistance);

	  // allCars needs to track it for movement
	allCars.push_back(temp);

	  // it is located in an intersection
	iGrid[startRow][startCol]->addCar(temp);
}


  // going into this function, you KNOW that the direction is a valid
  // one or it wouldn't even have been considered by getNextMove()

  // this returns true if the car has left the simulation and needs to be
  // removed from allCars
bool Simulation::moveCarInDirection(SmartPtr<Car, SharedPtr> carPtr, Direction d)
{
	int currRow = carPtr->row();
	int currCol = carPtr->col();
	int nextRow = currRow;
	int nextCol = currCol;

	  // if you can move in that direction, then remove the car from the intersection
	  // you're at currently and insert yourself at the one you're going to
	if (getRowColInDirection(nextRow, nextCol, d)) {
		  // if the car is done, remove it
		if (nextRow == endRow && nextCol == endCol) {
			iGrid[currRow][currCol]->removeCar(carPtr);
			return true;
		}

		iGrid[nextRow][nextCol]->addCar(carPtr);
		iGrid[currRow][currCol]->removeCar(carPtr);

		carPtr->setRow(nextRow);
		carPtr->setCol(nextCol);
	}

	return false;
}


/*
	This function takes in a row and column by reference that corresponds
	to the actual position of a car as well as a direction that car would
	like to see if it can move in. It checks to see if that move will take
	the car out of bounds. If so, it leaves row and col unchanged and returns
	false. If not, it adjusts row or col to reflect that move and returns
	true.
*/
bool Simulation::getRowColInDirection(int& row, int& col, Direction d) const
{
	  // assume the move is invalid and then check to see if it results
	  // in going out of bounds. If it doesn't, then adjust the row/col
	  // and return true
	bool validMove = false;
	switch (d) {
		case Up:
			if (row > 0) {
				row--;
				validMove = true;
			}
			break;

		case Right:
			if (col < maxCols - 1) {
				col++;
				validMove = true;
			}
			break;

		case Down:
			if (row < maxRows - 1) {
				row++;
				validMove = true;
			}
			break;

		case Left:
			if (col > 0) {
				col--;
				validMove = true;
			}
			break;

		case Bad:
			break;
	}

	return validMove;
}


  // this is where A* happens
Direction RouteComputer::getNextMove(SmartPtr<Car, SharedPtr> car, Simulation& sim) {

	  // init empty
	priority_queue<node, vector<node>, decltype(cmpNodes)> openList(cmpNodes);

	  // initialize closedList
	vector<vector<node>> closedList(sim.getMaxRows());
	for (int i = 0; i < sim.getMaxRows(); i++) {
		closedList[i] = vector<node>(sim.getMaxCols());
	}

	map<pair<int, int>, double> closedFVals;
	map<pair<int, int>, double> openFVals;

	  // starting point goes in open list, zero f
	openList.push(node(car->row(), car->col(), -1, -1, 0, 0));

	while (!openList.empty()) {

		  // get the node with the smallest f and pop off open list
		node parent = openList.top();
		openList.pop();

		  // try each direction
		for (int i = 0; i < 4; i++) {
			int nextRow = parent.row;
			int nextCol = parent.col;
			Direction d = (Direction) i;

			  // try up
			if (sim.getRowColInDirection(nextRow, nextCol, d)) {

				  // if you're at the end, be done!
				if (parent.row == sim.getEndRow() && parent.col == sim.getEndCol()) {
					  // add to the closed list
					closedList[parent.row][parent.col] = parent;

					  // and backtrack until you're where the car is, return
					  // the direction it should move in
					return backtrack(parent.row, parent.row,
								     car->row(), car->col(), closedList);
				}

				  // if it's in the closed list don't check it again
				if (closedList[nextRow][nextCol].f == -1)
				{
					  // add it to the open list as long as there isn't an f value
					  // corresponding to that point on the grid that is smaller than
					  // this one
					double hVal = car->getHValue(nextRow, nextCol, &sim);
					node child(nextRow, nextCol, parent.row, parent.col, parent.g + 1, hVal);

					if (!betterOptionIn(child.row, child.col, child.f, openFVals) &&
						!betterOptionIn(child.row, child.col, child.f, closedFVals))
					{
						openList.push(child);
						openFVals[pair<int, int>(child.row, child.col)] = child.f;
					}
				}
			}
		}

		  // add it to the closed vals
		closedList[parent.row][parent.col] = parent;
		closedFVals[pair<int, int>(parent.row, parent.col)] = parent.f;
	}

	return Bad;
}


Direction RouteComputer::backtrack(int r, int c, int startRow, int startCol,
									 vector<vector<node>>& closedList)
{
	int parentRow = closedList[r][c].parentRow;
	int parentCol = closedList[r][c].parentCol;

	  // if you're back at the beginning, return the direction it should go
	if (parentRow == startRow && parentCol == startCol) {
		return evalDirection(parentRow, parentCol, r, c);
	}

	  // if not, then keep following the parent route
	return backtrack(parentRow, parentCol, startRow, startCol, closedList);
}

bool RouteComputer::betterOptionIn(int r, int c, double f,
									 map<pair<int, int>, double>& fVals)
{
	map<pair<int, int>, double>::iterator it = fVals.find(pair<int, int>(r, c));

	  // if there's an f value in there and it's better than the one we have, return
	  // true
	if (it != fVals.end() && it->second < f) {
		return true;
	}

	return false;
}

Direction RouteComputer::evalDirection(int fromR, int fromC,
										 int toR, int toC)
{
	  // coming from row above, going down
	if (toR == fromR + 1) {
		return Down;
	}

	if (toR == fromR - 1) {
		return Up;
	}

	  // coming from left, going right
	if (toC == fromC + 1) {
		return Right;
	}

	if (toC == fromC - 1) {
		return Left;
	}

	return Bad;
}























void RouteComputer::printClosedList(vector<vector<node>>& l) {
	for (unsigned int i = 0; i != l.size(); i++) {
		for (unsigned int j = 0; j != l[i].size(); j++) {
			if (l[i][j].parentRow == -1 && l[i][j].parentCol == -1) {
				cout << "BEG ";
			}
			else if (l[i][j].f < 0) {
				cout << "INV ";
			} else {
				cout << l[i][j].parentRow << "," << l[i][j].parentCol << " ";
			}
		}
		cout << endl;
	}
	cout << endl << endl;
}