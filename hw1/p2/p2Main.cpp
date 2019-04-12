/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "limits.h"
#include "p2Table.h"

using namespace std;

size_t Countcol(string& csvFile);

int main()
{
    Table table;
    Row row;
    
    // Read in the csv file. Do NOT change this part of code.
    string csvFile;
    cout << "Please enter the file name: ";
    cin >> csvFile;
    if (table.read(csvFile))
       cout << "File \"" << csvFile << "\" was read in successfully." << endl;
    else
    {
       cerr << "Failed to read in file \"" << csvFile << "\"!" << endl;
       exit(-1); // csvFile does not exist.
    }
    
    // TODO read and execute commands
    table.Countcol(csvFile);
    int COLS = table.nCols();
    table.SetTableData(COLS, row, csvFile);

    while(true)
    {
        int colnumber;
        string command;
        cin >> command;
        if(command == "PRINT")
        {
            table.print();
        }
        else if(command == "SUM")
        {
            cin >> colnumber;
            if(colnumber < 0 || colnumber > COLS)
            {
                cout << "Column number is out of range, please try again."<<endl;
            }
            else
                table.sum(colnumber);
        }
        else if(command == "MAX")
        {
            cin >> colnumber;
            if(colnumber < 0 || colnumber > COLS)
            {
                cout << "Column number is out of range, please try again."<<endl;
            }
            else
                table.max(colnumber);
        }
        else if(command == "MIN")
        {
            cin >> colnumber;
            if(colnumber < 0 || colnumber > COLS)
            {
                cout << "Column number is out of range, please try again."<<endl;
            }
            else
                table.min(colnumber);
        }
        else if(command == "DIST")
        {
            cin >> colnumber;
            if(colnumber < 0 || colnumber > COLS)
            {
                cout << "Column number is out of range, please try again."<<endl;
            }
            else
                table.distinct(colnumber);
        }
        else if(command == "AVE")
        {
            cin >> colnumber;
            if(colnumber < 0 || colnumber > COLS)
            {
                cout << "Column number is out of range, please try again."<<endl;
            }
            else
                table.ave(colnumber);
        }
        else if(command == "ADD")
        {
                table.add(row, COLS);
        }
        else if(command == "EXIT")
        {
            break;
        }
        else
            cout << "Invalid command, please try again."<<endl;
    }
    
    return 0;
}

