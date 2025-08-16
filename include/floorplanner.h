#include "module.h"
#include "BStarTree.h"
#ifndef FLOORPLANNER_H
#define FLOORPLANNER_H



class Floorplanner
{
public:
    Floorplanner(double alpha, std::ifstream& input_blk, std::ifstream& input_net) : _alpha(alpha), _input_blk(input_blk), _input_net(input_net) { }
    PlacementManager manager;
    void floorplan();
    
    bool readBlockFile(std::ifstream &input_blk);
    bool readNetFile(std::ifstream &input_net);
    int outlineHeight;
    int outlineWidth;
    void checkPlacementInformation() { manager.printInformation(); };
    void calcPerturbations();
    double calcCost();
    double calcPenaltyCost();
    double calcOutlinePenalty();
    double calcOverlapPenalty();
    void rotateOperation(BStarTree& tree, std::vector<Block>& blocks);
    void moveOperation(BStarTree& tree, std::vector<Block>& blocks);
    void swapOperation(BStarTree& tree, std::vector<Block>& blocks);
    int recordOutput(std::vector<Block>& blocks, const std::vector<Net>& nets, const std::vector<Terminal>& terminals, double bestCost, double runtime, std::string output);
    void calculateChipDimensions(std::vector<Block>& blocks, int* maxX, int* maxY);
    BStarTree* getTree() {return &tree;};
    double getAverageUphillCost() { return averageUphillCost; };
private:
    BStarTree tree;
    double _alpha;
    std::ifstream& _input_blk;
    std::ifstream& _input_net;
    double calcW(std::vector<Net>& nets);
    double calcA(std::vector<Block>& blocks);
    double Wnorm;
    double Anorm;
    double averageUphillCost = 0;
    bool isDescendant(TreeNode* target, TreeNode* dest);
};

#endif