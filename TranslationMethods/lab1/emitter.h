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
   void emitOperand(const std::string& s); // для идентификаторов, констант
   void emitOperator(const std::string& op); // для операторов, таких как '==', '+', '*', и т.д.
   void emitStore(const std::string &ident); // для сохранения значения в переменную
   void emitInstr(const std::string &instr, const std::string &arg = ""); // для генерации инструкций
   void emitLabel(const std::string &label); // для генерации меток

   // заголовки и идентификаторы для switch-case
   int nextSwitchId();// для генерации уникальных идентификаторов switch
   std::string newLabel(const std::string& base);// для генерации уникальных меток на основе базовой строки

   void pushSwitchEnd(const std::string& label);// для управления стеком меток конца switch, чтобы поддерживать вложенные switch и корректную работу break
   void popSwitchEnd();// для удаления верхней метки конца switch из стека
   std::string currentSwitchEnd() const;// для получения текущей метки конца switch

   // для буфферизированного эмиттера
   std::string getBufferContent() const; // для получения сгенерированного кода из буфера
   void setInitialSwitchStack(const std::vector<std::string>& stack); // для установки начального стека меток конца switch (используется при переключении между эмиттерами)

private:
   std::ostream *out = nullptr;
   std::ostream *err = nullptr;
   std::ostringstream buffer;
   bool useBuffer = false;

   int switchCounter = 0;
   std::vector<std::string> switchEndStack;
};
