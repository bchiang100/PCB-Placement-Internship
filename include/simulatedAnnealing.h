#include "module.h"
#include "floorplanner.h"
#include "BStarTree.h"

#ifndef SIMULATEDANNEALING_H
#define SIMULATEDANNEALING_H
#define NUM_ITERATIONS 100000
#define INITIAL_TEMP 1000
#define COOLING_RATE 0.99

class SimulatedAnnealing {
    public:
        SimulatedAnnealing(Floorplanner* fp) : _floorplanner(fp) {}
        void runFastSA();
        bool checkOutlineValidity();
        bool checkOverlap();
        bool checkTreeValidity(BStarTree& tree);
        bool acceptSolution(double newCost, double currentCost, double temperature);
    private:
        Floorplanner* _floorplanner;
        

};
#endif