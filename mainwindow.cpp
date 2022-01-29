#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "VB_Command.h"
#include "content.h"
#include "in_mainwindow.h"

#include <string>
#include <vector>
#include <map>
#include <QTextStream>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QGuiApplication>


using namespace std;

/* eroor may be reported:
 * wrong instruction type
 * XXX is not a line num
 * XXX is not a right line num
 * XXX is undefined
 * Division by 0
 * Illegal operator in expression
 * wrong if-then instruction structure
 * wrong input*/

/*meaning of some vars:
 * com_list:store the command , sorted by line number , don't parse them
 * sta_list:store the command , sorted by line number , after parsing , and can run
 * com_lit and sta_list are seperated , so the commands will not be parsed until the user finishes all the input , so a wrong input will not cause error before
 * parsing,the user can correct it
 * run_cont:store the inpormation in running, such as next line to run , legal symbol and its value , error and so on*/

QList<QPair<int, QColor>> highlights = {};

void clear_all_GREEN(){

    foreach(auto item,highlights){
        if(item.second==QColor(100, 255, 100))
            highlights.removeOne(item);
    }

}

void clear_all_RED(){

    foreach(auto item,highlights){
        if(item.second==QColor(255, 100, 100))
            highlights.removeOne(item);
    }

}

void MainWindow::highlight(int pos,string color){

QTextBrowser *code = ui->textBrowser;
QTextCursor cursor(code->document());


QList<QTextEdit::ExtraSelection> extras;

if(color=="RED")
    highlights.push_back({pos, QColor(255, 100, 100)});
else if(color=="GREEN")
    highlights.push_back({pos, QColor(100, 255, 100)});

for (auto &line : highlights) {
    QTextEdit::ExtraSelection h;
    h.cursor = cursor;

    h.cursor.setPosition(line.first);
    h.cursor.movePosition(QTextCursor::StartOfLine);
    h.cursor.movePosition(QTextCursor::EndOfLine);
    h.format.setProperty(QTextFormat::FullWidthSelection, true);
    h.format.setBackground(line.second);
    extras.append(h);
}

code->setExtraSelections(extras);
}

vector<VB_Command> com_list;
map<int,int> pos_table;

/*Constructor and destructor*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    debug=false;
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*Error: Instruction type error */
void MainWindow::wrong_instruction_type(int x){
    string wrong_ins_error="wrong type of instruction: "+to_string(x);
    ui->textBrowser_4->append(QString::fromStdString(wrong_ins_error));
    ui->textBrowser_3->append(QString::fromStdString(wrong_ins_error));
}

/*Error: Instruction error */
void MainWindow::wrong_instruction(string x){
    ui->textBrowser_3->append(QString::fromStdString(x));
    ui->textBrowser_4->append(QString::fromStdString(x));
}

content run_cont;
bool parse_pass=true;

