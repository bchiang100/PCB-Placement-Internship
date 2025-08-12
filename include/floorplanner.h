#include "module.h"

#ifndef FLOORPLANNER_H
#define FLOORPLANNER_H

class Floorplanner
{
public:
    Floorplanner(std::ifstream& input_blk, std::ifstream& input_net) : _input_blk(input_blk), _input_net(input_net) { }
    PlacementManager manager;
    void floorplan();
    double calcW(std::vector<Net>& nets);
    bool readBlockFile(std::ifstream &input_blk);
    bool readNetFile(std::ifstream &input_net);
    int outlineHeight;
    int outlineWidth;
    void checkPlacementInformation() { manager.printInformation(); };
private:
    
    std::ifstream& _input_blk;
    std::ifstream& _input_net;
};

#endif