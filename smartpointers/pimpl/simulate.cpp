#include <iostream>
#include "timer.h"
#include "simulation.h"

using namespace std;

void run(Timer& t, double& constructTime, double& simulationTime) {
	
	// restart time to account for function call time
	t.start();

	// create simulation - number of rows and columns
	Simulation sim(10, 10,  // maxRows and maxCols
				   0, 0,    // startRow and startCol
				   9, 9,    // endRow and endCol
				   5        // car frequency
				   );       
	constructTime = t.elapsed();

	// restart timer
	t.start();

	// start simulation
	sim.runSimulation(100);
	simulationTime = t.elapsed();

	// restart time
	t.start();

}

int main() {
	double constructTime, simulationTime, destructAndReturnTime;
	// start timer
	Timer t;
	run(t, constructTime, simulationTime);
	destructAndReturnTime = t.elapsed();

	cout << "Construction time " << constructTime << "ms" << endl;
	cout << "Simulation took " << simulationTime << "ms" << endl;
	cout << "Destruct and return time " << destructAndReturnTime << "ms" << endl;

}