#pragma once
#include <iostream>
#include <memory>

// TODO: Unique all types, create a type table,
//       use pointer equality, and swith to unique_ptr in AST.

struct Type {
    virtual ~Type() {}
    virtual void Print(std::ostream& os) const = 0;
    virtual bool Equals(const Type* o) const = 0;
};

std::ostream& operator<<(std::ostream& os, const Type* t);

// i8, i16, i32, i64.
struct IntType : Type {
    unsigned bits;
    bool signd;
    IntType(unsigned bits, bool signd): bits(bits), signd(signd) {}
    void Print(std::ostream& os) const override;
    bool Equals(const Type* o) const override;
};

// f32, f64.
struct FloatType : Type {
    unsigned bits;
    FloatType(unsigned bits): bits(bits) {}
    void Print(std::ostream& os) const override;
    bool Equals(const Type* o) const override;
};