/*Command input window*/
void MainWindow::on_lineEdit_returnPressed()
{
    /* when the user finishes input and enters , this function will be used and the code will run again following the previous*/
    if(x==1||x==2){
        int pre_x=x;
        x=0;
        string input_;
        input_=ui->lineEdit->text().toStdString();
            if(input_[0]=='?')
        input_.erase(0,1);//earse "?"
        if(pre_x==1){
            for(int i=0;i<(int)input_.size();i++)
                if((input_[i]<'0'||input_[i]>'9')&&input_[i]!='\0'&&input_[i]!='-')
                {

                    wrong_instruction("wrong input");
                    if(debug)
                        exit_debug(1);

                    return;
                }
            int value=stoi(input_);
            run_cont.set_input_var_value(value);

        }
        else{
            for(int i=0;i<(int)input_.size();i++)
                if(input_[i]=='\''||input_[i]=='"')
                {
                    wrong_instruction("wrong input");
                    if(debug)
                          exit_debug(1);
                    return;
                }
            string value=input_;
            run_cont.set_input_var_value_str(value);

        }

        if(!debug)
            on_pushButton_2_clicked();

        ui->lineEdit->clear();
        return;
    }
    //Get the input command
    QString com=this->ui->lineEdit->text();

    string a=com.toStdString();
    int begin = 0;
    begin = a.find(" ",begin);
    while(begin != -1){
        a.replace(begin, 1, "");
        begin = a.find(" ",begin);
    }
    if(a=="")return;
    if(a=="LIST"){
        this->ui->lineEdit->clear();
        return;
    }
    if(a=="DEBUG"){
//        this->ui->lineEdit->clear();
        on_pushButton_5_clicked();
        return;
    }
    if(a=="STEP"){
        //this->ui->lineEdit->clear();
        on_pushButton_5_clicked();
        return;
    }
    if(a=="RUN"){
        this->ui->lineEdit->clear();
        on_pushButton_2_clicked();
        return;
    }
    if(a=="LOAD"){
        this->ui->lineEdit->clear();
        on_pushButton_4_clicked();
        return;
    }
    if(a=="CLEAR"){
        this->ui->lineEdit->clear();
        on_pushButton_clicked();
        return;
    }
    if(a=="QUIT"){
        this->ui->lineEdit->clear();
        QApplication::exit();
        return;
    }
    if(a=="HELP"){
        this->ui->lineEdit->clear();
        QMessageBox::information(this, "HELP", "This is a Basic Interpreter, made by He Jingkai. For more information, read Basic-doc.");
        this->show();
        return;
    }

    insert_com_to_list(com,com_list);
    parse_pass=true;


    //Update the code display window and clear the input window
    ui->textBrowser->clear();
    for(int i= 0; i < (int)com_list.size(); ++i){
         ui->textBrowser->append(QString::fromStdString(com_list[i].command));
    }

    //clear the line_Edit
    ui->lineEdit->clear();

}


vector<statement*> sta_list(com_list.size()+1);


/*clear all,used whe error and clear()*/
void MainWindow::clear_all(){
    com_list.clear();
    run_cont.clear();
    for(int i=0;i<(int)sta_list.size();i++)
        delete sta_list[i];
    sta_list.clear();
}

void build_pos_table(){
    int pos=0;
    int i;
    pos_table[0]=pos;
    for(i=0;i<(int)com_list.size();i++){
         pos+=QString::fromStdString(com_list[i].command).size()+1;
         pos_table[i+1]=pos-1;
    }
}

/*build syntax tree
 * is_error:if their if error in parsing,it will be changed to true,and the parsing will stop,run_con,sta_list,com_list will be cleared immediately*/
