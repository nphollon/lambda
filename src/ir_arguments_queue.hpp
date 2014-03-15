#ifndef H_IR_ARGUMENTS_QUEUE
#define H_IR_ARGUMENTS_QUEUE

#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"

#include "ir_queue_node.hpp"

using namespace llvm;

class IRQueueNode;

class IRArity {
public:
  static Type* getType(LLVMContext&);
};

class IRArgumentsQueue {
public:
  static Type* getPointerType(LLVMContext&);

  IRArgumentsQueue(Function*, BasicBlock*);
  ~IRArgumentsQueue();

  void setLength(int, BasicBlock*);
  void setHead(IRQueueNode*, BasicBlock*);
  Value* getValue();

private:
  static StructType* type;
  Value* irStruct;

  static Value* getSize(LLVMContext&);
  static Type* getType(LLVMContext&);

  Type* getType();
  Type* getLengthType();
  Value* getElementPointer(int, BasicBlock*);

  static void defineType(LLVMContext&);
  static void describeType(LLVMContext&);
};

#endif
