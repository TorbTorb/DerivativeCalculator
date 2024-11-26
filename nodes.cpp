#include <string>
#include "nodes.h"

//EmptyNode

EmptyNode::~EmptyNode() {}
std::string EmptyNode::out() {
    return "";
}
Node* EmptyNode::derive() {
    return this;
}



//Constant Node
ConstantNode::~ConstantNode() {
    delete(Value);
}
std::string ConstantNode::out() {return *Value;};
Node* ConstantNode::derive() {
    return new ConstantNode("0");
}


//Variable Node
VariableNode::~VariableNode() {
    delete(Name);
}

std::string VariableNode::out() {
    return *Name;
}

Node* VariableNode::derive() {
    if (*Name == "x") {
        return new ConstantNode("1");
    }
    return new ConstantNode("0");
}


//Unary Nodes
UnaryNode::~UnaryNode() {
    delete(Arg);
}

//LnNode
std::string LnNode::out() {
    return "ln(" + Arg->out() + ")";
}

Node* LnNode::derive() {
    return new DivisionNode(Arg->derive(), Arg);
}


//ExpNode
std::string ExpNode::out() {
    return "exp(" + Arg->out() + ")";
}

Node* ExpNode::derive() {   //(e^u)' = u' * e^u
    return new MultiplicationNode(Arg->derive(), this);
}


//NegationNode
std::string NegationNode::out() {
    return "-(" + Arg->out() + ")";
}

Node* NegationNode::derive() {
    return new NegationNode(Arg->derive());
}


//SqrtNode
std::string SqrtNode::out() {
    return "sqrt(" + Arg->out() + ")";
}

Node* SqrtNode::derive() {   // -> f'/2*sqrt(f)
    return new DivisionNode(
        Arg->derive(),
        new MultiplicationNode(
            new ConstantNode("2"),
            this)
        );
}


//SinNode
std::string SinNode::out() {
    return "sin(" + Arg->out() + ")";
}

Node* SinNode::derive() {
    return new MultiplicationNode(
        Arg->derive(),
        new CosNode(Arg)
    );
}


//CosNode
std::string CosNode::out() {
    return "cos(" + Arg->out() + ")";
}

Node* CosNode::derive() {
    return new MultiplicationNode(
        Arg->derive(),
        new NegationNode(new SinNode(Arg))
    );
}


//TanNode
std::string TanNode::out() {
    return "tan(" + Arg->out() + ")";
}

Node* TanNode::derive() {
    return (new DivisionNode(
        new SinNode(Arg),
        new CosNode(Arg)
    ))->derive();
}


//BonaryNode ABC
BinaryNode::~BinaryNode() {
    delete(Left);
    delete(Right);
}


//AdditionNode
std::string AdditionNode::out() {
    return "(" + Left->out() + ") + (" + Right->out() + ")";
}

Node* AdditionNode::derive() {
    return new AdditionNode(Left->derive(), Right->derive());
}


//SubtractionNode
std::string SubtractionNode::out() {
    return "(" + Left->out() + ") - (" + Right->out() + ")";
}

Node* SubtractionNode::derive() {
    return new SubtractionNode(Left->derive(), Right->derive());
}


//MultiplicationNode
std::string MultiplicationNode::out() {
    return "(" + Left->out() + ") * (" + Right->out() + ")";
}

Node* MultiplicationNode::derive() {
    return new AdditionNode(
        new MultiplicationNode(Left->derive(), Right),
        new MultiplicationNode(Left, Right->derive()));
}


//DivisionNode
std::string DivisionNode::out() {
    return "(" + Left->out() + ") / (" + Right->out() + ")";
}

Node* DivisionNode::derive() {   //(u/v)' = (u'*v - u*v')/v*v
    return new DivisionNode(
        new SubtractionNode(
            new MultiplicationNode(Left->derive(), Right),
            new MultiplicationNode(Left, Right->derive())
        ),
        new MultiplicationNode(Right, Right)   //TODO change to exponentiation???
    );
}


//LogarithmNode
std::string LogarithmNode::out() {
    return "log(" + Left->out() + ", " + Right->out() + ")";
}

Node* LogarithmNode::derive() {   //log(u, v)' = (ln(u) / ln(v))'
    return (new DivisionNode(
        (new LnNode(Left))->derive(),
        (new LnNode(Right))->derive()
    ))->derive();
}


//ExponentiationNode
std::string ExponentiationNode::out() {
    return "(" + Left->out() + ") ^ (" + Right->out() + ")";
}

Node* ExponentiationNode::derive() {   //(u^v)' = u^v * (v*ln(u))')
    return new MultiplicationNode(
        this,       //TODO this might cause nullptr deref
        (new MultiplicationNode(
            Right,
            new LnNode(Left)
        ))->derive()
    );
}
