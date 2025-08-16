#include "module.h"
#include "floorplanner.h"
#include "simulatedAnnealing.h"
#include "BStarTree.h"

void Floorplanner::floorplan()
{
    bool blockReadSuccess;
    bool netReadSuccess;
    blockReadSuccess = readBlockFile(_input_blk);
    netReadSuccess = readNetFile(_input_net);
    tree.buildTree(manager._blocks);
    tree.packFloorplan(manager._blocks);
    calcNorms();
    SimulatedAnnealing SA(this);
    SA.runFastSA();
}

double Floorplanner::calcW(std::vector<Net> &nets)
{
    double W = 0;
    for (Net &net : nets)
    {
        W += net.calcHPWL();
    }
    return W;
}

double Floorplanner::calcA(std::vector<Block> &blocks) {
    int greatestX = 0;
    int greatestY = 0;
    for (int i = 0; i < blocks.size(); i++) {
        if (blocks[i].getX1() + blocks[i].getWidth() > greatestX) {
            greatestX = blocks[i].getX1() + blocks[i].getWidth();
        }
        if (blocks[i].getY1() + blocks[i].getHeight() > greatestY) {
            greatestY = blocks[i].getY1() + blocks[i].getHeight();
        }
    }
    return greatestX * greatestY;
}

void Floorplanner::calcNorms() {
    double totalArea = 0.0;
    double totalWirelength = 0.0;
    int m = manager._blocks.size() * 10;
    BStarTree original = tree;
     for (int i = 0; i < m; i++) {
        int operation = rand() % 3;
        if (operation == 0) {
            rotateOperation(*getTree(), manager._blocks);
        } else if (operation == 1) {
            moveOperation(*getTree(), manager._blocks);
        } else {
            swapOperation(*getTree(), manager._blocks);
        }
        

        tree.packFloorplan(manager._blocks);
     
        double currentArea = calcA(manager._blocks);
        double currentWirelength = calcW(manager._nets);
        
        totalArea += currentArea;
        totalWirelength += currentWirelength;
    }
    
    Anorm = totalArea / m;
    Wnorm = totalWirelength / m;
    tree = original;
    tree.packFloorplan(manager._blocks);
}

double Floorplanner::calcCost() {
    double A = calcA(manager._blocks);
    double W = calcW(manager._nets);
    
    return _alpha * (A / Anorm) + (1.0 - _alpha) * (W / Wnorm);
}

double Net::calcHPWL()
{
    double minX = INT_MAX;
    double minY = INT_MAX;
    double maxX = 0.0;
    double maxY = 0.0;

    for (Terminal *term : getTermList())
    {
        minX = (term->getX1() < minX) ? term->getX1() : minX;
        minY = (term->getY1() < minY) ? term->getY1() : minY;
        maxX = (term->getX2() > maxX) ? term->getX2() : maxX;
        maxY = (term->getY2() > maxY) ? term->getY2() : maxY;
    }
    return ((maxX - minX)) + ((maxY) - (minY));
}

bool Floorplanner::readBlockFile(std::ifstream &input_blk)
{
    if (!input_blk.is_open())
        return false;
    std::string temp;
    int numBlocks;
    int numTerminals;

    std::string blockName;
    int blockWidth;
    int blockHeight;

    std::string terminalName;
    int coord_x;
    int coord_y;

    getline(input_blk, temp);
    std::istringstream stream1(temp);
    stream1 >> temp >> outlineWidth >> outlineHeight;

    getline(input_blk, temp);
    std::istringstream stream2(temp);
    stream2 >> temp >> numBlocks;

    getline(input_blk, temp);
    std::istringstream stream3(temp);
    stream3 >> temp >> numTerminals;
    getline(input_blk, temp);
    for (int i = 0; i < numBlocks; i++)
    {
        getline(input_blk, temp);
        std::istringstream streamBlocks(temp);
        streamBlocks >> blockName >> blockWidth >> blockHeight;
        manager.addBlock(blockName, blockWidth, blockHeight);
        // macros.push_back(Macro(blockName, blockWidth, blockHeight));
    }
    getline(input_blk, temp);
    for (int i = 0; i < numTerminals; i++)
    {
        getline(input_blk, temp);
        std::istringstream streamTerminals(temp);
        streamTerminals >> terminalName >> temp >> coord_x >> coord_y;
        manager.addTerminal(terminalName, coord_x, coord_y);
        // terminals.push_back(Terminal(terminalName, coord_x, coord_y));
    }
    return true;
}