void MainWindow::on_pushButton_3_clicked()
{
    int i;
    int is_this_error=false;

    build_pos_table();

    parse_pass=true;

    for(i=0;i<(int)com_list.size();i++)
    {
        is_this_error=false;

        switch(com_list[i].command_no_num[0])
        {
            case 'R':
            if(!(com_list[i].command_no_num[1]=='E'&&com_list[i].command_no_num[2]=='M'&&com_list[i].command_no_num[3]==' ')){
                is_this_error=true;
                wrong_instruction_type(com_list[i].line_num);
                break;
            }
            sta_list[i+1]=new RemSta(com_list[i].line_num,com_list[i].command_no_num,i+1);
            break;


            case 'G'://GOTO
            if(!(com_list[i].command_no_num[1]=='O'&&com_list[i].command_no_num[2]=='T'&&
                    com_list[i].command_no_num[3]=='O'&&com_list[i].command_no_num[4]==' ')){
                is_this_error=true;
                wrong_instruction_type(com_list[i].line_num);
                break;
            }

            sta_list[i+1]=new GotoSta(com_list[i].line_num,com_list[i].command_no_num,i+1,run_cont);
            break;


            case 'E'://END
            if(!(com_list[i].command_no_num[1]=='N'&&com_list[i].command_no_num[2]=='D')){
                is_this_error=true;
                wrong_instruction_type(com_list[i].line_num);
                break;
            }
            sta_list[i+1]=new EndSta(com_list[i].line_num,i+1);
            break;


            case 'L'://LET
            if(!(com_list[i].command_no_num[1]=='E'&&com_list[i].command_no_num[2]=='T'&&com_list[i].command_no_num[3]==' ')){
                is_this_error=true;
                wrong_instruction_type(com_list[i].line_num);
                break;
            }
            if((com_list[i].command_no_num.find("'")!=com_list[i].command_no_num.npos)||
                    (com_list[i].command_no_num.find("\"")!=com_list[i].command_no_num.npos)){
                sta_list[i+1]=new LetsSta(com_list[i].line_num,com_list[i].command_no_num,i+1,run_cont);
                break;
            }
            else{
                sta_list[i+1]=new LetSta(com_list[i].line_num,com_list[i].command_no_num,i+1,run_cont);
                break;
            }


            case 'P'://PRINT
            if(com_list[i].command_no_num[5]=='F'){
                if(!(com_list[i].command_no_num[1]=='R'&&com_list[i].command_no_num[2]=='I'&&com_list[i].command_no_num[3]=='N'
                        &&com_list[i].command_no_num[4]=='T'&&com_list[i].command_no_num[5]=='F'&&com_list[i].command_no_num[6]==' ')){
                    is_this_error=true;
                    wrong_instruction_type(com_list[i].line_num);
                    break;
                }
                sta_list[i+1]=new PrintfSta(com_list[i].line_num,com_list[i].command_no_num,i+1,run_cont);
                break;

            }
            else{
                if(!(com_list[i].command_no_num[1]=='R'&&com_list[i].command_no_num[2]=='I'&&com_list[i].command_no_num[3]=='N'
                    &&com_list[i].command_no_num[4]=='T'&&com_list[i].command_no_num[5]==' ')){
                    is_this_error=true;
                    wrong_instruction_type(com_list[i].line_num);
                    break;
                }

                sta_list[i+1]=new PrintSta(com_list[i].line_num,com_list[i].command_no_num,i+1,run_cont);
                break;
            }


            case 'I'://INPUT
            if(com_list[i].command_no_num[1]=='N'){
                if(com_list[i].command_no_num[5]=='S'){

                        if(!(com_list[i].command_no_num[2]=='P'&&com_list[i].command_no_num[3]=='U'&&com_list[i].command_no_num[4]=='T'
                                &&com_list[i].command_no_num[5]=='S'&&com_list[i].command_no_num[6]==' ')){
                            is_this_error=true;
                            wrong_instruction_type(com_list[i].line_num);
                            break;
                        }

                        sta_list[i+1]=new InputsSta(com_list[i].line_num,com_list[i].command_no_num,i+1);
                        break;

                }
                else{
                    if(!(com_list[i].command_no_num[2]=='P'&&com_list[i].command_no_num[3]=='U'&&com_list[i].command_no_num[4]=='T'
                         &&com_list[i].command_no_num[5]==' ')){
                        is_this_error=true;
                        wrong_instruction_type(com_list[i].line_num);
                        break;
                    }

                    sta_list[i+1]=new InputSta(com_list[i].line_num,com_list[i].command_no_num,i+1);
                    break;
                }


            }
            else{
                if(!(com_list[i].command_no_num[1]=='F'&&com_list[i].command_no_num[2]==' ')){
                    is_this_error=true;
                    wrong_instruction_type(com_list[i].line_num);
                    break;
                }

                sta_list[i+1]=new IfthenSta(com_list[i].line_num,com_list[i].command_no_num,i+1,run_cont);
                break;
            }

        default:
            is_this_error=true;
            wrong_instruction_type(com_list[i].line_num);

        }
        if(run_cont.is_ins_error()){
            is_this_error=true;
            wrong_instruction(run_cont.print_ins_error());

        }
        if(is_this_error){
            highlight(pos_table[i+1], "RED");
            parse_pass=false;
            run_cont.back();
            continue;

        }

        run_cont.table[com_list[i].line_num]=i+1;
        ui->textBrowser_3->append(QString::fromStdString(sta_list[i+1]->print()));
    }
    //if(is_error)
        //clear_all();
}



