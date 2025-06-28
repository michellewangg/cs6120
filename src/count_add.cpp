#include <iostream>
#include <fstream>
#include <cstddef>
#include "json.hpp"

using json = nlohmann::json;

size_t count_add (json& prog) {
    auto numAdds = 0;
    for (const auto& func : prog["functions"]) {
        for (const auto& instr : func["instrs"]) {
            if (instr["op"] == "add") {
                ++numAdds;
            }
        }
    }
    return numAdds;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << "<filename.json>\n";
        return 1;
    }
    std::string filename = argv[1];
    std::ifstream file(filename);
    json prog;
    file >> prog;
    std::cout << "Count add: " << count_add(prog) << std::endl;
    return 0;
}