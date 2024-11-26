#include <string>
#include <iostream>
#include <optional>
#include <vector>
#include <algorithm>
#include <functional>
#include "nodes.h"


enum TokenEnum {
    Empty,
    ParanthesisClose,
    ParanthesisOpen,
    Comma,
    OperatorPlus,
    OperatorMinus,
    OperatorMult,
    OperatorDiv,
    OperatorExp,
    Literal,
    Identifier,
    _TokenEnumCount
};

const std::string tokenTypeSymbols[_TokenEnumCount] = {
    "",   //Empty
    ")",  //ParanthesisClose
    "(",  //ParanthesisOpen
    ",",  //Comma
    "+",  //OperatorPlus
    "-",  //OperatorMinus
    "*",  //OperatorMult
    "/",  //OperatorDiv
    "^",   //OperatorExp
    "<Literal>",    //Literal (number)
    "<Identifier>"  //variables/constant (letter thingies)
};

class Token {
    public:
        std::optional<std::string*> Value;
        TokenEnum TokenType;
        Token(TokenEnum TokenType, std::string* Value):
            TokenType(TokenType), Value(Value){}
        Token(TokenEnum TokenType):
            TokenType(TokenType) {}
        ~Token(){}
};



std::vector<Token>* tokenize(std::string input) {
    int indexTokenizer = 0;
    auto tokenVec = new std::vector<Token>;


    auto error = [] (std::string errStr) {
        std::cout << "Error occured during Tokenization: " <<  errStr;
        exit(-1);
    };
    auto peek = [&] () {
        if (indexTokenizer < input.length()) return input[indexTokenizer];
        return ' ';
    };
    auto consume = [&] () {
        indexTokenizer++;
    };


    while (indexTokenizer < input.length()) {

        if (std::isspace(peek())) {       // whitespaces
            consume();
            continue;
        }
        else if (std::isalpha(peek())) {        //functions, variable
            std::string* buffer = new std::string;
            while (std::isalpha(peek())) {
                *buffer += peek();
                consume();
            }
            tokenVec->push_back(Token(Identifier, buffer));
            continue;
        }
    
        else if (std::isdigit(peek())) {
            std::string* buffer = new std::string;
            while (std::isdigit(peek()) || peek() == '.' || peek() == '_') {     //ignore underscore
                *buffer += peek();
                consume();
            }

            if (std::count(buffer->begin(), buffer->end(), '.') > 1) {
                error("invalid literal");
            }
            
            tokenVec->push_back(Token(Literal, buffer));
            continue;
        }

        switch (peek()) {
            case '+':
                tokenVec->push_back(Token(OperatorPlus));
                break;
            case '-':
                tokenVec->push_back(Token(OperatorMinus));
                break;
            case '*':
                tokenVec->push_back(Token(OperatorMult));
                break;
            case '/':
                tokenVec->push_back(Token(OperatorDiv));
                break;
            case '^':
                tokenVec->push_back(Token(OperatorExp));
                break;

            case '(':
                tokenVec->push_back(Token(ParanthesisOpen));
                break;
            case ')':
                tokenVec->push_back(Token(ParanthesisClose));
                break;
            case ',':
                tokenVec->push_back(Token(Comma));
                break;

            default:
                error((std::string) "invalid character: " + peek());
            }
        consume();
    }


    return tokenVec;
}


