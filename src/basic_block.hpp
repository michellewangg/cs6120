#pragma once

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

    size_t numInstrs() {
        return instrs.size();
    }

    json first() const {
        return instrs[0];
    }

    json last() const {
        return instrs[instrs.size() - 1];
    }

    void removeFirst() {
        instrs.erase(instrs.begin());
    }

    friend std::ostream& operator<<(std::ostream& os, const BasicBlock& bb);

    // TODO: Make private
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