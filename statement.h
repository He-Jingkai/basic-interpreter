#ifndef STATEMENT_H
#define STATEMENT_H

#include <string>

#include "content.h"
//#include "exp.h"
#include "token.h"

/*all the statement classes are defined here*/

using namespace std;

class statement{
public:
    statement(){};
    virtual ~statement(){};
    virtual string print() = 0;
    virtual string type() = 0;
    virtual string run(content &cont) =0;
    virtual int get_num() =0;


};



/*statement REM*/
class RemSta:public statement{
private:
    int num;
    string str;
    int count_of_line;
public:
    RemSta(int number,string a,int x){
        str=a.erase(0, a.find_first_of(" ")+1);
        num=number;
        count_of_line=x;
    }
    ~RemSta(){}

    string print(){
        return to_string(num)+" REM\n\t"+str;
    }

    string type() {
        return "REM";
    }
    string run(content &cont) {
       cont.set_line(count_of_line+1);
       return "";
    }

    int get_num(){
        return num;
    }

};



/*statement GOTO*/
class GotoSta:public statement{
private:
    int num;
    int goto_line;
    int count_of_line;
public:
    GotoSta(int number,string a,int x,content &cont){
        bool wrong=false;
        num=number;
        string int_str=a.erase(0,a.find_first_of(" ")+1);
        for(int i=0;i<(int)int_str.size();i++)
        {
            if((int_str[i]>'9'||int_str[i]<'0')&&int_str[i]!='\0'){\
                cont.set_ins_error(to_string(num)+" : "+int_str+" is not a line number!");
                wrong=true;
                break;
            }

        }
        if(!wrong){
            goto_line=stoi(int_str);
            count_of_line=x;
        }
    }
    ~GotoSta(){}

    string print(){
        return  to_string(num)+" GOTO\n\t"+to_string(goto_line);
    }

    string type(){
        return "GOTO";
    }

    string run(content &cont) {
        if(!cont.is_line_num_right(goto_line))
        {
            cont.set_ins_error(to_string(num)+" : "+to_string(goto_line)+" is not a right line number");
            return "";
        }
        cont.set_line(cont.table[goto_line]);
        return "";
    }

    int get_num(){
        return num;
    }

};



/*stament END */
class EndSta:public statement{
private:
    int num;
    int count_of_line;
public:
    EndSta(int number,int x){
        num=number;
        count_of_line=x;
    }
    ~EndSta(){}

    string print(){
        return  to_string(num)+" END\n  ";
    }

    string type(){
        return "END";
    }

    string run(content &cont) {
        cont.set_line(0);
        return "";
    }

    int get_num(){
        return num;
    }

};



