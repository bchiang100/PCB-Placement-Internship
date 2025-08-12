#include "../include/module.h"
#include "../include/floorplanner.h"
#include <iostream>
#include <fstream>
#include <vector>



int main(int argc, char** argv)
{
    std::ifstream input_blk, input_net;
    std::ofstream output;

    double alpha;

    if (argc == 5) {
        alpha = std::stod(argv[1]);
        input_blk.open(argv[2], std::ios::in);
        input_net.open(argv[3], std::ios::in);
        output.open(argv[4], std::ios::out);
        if (!input_blk) {
            std::cerr << "Cannot open the input file \"" << argv[2]
                 << "\". The program will be terminated..." << std::endl;
            exit(1);
        }
        if (!input_net) {
            std::cerr << "Cannot open the input file \"" << argv[3]
                 << "\". The program will be terminated..." << std::endl;
            exit(1);
        }
        if (!output) {
            std::cerr << "Cannot open the output file \"" << argv[4]
                 << "\". The program will be terminated..." << std::endl;
            exit(1);
        }
    }
    else {
        std::cerr << "Usage: ./Floorplanner <alpha> <input block file> " <<
                "<input net file> <output file>" << std::endl;
        exit(1);
    }

    Floorplanner* fp = new Floorplanner(input_blk, input_net);
    fp->floorplan();
    //fp->checkPlacementInformation();

    return 0;
}

