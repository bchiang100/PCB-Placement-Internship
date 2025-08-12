#ifndef MODULE_H
#define MODULE_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <climits>

class Terminal
{
public:
    // constructor and destructor
    Terminal(const std::string &name, size_t x, size_t y) : _name(name), _x1(x), _y1(y), _x2(x), _y2(y) {}
    ~Terminal() {}

    // basic access methods
    const std::string getName() { return _name; }
    const size_t getX1() { return _x1; }
    const size_t getX2() { return _x2; }
    const size_t getY1() { return _y1; }
    const size_t getY2() { return _y2; }

    // set functions
    void setName(std::string &name) { _name = name; }
    void setPos(size_t x1, size_t y1, size_t x2, size_t y2)
    {
        _x1 = x1;
        _y1 = y1;
        _x2 = x2;
        _y2 = y2;
    }

protected:
    std::string _name; // module name
    size_t _x1;        // min x coordinate of the terminal
    size_t _y1;        // min y coordinate of the terminal
    size_t _x2;        // max x coordinate of the terminal
    size_t _y2;        // max y coordinate of the terminal
};

class Block : public Terminal
{
public:
    // constructor and destructor
    Block(const std::string &name, size_t w, size_t h) : Terminal(name, 0, 0), _w(w), _h(h) {}
    ~Block() {}

    // basic access methods
    const size_t getWidth(bool rotate = false) { return rotate ? _h : _w; }
    const size_t getHeight(bool rotate = false) { return rotate ? _w : _h; }
    const size_t getArea() { return _h * _w; }
    static size_t getMaxX() { return _maxX; }
    static size_t getMaxY() { return _maxY; }

    // set functions
    void setWidth(size_t w) { _w = w; }
    void setHeight(size_t h) { _h = h; }
    static void setMaxX(size_t x) { _maxX = x; }
    static void setMaxY(size_t y) { _maxY = y; }

private:
    size_t _w;           // width of the block
    size_t _h;           // height of the block
    static size_t _maxX; // maximum x coordinate for all blocks
    static size_t _maxY; // maximum y coordinate for all blocks
};

class Net
{
public:
    // constructor and destructor
    Net() {}
    ~Net() {}

    // basic access methods
    const std::vector<Terminal *> getTermList() { return _termList; }

    // modify methods
    void addTerm(Terminal *term) { _termList.push_back(term); }

    // other member functions

    double calcHPWL();

private:
    std::vector<Terminal *> _termList; // list of terminals the net is connected to
};


class PlacementManager
{
public:
    std::vector<Block> _blocks;
    std::vector<Terminal> _terminals;
    std::vector<Net> _nets;
    void addBlock(const std::string &name, size_t w, size_t h)
    {
        _blocks.emplace_back(name, w, h);
    }

    void addTerminal(const std::string &name, size_t x, size_t y)
    {
        _terminals.emplace_back(name, x, y);
    }

    void addNet(const std::vector<Net *> &nets)
    {
        _nets.emplace_back();
    }

    Terminal* findTerminal(const std::string &name)
    {
        for (auto &term : _terminals)
        {
            //std::cout << "comparing term name is " << term.getName() << std::endl;
            if (term.getName() == name)
            {
                
                return &term;
            }
        }

        for (auto &block : _blocks) 
        {
           //std::cout << "comparing block name is " << block.getName() << std::endl;
            if (block.getName() == name)
            {
                
                return &block;
            }
        }
        return nullptr;
    }

    void printInformation() {
        std::cout << "Terminal size is " << _terminals.size() << std::endl;
        std::cout << "Block size is " << _blocks.size() << std::endl;
        for (size_t i = 0; i < _terminals.size(); i++) 
        {
            std::cout << "Terminal " << i + 1 << ": " <<_terminals[i].getName() << std::endl;
        }

        for (size_t i = 0; i < _blocks.size(); i++)
        {
            std::cout << "Block " << i + 1 << ": " << _blocks[i].getName() << std::endl;
        }

        for (size_t i = 0; i < _nets.size(); i ++)
        {
            std::cout << "Netlist " << i + 1 << ": " << std::endl;
            for (size_t j = 0; j < _nets[i].getTermList().size(); j++)
            {
                std::cout << _nets[i].getTermList()[j]->getName() << std::endl;
            }
        }
    }
    const std::vector<Block> &getBlocks() const { return _blocks; }
    const std::vector<Terminal> &getTerminals() const { return _terminals; }
    const std::vector<Net> &getNets() const { return _nets; }
};

#endif // MODULE_H