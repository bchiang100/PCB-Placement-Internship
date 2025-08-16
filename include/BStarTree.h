#ifndef BSTARTREE_H
#define BSTARTREE_H
#include "module.h"
struct TreeNode
{
    int blockID;
    TreeNode *parent;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int val) : blockID(val), parent(nullptr), left(nullptr), right(nullptr) {}
};
class BStarTree
{
public:
    void buildTree(const std::vector<Block> &blocks);
    void printTree(TreeNode *root);
    TreeNode *getRoot() { return root; };
    std::vector<TreeNode *> getNodes() { return nodes; };
    void packFloorplan(std::vector<Block> &blocks);
    void packRest(TreeNode *node, std::vector<Block> &blocks, int x, int y);
    TreeNode *getNode(int blockID) { return nodes[blockID]; };

private:
    TreeNode *root;
    std::vector<TreeNode *> nodes;
};
#endif