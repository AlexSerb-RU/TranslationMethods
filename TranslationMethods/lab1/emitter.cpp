#include "emitter.h"
#include <iostream>

Emitter::Emitter(std::ostream &out_stream, std::ostream &err_stream) {
    out = &out_stream;
    err = &err_stream;
    useBuffer = false;
}

Emitter::Emitter() {
    out = nullptr;
    err = nullptr;
    useBuffer = true;
}

void Emitter::emitOperand(const std::string &s) {
    if (useBuffer) buffer << s << ' ';
    else if (out) (*out) << s << ' ';
}

void Emitter::emitOperator(const std::string &op) {
    if (useBuffer) buffer << op << '\n';
    else if (out) (*out) << op << '\n';
}

void Emitter::emitStore(const std::string &ident) {
	// репрезентация для хранения значения в идентификаторе: операнд идентификатора, оператор '='
    emitOperand(ident);
    emitOperator("=");
}

void Emitter::emitInstr(const std::string &instr, const std::string &arg) {
    if (useBuffer) {
        buffer << instr;
        if (!arg.empty()) buffer << ' ' << arg;
        buffer << '\n';
    } else if (out) {
        (*out) << instr;
        if (!arg.empty()) (*out) << ' ' << arg;
        (*out) << '\n';
    }
}

void Emitter::emitLabel(const std::string &label) {
    if (useBuffer) buffer << label << '\n';
    else if (out) (*out) << label << '\n';
}

int Emitter::nextSwitchId() {
    return ++switchCounter;
}

std::string Emitter::newLabel(const std::string &base) {
    // base may contain placeholders; append unique number
    int id = ++switchCounter;
    return base + std::to_string(id);
}

void Emitter::pushSwitchEnd(const std::string &label) {
    switchEndStack.push_back(label);
}

void Emitter::popSwitchEnd() {
    if (!switchEndStack.empty()) switchEndStack.pop_back();
}

std::string Emitter::currentSwitchEnd() const {
    if (switchEndStack.empty()) return std::string();
    return switchEndStack.back();
}

std::string Emitter::getBufferContent() const {
    return buffer.str();
}

void Emitter::setInitialSwitchStack(const std::vector<std::string> &stack) {
    switchEndStack = stack;
}