/*statement LET */
 class LetSta:public statement{
 private:
     int num;
     Expression *exp;
     int count_of_line;
 public:
     LetSta(int number,string a,int x,content &cont){
         num=number;
         a.erase(0, a.find_first_of(" ")+1);

//         if(!check_expression(a))
//             exp=nullptr;
//         else
             exp=build_tree(a);

         count_of_line=x;

         if(!exp)
             cont.set_ins_error(to_string(num)+" : "+"wrong expression");
     }

     ~LetSta(){}

     string type(){
         return "LET";
     }

     string print(){
         return to_string(num)+" LET"+exp->to_str();
     }

     string run(content &cont){

         if(!exp)
             cont.set_ins_error(to_string(num)+" : "+"wrong expression");
         else
            exp->eval(cont);

         if(cont.is_exp_error())cont.set_ins_error(to_string(num)+" : "+cont.print_exp_error());
         cont.set_line(count_of_line+1);
         return "";
     }

     int get_num(){
         return num;
     }

 };



 /*statement print */
 class PrintSta:public statement{
 private:
     int num;
     Expression *exp;
     int count_of_line;
 public:
     PrintSta(int number,string a,int x,content &cont){
         num=number;
         a.erase(0, a.find_first_of(" ")+1);

//         if(!check_expression(a))
//             exp=nullptr;
//         else
            exp=build_tree(a);

         count_of_line=x;
         if(!exp)
             cont.set_ins_error(to_string(num)+" : "+"wrong expression");
     }

     ~PrintSta(){}

     string type(){
         return "PRINT";
     }

     string print(){
         return to_string(num)+" PRINT\n\t"+exp->to_str();
     }

     string run(content &cont){
          int k=0;

         if(!exp)
             cont.set_ins_error(to_string(num)+" : "+"wrong expression");
         else
             k=exp->eval(cont);
         if(cont.is_exp_error())cont.set_ins_error(to_string(num)+" : "+cont.print_exp_error());
         cont.set_line(count_of_line+1);
         return to_string(k);
     }

     int get_num(){
         return num;
     }

 };



 /*statement Input */
 class InputSta:public statement{
 private:
     int num;
     IdentifierExp *ide;
      int count_of_line;
 public:
     InputSta(int number,string a,int x){
         num=number;
         a.erase(0, a.find_first_of(" ")+1);

         ide=new IdentifierExp(a);
         count_of_line=x;
     }

     ~InputSta(){}

    string type(){
        return "INPUT";
     }

     string print(){
         return to_string(num)+" INPUT\n\t"+ide->to_str();
     }

     string run(content &cont){
         cont.set_line(count_of_line+1);
         return ide->to_str();
    }

     int get_num(){
         return num;
     }
 };



 /*statement ifthen */
 class IfthenSta:public statement{
 private:
     int num;
     Expression *exp1;
     Expression *exp2;
     string op;
     int n;
     int count_of_line;

 public:
     IfthenSta(int number,string a,int x,content &cont){
         num=number;
         string exp1_string;

         for(int i=0;i<(int)a.size();i++)
             if(a[i]=='='||a[i]=='>'||a[i]=='<')
             {
                 a.insert(i," ");
                 a.insert(i+2," ");
                 break;
             }

         a.erase(a.find_last_not_of(" ") + 1);//erase the space after the string

         a.erase(0, a.find_first_of(" ")+1);//erase IF
         a.erase(0,a.find_first_not_of(" "));//erase the space ahead of the string

         exp1_string=a.substr(0, a.find_first_of(" "));

//         if(!check_expression(exp1_string))
//             exp1=nullptr;
//         else
             exp1=build_tree(exp1_string);

         a.erase(0, a.find_first_of(" ")+1);//erase exp1
         a.erase(0,a.find_first_not_of(" "));//erase the space ahead of the string

         if(a.size()==0){cont.set_ins_error(to_string(num)+" : "+"wrong if-then instruction structure!");return;}
         op=a.substr(0, a.find_first_of(" "));
         if(op!="="&&op!="<"&&op!=">")
             {cont.set_ins_error(to_string(num)+" : "+"wrong if-then operator!");return;}
         a.erase(0, a.find_first_of(" ")+1);//erase op
         a.erase(0,a.find_first_not_of(" "));//erase the space ahead of the string


         if(a.size()==0){cont.set_ins_error(to_string(num)+" : "+"wrong if-then instruction structure!");return;}

//         if(!check_expression(a.substr(0, a.find_first_of(" "))))
//             exp2=nullptr;
//         else
             exp2=build_tree(a.substr(0, a.find_first_of(" ")));

         a.erase(0, a.find_first_of(" ")+1);//erase exp2
         a.erase(0,a.find_first_not_of(" "));//erase the space ahead of the string

         if(a.size()<=5||a[0]!='T'||a[1]!='H'||a[2]!='E'||a[3]!='N'){cont.set_ins_error(to_string(num)+" : "+"wrong if-then instruction structure!");return;}
         a.erase(0, a.find_first_of(" ")+1);//erase THEN
         a.erase(0,a.find_first_not_of(" "));//erase the space ahead of the string


         if(a.size()==0){cont.set_ins_error(to_string(num)+" : "+"wrong if-then instruction structure!");return;}
         for(int i=0;i<(int)a.size();i++)
         {
             if((a[i]>'9'||a[i]<'0')&&a[i]!='\0'){\
                 cont.set_ins_error(to_string(num)+" : "+a+" is not a line number!");
                 return;
             }

         }
         n=stoi(a);

         count_of_line=x;
         if(!exp1)
              cont.set_ins_error(to_string(num)+" : "+"wrong expression");
         if(!exp2)
              cont.set_ins_error(to_string(num)+" : "+"wrong expression");
     }

     ~IfthenSta(){}

    string type(){
        return "IF";
     }

     string print(){
         return to_string(num)+" IF THEN\n\t"+exp1->to_str()+"\n\t"+op+"\n\t"+exp2->to_str()+"\n\t"+to_string(n);
     }

     string run(content &cont){
         int value1;
         int value2;

         if(!exp1)
             {cont.set_ins_error(to_string(num)+" : "+"wrong expression");return "";}
         else
             value1=exp1->eval(cont);

         if(cont.is_exp_error()){cont.set_ins_error(to_string(num)+" : "+cont.print_exp_error());return "";}

         if(!exp2)
             {cont.set_ins_error(to_string(num)+" : "+"wrong expression");return "";}
         else
             value2=exp2->eval(cont);

         if(cont.is_exp_error()){cont.set_ins_error(to_string(num)+" : "+cont.print_exp_error());return "";}

         if(op=="="){
             if(value1==value2)
             {if(!cont.is_line_num_right(n))
                 {
                     cont.set_ins_error(to_string(num)+" : "+to_string(n)+"is not a right line number");
                     return "";
                 }
                 cont.set_line(cont.table[n]);}
             else

                 cont.set_line(count_of_line+1);
         }
         else if(op==">"){
             if(value1>value2)
             {if(!cont.is_line_num_right(n))
                 {
                     cont.set_ins_error(to_string(num)+" : "+to_string(n)+"is not a right line number");
                     return "";
                 }
                 cont.set_line(cont.table[n]);}
             else
                 cont.set_line(count_of_line+1);
         }
         else {
             if(value1<value2)
             {if(!cont.is_line_num_right(n))
                 {
                     cont.set_ins_error(to_string(num)+" : "+to_string(n)+"is not a right line number");
                     return "";
                 }
                 cont.set_line(cont.table[n]);}
             else
                 cont.set_line(count_of_line+1);
         }

         return "";

    }

     int get_num(){
         return num;
     }
 };



 /*statement Input */
 class InputsSta:public statement{
 private:
     int num;
     string ide;
      int count_of_line;
 public:
     InputsSta(int number,string a,int x){
         num=number;
         a.erase(0, a.find_first_of(" ")+1);

         int begin = 0;
         begin = a.find(" ",begin);
         while(begin != -1){
         a.replace(begin, 1, "");
         begin = a.find(" ",begin);
         }

         ide=a;
         count_of_line=x;
     }

     ~InputsSta(){}

    string type(){
        return "INPUTS";
     }

     string print(){
         return to_string(num)+" INPUTS\n\t"+ide;
     }

     string run(content &cont){
         cont.set_line(count_of_line+1);
         return ide;
    }

     int get_num(){
         return num;
     }
 };



 /*statement Input */
 class PrintfSta:public statement{
 private:
     int num;
     string striction;
     vector<string> constants;
     vector<string> tmp_dir;
     int count_of_line;

 public:
     PrintfSta(int number,string a,int x,content &cont){
         num=number;
         count_of_line=x;

         a.erase(0, a.find_first_of(" ")+1);

         a.erase(0, 1);

         int m = min(a.find_first_of("'") , a.find_first_of("\"") );
         striction = a.substr(0, m+1);

         striction.erase(striction.size() - 1, 1);

         a.erase(0, m + 2);

//         int begin = 0;
//         begin = a.find(" ",begin);
//         while(begin != -1){
//         a.replace(begin, 1, "");
//         begin = a.find(" ",begin);
//         }

         string tmp;
         while(a!=""){
             if(a.find_first_of(",")!=a.npos)
                tmp=a.substr(0,a.find_first_of(","));
             else
                 tmp=a;

             tmp_dir.push_back(tmp);

             if(a.find_first_of(",")!=a.npos)
                a.erase(0,a.find_first_of(",")+1);
             else
                 break;
         }

         int num1 = 0;
         for (size_t i=0; (i=striction.find("{}",i)) != striction.npos; num1++, i+=2);

         if(num1!=(int)tmp_dir.size())
             cont.set_ins_error(to_string(num)+" : Wrong number of {}");

         count_of_line=x;



         striction.erase(striction.find_last_not_of(" ") + 1);//erase the space after the string
         striction.erase(0,striction.find_first_not_of(" "));//erase the space ahead of the string

         for(int j=0;j<(int)tmp_dir.size();j++){
             tmp_dir[j].erase(tmp_dir[j].find_last_not_of(" ") + 1);//erase the space after the string
             tmp_dir[j].erase(0,tmp_dir[j].find_first_not_of(" "));//erase the space ahead of the string
         }

     }

     ~PrintfSta(){}

    string type(){
        return "PRINTF";
     }

     string print(){
         string ret=to_string(num)+" PRINTF\n\t"+striction;
         for(int i=0;i<(int)tmp_dir.size();i++)
             ret=ret+"\n\t"+tmp_dir[i];
         return ret;
     }

     string run(content &cont){
         string str_ret=striction;
         string tmp;

         for(int i=0;i<(int)tmp_dir.size();i++){
             tmp=tmp_dir[i];
             if(tmp[0]=='\''||tmp[0]=='\"'){
                 tmp.erase(0,1);
                 tmp.erase(tmp.size()-1,1);
                 constants.push_back(tmp);
             }
             else if(tmp[0]>='0'&&tmp[0]<='9')
                 constants.push_back(tmp);
             else{
                 if(!cont.isDefined(tmp))
                     cont.set_ins_error(to_string(num)+" : "+tmp+" is not defined");
                 else
                     constants.push_back(cont.getValue_str(tmp));
             }
         }

         for(int i=0;i<(int)constants.size();i++){
             str_ret = str_ret.replace(str_ret.find("{}"), 2, constants[i]);
         }

         cont.set_line(count_of_line+1);
         return str_ret;
    }

     int get_num(){
         return num;
     }

 };



