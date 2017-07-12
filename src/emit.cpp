#include "emit.h"

#include <unordered_map>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "ast.h"
#include "type.h"
#include "util.h"
#include "visit.h"

using namespace llvm;

struct Emitter : Visitor {
    Module& mod;
    LLVMContext& context;
    IRBuilder<> builder;
    std::unordered_map<VarDecl*, Value*> vars;
    std::unordered_map<Expr*, Value*> vals;

    Emitter(Module& mod)
        : mod(mod)
        , context(mod.getContext())
        , builder(context) {}

    llvm::Type* GetLlvmType(const ::Type* type);

    void AfterId(Id& id) override;
    void AfterBinary(Binary& binary) override;
    void AfterIntLit(IntLit& int_lit) override;
    void AfterFloatLit(FloatLit& float_lit) override;
    void AfterVarDecl(VarDecl& decl) override;
    void BeforeFnDecl(FnDecl& fn) override;
    void AfterFnDecl(FnDecl& fn) override;
};

llvm::Type* Emitter::GetLlvmType(const ::Type* type) {
    if (auto cast = dynamic_cast<const IntType*>(type)) {
        return llvm::Type::getIntNTy(context, cast->bits);
    }
    else if (auto cast = dynamic_cast<const IntType*>(type)) {
        if (cast->bits == 32)
            return llvm::Type::getFloatTy(context);
        else if (cast->bits == 64)
            return llvm::Type::getDoubleTy(context);
        else LOG(FATAL) << "Invalid width for float type: " << cast->bits;
    }
    else LOG(FATAL) << "Unhandled type " << *type;
    return nullptr;
}

void Emit(Node& ast, Module& mod) {
    Emitter emitter(mod);
    ast.Accept(emitter);
}

Value* EmitExpr(Expr& expr, Module& mod) {
    Emitter emitter(mod);
    expr.Accept(emitter);
    return emitter.vals.at(&expr);
}

void Emitter::AfterId(Id& id) {
    vals[&id] = vars[id.resolved];
}

void Emitter::AfterBinary(Binary& binary) {
    auto l = vals.at(binary.lhs.get());
    auto r = vals.at(binary.rhs.get());
    auto op = binary.op;
    auto op_kind = op.kind;
    Value* val = nullptr;
    if (auto int_type = dynamic_cast<IntType*>(binary.type.get())) {
             if (op_kind == kPlus)  val = builder.CreateAdd(l, r, "plus");
        else if (op_kind == kMinus) val = builder.CreateSub(l, r, "minus");
        else if (op_kind == kTimes) val = builder.CreateMul(l, r, "times");
        else if (op_kind == kDiv || op_kind == kMod) {
            if (op_kind == kDiv) {
                int_type->signd
                    ? val = builder.CreateSDiv(l, r, "div")
                    : val = builder.CreateUDiv(l, r, "div");
            } else {
                int_type->signd
                    ? val = builder.CreateSRem(l, r, "mod")
                    : val = builder.CreateURem(l, r, "mod");
            }
        }
    }
    else if (dynamic_cast<FloatType*>(binary.type.get())) {
             if (op_kind == kPlus)  val = builder.CreateFAdd(l, r, "plus");
        else if (op_kind == kMinus) val = builder.CreateFSub(l, r, "minus");
        else if (op_kind == kTimes) val = builder.CreateFMul(l, r, "times");
        else if (op_kind == kDiv)   val = builder.CreateFDiv(l, r, "div");
    }
    if (!val) LOG(FATAL) << "Binary operator \'" << op << "\' not translated";
    vals[&binary] = val;
}

void Emitter::AfterIntLit(IntLit& int_lit) {
    static constexpr unsigned bits = CHAR_BIT * sizeof(IntLitRep);
    vals[&int_lit] = ConstantInt::get(context, APInt(bits, int_lit.val));
}

void Emitter::AfterFloatLit(FloatLit& float_lit) {
    vals[&float_lit] = ConstantFP::get(context, APFloat(float_lit.val));
}

void Emitter::AfterVarDecl(VarDecl& decl) {
    // TODO
}

void Emitter::BeforeFnDecl(FnDecl& fn) {
    std::vector<llvm::Type*> arg_types(fn.proto->args.size());
    for (size_t i = 0, e = arg_types.size(); i < e; ++i)
        arg_types[i] = GetLlvmType(fn.proto->args[i]->parsed_type->type.get());
    auto ret_type = GetLlvmType(fn.proto->ret_type->type.get());
    auto fn_type = FunctionType::get(ret_type, arg_types, /*vararg*/ false);

    auto fn_ir = Function::Create(
        fn_type, Function::ExternalLinkage, // TODO: Linkage?
        fn.name.str_val, &mod);
    size_t idx = 0;
    for (auto& arg_ir : fn_ir->args()) {
        arg_ir.setName(fn.proto->args[idx]->name.str_val);
        vars[fn.proto->args[idx].get()] = &arg_ir;
        ++idx;
    }

    auto bb = BasicBlock::Create(context, "entry", fn_ir);
    builder.SetInsertPoint(bb);
}

void Emitter::AfterFnDecl(FnDecl& fn) {
    builder.CreateRet(vals.at(fn.body.get()));
    auto fn_ir = mod.getFunction(fn.name.str_val);
    if (verifyFunction(*fn_ir, &llvm::errs())) {
        LOG(FATAL) << "LLVM function verification failed.";
    }
}