/* The meaning of x:
 * it has three functions related to it:false_x(),true_x(),get_x()
 * if x is false:that mean the code is running,anything the user input in lineEdit_2() will be ignored
 * if x is true,that mean the code is stop to wait for user to insput,the code will not run again before the user enters the input
 * after the user entered the input, the code will run again,following the previous */
void MainWindow::on_pushButton_2_clicked()
{
    x=0;
    ui->textBrowser_3->clear();
    ui->textBrowser_4->clear();
    on_pushButton_3_clicked();
    if(!parse_pass){
        run_cont.clear();
        sta_list.clear();
        highlights.clear();
        return;
    }
    int next_line=run_cont.get_next_line();
    string app;
    while(next_line!=0&&next_line<(int)com_list.size()+1)
    {

        if(sta_list[next_line]->type()=="INPUT"){
            ui->lineEdit->setText("?");
            qApp->processEvents();


            qApp->processEvents();

            string a=sta_list[next_line]->run(run_cont);
            run_cont.set_input_var(a);
            x=1;
            qApp->processEvents();

            break;
        }
        else if(sta_list[next_line]->type()=="INPUTS"){
            ui->lineEdit->setText("?");
            qApp->processEvents();

            string a=sta_list[next_line]->run(run_cont);
            run_cont.set_input_var(a);
            x=2;
            break;
        }
        else{

            app=sta_list[next_line]->run(run_cont);
            if(run_cont.is_ins_error()){
                wrong_instruction(run_cont.print_ins_error());
                run_cont.clear();
                highlight(pos_table[next_line], "RED");
                //com_list.clear();
                break;
            }
            if(app!="")
                ui->textBrowser_2->append(QString::fromStdString(app));
            next_line=run_cont.get_next_line();
        }
    }
    if(x==0)
        variable_display();

    if(debug){
        debug=false;
        ui->pushButton->setDisabled(false);
        ui->pushButton_3->setDisabled(false);
        ui->pushButton_4->setDisabled(false);
    }
}


/*clear button,clear all thing*/
void MainWindow::on_pushButton_clicked(){
    //clear all
    ui->textBrowser->clear();
    ui->textBrowser_2->clear();
    ui->textBrowser_3->clear();
    ui->textBrowser_4->clear();
    ui->textBrowser_5->clear();
    ui->lineEdit->clear();
    highlights.clear();
    clear_all();
    x=0;
}

