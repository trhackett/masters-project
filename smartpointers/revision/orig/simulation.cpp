
#include "simulation.h"
#include <cstdlib>
#include <queue>

/* ============================================================================
	Potential heuristic functions
   ============================================================================ */ 
double manhattenDistance(int row, int col, const Simulation* sim) {
	return abs(row - sim->getEndRow()) +
       abs(col - sim->getEndCol());
}

double avoidTraffic(int row, int col, const Simulation* sim) {
	return sim->getIntersectionAt(row, col)->getNumCars();
}


void Car::init(int row, int col, double (*hFunc) (int, int, const Simulation*))
{
	m_row = row;
	m_col = col;
	m_heuristicFunc = hFunc;
	lastIterMoved = -1;
	routeComputeTime = 0.0;
	m_timer.start();
}

double Car::getHValue(int row, int col, const Simulation* sim) {
	return m_heuristicFunc(row, col, sim);
}


Simulation::Simulation(int r, int c, int sR, int sC, int eR, int eC, int F, int numI, int carM)
 : iGrid(r, vector<SmartPtr<Intersection, UniquePtr>>(c)),
   maxRows(r), maxCols(c), startRow(sR), startCol(sC),
   endRow(eR), endCol(eC), carFrequency(F),
   carMovement(carM), numIterations(numI)
{
	cout << "simulation constructor done" << endl;
}


  // use the destructor only to print statistics
Simulation::~Simulation()
{
}


double Simulation::runSimulation() {

	  // each run of the for loop is an iteration
	for (currentIteration = 0; currentIteration < numIterations; currentIteration++) {

		  // potentially insert another car
		for (int i = 0; i < carFrequency; i++) {
			insertNewCar();
		}

		  // each intersection has the chance to move carMovement cars
		for (size_t i = 0; i < iGrid.size(); i++) {
			for (size_t j = 0; j < iGrid[i].size(); j++) {
				moveCarsAtIntersection(i, j);
			}
		}
	}

	  // before you're done, print out how long each car took on average
	return computeStats();
}

void Simulation::moveCarsAtIntersection(int row, int col) {
	  // move at most carMovement cars per intersection
	int carsMoved = 0;
	while (iGrid[row][col]->hasCars() && carsMoved < carMovement) {

		RouteComputer rc;
		SmartPtr<Car, SharedPtr> c = iGrid[row][col]->getCarToMove();

		  // impossible for a car that was moved to this intersection
		  // this iteration to be ahead of one that was moved here in
		  // a previous iteration (thanks queue) so break when you 
		  // find one that just moved here
		if (c->getLastIterMoved() == currentIteration) {
			break;
		}

		  // if moveCarInDirection returns true, the car is out of
		  // the simulation so mark its time
		double computationTime;
		if (moveCarInDirection(c, rc.getNextMove(c, *this, computationTime), computationTime)) {
			  // register how long it took to get from start to finish
			startToFinishTimes.push_back(c->getTime());
		}

		carsMoved++;
	}
}

double Simulation::computeStats() {
	double ave = 0.0;

	for (size_t i = 0; i < startToFinishTimes.size(); i++) {
		ave += startToFinishTimes[i];
	}

	return ave / (double)startToFinishTimes.size();
}


void Simulation::insertNewCar() {
	SmartPtr<Car, SharedPtr> temp(
		[] () { return new Car; }, 
		[] (Car* c) { delete c; });

	  // to test out different heuristics, pass different
	  // functions into here
	temp->init(startRow, startCol, &manhattenDistance);

	  // it is located in an intersection
	iGrid[startRow][startCol]->addCar(temp);
}


  // going into this function, you KNOW that the direction is a valid
  // one or it wouldn't even have been considered by getNextMove()

  // this returns true if the car has left the simulation and needs to be
  // removed from it's intersection
bool Simulation::moveCarInDirection(SmartPtr<Car, SharedPtr> carPtr, Direction d,
									double& computationTime)
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
			iGrid[currRow][currCol]->removeCar();
			return true;
		}

		iGrid[nextRow][nextCol]->addCar(carPtr);
		iGrid[currRow][currCol]->removeCar();

		carPtr->setRow(nextRow);
		carPtr->setCol(nextCol);
		carPtr->setLastIterMoved(currentIteration);
		carPtr->addComputationTime(computationTime);
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
Direction RouteComputer::getNextMove(SmartPtr<Car, SharedPtr> car, const Simulation& sim,
									 double& computeTime)
{
	Timer computeTimer;

	  // init empty
	priority_queue<node, vector<node>, decltype(cmpNodes)> openList(cmpNodes);

	  // initialize closedList and fvals
	vector<vector<node>> closedList(sim.getMaxRows());
	vector<vector<double>> closedFVals(sim.getMaxRows());
	vector<vector<double>> openFVals(sim.getMaxRows());

	for (int i = 0; i < sim.getMaxRows(); i++) {
		closedList[i] = vector<node>(sim.getMaxCols());
		closedFVals[i] = vector<double>(sim.getMaxCols(), -1);
		openFVals[i] = vector<double>(sim.getMaxCols(), -1);
	}

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
					Direction ret = backtrack(parent.row, parent.row,
								     car->row(), car->col(), closedList);
					computeTime = computeTimer.elapsed();
					return ret;
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
						openFVals[child.row][child.col] = child.f;
					}
				}
			}
		}

		  // add it to the closed vals
		closedList[parent.row][parent.col] = parent;
		closedFVals[parent.row][parent.col] = parent.f;
	}

	computeTime = computeTimer.elapsed();
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
									 vector<vector<double>>& fVals)
{
	  // if there's an f value in there that's better than this one
	  // return true
	if (fVals[r][c] != -1 && fVals[r][c] < f) {
		return true;
	}

	  // or else there's not a better option so return false
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