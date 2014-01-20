define void @setBody(%Function* %f, %Body %newBody) {
  %body_p = call %Body* @getBodyPointer(%Function* %f)
  store %Body %newBody, %Body* %body_p
  ret void
}

define %Index @getArity(%Function* %f) {
  %arity_p = call %Index* @getArityPointer(%Function* %f)
  %arity = load %Index* %arity_p
  ret %Index %arity
}

define void @setArity(%Function* %f, %Index %newArity) {
  %arity_p = call %Index* @getArityPointer(%Function* %f)
  store %Index 1, %Index* %arity_p
  ret void
}

define %Queue* @getArguments(%Function* %f) {
  %args_p = call %Queue** @getArgumentsPointer(%Function* %f)
  %args = load %Queue** %args_p
  ret %Queue* %args
}

define void @initializeArguments(%Function* %f) {
  %args_p = call %Queue** @getArgumentsPointer(%Function* %f)
  %args = call %Queue* @createEmptyQueue()
  store %Queue* %args, %Queue** %args_p
  ret void
}

define %Function* @createICombinator() {
  %f_size = load i32* @.FUNCTION_SIZE
  %f_i8 = tail call noalias i8* @malloc(i32 %f_size) nounwind
  %f = bitcast i8* %f_i8 to %Function*

  call void @setBody(%Function* %f, %Body @dequeue)
  call void @setArity(%Function* %f, %Index 1)
  call void @initializeArguments(%Function* %f)

  ret %Function* %f
}

define void @fDestroy(%Function* %f) {
  ; Free arguments queue
  %args = call %Queue* @getArguments(%Function* %f)
  call void @qDestroy(%Queue* %args)
  ; Free function
  %fCast = bitcast %Function* %f to i8*
  call void @free(i8* %fCast) nounwind
  ret void
}