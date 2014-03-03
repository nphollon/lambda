#ifndef H_IR_MODULE_WRITER
#define H_IR_MODULE_WRITER

#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"

#include "module_writer.hpp"

using namespace llvm;

class IRSlalomFunction : public SlalomFunction {
public:
  static Type* getType(LLVMContext&);
  static Type* getPointerType(LLVMContext&);

  IRSlalomFunction(Function*, BasicBlock*);
  ~IRSlalomFunction();

  void setArity(int, BasicBlock*);
  void setReturn(BasicBlock*);
private:
  Value* irStruct;

  Type* getType();
  Value* getArityPointer(BasicBlock*);
};



class IRModuleWriter : public ModuleWriter {
public:
  static ModuleWriter* createModuleWriter(Module*);
  
  SlalomFunction* createICombinator();
  SlalomFunction* createKCombinator();
  SlalomFunction* createSCombinator();
  SlalomFunction* createDerivedCombinator(const std::string&);
  SlalomFunction* createApplication(SlalomFunction*, SlalomFunction*);
private:
  Module *module;

  IRModuleWriter(Module*);
  
  Function* malloc;

  void generateFramework();
  void declareMalloc();
  Value* newSlalomFunctionStruct();
};

#endif
