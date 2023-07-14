// Source: https://github.com/lowbyteproductions/JavaScript-Is-Weird

#include <fstream>
#include <ios>
#include <iostream>

#include "transpiler.hpp"

auto main(int argc, char** argv) -> int
{
    using std::cin;
    using std::cout;
    using std::ifstream;
    using std::string;

    if (argc != 2)
    {
        cout << "Please provide the file you want to transpile as the only argument.\n";
        return -1;
    }
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic): No choice here.
    string fileName(argv[1]);
    string targetFilename = "bs_" + fileName;
    string code;

    {
        ifstream ifs(fileName);
        if (ifs.fail())
        {
            cout << "Failed to open \"" << fileName << "\" for reading.\n";
            return -1;
        }
        string line;

        while (!ifs.eof())
        {
            std::getline(ifs, line, '\n');
            code += line + '\n';
        }
    }
    {
        Transpiler transpiler(std::move(code));
        code = transpiler.Transpile();
    }
    // cout << "======== START ========\n" << code << "\n ======== END ========\n";

    {
        std::string   outFileName = "BS_" + fileName;
        std::ofstream ofs(outFileName, std::ios_base::out & std::ios_base::trunc);
        if (ofs.fail())
        {
            cout << "Failed to open \"" << outFileName << "\" for writing.\n";
            return -1;
        }
        ofs << code;
    }

    return 0;
}
