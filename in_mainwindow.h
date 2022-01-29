#include <vector>

#include "VB_Command.h"
#include "statement.h"
using namespace std;

/*This function is used in on_lineEdit_returnPressed(),It puts the instructions in the vector sorted by line number.
Please note: This process does not check the correctness of the instructions!
If the instruction should be run immediately,the the smallest line number:000,will be insert as its line number*/
void insert_com_to_list(QString com,vector<VB_Command> &com_list);
