
#ifndef SIMULATION_H
#define SIMULATION_H

#include "LeaklessPtr.h"
#include "timer.h"
#include <vector>
#include <list>
#include <iostream>
using namespace std;

  // potentially four directions a car can move in
enum Direction { Up, Right, Down, Left, Bad };

class Simulation;

struct Stats {
	int numCarsMadeIt;
	double aveTime;
	Stats(int n, double d) : numCarsMadeIt(n), aveTime(d) {}
	Stats() : numCarsMadeIt(0), aveTime(0.0) {}
};

/* ============================================================================
	Cars can have individual heuristic functions and each has their
	own position and timer running
   ============================================================================ */
class Car {
public:
	Car() {}

	void init(int r, int c) {
		m_row = r;
		m_col = c;
		lastIterMoved = -1;
		routeComputeTime = 0.0;
		m_timer.start();
	}


	  // basic get/set functions
	void restartTime()    { m_timer.start(); }
	int getTime() const   { return m_timer.elapsed() - routeComputeTime; }
	int row() const       { return m_row; }
	int col() const       { return m_col; }
	int getLastIterMoved() const { return lastIterMoved; }

	void setRow(int r)    { m_row = r; }
	void setCol(int c)    { m_col = c; }
	void setLastIterMoved(int l) { lastIterMoved = l; }
	void addComputationTime(double d) { routeComputeTime += d; }

private:
	Timer m_timer;
	double routeComputeTime;
	int m_row, m_col;
	int lastIterMoved;
};


/* ================================================================================
	Each simulation has a grid of Intersections that has a list of cars which will
	change as the simulation progresses.

	There can be any number of cars at an intersection at any given time but a car
	can be at only one intersection at any given time.
   ================================================================================ */
class Intersection {
public:
	Intersection()
	{ }

	  // get the number of cars at the intersection (may want more data points)
	  // to support more heuristics, that's fine
	int getNumCars() const { return cars.size(); }
	  // see if there are cars
	bool hasCars() const   { return !cars.empty(); }
	  // get the car that's been waiting the longest
	LeaklessPtr<Car, AltruisticPtrImpl> getCarToMove()
		{ return cars.front(); }

	  // make adjustments to the set of cars here
	void removeCar()
		{ cars.pop_front(); }
	void addCar(LeaklessPtr<Car, AltruisticPtrImpl> carPtr)
		{ cars.push_back(std::move(carPtr)); }


private:
	list<LeaklessPtr<Car, AltruisticPtrImpl>> cars;
};

/* ================================================================================
	Route Computer
   ================================================================================ */

class RouteComputer {
public:
	  // when getting the next move, you have complete knowledge
	  // of the simulation at that time - you know how many cars
	  // are at each intersection and you also know the goal
	Direction getNextMove(LeaklessPtr<Car, AltruisticPtrImpl> carPtr, const Simulation& sim,
						  double& dT);

private:
	  // node struct and comparison function only for A*
	struct node {
		int row, col;
		int parentRow, parentCol;
		double g, h, f;
		node(int r, int c, int pr, int pc, double gVal, double hVal)
		 : row(r), col(c), parentRow(pr), parentCol(pc), g(gVal), h(hVal), f(g+h)
		{ }
		  // if f < 0, then it's invalid
		node() : f(-1) {}
	};

	  // comparison function that allows us to use priority queue with
	  // nodes - this is what ensures that the lowest f values are
	  // popped off first
	bool (*cmpNodes) (const node&, const node&) =
		[] (const node& left, const node& right)
		{ return left.f > right.f; };

	Direction backtrack(int r, int c, int startRow, int startCol,
					    vector<vector<node>>& closedList);
	bool betterOptionIn(int r, int c, double f, vector<vector<double>>& fVals);
	Direction evalDirection(int fromR, int fromC, int toR, int toC);

	void printClosedList(vector<vector<node>>& l);
};



/* ============================================================================
	Meat and Bones
   ============================================================================ */

class Simulation {
public:

	Simulation(int r, int c, int sR, int sC, int eR, int eC, int Fz, int numI, int numC,
			   double (*hFunc) (int, int, const Simulation*));
	~Simulation();
	Stats runSimulation();

	int getEndRow() const  { return endRow; }
	int getEndCol() const  { return endCol; }
	int getMaxRows() const { return maxRows; }
	int getMaxCols() const { return maxCols; }

	const Intersection& getIntersectionAt(int r, int c) const
		{ return *iGrid[r][c]; }

	bool getRowColInDirection(int& row, int& col, Direction d) const;

	// call the heuristic function
	double getHvalue(int sRow, int sCol) const { return heuristicFunc(sRow, sCol, this); }

	void printSim() const {
		for (size_t i = 0; i < iGrid.size(); i++) {
			for (size_t j = 0; j < iGrid.size(); j++) {
				cout << iGrid[i][j]->getNumCars() << " ";
			}
			cout << endl;
		}
		cout << endl;
	}

private:
	  // stored as a two dimensional array of smart pointers to an
	  // intersection
	using IntersectionGrid = vector<vector<LeaklessPtr<Intersection, SelfishPtrImpl>>>;

	  // 2D array (vector) of Intersections
	IntersectionGrid iGrid;

	int maxRows, maxCols;
	  // the simulation has an index at which cars enter and an index at which
	  // they have arrive at their destination
	int startRow, startCol, endRow, endCol;

	  // release a car every carFrequency iterations
	int carFrequency;
	  // number of cars that can leave intersection each tick
	int carMovement;

	  // number of iterations the simulation goes through
	int numIterations;
	  // current iteration
	int currentIteration;
	// heuristic function for route computers
	double (*heuristicFunc) (int, int, const Simulation*);

	  // newly inserted cars start at startRow,startCol and do nothing their
	  // first iteration
	void insertNewCar();
	  // takes coordinates and moves the cars there
	void moveCarsAtIntersection(int row, int col);
	  // takes a car and a direction and moves it in that direction
	bool moveCarInDirection(const LeaklessPtr<Car, AltruisticPtrImpl> carPtr, Direction d, double& cT);

	  // only basic stats
	vector<double> startToFinishTimes;
	Stats computeStats();
};

#endif