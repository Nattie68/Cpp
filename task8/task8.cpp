#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

class Expression {
public:
    virtual ~Expression() = default;
    virtual int calculate(const std::map<std::string, int>& context) const = 0;
    virtual void print(std::ostream& os) const = 0;
    friend std::ostream& operator<<(std::ostream& os, const Expression& expr) {
        expr.print(os);
        return os;
    }
};

class Constant : public Expression {
    int value;
    Constant(int v) : value(v) {}
public:
    static Constant* create(int v) { return new Constant(v); }
    int calculate(const std::map<std::string, int>&) const override { return value; }
    void print(std::ostream& os) const override { os << value; }
};

class Variable : public Expression {
    std::string name;
    Variable(const std::string& n) : name(n) {}
public:
    static Variable* create(const std::string& n) { return new Variable(n); }
    int calculate(const std::map<std::string, int>& context) const override {
        auto it = context.find(name);
        if (it != context.end()) return it->second;
        throw std::runtime_error("Variable not found: " + name);
    }
    void print(std::ostream& os) const override { os << name; }
};

class Addition : public Expression {
    Expression* left;
    Expression* right;
public:
    Addition(Expression* l, Expression* r) : left(l), right(r) {}
    
    int calculate(const std::map<std::string, int>& context) const override {
        return left->calculate(context) + right->calculate(context);
    }
    
    void print(std::ostream& os) const override {
        os << "(" << *left << " + " << *right << ")";
    }
    
    ~Addition() {}
};

class ExpressionFactory {
    std::unordered_map<int, Constant*> constants;
    std::unordered_map<std::string, Variable*> variables;
    
public:
    ExpressionFactory() {
        for (int i = -5; i <= 256; ++i) {
            constants[i] = Constant::create(i);
        }
    }
    
    Constant* createConstant(int value) {
        auto it = constants.find(value);
        if (it != constants.end()) return it->second;
        
        Constant* newConst = Constant::create(value);
        constants[value] = newConst;
        return newConst;
    }
    
    Variable* createVariable(const std::string& name) {
        auto it = variables.find(name);
        if (it != variables.end()) return it->second;
        
        Variable* newVar = Variable::create(name);
        variables[name] = newVar;
        return newVar;
    }
    
    void releaseConstant(int value) {
        if (value >= -5 && value <= 256) return;
        
        auto it = constants.find(value);
        if (it != constants.end()) {
            delete it->second;
            constants.erase(it);
        }
    }
    
    void releaseVariable(const std::string& name) {
        auto it = variables.find(name);
        if (it != variables.end()) {
            delete it->second;
            variables.erase(it);
        }
    }
    
    ~ExpressionFactory() {
        for (auto& pair : constants) {
            delete pair.second;
        }
        for (auto& pair : variables) {
            delete pair.second;
        }
    }
};

int main() {
    try {
        ExpressionFactory factory;
        
        Constant* c = factory.createConstant(2);
        Variable* v = factory.createVariable("x");
        Addition* expression = new Addition(c, v);
        
        std::map<std::string, int> context;
        context["x"] = 3;
        
        std::cout << "Expression: " << *expression << std::endl;
        std::cout << "x: " << context["x"] << std::endl;
        std::cout << "Result: " << expression->calculate(context) << std::endl;
        
        delete expression;        
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}