bool Floorplanner::readNetFile(std::ifstream &input_net)
{
    if (!input_net.is_open())
        return false;

    std::string temp;
    int numNets;
    int netDegree;
    std::string netName;

    input_net >> temp >> numNets;

    for (int i = 0; i < numNets; i++)
    {
        std::vector<std::string> elements;

        input_net >> temp >> netDegree;

        for (int j = 0; j < netDegree; j++)
        {
            input_net >> netName;
            elements.push_back(netName);
        }

        manager._nets.emplace_back();
        Net &currentNet = manager._nets.back();

        for (const auto &termName : elements)
        {
            Terminal *term = manager.findTerminal(termName);
            if (term)
            {
                currentNet.addTerm(term);
            }
        }
    }
    return true;
}

void PlacementManager::printInformation()
{
    std::cout << "Terminal size is " << _terminals.size() << std::endl;
    std::cout << "Block size is " << _blocks.size() << std::endl;
    for (size_t i = 0; i < _terminals.size(); i++)
    {
        std::cout << "Terminal " << i + 1 << ": " << _terminals[i].getName() << std::endl;
    }

    for (size_t i = 0; i < _blocks.size(); i++)
    {
        std::cout << "Block " << i + 1 << ": " << _blocks[i].getName() << std::endl;
        std::cout << "Block " << i + 1 << " x1: " << _blocks[i].getX1() << std::endl;
        std::cout << "Block " << i + 1 << " y1: " << _blocks[i].getY1() << std::endl;
        std::cout << "Block " << i + 1 << " x2: " << _blocks[i].getX2() << std::endl;
        std::cout << "Block " << i + 1 << " y2: " << _blocks[i].getY2() << std::endl;
    }

    for (size_t i = 0; i < _nets.size(); i++)
    {
        std::cout << "Netlist " << i + 1 << ": " << std::endl;
        for (size_t j = 0; j < _nets[i].getTermList().size(); j++)
        {
            std::cout << _nets[i].getTermList()[j]->getName() << std::endl;
        }
    }
}

void BStarTree::buildTree(const std::vector<Block> &blocks)
{
    for (int i = 0; i < blocks.size(); i++)
    {
        nodes.push_back(new TreeNode(i));
    }

    for (int i = 0; i < nodes.size() / 2; i++)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < nodes.size())
        {
            nodes[i]->left = nodes[left];
            nodes[left]->parent = nodes[i];
        }
        if (right < nodes.size())
        {
            nodes[i]->right = nodes[right];
            nodes[right]->parent = nodes[i];
        }
        root = nodes[0];
    }
}

void BStarTree::printTree(TreeNode *root)
{
    if (root == nullptr)
        return;
    std::cout << "Block " << root->blockID << std::endl;
    printTree(root->left);
    printTree(root->right);
}

void BStarTree::packFloorplan(std::vector<Block> &blocks)
{
    if (root == nullptr)
        return;
    for (auto &block : blocks)
    {
        block.setPos(0, 0, 0, 0);
    }
    packRest(root, blocks, 0, 0);
}

void BStarTree::packRest(TreeNode *node, std::vector<Block> &blocks, int x, int y)
{
    if (node == nullptr)
        return;
    Block &curr = blocks[node->blockID];
    curr.setPos(x, y, x + curr.getWidth(), y + curr.getHeight());
    if (node->left != nullptr)
    {
        packRest(node->left, blocks, x + curr.getWidth(), y);
    }
    if (node->right != nullptr)
    {
        packRest(node->right, blocks, x, y + curr.getHeight());
    }
}

