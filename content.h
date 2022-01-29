#ifndef CONTENT_H
#define CONTENT_H

#include <map>
#include <string>

#include "VB_Command.h"

//in this file I define class content
//Content is used when the program run, it contains a map record all the variables' value ; and an int, next_line, functions
//like pc to record the next line to operate

using namespace std;

class content{
private:

    int next_line;
    string exp_error;
    string ins_error;
    string input_var;


public:
    map<int,int> table;
    map<string,int> dictionary;
    map<string,string> string_dictionary;
    map<string,int> type_dictionary;

    content(){
        exp_error="";
        next_line=1;
    }

    ~content(){}

    void set_line(int a){
        next_line=a;
    }

     bool isDefined(string var){
         return type_dictionary.find(var)!=type_dictionary.end();
     }

     void setValue(string var,int value, string type,string str=""){
         if(type=="INT"){
             type_dictionary[var]=1;
             dictionary[var]=value;
             if(string_dictionary.find(var)!=string_dictionary.end())
                 string_dictionary.erase(var);
         }

         else if(type=="STR"){
             type_dictionary[var]=2;
             string_dictionary[var]=str;
             if(dictionary.find(var)!=dictionary.end())
                 dictionary.erase(var);
         }

     }

     int getValue(string var){
         return dictionary[var];
     }

     string getValue_str(string var){
         if(type_dictionary[var]==1)
             return to_string(dictionary[var]);
         return string_dictionary[var];
     }

     int get_next_line(){
         return next_line;
     }

     void clear(){
         next_line=1;
         dictionary.clear();
         string_dictionary.clear();
         table.clear();
         type_dictionary.clear();
         exp_error.clear();
         ins_error.clear();
         input_var.clear();

     }

     void set_exp_error(string x){
         exp_error=x;
     }

     bool is_exp_error(){
         return (exp_error!="");
     }

     string print_exp_error(){
         return exp_error;
     }

     void set_ins_error(string x){
         ins_error=x;
     }

     bool is_ins_error(){
         return (ins_error!="");
     }

     string print_ins_error(){
         return ins_error;
     }

     bool is_line_num_right(int x){
         return table.find(x)!=table.end();
     }

     void set_input_var(string s){
         input_var=s;
     }

     void set_input_var_value(int x){
         setValue(input_var,x, "INT");
     }

     void set_input_var_value_str(string x){
         setValue(input_var,0, "STR",x);
     }

     void do_nothing(){
         return;
     }

     void back(){
         exp_error="";
         ins_error="";
         input_var="";
     }

};

#endif // CONTENT_H
