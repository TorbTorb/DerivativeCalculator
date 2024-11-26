#include <string>

#ifndef NODES_H
#define NODES_H

//abstract class for all Nodes
class Node {
    public:
        Node(){};
        virtual ~Node() {};
        virtual Node* derive() = 0;
        virtual std::string out() = 0;
};

class EmptyNode: public Node{
    public:
        EmptyNode() {};
        ~EmptyNode();
        std::string out() override;
        Node* derive() override;
};

class ConstantNode: public Node {
    protected:
        std::string* Value;
    public:
        ConstantNode(std::string* Value): Value(Value) {};
        ConstantNode(const char* Value): Value(new std::string(Value)) {};
        ~ConstantNode();
        std::string out() override;
        Node* derive() override;
};

class VariableNode: public Node {
    protected:
        std::string* Name;
    public:
        VariableNode(std::string* Name): Name(Name) {};
        VariableNode(const char* Name): Name(new std::string(Name)) {};
        ~VariableNode();
        std::string out() override;

        Node* derive() override;
}; 

//TODO actually make this abstract
//abstract base class for unary operatioons (1 argument)
class UnaryNode: public Node {
    protected:
        Node* Arg;
    public:
        UnaryNode(Node* Arg): Arg(Arg) {};
        ~UnaryNode();
};

//all unary nodes
class LnNode: public UnaryNode {
    public:
        LnNode(Node* Arg):
            UnaryNode(Arg) {};
        
        std::string out() override;
        Node* derive() override;
};

class ExpNode: public UnaryNode {
    public:
        ExpNode(Node* Arg):
            UnaryNode(Arg) {};
        
        std::string out() override;
        Node* derive() override;
};

class NegationNode: public UnaryNode {
    public:
        NegationNode(Node* Arg):
            UnaryNode(Arg) {};
        
        std::string out() override;
        Node* derive() override;
};

class SqrtNode: public UnaryNode {
    public:
        SqrtNode(Node* Arg):
            UnaryNode(Arg) {};
        
        std::string out() override;
        Node* derive() override;
};

class SinNode: public UnaryNode {
    public:
        SinNode(Node* Arg):
            UnaryNode(Arg) {};
        
        std::string out() override;
        Node* derive() override;
};

class CosNode: public UnaryNode {
    public:
        CosNode(Node* Arg):
            UnaryNode(Arg) {};
        
        std::string out() override;
        Node* derive() override;
};

class TanNode: public UnaryNode {
    public:
        TanNode(Node* Arg):
            UnaryNode(Arg) {};
        
        std::string out() override;
        Node* derive() override;
};
//TODO actually make this abstract
//abstract base class for binary operations (2 arguments)
class BinaryNode: public Node {
    protected:
        Node* Left;
        Node* Right;
    public:
        BinaryNode(Node* Left, Node* Right): Left(Left), Right(Right) {};
        ~BinaryNode();
};

class AdditionNode: public BinaryNode {
    public:
        AdditionNode(Node* Left, Node* Right):
            BinaryNode(Left, Right){};
        
        std::string out() override;
        Node* derive() override;
};

class SubtractionNode: public BinaryNode {
    public:
        SubtractionNode(Node* Left, Node* Right):
            BinaryNode(Left, Right){};
        
        std::string out() override;
        Node* derive() override;
};

class MultiplicationNode: public BinaryNode {
    public:
        MultiplicationNode(Node* Left, Node* Right):
            BinaryNode(Left, Right){};
        
        std::string out() override;
        Node* derive() override;
};

class DivisionNode: public BinaryNode {
    public:
        DivisionNode(Node* Left, Node* Right):
            BinaryNode(Left, Right){};
        
        std::string out() override;
        Node* derive() override;
};

class LogarithmNode: public BinaryNode {    //log(u, v) -> log of u to base v
    public:
        LogarithmNode(Node* Left, Node* Right):
            BinaryNode(Left, Right){};
        
        std::string out() override;
        Node* derive() override;
};

class ExponentiationNode: public BinaryNode {
    public:
        ExponentiationNode(Node* Left, Node* Right):
            BinaryNode(Left, Right){};
        
        std::string out() override;
        Node* derive() override;
};

#endif