/*statement LET */
class LetsSta:public statement{
private:
    int num;
    string str1;
    string str2;
    int count_of_line;
public:
      LetsSta(int number,string a,int x,content &cont){
          num=number;
          a.erase(0, a.find_first_of(" ")+1);

//          int begin = 0;
//          begin = a.find(" ",begin);
//          while(begin != -1){
//          a.replace(begin, 1, "");
//          begin = a.find(" ",begin);
//          }

          count_of_line=x;

          if(a.find_first_of("=")==a.npos){
              cont.set_ins_error(to_string(num)+" : "+"wrong expression");
              str1=str2="";
          }

          else{
              str1=a.substr(0,a.find_first_of("="));
              a.erase(0,a.find_first_of("=")+1);

              str2=a;

              if(str1==""||str2=="")
                   cont.set_ins_error(to_string(num)+" : "+"wrong expression");
          }

          str1.erase(str1.find_last_not_of(" ") + 1);//erase the space after the string
          str1.erase(0,str1.find_first_not_of(" "));//erase the space ahead of the string

          str2.erase(str2.find_last_not_of(" ") + 1);//erase the space after the string
          str2.erase(0,str2.find_first_not_of(" "));//erase the space ahead of the string


      }

      ~LetsSta(){}

      string type(){
          return "LET";
      }

      string print(){
          return to_string(num)+" LET =\n\t" +str1+"\n\t"+str2;
      }

      string run(content &cont){
          cont.set_line(count_of_line+1);
          string tmp=str2;

          if(tmp[0]=='\"'||tmp[0]=='\''){
              tmp.erase(0,1);
              tmp.erase(tmp.size()-1,1);
              cont.setValue(str1,0,"STR",tmp);
          }
          else{
              if(!cont.isDefined(str1))
                  cont.set_ins_error(to_string(num)+" : wrong expression");
              else
                  cont.setValue(str1,0,"STR",cont.getValue_str(str2));
          }

          return "";
      }

      int get_num(){
          return num;
      }

  };



#endif // STATEMENT_H
