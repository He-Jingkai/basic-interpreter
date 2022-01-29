#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>

using namespace std;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    int x;
    bool debug;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void wrong_instruction_type(int x);
    void wrong_instruction(string x);

    void clear_all();

    void highlight(int line_num,string color);

private slots:

    void on_lineEdit_returnPressed();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void variable_display();

    void single_line(int next_line);

    void exit_debug(int i);


private:
    Ui::MainWindow *ui;
    //bool x;
};
#endif // MAINWINDOW_H
