
#ifndef SIMULATION_H
#define SIMULATION_H

#include "SmartPtr.h"
#include "SharedPtr.h"
#include "UniquePtr.h"
#include "timer.h"
#include <vector>
#include <list>
#include <set>
#include <map>
using namespace std;

  // potentially four directions a car can move in
enum Direction { upDir, rightDir, downDir, leftDir, badDir };

class Simulation {
public:
	Simulation(int r, int c);
	void runSimulation(int numIterations);

	  // the only thing that cars need to know about are its
	  // position and it's routing function
	struct Car {
		Timer t;
		int row, col;
		  // heuristic function used by A*
		double (*hFunc) (int, int, Simulation*);
	};
	  // the road is simplified to a 2D grid of intersections.
	  // There can be any number of cars at an intersection at
	  // any given time but each car can only be at one intersection
	  // at any given time. Each time step, every car gets to move
	  // to the next intersection - provided by it routing function
	struct Intersection {
		set<SmartPtr<Car, SharedPtr>> cars;
		int getNumCars() const { return cars.size(); }

		  // cars leave intersections
		void removeCar(SmartPtr<Car, SharedPtr> carPtr) {
			set<SmartPtr<Car, SharedPtr>>::iterator it = cars.find(carPtr);

			  // if it's in there, remove it
			if (it != cars.end()) {
				cars.erase(it);
			}
		}

		  // and cars come into intersections
		void addCar(SmartPtr<Car, SharedPtr> carPtr) {
			cars.insert(carPtr);
		}
	};

	int getEndRow() const { return endRow; }
	int getEndCol() const { return endCol; }
	int getMaxRows() const { return maxRows; }
	int getMaxCols() const { return maxCols; }
	  // must return by const reference to obey the promise the const function makes
	const vector<vector<SmartPtr<Intersection, UniquePtr>>>& getIntersections() const {
		return intersections;
	}

	int getRowInDirection(int, Direction) const;
	int getColInDirection(int, Direction) const;

private:
	  // stored as a two dimensional array of smart pointers to an
	  // intersection
	vector< vector< SmartPtr<Intersection, UniquePtr> > > intersections;
	// Intersection** intersections;

	int maxRows, maxCols;
	  // the simulation has an index at which cars enter and an index at which
	  // they have arrive at their destination
	int startRow, startCol, endRow, endCol;

	  // release a car every carFrequency iterations
	int carFrequency;

	list<SmartPtr<Car, SharedPtr>> allCars;

	  // newly inserted cars start at startRow,startCol and do nothing their
	  // first iteration
	void insertNewCar();
	  // takes a car and a direction and moves it in that direction
	void moveCarInDirection(SmartPtr<Car, SharedPtr>, Direction);
	  // when getting the next move, you have complete knowledge
	  // of the simulation at that time - you know how many cars
	  // are at each intersection and you also know the goal
	Direction getNextMove(SmartPtr<Car, SharedPtr>);

	  // node struct and comparison function only for A*
	struct node {
		int row, col;
		int parentRow, parentCol;
		double g, h, f;
		node(int r, int c, int pr, int pc, double gVal, double hVal)
		 : row(r), col(c), parentRow(pr), parentCol(pc), g(gVal), h(hVal), f(g+h)
		{ }
		node() {}
	};

	bool (*cmpNodes) (const node&, const node&) =
		[] (const node& left, const node& right)
		{ return left.f > right.f; };

	Direction backtrack(int r, int c, map<pair<int, int>, node>& closedList);
	bool betterOptionIn(int r, int c, double f, map<pair<int, int>, double>& fVals);
	Direction evalDirection(int fromR, int fromC, int toR, int toC);
};

#endif