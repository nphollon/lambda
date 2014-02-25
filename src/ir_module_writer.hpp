#ifndef H_IR_MODULE_WRITER
#define H_IR_MODULE_WRITER

#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"

#include "module_writer.hpp"

using namespace llvm;

class IRModuleWriter : public ModuleWriter {
public:
  static ModuleWriter* createModuleWriter(Module*, LLVMContext*);
  
  SlalomFunction* createICombinator();
  SlalomFunction* createKCombinator();
  SlalomFunction* createSCombinator();
  SlalomFunction* createDerivedCombinator(const std::string&);
  SlalomFunction* createApplication(SlalomFunction*, SlalomFunction*);
private:
  Module *module;
  LLVMContext *context;
  IRBuilder<> builder;

  IRModuleWriter(Module*, LLVMContext*);
  
  void generateFramework();
  Value* createSlalomFunctionStruct(Type*);
};

#endif
