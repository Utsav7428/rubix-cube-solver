#include <fstream>
#include <iostream>
#include <string>

#include "Solver/DFSSolver.h"
#include "Solver/BFSSolver.h"
#include "Solver/IDDFSSolver.h"
#include "Solver/IDAstarSolver.h"
#include "PatternDatabases/CornerDBMaker.h"

using namespace std;

int main(int argc, char *argv[]) {

    string fileName = "Databases/cornerDepth5V1.txt";
    int shuffleCount = 13;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if (arg == "--help") {
            cout << "Usage: rubiks_cube_solver [db_file] [--shuffle N]\n";
            cout << "Example: rubiks_cube_solver Databases/cornerDepth5V1.txt --shuffle 20\n";
            return 0;
        }

        if (arg == "--shuffle") {
            if (i + 1 >= argc) {
                cerr << "Missing value for --shuffle\n";
                return 1;
            }
            shuffleCount = stoi(argv[++i]);
            if (shuffleCount < 0) {
                cerr << "Shuffle count must be non-negative\n";
                return 1;
            }
            continue;
        }

        const string shufflePrefix = "--shuffle=";
        if (arg.rfind(shufflePrefix, 0) == 0) {
            shuffleCount = stoi(arg.substr(shufflePrefix.size()));
            if (shuffleCount < 0) {
                cerr << "Shuffle count must be non-negative\n";
                return 1;
            }
            continue;
        }

        if (arg.rfind("--", 0) == 0) {
            cerr << "Unknown option: " << arg << "\n";
            return 1;
        }

        fileName = arg;
    }

    ifstream dbFile(fileName, ios::binary);
    if (!dbFile.is_open()) {
        cerr << "Pattern database file not found: " << fileName << "\n";
        cerr << "Pass file path as first argument, for example: "
             << "rubiks_cube_solver Databases/cornerDepth5V1.txt\n";
        return 1;
    }

    RubiksCubeBitboard cube;
    auto shuffleMoves = cube.randomShuffleCube(static_cast<unsigned int>(shuffleCount));
    cube.print();
    for (auto move: shuffleMoves) cout << cube.getMove(move) << " ";
    cout << "\n";

    IDAstarSolver<RubiksCubeBitboard, HashBitboard> idaStarSolver(cube, fileName);
    auto moves = idaStarSolver.solve();

    idaStarSolver.rubiksCube.print();
    for (auto move: moves) cout << cube.getMove(move) << " ";
    cout << "\n";


    return 0;
}
