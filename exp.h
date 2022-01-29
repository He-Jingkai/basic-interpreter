#ifndef EXP_H
#define EXP_H

#include <string>

#include "content.h"
using namespace std;

//In this file, I define abstact class expression,constantExp ,IdentifierExp, and compundExp

//insert a \t after every \n
string insert_tab(string a);

class Expression {
public:
    Expression(){};
    virtual ~Expression(){};

    virtual int eval(content &cont)=0;
    virtual string type()=0;
    virtual string to_str()=0;

    virtual int getConstantValue(){return 0;};
    virtual string getIdentifierName(){return "";};
    virtual string getOperator(){return "";};
    virtual Expression *getLHS(){return NULL;};
    virtual Expression *getRHS(){return NULL;};
};



class ConstantExp:public Expression{
private:
    int value;
public:
    ConstantExp(string val){
        value=stoi(val);
    };
    ~ConstantExp(){};


    int eval(content &cont){
        cont.do_nothing();
        return value;
    };

    string type(){
        return "CON";
    };

    string to_str(){
        return to_string(value);
    };


    int getConstantValue()
    {
        return value;
    };
};



class IdentifierExp:public Expression{
private:
    string name;
public:
    IdentifierExp(string na){
        name=na;
    };

    ~IdentifierExp(){};


    int eval(content &cont){
        if(!cont.isDefined(name))
        {
            cont.set_exp_error(name+" is undefined");
            return 0;
        }
        return cont.getValue(name);
    };

    string type(){
        return "IDE";
    };

    string to_str(){
        return name;
    };


    string getIdentifierName(){
        return name;
    };

};

class CompoundExp:public Expression{
private:
    string op;
    Expression *lhs;
    Expression *rhs;

public:
    CompoundExp(string op1,Expression *lh=NULL,Expression *rh=NULL){
        op=op1;
        lhs=lh;
        rhs=rh;
    };

    ~CompoundExp(){
        delete lhs;
        delete rhs;
    };


    int eval(content &cont){
        int right=rhs->eval(cont);
        if(cont.is_exp_error())return 0;
        if(op=="="){
            cont.setValue(lhs->getIdentifierName(),right,"INT");
            return right;
        }
        int left=lhs->eval(cont);
        if(cont.is_exp_error())return 0;
        if(op=="+")return left+right;
        if(op=="-")return left-right;
        if(op=="*")return left*right;
        if(op=="^"){
            int temp_temp=1;
            for(int i=0;i<right;i++)
                temp_temp*=left;
            return temp_temp;
        }

        if(op=="/"){
            if(right==0){
                cont.set_exp_error("Division by 0");
                return 0;
            }
            else
                return left/right;
        }
        cont.set_exp_error("Illegal operator in expression");
        return 0;

    };

    string type(){
        return "COM";
    };

    string to_str(){
        if(op=="^")
            return "**"+insert_tab("\n"+lhs->to_str()+"\n"+rhs->to_str());//insert_tab:insert a \t after every \n
        else
            return op+insert_tab("\n"+lhs->to_str()+"\n"+rhs->to_str());//insert_tab:insert a \t after every \n


    };


    string getOperator(){
        return op;
    };
    Expression * getLHS() {
        return lhs;
    };

    Expression * getRHS() {
        return rhs;
    };
};

#endif // EXP_H