Node* parse(std::vector<Token>* input) {
    int indexParser = 0;
    
    //"helper" functions
    auto error = [] (std::string errStr) {
        std::cout << "Error occured during Parsing: " <<  errStr;
        std::exit(1);
    };
    auto peek = [&] () {
        if (indexParser < input->size()) return input->at(indexParser);
        return Token(Empty);
    };
    auto consume = [&] () {
        indexParser++;
    };
    auto expect = [&] (TokenEnum expected) {
        if (peek().TokenType == expected) {
            consume();
            return;
        }
        error((std::string)"Expected " + tokenTypeSymbols[expected] + " but got " + tokenTypeSymbols[peek().TokenType]);
    };
    
    //the actual parse logic (like operator precendence and nested paranthesis) is using a series of recursive functions
    std::function<Node*(void)> parseLowestPriority;
    std::function<Node*(void)> parseLowPriority;    //kinda fucked up forward declartion but whatever
    std::function<Node*(void)> parseMediumPriority;
    std::function<Node*(void)> parseHighPriority;

    //this is really fucked up
    std::function<Node*(std::function<UnaryNode*(Node*)>)> parseUnaryFunction =
    [&] (std::function<UnaryNode*(Node*)> constructor) {
        expect(ParanthesisOpen);
        Node* expr = parseLowestPriority();
        expect(ParanthesisClose);
        return constructor(expr);
    };

    std::function<Node*(std::function<BinaryNode*(Node*, Node*)>)> parseBinaryFunction =
    [&] (std::function<BinaryNode*(Node*, Node*)> constructor) {
        expect(ParanthesisOpen);
        Node* exprLeft = parseLowestPriority();
        expect(Comma);
        Node* exprRight = parseLowestPriority();
        expect(ParanthesisClose);
        return constructor(exprLeft, exprRight);
    };

    //first layer of parsing, this is for lowest precendence operators like + and -
    parseLowestPriority = [&] () {
        Node* left = parseLowPriority();
        Node* right;
        while (true) {
            switch (peek().TokenType)
            {
            case OperatorPlus:
                consume();
                right = parseLowPriority();
                left = new AdditionNode(left, right);
                break;
            case OperatorMinus:
                consume();
                right = parseLowPriority();
                left = new SubtractionNode(left, right);
                break;
            default:
                return left;
            }
        }
    };
    
    //low precendec like * and /
    parseLowPriority = [&] () {
        Node* left = parseMediumPriority();
        Node* right;
        while (true) {
            switch (peek().TokenType)
            {
            case OperatorMult:
                consume();
                right = parseMediumPriority();
                left = new MultiplicationNode(left, right);
                break;
            case OperatorDiv:
                consume();
                right = parseMediumPriority();
                left = new DivisionNode(left, right);
                break;
            default:
                return left;
            }
        }
    };

    //medium precendce: exponentiation
    parseMediumPriority = [&] () {
        Node* left = parseHighPriority();
        Node* right;
        while (true) {
            switch (peek().TokenType)
            {
            case OperatorExp:
                consume();
                right = parseHighPriority();
                left = new ExponentiationNode(left, right);
                break;
            default:
                return left;
            }
        }
    };
    
    //high precendece: paranthesis, negation, literals, functions
    parseHighPriority = [&] () {
        Node* expr;
        std::string* value;
        std::string identifier;
        switch (peek().TokenType)
        {
        case ParanthesisOpen:       //expression in paranthesis
            consume();
            expr = parseLowestPriority();
            expect(ParanthesisClose);
            return expr;
        case OperatorMinus:     //negation
            consume();
            expr = new NegationNode(parseHighPriority());
            return expr;
        case Literal:           //lierally a literal
            value = peek().Value.value();
            consume();
            expr =  new ConstantNode(value);
            return expr;
        case Identifier:        //functions, constants, variables
            identifier = *peek().Value.value();
            consume();
            if (identifier == "pi") {
                expr = new ConstantNode("pi");
            }
            else if (identifier == "e") {
                expr = new ConstantNode("e");
            }
            else if (identifier == "i") {
                expr = new ConstantNode("i");
            }
            else if (identifier == "x") {
                expr = new VariableNode("x");
            }

            else if (identifier == "sin") {
                expr = parseUnaryFunction([] (Node* arg) {return new SinNode(arg);});
            }
            else if (identifier == "cos") {
                expr = parseUnaryFunction([] (Node* arg) {return new CosNode(arg);});
            }
            else if (identifier == "tan") {
                expr = parseUnaryFunction([] (Node* arg) {return new TanNode(arg);});
            }
            else if (identifier == "exp") {
                expr = parseUnaryFunction([] (Node* arg) {return new ExpNode(arg);});
            }
            else if (identifier == "ln") {
                expr = parseUnaryFunction([] (Node* arg) {return new LnNode(arg);});
            }
            else if (identifier == "sqrt") {
                expr = parseUnaryFunction([] (Node* arg) {return new ExpNode(arg);});
            }

            else if (identifier == "log") {
                expr = parseBinaryFunction([] (Node* left, Node* right) {return new LogarithmNode(left, right);});
            }
            return expr;
            
        default:
            error((std::string) "unknown function/constant: " + identifier);
        }
        error("this should be unreachable");
        //unreachable
        return (Node*) new EmptyNode();     //just there to remove compiler warning
    };

    //start parsing with lowest priority
    return parseLowestPriority();
}




int main() {
    std::string input;

    std::cout << "Please Enter equation to derive:" << std::endl;
    std::cin >> input;

    std::vector<Token>* tokenVec = tokenize(input);
    Node* equation = parse(tokenVec);

    std::cout << "You entered:" << std::endl;

    std::cout << equation->out() << std::endl;

    std::cout << "Derived Equation:" << std::endl;

    std::cout << equation->derive()->out() << std::endl;

    std::cout << "Done" << std::endl;

    delete(tokenVec);
    delete(equation);
    return 0;
}