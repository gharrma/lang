#pragma once
#include <iostream>
#include <memory>
#include <vector>

// TODO: Unique all types, create a type table,
//       use pointer equality, and swith to unique_ptr in AST.

struct Type {
    virtual ~Type() {}
    virtual void Print(std::ostream& os) const = 0;
    virtual bool Equals(const Type* o) const = 0;
};

std::ostream& operator<<(std::ostream& os, const Type& t);

struct UnitType : Type {
    void Print(std::ostream& os) const override;
    bool Equals(const Type* o) const override;
};

// i8, i16, i32, i64.
struct IntType : Type {
    uint8_t bits;
    bool signd;
    IntType(uint8_t bits, bool signd): bits(bits), signd(signd) {}
    void Print(std::ostream& os) const override;
    bool Equals(const Type* o) const override;
};

// f32, f64.
struct FloatType : Type {
    uint8_t bits;
    FloatType(uint8_t bits): bits(bits) {}
    void Print(std::ostream& os) const override;
    bool Equals(const Type* o) const override;
};

struct FnType : Type {
    std::vector<std::shared_ptr<Type>> arg_types;
    std::shared_ptr<Type> ret_type;
    FnType(std::vector<std::shared_ptr<Type>> arg_types,
           std::shared_ptr<Type> ret_type)
        : arg_types(arg_types)
        , ret_type(ret_type) {}
    void Print(std::ostream& os) const override;
    bool Equals(const Type* o) const override;
};
