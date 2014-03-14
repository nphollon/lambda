#include "ir_slalom_function.hpp"

#include "llvm/IR/IRBuilder.h"

// Public static methods

IRSlalomFunction* IRSlalomFunction::getNull(LLVMContext& context) {
  return new IRSlalomFunction(context);
}

PointerType* IRSlalomFunction::getPointerType(LLVMContext& context) {
  return getType(context)->getPointerTo();
}

IRSlalomFunction::IRSlalomFunction(Function* malloc, BasicBlock* block) {
  LLVMContext *context = &block->getContext();
  Value* allocSize = getSize(*context);
  IRBuilder<> builder(block);
  Value* mallocResult = builder.CreateCall(malloc, allocSize);
  irStruct = builder.CreateBitCast(mallocResult, getPointerType(*context));
}

IRSlalomFunction::~IRSlalomFunction() {}

// Public methods

Value* IRSlalomFunction::getValue() {
  return irStruct;
}

void IRSlalomFunction::setArity(int arity, BasicBlock* block) {
  Value* arityValue = ConstantInt::get(getArityType(), arity);
  IRBuilder<> builder(block);
  builder.CreateStore(arityValue, getArityPointer(block));
}

void IRSlalomFunction::setName(const std::string& name, BasicBlock* block) {
  IRBuilder<> builder(block);
  Value* nameValue = builder.CreateGlobalStringPtr(name);
  builder.CreateStore(nameValue, getNamePointer(block));
}

void IRSlalomFunction::setArguments(IRArgumentsQueue* args, BasicBlock* block) {
  Value* argsValue = args->getValue();
  Value* argsPtr = getElementPointer(2, block);
  IRBuilder<> builder(block);
  builder.CreateStore(argsValue, argsPtr);
}

void IRSlalomFunction::setReturn(BasicBlock* block) {
  IRBuilder<> builder(block);
  builder.CreateRet(irStruct);
}

// Private static methods

Value* IRSlalomFunction::getSize(LLVMContext& context) {
  return ConstantExpr::getSizeOf(getType(context));
}

Type* IRSlalomFunction::getType(LLVMContext& context) {
  Type* arityTy = IRArity::getType(context);
  Type* nameTy = Type::getInt8PtrTy(context);
  Type* argsType = IRArgumentsQueue::getPointerType(context);

  return StructType::get(arityTy, nameTy, argsType, NULL);
}

IRSlalomFunction::IRSlalomFunction(LLVMContext& context) {
  irStruct = ConstantPointerNull::get(getPointerType(context));
}

// Private methods

Type* IRSlalomFunction::getType() {
  return getPointerType()->getPointerElementType();
}

Type* IRSlalomFunction::getPointerType() {
  return irStruct->getType();
}

Type* IRSlalomFunction::getArityType() {
  return getType()->getStructElementType(0);
}

Type* IRSlalomFunction::getNameType() {
  return getType()->getStructElementType(1);
}

Value* IRSlalomFunction::getArityPointer(BasicBlock* block) {
  return getElementPointer(0, block);
}

Value* IRSlalomFunction::getNamePointer(BasicBlock* block) {
  return getElementPointer(1, block);
}

Value* IRSlalomFunction::getElementPointer(int i, BasicBlock* block) {
  IRBuilder<> builder(block);
  std::vector<Value*> idxList(2);
  idxList[0] = ConstantInt::get(Type::getInt64Ty(block->getContext()), 0);
  idxList[1] = ConstantInt::get(Type::getInt32Ty(block->getContext()), i);
  return builder.CreateGEP(irStruct, idxList);
}
