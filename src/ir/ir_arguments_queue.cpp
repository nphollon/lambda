#include "llvm/IR/IRBuilder.h"

#include "ir_arguments_queue.hpp"

Type* IRArity::getType(LLVMContext& context) {
  return Type::getInt32Ty(context);
}

StructType* IRArgumentsQueue::type = NULL;

Type* IRArgumentsQueue::getPointerType(LLVMContext& context) {
  defineType(context);
  return type->getPointerTo();
}

IRArgumentsQueue::IRArgumentsQueue(Function* malloc, BasicBlock* block) {
  LLVMContext *context = &block->getContext();
  Value* argsAllocSize = getSize(*context);
  IRBuilder<> builder(block);
  Value* mallocResult = builder.CreateCall(malloc, argsAllocSize);
  irStruct = builder.CreateBitCast(mallocResult, getPointerType(*context));
}

IRArgumentsQueue::~IRArgumentsQueue() {}

void IRArgumentsQueue::setLength(int length, BasicBlock* block) {
  IRBuilder<> builder(block);
  Value* argsLengthPtr = getElementPointer(0, block);
  Value* argsLengthValue = ConstantInt::get(getLengthType(), 0);
  builder.CreateStore(argsLengthValue, argsLengthPtr);
}

void IRArgumentsQueue::setHead(IRQueueNode* head, BasicBlock* block) {
  IRBuilder<> builder(block);
  Value* headPtr = getElementPointer(1, block);
  Value* headValue = head->getValue();
  builder.CreateStore(headValue, headPtr);
}

Value* IRArgumentsQueue::getValue() {
  return irStruct;
}

Value* IRArgumentsQueue::getSize(LLVMContext& context) {
  return ConstantExpr::getSizeOf(getType(context));
}

Type* IRArgumentsQueue::getType(LLVMContext& context) {
  describeType(context);
  return type;
}

Type* IRArgumentsQueue::getType() {
  return irStruct->getType()->getPointerElementType();
}

Type* IRArgumentsQueue::getLengthType() {
  return getType()->getStructElementType(0);
}

Value* IRArgumentsQueue::getElementPointer(int i, BasicBlock* block) {
  IRBuilder<> builder(block);
  std::vector<Value*> idxList(2);
  idxList[0] = ConstantInt::get(Type::getInt64Ty(block->getContext()), 0);
  idxList[1] = ConstantInt::get(Type::getInt32Ty(block->getContext()), i);
  return builder.CreateGEP(irStruct, idxList);
}

void IRArgumentsQueue::defineType(LLVMContext& context) {
  if (!type) {
    type = StructType::create(context, "arguments_queue");
  }
}

void IRArgumentsQueue::describeType(LLVMContext& context) {
  defineType(context);
  if (type->isOpaque()) {
    Type* arityType = IRArity::getType(context);
    Type* headType = IRQueueNode::getPointerType(context);
    type->setBody(arityType, headType, NULL);
  }
}
