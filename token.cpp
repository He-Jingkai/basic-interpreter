#include "token.h"

/* This file defines two functions (also can be said to be one)
 * Seperate(string input) is used to seperate a expression string into parts(opraters , constant expression,
 * and identifier expression
 * Buid_tree function is used to make the expression to a tree */

using namespace std;

/* Seperate(string input) is used to seperate a expression string into parts(opraters , constant expression,
 * and identifier expression*/
vector<string> sperate(string input){

    int begin = 0;
    begin = input.find(" ",begin);
    while(begin != -1){
    input.replace(begin, 1, "");
    begin = input.find(" ",begin);
    }
    vector<string> tmp;

    if(input.find("++")!=input.npos||input.find("--")!=input.npos||input.find("//")!=input.npos||input.find("+=")!=input.npos||input.find("-=")!=input.npos||
            input.find("*=")!=input.npos||input.find("/=")!=input.npos||input.find("=+")!=input.npos){
        return tmp;
    }





    while(input.find("**")!=input.npos){
        input=input.replace(input.find("**"),2,"^");
    }


    for(int k=1;k<(int)input.size();k++){
        if((input[k]=='-'&&input[k-1]=='='))
            input[k]='%';
        if(input[k]=='-'&&input[k-1]=='('){
            input[k]='%';
        }
    }

    int i=0,pre=0;
//    vector<string> tmp;
    for(i=0;i<(int)input.size();i++)
        if(input[i]=='='
                ||input[i]=='+'||input[i]=='-'
                ||input[i]=='*'||input[i]=='/'||input[i]=='^'
                ||input[i]==')'||input[i]=='(')
        {
             if(i!=pre)tmp.push_back(input.substr(pre,i-pre));
            pre=i+1;
            switch(input[i]){
            case '=':
                tmp.push_back("=");
                break;
            case '+':
                tmp.push_back("+");
                break;
            case '-':
                tmp.push_back("-");
                break;
            case '*':
                tmp.push_back("*");
                break;
            case '/':
                tmp.push_back("/");
                break;
            case '(':
                tmp.push_back("(");
                break;
            case ')':
                tmp.push_back(")");
                break;
            case '^': //////
                tmp.push_back("^");
                break;
            }
        }
    if (i != pre)
        tmp.push_back(input.substr(pre,i-pre));
    return tmp;
    }



/*Buid_tree function is used to make the expression to a tree*/
Expression* build_tree(string in_string)
{
    vector<string> tmp1=sperate(in_string);
    vector<Expression*> vec_exp(tmp1.size());






    if(tmp1.size()==0)return NULL;








    for(int i=0;i<(int)tmp1.size();i++)
    {

        if((tmp1[i][0]-'0'>=0&&tmp1[i][0]-'0'<=9)||(tmp1[i][0]=='%')){
            if(tmp1[i][0]=='%')tmp1[i][0]='-';
            vec_exp[i]=new ConstantExp(tmp1[i]);
        }

        else if(tmp1[i][0]=='+'||
                tmp1[i][0]=='-'||
                tmp1[i][0]=='*'||
                tmp1[i][0]=='^'||
                tmp1[i][0]=='/'||
                tmp1[i][0]=='('||
                tmp1[i][0]==')'||
                tmp1[i][0]=='=')
            vec_exp[i]=new CompoundExp(tmp1[i]);
        else
            vec_exp[i]=new IdentifierExp(tmp1[i]);
    }

//    unsigned int length_vec_exp=vec_exp.size();
//    if(vec_exp[0]->type()=="COM")
//        return NULL;
//    if(vec_exp[length_vec_exp-1]->type()=="COM")
//        return NULL;
//    for(unsigned int i=1;i<length_vec_exp-1;i++){
//        if(vec_exp[i]->type()=="COM"){
//            if(vec_exp[i-1]->type()!="CON"&&vec_exp[i-1]->type()!="IDE")
//                return NULL;
//            if(vec_exp[i+1]->type()!="CON"&&vec_exp[i+1]->type()!="IDE")
//                return NULL;
//        }
//    }


    stack<string> sta_op;
    stack<Expression*> sta_exp;

    string com;
    Expression* lef;
    Expression* rig;
    Expression* tmp;


    for(int i=0;i<(int)vec_exp.size();i++)
    {
        if(vec_exp[i]->type()!="COM")
            sta_exp.push(vec_exp[i]);
        else
        {
            //When encountering ‘)’, keep popping the stack until the top of the stack is ‘(’
            if(vec_exp[i]->getOperator()==")"){
                while(sta_op.top()!="(")
                {
                    com=sta_op.top();
                    sta_op.pop();
                    rig=sta_exp.top();
                    sta_exp.pop();
                    lef=sta_exp.top();
                    sta_exp.pop();

                    tmp=new CompoundExp(com,lef,rig);
                    sta_exp.push(tmp);
                }
                //pop '('
                sta_op.pop();
            }

            //When encountering ‘*’ or '/', pop all the "*" and "/"
            else if(vec_exp[i]->getOperator()=="*"||vec_exp[i]->getOperator()=="/"||vec_exp[i]->getOperator()=="^"){
                while(!sta_op.empty()&&(sta_op.top()=="*"||sta_op.top()=="/"))
                {
                    com=sta_op.top();
                    sta_op.pop();
                    rig=sta_exp.top();
                    sta_exp.pop();
                    lef=sta_exp.top();
                    sta_exp.pop();

                    tmp=new CompoundExp(com,lef,rig);
                    sta_exp.push(tmp);

                }
                //push the op
                sta_op.push(vec_exp[i]->getOperator());
            }

            //When encountering ‘+’ or '-', pop all the "+" , "-", "*" , "/"
            else if(vec_exp[i]->getOperator()=="+"||vec_exp[i]->getOperator()=="-"){
                while(!sta_op.empty()&&(sta_op.top()=="*"||sta_op.top()=="/"||sta_op.top()=="+"||sta_op.top()=="-"))
                {
                    com=sta_op.top();
                    sta_op.pop();
                    rig=sta_exp.top();
                    sta_exp.pop();
                    lef=sta_exp.top();
                    sta_exp.pop();

                    tmp=new CompoundExp(com,lef,rig);
                    sta_exp.push(tmp);
                }
                sta_op.push(vec_exp[i]->getOperator());
            }
            else
                sta_op.push(vec_exp[i]->getOperator());
        }
    }


    //Pop the stack until the stack is empty
    while(!sta_op.empty())
    {
        com=sta_op.top();
        sta_op.pop();
        rig=sta_exp.top();
        sta_exp.pop();
        lef=sta_exp.top();
        sta_exp.pop();

        tmp=new CompoundExp(com,lef,rig);
        sta_exp.push(tmp);
    }

    return sta_exp.top();
}
