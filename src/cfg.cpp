#include <map>
#include <unordered_set>
#include <vector>
#include "basic_block.hpp"
#include "form_blocks.cpp"

int tempgen() {
    static int count = 0;
    return ++count;
}

std::map<std::string, BasicBlock> create_block_map(std::vector<BasicBlock> blocks) {
    std::map<std::string, BasicBlock> blockMap;
    for (auto& block : blocks) {
        if (block.first().contains("label")) {
            std::string blockName = block.first()["label"];
            block.removeFirst();
            blockMap[blockName] = block;
        } 
        else {
            blockMap["_BB" + std::to_string(tempgen())] = block;
        }
    }
    return blockMap;
}

std::map<std::string, std::vector<std::string>> get_successors(
    const std::map<std::string, BasicBlock>& blockMap) {
    std::map<std::string, std::vector<std::string>> successors;
    for (const auto& entry : blockMap) {
        std::vector<std::string> succs;
        auto last = entry.second.last();
        if (last.contains("op") && (last["op"] == "jmp" || last["op"] == "br")) {
            for (auto& label : last["labels"]) {
                succs.push_back(label);
            }
        }
        successors[entry.first] = succs;
    }
    return successors;
}

void dce_block(BasicBlock& block) {
    bool changed = true;
    while (changed) {
        changed = false;
        std::unordered_set<std::string> uses;
        for (const auto& instr: block.instrs) {
            if (instr.contains("args")) {
                for (const auto& arg: instr["args"]) {
                    uses.insert(arg);
                }
            }
        }

        for (auto it = block.instrs.begin(); it != block.instrs.end(); ) {
            if ((*it).contains("dest") && (uses.find((*it)["dest"]) == uses.end())) {
                block.instrs.erase(it);
                changed = true;
            }
            else {
                it++;
            }
        }
    }
}

void dce_block_map(std::map<std::string, BasicBlock>& blockMap) {
    for (auto& block: blockMap) {
        dce_block(block.second);
    }
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
        auto blockMap = create_block_map(blocks);
        std::cout << "Original!!!" << std::endl;
        for (const auto& block: blockMap) {
            std::cout << block.first << ": " << std::endl;
            std::cout << block.second << std::endl;
        }
        dce_block_map(blockMap);
        std::cout << "DCE'd!!!" << std::endl;
        for (const auto& block: blockMap) {
            std::cout << block.first << ": " << std::endl;
            std::cout << block.second << std::endl;
        }
        // auto successors = get_successors(blockMap);
        // for (const auto& i : successors) {
        //     std::cout << i.first << ": ";
        //     for (const auto& succ : i.second) {
        //         std::cout << succ << ", ";
        //     }
        //     std::cout << std::endl;
        // }
    }
    return 0;
}