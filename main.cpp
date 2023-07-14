// Source: https://github.com/lowbyteproductions/JavaScript-Is-Weird

#include <fstream>
#include <ios>
#include <iostream>

#include "transpiler.hpp"

auto main() -> int
{
    using std::cin;
    using std::cout;
    using std::ifstream;
    using std::string;

    string fileName;

    cout << "Enter filename to transpile: ";
    std::getline(cin, fileName, '\n');

    string targetFilename = "bs_" + fileName;

    string   code;

    {
        ifstream ifs(fileName);
        string   line;

        while (!ifs.eof())
        {
            std::getline(ifs, line, '\n');
            code += line + '\n';
        }
    }

    Transpiler tplr(std::move(code));
    code = tplr.Transpile();

    cout << "======== START ========\n" << code << "\n ======== END ========\n";

    {
        std::ofstream ofs("BS_" + fileName, std::ios_base::out & std::ios_base::trunc);
        ofs << code;
    }

    return 0;
}
