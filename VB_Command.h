#pragma once
#ifndef VB_COMMAND_H
#define VB_COMMAND_H

#include<string>
#include<iostream>
#include<QString>
#include<vector>
using namespace std;


class VB_Command
{
//all public,eqaul to struct
public:
    int line_num;
    string command;
    bool del=false;//Check whether the command semantics is delete
    string command_no_num;


    VB_Command(string input_str)
    {
        command = input_str;

        //caculate line_num
        int i;
        for(i=0;i<(int)input_str.size();i++)
            if(input_str[i]-'0'<0||input_str[i]-'0'>9)break;
        string str1=input_str.substr(0, i);

        line_num = stoi(str1);
        //del
        if((int)input_str.size()==i)
           del=true;

        command.insert(i," ");
        string tmp=command;
        command_no_num = tmp.erase(0, tmp.find_first_of(" ")+1);
        command_no_num.erase(0,command_no_num.find_first_not_of(" "));
        command_no_num.erase(command_no_num.find_last_not_of(" ") + 1);

    }


    ~VB_Command(){}

};

#endif // VB_COMMAND_H
