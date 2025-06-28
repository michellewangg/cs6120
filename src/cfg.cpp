#include <map>
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
        auto successors = get_successors(blockMap);
        for (const auto& i : successors) {
            std::cout << i.first << ": ";
            for (const auto& succ : i.second) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
        }
    }
    return 0;
}