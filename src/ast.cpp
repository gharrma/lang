#include "ast.h"

#include <iostream>
#include <sstream>
#include "error.h"
#include "util.h"
#include "visit.h"

// Pretty-prints abstract syntax trees with reasonable indentation.
class PrettyPrinter {
public:
    static constexpr int32_t kTabWidth = 4;

    PrettyPrinter(std::ostream& os): os_(os) {}

    void BeginIndent() { indent_ += kTabWidth; }
    void EndIndent()   { indent_ -= kTabWidth; }

    void NewLine() {
        os_ << '\n';
        needs_indent_ = true;
    }

    template <typename T>
    PrettyPrinter& operator<<(const T& t) {
        if (needs_indent_) {
            for (int32_t i = 0; i < indent_; ++i)
                os_ << ' ';
            needs_indent_ = false;
        }
        os_ << t;
        return *this;
    }

private:
    int32_t indent_ = 0;
    bool needs_indent_ = true;
    std::ostream& os_;
};

std::ostream& operator<<(std::ostream& os, const Node& node) {
    PrettyPrinter pp(os);
    node.Print(pp);
    return os;
}

void Block::Print(PrettyPrinter& pp) const {
    pp << '{';
    pp.NewLine();
    pp.BeginIndent();
    for (auto& expr : exprs) {
        expr->Print(pp);
        pp.NewLine();
    }
    pp.EndIndent();
    pp << "}";
}

void Id::Print(PrettyPrinter& pp) const { pp << name; }

void Binary::Print(PrettyPrinter& pp) const {
    pp << '(' << *lhs << ' ' << op << ' ' << *rhs << ')';
}

void ParsedType::Print(PrettyPrinter& pp) const {
    // Unnamed parsed type probably means implicit Unit.
    if (name) pp << name;
}

void VarDecl::Print(PrettyPrinter& pp) const {
    pp << name << ' ' << *parsed_type;
}

void IntLit::Print(PrettyPrinter& pp) const { pp << val; }

void FloatLit::Print(PrettyPrinter& pp) const { pp << val; }

void FnProto::Print(PrettyPrinter& pp) const {
    if (!args.empty()) {
        pp << '(';
        for (auto it = args.begin(), e = args.end(); it != e; ++it) {
            if (it != args.begin())
                pp << ", ";
            pp << **it;
        }
        pp << ')';
    }
    std::ostringstream ss;
    ss << *ret_type;
    if (!ss.str().empty()) {
        pp << ' ' << ss.str();
    }
}

void FnDecl::Print(PrettyPrinter& pp) const {
    pp << kFn << ' ' << name << *proto << " = " << *body;
}