// rotates a block 90 degrees (swaps width and height)
void Floorplanner::rotateOperation(BStarTree &tree, std::vector<Block> &blocks)
{
    int blockID = rand() % blocks.size();
    Block &block = blocks[blockID];
    int temp = block.getWidth();
    block.setWidth(block.getHeight());
    block.setHeight(temp);
    //std::cout << blockID << "'s dimensions have been switched" << std::endl;
    tree.packFloorplan(blocks);
}

void Floorplanner::moveOperation(BStarTree &tree, std::vector<Block> &blocks)
{
    int targetID = rand() % blocks.size();
    int destID = rand() % blocks.size();
    TreeNode *target = tree.getNode(targetID);
    TreeNode *dest = tree.getNode(destID);

    while (target == dest || target->parent == nullptr || isDescendant(target, dest) || (dest->left != nullptr && dest->right != nullptr))
    {
        targetID = rand() % blocks.size();
        destID = rand() % blocks.size();
        target = tree.getNode(targetID);
        dest = tree.getNode(destID);
    }

    TreeNode *targetLeft = target->left;
    TreeNode *targetRight = target->right;
    TreeNode *targetParent = target->parent;

    if (targetParent->left == target)
        targetParent->left = nullptr;
    else if (targetParent->right == target)
        targetParent->right = nullptr;

    if (dest->left == nullptr && dest->right == nullptr)
    {
        if (rand() % 2) {
            dest->left = target;
        }

        else {
            dest->right = target;
        }
    } else if (dest->left == nullptr) {
        dest->left = target;
    } else {
        dest->right = target;
    }
    target->parent = dest;

    tree.packFloorplan(blocks);
}

bool Floorplanner::isDescendant(TreeNode* target, TreeNode* dest)
{
    if (target == nullptr)
        return false;
    if (target == dest) {
        //std::cout << "ERROR: IS DIRECT DESCENDANT" << std::endl;
        return true;
    }
    return isDescendant(target->left, dest) || isDescendant(target->right, dest);
}

void Floorplanner::swapOperation(BStarTree& tree, std::vector<Block>& blocks) {
    if (blocks.size() < 2) {
        return;
    }
    
    int node1_ID = rand() % blocks.size();
    int node2_ID = rand() % blocks.size();
    
    while (node1_ID == node2_ID) {
        node2_ID = rand() % blocks.size();
    }
    
    if (node1_ID == 0 || node2_ID == 0) {
        return;
    }
    
    if (node1_ID >= (int)tree.getNodes().size() || node2_ID >= (int)tree.getNodes().size() ||
        node1_ID < 0 || node2_ID < 0) {
        return;
    }
    
    TreeNode* node1 = tree.getNodes()[node1_ID];
    TreeNode* node2 = tree.getNodes()[node2_ID];
    
    if (node1 == nullptr || node2 == nullptr) {
        return;
    }
    
    if (node1->parent == node2 || node2->parent == node1) {
        return;
    }
    
    TreeNode* node1Parent = node1->parent;
    TreeNode* node1Left = node1->left;
    TreeNode* node1Right = node1->right;
    bool isLeft1 = (node1Parent != nullptr && node1Parent->left == node1);
    

    TreeNode* node2Parent = node2->parent;
    TreeNode* node2Left = node2->left;
    TreeNode* node2Right = node2->right;
    bool isLeft2 = (node2Parent != nullptr && node2Parent->left == node2);
    
    node1->parent = node2Parent;
    node1->left = node2Left;
    node1->right = node2Right;
    
    node2->parent = node1Parent;
    node2->left = node1Left;
    node2->right = node1Right;
    
    if (node1->parent != nullptr) {
        if (isLeft2) {
            node1->parent->left = node1;
        } else {
            node1->parent->right = node1;
        }
    }
    
    if (node2->parent != nullptr) {
        if (isLeft1) {
            node2->parent->left = node2;
        } else {
            node2->parent->right = node2;
        }
    }
    
    if (node1->left != nullptr) {
        node1->left->parent = node1;
    }
    if (node1->right != nullptr) {
        node1->right->parent = node1;
    }
    if (node2->left != nullptr) {
        node2->left->parent = node2;
    }
    if (node2->right != nullptr) {
        node2->right->parent = node2;
    }
    
    tree.packFloorplan(blocks);
}