/* load
*  read a file,put it to com_list */
void MainWindow::on_pushButton_4_clicked()
{
    QString qstr=QFileDialog::getOpenFileName();
    string a=qstr.toStdString();
    a.erase(0,a.find_first_not_of(" "));
    a.erase(a.find_last_not_of(" ") + 1);
    qstr=QString::fromStdString(a);

    QFile dataFile(qstr);

    if (dataFile.open(QFile::ReadOnly|QIODevice::Text)){
        QTextStream data(&dataFile);
        QString line;
        while (!data.atEnd()){
            line = data.readLine();
            line.remove('\n');
            insert_com_to_list(line,com_list);
            }
     }
    ui->textBrowser->clear();
    for(int i= 0; i < (int)com_list.size(); ++i){
         ui->textBrowser->append(QString::fromStdString(com_list[i].command));
    }
}
void MainWindow::exit_debug(int i){
    if(i==1){//error
        QMessageBox::information(this, "ERROR", "ERROR OCCURS, DUBUG EXITS");
        this->show();
    }
    if(i==2){//error
        QMessageBox::information(this, "CODE END", "CODE END NORMALLY");
        this->show();
    }
    debug=false;
    ui->pushButton->setDisabled(false);
    ui->pushButton_3->setDisabled(false);
    ui->pushButton_4->setDisabled(false);

}
void MainWindow::single_line(int next_line){

    if(next_line==0||next_line>=(int)com_list.size()+1){
        exit_debug(2);
        return;
    }

    bool is_this_error=false;

    int i=next_line-1;
    switch(com_list[i].command_no_num[0])
    {
        case 'R':
        if(!(com_list[i].command_no_num[1]=='E'&&com_list[i].command_no_num[2]=='M'&&com_list[i].command_no_num[3]==' ')){

            is_this_error=true;
            wrong_instruction_type(com_list[i].line_num);
            break;
        }
        sta_list[i+1]=new RemSta(com_list[i].line_num,com_list[i].command_no_num,i+1);
        break;


        case 'G'://GOTO
        if(!(com_list[i].command_no_num[1]=='O'&&com_list[i].command_no_num[2]=='T'&&
                com_list[i].command_no_num[3]=='O'&&com_list[i].command_no_num[4]==' ')){

            is_this_error=true;
            wrong_instruction_type(com_list[i].line_num);
            break;
        }

        sta_list[i+1]=new GotoSta(com_list[i].line_num,com_list[i].command_no_num,i+1,run_cont);
        break;


        case 'E'://END
        if(!(com_list[i].command_no_num[1]=='N'&&com_list[i].command_no_num[2]=='D')){

            is_this_error=true;
            wrong_instruction_type(com_list[i].line_num);
            break;
        }
        sta_list[i+1]=new EndSta(com_list[i].line_num,i+1);
        break;


        case 'L'://LET
        if(!(com_list[i].command_no_num[1]=='E'&&com_list[i].command_no_num[2]=='T'&&com_list[i].command_no_num[3]==' ')){

            is_this_error=true;
            wrong_instruction_type(com_list[i].line_num);
            break;
        }
        if((com_list[i].command_no_num.find("'")!=com_list[i].command_no_num.npos)||
                (com_list[i].command_no_num.find("\"")!=com_list[i].command_no_num.npos)){
            sta_list[i+1]=new LetsSta(com_list[i].line_num,com_list[i].command_no_num,i+1,run_cont);
            break;
        }
        else{
            sta_list[i+1]=new LetSta(com_list[i].line_num,com_list[i].command_no_num,i+1,run_cont);
            break;
        }


        case 'P'://PRINT
        if(com_list[i].command_no_num[5]=='F'){
            if(!(com_list[i].command_no_num[1]=='R'&&com_list[i].command_no_num[2]=='I'&&com_list[i].command_no_num[3]=='N'
                    &&com_list[i].command_no_num[4]=='T'&&com_list[i].command_no_num[5]=='F'&&com_list[i].command_no_num[6]==' ')){

                is_this_error=true;
                wrong_instruction_type(com_list[i].line_num);
                break;
            }
            sta_list[i+1]=new PrintfSta(com_list[i].line_num,com_list[i].command_no_num,i+1,run_cont);
            break;

        }
        else{
            if(!(com_list[i].command_no_num[1]=='R'&&com_list[i].command_no_num[2]=='I'&&com_list[i].command_no_num[3]=='N'
                &&com_list[i].command_no_num[4]=='T'&&com_list[i].command_no_num[5]==' ')){

                is_this_error=true;
                wrong_instruction_type(com_list[i].line_num);
                break;
            }

            sta_list[i+1]=new PrintSta(com_list[i].line_num,com_list[i].command_no_num,i+1,run_cont);
            break;
        }


        case 'I'://INPUT
        if(com_list[i].command_no_num[1]=='N'){
            if(com_list[i].command_no_num[5]=='S'){

                    if(!(com_list[i].command_no_num[2]=='P'&&com_list[i].command_no_num[3]=='U'&&com_list[i].command_no_num[4]=='T'
                            &&com_list[i].command_no_num[5]=='S'&&com_list[i].command_no_num[6]==' ')){

                        is_this_error=true;
                        wrong_instruction_type(com_list[i].line_num);
                        break;
                    }

                    sta_list[i+1]=new InputsSta(com_list[i].line_num,com_list[i].command_no_num,i+1);
                    break;

            }
            else{
                if(!(com_list[i].command_no_num[2]=='P'&&com_list[i].command_no_num[3]=='U'&&com_list[i].command_no_num[4]=='T'
                     &&com_list[i].command_no_num[5]==' ')){

                    is_this_error=true;
                    wrong_instruction_type(com_list[i].line_num);
                    break;
                }

                sta_list[i+1]=new InputSta(com_list[i].line_num,com_list[i].command_no_num,i+1);
                break;
            }


        }
        else{
            if(!(com_list[i].command_no_num[1]=='F'&&com_list[i].command_no_num[2]==' ')){

                is_this_error=true;
                wrong_instruction_type(com_list[i].line_num);
                break;
            }

            sta_list[i+1]=new IfthenSta(com_list[i].line_num,com_list[i].command_no_num,i+1,run_cont);
            break;
        }

    default:

        is_this_error=true;
        wrong_instruction_type(com_list[i].line_num);

    }
    if(run_cont.is_ins_error()){

        is_this_error=true;
        wrong_instruction(run_cont.print_ins_error());

    }
    if(is_this_error){
        clear_all_GREEN();
        highlight(pos_table[i+1], "RED");///

        exit_debug(1);
        return;

    }


    run_cont.table[com_list[i].line_num]=i+1;
    ui->textBrowser_3->clear();
    ui->textBrowser_3->append(QString::fromStdString(sta_list[i+1]->print()));


    string app;
    if(sta_list[next_line]->type()=="INPUT"){
        ui->lineEdit->setText("?");

        string a=sta_list[next_line]->run(run_cont);
        run_cont.set_input_var(a);
        x=1;
        return;
    }
    else if(sta_list[next_line]->type()=="INPUTS"){
        ui->lineEdit->setText("?");

        string a=sta_list[next_line]->run(run_cont);
        run_cont.set_input_var(a);
        x=2;
        return;
    }
    else{

        app=sta_list[next_line]->run(run_cont);
        if(run_cont.is_ins_error()){
            wrong_instruction(run_cont.print_ins_error());
            run_cont.clear();

            clear_all_GREEN();
            highlight(pos_table[next_line], "RED");

            //com_list.clear();
            exit_debug(1);
            return;
        }
        if(app!="")
            ui->textBrowser_2->append(QString::fromStdString(app));

    }
}
void MainWindow::on_pushButton_5_clicked(){
    if(!debug){
        debug=true;
        ui->pushButton->setDisabled(true);
        ui->pushButton_3->setDisabled(true);
        ui->pushButton_4->setDisabled(true);
        ui->textBrowser_4->clear();
        clear_all_RED();
        return;
    }

    int next_line=run_cont.get_next_line();

    on_pushButton_3_clicked();
    ui->textBrowser_3->clear();

    clear_all_GREEN();



    highlight(pos_table[run_cont.get_next_line()], "GREEN");

    single_line(run_cont.get_next_line());
    ui->textBrowser_5->clear();
    variable_display();

}



void MainWindow::variable_display(){
    ui->textBrowser_5->clear();
    string tmp;

    for(auto it:run_cont.type_dictionary){
        if(it.second==1){
            tmp=it.first+": INT = "+to_string(run_cont.dictionary[it.first]);
            ui->textBrowser_5->append(QString::fromStdString(tmp));
        }
        else{
            tmp=it.first+": STR = "+run_cont.string_dictionary[it.first];
            ui->textBrowser_5->append(QString::fromStdString(tmp));
        }

    }

}

