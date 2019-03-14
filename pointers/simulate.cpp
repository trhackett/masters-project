#include <iostream>
#include "timer.h"
#include "simulation.h"

using namespace std;

/* ============================================================================
	Potential heuristic functions
   ============================================================================ */ 
double manhattenDistance(int row, int col, const Simulation* sim) {
	return abs(row - sim->getEndRow()) +
       abs(col - sim->getEndCol());
}

double avoidTraffic(int row, int col, const Simulation* sim) {
	return sim->getIntersectionAt(row, col).getNumCars();
}

void run(Timer& t, double& constructTime, double& simulationTime,
		 Stats& aveStartToFinishTime) {
	
	// restart time to account for function call time
	t.start();

	// create simulation
	Simulation sim(10, 10,  // maxRows and maxCols
				   0, 0,    // startRow and startCol
				   9, 9,    // endRow and endCol
				   5,       // number of cars to enter per iteration
				   100,     // number of iterations 
				   1,       // number of cars that leave intersection each iteration
				   &avoidTraffic); // heuristic function
	constructTime = t.elapsed();

	// restart timer
	t.start();

	// start simulation
	aveStartToFinishTime = sim.runSimulation();
	simulationTime = t.elapsed();

	// restart time
	t.start();

}

int main() {
	double constructTime, simulationTime, destructAndReturnTime;
	Stats stats;
	// start timer
	Timer t;
	run(t, constructTime, simulationTime, stats);
	destructAndReturnTime = t.elapsed();

	cout << stats.numCarsMadeIt << " cars made it from start to finish, ";
	cout << "taking on average " << stats.aveTime << " ms to do so" << endl;

	cout << "Construction time " << constructTime << "ms" << endl;
	cout << "Simulation took " << simulationTime << "ms" << endl;
	cout << "Destruct and return time " << destructAndReturnTime << "ms" << endl;

}