void SimulatedAnnealing::runFastSA()
{
    double temperature = INITIAL_TEMP;
    double coolingRate = COOLING_RATE;
    int maxIterations = NUM_ITERATIONS;
    
    BStarTree* tree = _floorplanner->getTree();
    std::vector<Block>& blocks = _floorplanner->manager._blocks;
    
    double currentCost = _floorplanner->calcCost();
    double bestCost = currentCost;
    
    std::vector<Block> currentBlocks = blocks;
    std::vector<Block> bestBlocks = currentBlocks;
    BStarTree currentTree = *tree;
    BStarTree bestTree = *tree;
    
    int acceptedMoves = 0;
    int validSolutions = 0;
    bool foundValidSolution = false;
    
    clock_t startTime = clock();
    
    for (int i = 0; i < maxIterations; i++) {

        std::vector<Block> originalBlocks = blocks;
        BStarTree originalTree = *tree;
        
        int method = rand() % 3;
        if (method == 0) _floorplanner->moveOperation(*tree, blocks);
        else if (method == 1) _floorplanner->swapOperation(*tree, blocks);
        else if (method == 2) _floorplanner->rotateOperation(*tree, blocks);
        
        double newCost = _floorplanner->calcCost();
    
        bool isValid = checkOutlineValidity() && !checkOverlap();
        if (isValid) {
            validSolutions++;
            foundValidSolution = true;
        }
        
        if (acceptSolution(newCost, currentCost, temperature)) {
            currentBlocks = blocks;
            currentTree = *tree;
            currentCost = newCost;
            acceptedMoves++;
            
            if (isValid && newCost < bestCost) {
                bestCost = newCost;
                bestBlocks = blocks;
                bestTree = *tree;
            }
        } else {
            blocks = originalBlocks;
            *tree = originalTree;
        }
        
        temperature *= coolingRate;
        
        if (i % 10000 == 0) {
            double acceptanceRate = (double)acceptedMoves / (i + 1) * 100;
            double validRate = (double)validSolutions / (i + 1) * 100;
            bool currentValid = checkOutlineValidity() && !checkOverlap();
            
            std::cout << "Iteration " << i 
                     << ", Current Cost: " << currentCost 
                     << ", Best Cost: " << bestCost
                     << ", Valid: " << currentValid
                     << ", Temp: " << temperature
                     << ", Accept%: " << acceptanceRate
                     << ", Valid%: " << validRate << std::endl;
        }
    }
    
    clock_t endTime = clock();
    double runtime = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;
    
    if (foundValidSolution) {
        blocks = bestBlocks;
        *tree = bestTree;
        tree->packFloorplan(blocks);
        
        std::cout << "\n=== FINAL RESULTS ===" << std::endl;
        std::cout << "Final best valid cost: " << bestCost << std::endl;
        std::cout << "Runtime: " << runtime << " seconds" << std::endl;
        std::cout << "Total accepted moves: " << acceptedMoves << std::endl;
        std::cout << "Total valid solutions found: " << validSolutions << std::endl;
        
        _floorplanner->recordOutput(
            _floorplanner->manager._blocks,
            _floorplanner->manager._nets, 
            _floorplanner->manager._terminals,
            bestCost,
            runtime,
            "output.rpt"
        );
        
        std::cout << "Valid solution written to output.rpt" << std::endl;
        
    } else {
        std::cout << "\n=== NO VALID SOLUTION FOUND ===" << std::endl;
        std::cout << "Final current cost: " << currentCost << std::endl;
        std::cout << "Runtime: " << runtime << " seconds" << std::endl;
        std::cout << "Total accepted moves: " << acceptedMoves << std::endl;
        
        _floorplanner->recordOutput(
            _floorplanner->manager._blocks,
            _floorplanner->manager._nets, 
            _floorplanner->manager._terminals,
            currentCost,
            runtime,
            "output.rpt"
        );
        
        std::cout << "Debug output written to output.rpt (INVALID SOLUTION)" << std::endl;
    }
}

