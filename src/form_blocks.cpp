#include <iostream>
#include <fstream>
#include "basic_block.hpp"
#include "json.hpp"

using json = nlohmann::json;

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
            if (block.numInstrs() > 0) {
                blocks.push_back(block);
                block.clear();
            }
            block.addInstr(instr);
        }
        else {
            block.addInstr(instr);
        }
    }

    if (block.numInstrs() > 0) {
        blocks.push_back(block);
    }

    return blocks;
}