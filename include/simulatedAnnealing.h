#include "module.h"
#include "floorplanner.h"
#include "BStarTree.h"

#ifndef SIMULATEDANNEALING_H
#define SIMULATEDANNEALING_H
#define NUM_ITERATIONS 1000000
#define FAST_SA_C 100
#define FAST_SA_K 7
#define FAST_SA_P 0.90

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