double Floorplanner::calcPenaltyCost() {
    double baseCost = calcCost();
    
    double outlinePenalty = calcOutlinePenalty();
    double overlapPenalty = calcOverlapPenalty();
    
    return baseCost + outlinePenalty + overlapPenalty;
}

double Floorplanner::calcOutlinePenalty() {

}

double Floorplanner::calcOverlapPenalty() {

}

bool SimulatedAnnealing::checkOutlineValidity() {
    for (int i = 0; i < _floorplanner->manager._blocks.size(); i++) {
        if (_floorplanner->manager._blocks[i].getX1() + _floorplanner->manager._blocks[i].getWidth() > _floorplanner->outlineWidth) {
            return false;
        }
        if (_floorplanner->manager._blocks[i].getY1() + _floorplanner->manager._blocks[i].getHeight() > _floorplanner->outlineHeight) {
            return false;
        }
    }
    return true;
}

bool SimulatedAnnealing::checkOverlap() {
    for (int i = 0; i < _floorplanner->manager._blocks.size(); i++) {
        for (int j = i + 1; j < _floorplanner->manager._blocks.size(); j++) {
            if (_floorplanner->manager._blocks[i].getX1() < _floorplanner->manager._blocks[j].getX1() + _floorplanner->manager._blocks[j].getWidth() &&
                _floorplanner->manager._blocks[i].getX1() + _floorplanner->manager._blocks[i].getWidth() > _floorplanner->manager._blocks[j].getX1() &&
                _floorplanner->manager._blocks[i].getY1() < _floorplanner->manager._blocks[j].getY1() + _floorplanner->manager._blocks[j].getHeight() &&
                _floorplanner->manager._blocks[i].getY1() + _floorplanner->manager._blocks[i].getHeight() > _floorplanner->manager._blocks[j].getY1()) {
                return true;
            }
        }
    }
    return false;
}

bool SimulatedAnnealing::checkTreeValidity(BStarTree& tree) {
    std::vector<TreeNode*> nodes = tree.getNodes();
    
    for (auto node : nodes) {
        if (node == nullptr) continue;
        
        if (node->left && node->left->parent != node) {
            return false;
        }
        
        if (node->right && node->right->parent != node) {
            return false;
        }
    }
    return true;
}

bool SimulatedAnnealing::acceptSolution(double newCost, double currentCost, double temperature) {
    if (newCost <= currentCost) {
        return true;
    }
    
    double probability = exp(-(newCost - currentCost) / temperature);
    return (rand() / (double)RAND_MAX) < probability;
}

int Floorplanner::recordOutput(std::vector<Block>& blocks, const std::vector<Net>& nets, const std::vector<Terminal>& terminals, double bestCost, double runtime, std::string output) {
    int maxX;
    int maxY;
    calculateChipDimensions(blocks, &maxX, &maxY);
    
    std::ofstream file(output);
    if (!file.is_open()) return false;
    
    file << bestCost << std::endl;
    file << calcW(const_cast<std::vector<Net>&>(nets)) << std::endl;
    file << maxX * maxY << std::endl;
    file << maxX << " " << maxY << std::endl;
    file << runtime << std::endl;
    
    for (int i = 0; i < blocks.size(); i++) {
        file << blocks[i].getName() << " " << blocks[i].getX1() << " " << blocks[i].getY1() << " " 
             << blocks[i].getX2() << " " << blocks[i].getY2() << std::endl;
    }
    
    return true;
}

void Floorplanner::calculateChipDimensions(std::vector<Block>& blocks, int* maxX, int* maxY) {
    *maxX = 0;
    *maxY = 0;
    
    for (Block& block : blocks) {
        if (block.getX1() + block.getWidth() > *maxX) {
            *maxX = block.getX1() + block.getWidth();
        }
        if (block.getY1() + block.getHeight() > *maxY) {
            *maxY = block.getY1() + block.getHeight();
        }
    }
}
