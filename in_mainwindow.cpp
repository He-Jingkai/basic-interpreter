#include <in_mainwindow.h>
using namespace std;

/*This function is used in on_lineEdit_returnPressed(),It puts the instructions in the vector sorted by line number.
Please note: This process does not check the correctness of the instructions!
If the instruction should be run immediately,the the smallest line number:000,will be insert as its line number*/
void insert_com_to_list(QString com,vector<VB_Command> &com_list){
     string a=com.toStdString();

    //Remove spaces before and after the string
    a.erase(0,a.find_first_not_of(" "));
    a.erase(a.find_last_not_of(" ") + 1);

    //Reorder the commands (including insert, change, delete and continue writing)
    unsigned int i ;
    if(a[0]<'0'||a[0]>'9')a.insert(0,"000 ");
    VB_Command vb_com(a);
    for (i= 0; i < com_list.size(); ++i)
        if(com_list[i].line_num>=vb_com.line_num)break;
    if(i<com_list.size()&&com_list[i].line_num==vb_com.line_num)
    {
        if(vb_com.del)
            com_list.erase(com_list.begin()+i);
        else
            com_list[i]=vb_com;
    }
    else if(!vb_com.del)com_list.insert(com_list.begin()+i,vb_com);

}
