#include <iostream>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

class BasicBlock {
public:
    BasicBlock() = default;

    void addInstr(const json& instr) {
        instrs.push_back(instr);
    }

    void clear() {
        instrs.clear();
    }

    friend std::ostream& operator<<(std::ostream& os, const BasicBlock& bb);

private:
    std::vector<json> instrs;
};

std::ostream& operator<<(std::ostream& os, const BasicBlock& bb) {
    os << "[";
    bool first = true;
    for (const auto& instr : bb.instrs) {
        if (!first) os << ", ";
        os << instr;
        first = false;
    }
    os << "]";
    return os;
}

std::vector<BasicBlock> form_blocks(const json& func) {
    std::vector<BasicBlock> blocks;
    BasicBlock block;
    for (const auto& instr: func["instrs"]) {
        if (instr.contains("op") && (instr["op"] == "jmp" || instr["op"] == "br")) {
            block.addInstr(instr);
            blocks.push_back(block);
            block.clear();
        }
        else if (instr.contains("label")) {
            blocks.push_back(block);
            block.clear();
            block.addInstr(instr);
        }
        else {
            block.addInstr(instr);
        }
    }
    return blocks;
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
    for (const auto& func : prog["functions"]) {
        std::vector<BasicBlock> blocks = form_blocks(func);
        for (size_t i = 0; i < blocks.size(); ++i) {
            std::cout << "block_" << i << ": " << blocks[i] << std::endl;
        }
    }
    return 0;
}