#include "module.h"
#include "floorplanner.h"

void Floorplanner::floorplan() {
    bool blockReadSuccess;
    bool netReadSuccess;
    blockReadSuccess = readBlockFile(_input_blk);
    netReadSuccess = readNetFile(_input_net);
}

double Floorplanner::calcW(std::vector<Net>& nets) {
    double W = 0;
    for (Net& net : nets) {
        W += net.calcHPWL();
    }
    return W;
}

double Net::calcHPWL() {
    double minX = INT_MAX;
    double minY = INT_MAX;
    double maxX = 0.0;
    double maxY = 0.0;

    for (Terminal* term : getTermList()) {
        minX = (term->getX1() < minX) ? term->getX1() : minX;
        minY = (term->getY1() < minY) ? term->getY1() : minY;
        maxX = (term->getX2() > maxX) ? term->getX2() : maxX;
        maxY = (term->getY2() > maxY) ? term->getY2() : maxY;
    }
    return ((maxX - minX)) + ((maxY) - (minY));
}

bool Floorplanner::readBlockFile(std::ifstream &input_blk) {
    if (!input_blk.is_open()) return false;
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
    for (int i = 0; i < numBlocks; i++) {
        getline(input_blk, temp);
        std::istringstream streamBlocks(temp);
        streamBlocks >> blockName >> blockWidth >> blockHeight;
        manager.addBlock(blockName, blockWidth, blockHeight);
        //macros.push_back(Macro(blockName, blockWidth, blockHeight));
    }
    getline(input_blk, temp);
    for (int i = 0; i < numTerminals; i++) {
        getline(input_blk, temp);
        std::istringstream streamTerminals(temp);
        streamTerminals >> terminalName >> temp >> coord_x >> coord_y;
        manager.addTerminal(terminalName, coord_x, coord_y);
        //terminals.push_back(Terminal(terminalName, coord_x, coord_y));
    }
    return true;
}

bool Floorplanner::readNetFile(std::ifstream &input_net) {
    if (!input_net.is_open()) return false;
    
    std::string temp;
    int numNets;
    int netDegree;
    std::string netName;
    
    // Read "NumNets: 121"
    input_net >> temp >> numNets;  // temp="NumNets:", numNets=121
    
    for (int i = 0; i < numNets; i++) {
        std::vector<std::string> elements;
        
        // Read "NetDegree: X"
        input_net >> temp >> netDegree;  // temp="NetDegree:", netDegree=X
        
        // Collect all terminal names for this net
        for (int j = 0; j < netDegree; j++) {
            input_net >> netName;
            elements.push_back(netName);
        }
        
        // Add one net with all the terminals
        manager._nets.emplace_back();
        Net& currentNet = manager._nets.back();
        
        // Add each terminal to this net
        for (const auto& termName : elements) {
            Terminal* term = manager.findTerminal(termName);
            if (term) {
                currentNet.addTerm(term);
            }
        }
    }
    return true;
}





