#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

class Emitter {
public:
   Emitter(std::ostream &out_stream, std::ostream &err_stream);
   Emitter(); // буфферизированный эмиттер для генерации в строку

   // эмиттер для операндов, операторов, инструкций и меток
   void emitOperand(const std::string &s);
   void emitOperator(const std::string &op);
   void emitStore(const std::string &ident);
   void emitInstr(const std::string &instr, const std::string &arg = "");
   void emitLabel(const std::string &label);

   // заголовки и идентификаторы для switch-case
   int nextSwitchId();
   std::string newLabel(const std::string &base);

   void pushSwitchEnd(const std::string &label);
   void popSwitchEnd();
   std::string currentSwitchEnd() const;

   // для буфферизированного эмиттера
   std::string getBufferContent() const;
   void setInitialSwitchStack(const std::vector<std::string> &stack);

private:
   std::ostream *out = nullptr;
   std::ostream *err = nullptr;
   std::ostringstream buffer;
   bool useBuffer = false;

   int switchCounter = 0;
   std::vector<std::string> switchEndStack;
};
