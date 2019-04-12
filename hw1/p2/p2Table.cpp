/****************************************************************************
  FileName     [ p2Table.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Row and Table ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include "p2Table.h"
#include "limits.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

// Implement member functions of class Row and Table here

void Row::SetRowdata(size_t _nCols, string& csvFile)
{
    int s = _nCols;
    _data = new int[s];
    int datasize = 0;
    
    int BUFFSIZE = 800;
    int result;
    char buff[BUFFSIZE];
    
    ifstream file(csvFile);
    stringstream ss;
    
    file.getline(buff, BUFFSIZE);
    ss<<buff;
    
    for(int col=0; col<s; col++)
    {
        ss.getline(buff, 100, ',');
        (stringstream(buff) >> result)?_data[datasize] = result:_data[datasize] = INT_MAX;
        datasize++;
    }
    
    ss<<"";
    ss.clear();
    currentrowcnt = 2;
}

void Row::Setaddedrow(size_t _nCols)
{
    int s = _nCols;
    _data = new int[s];
    int datasize = 0;
    for(int i=0; i < s; i++)
    {
        string value;
        cin >> value;
        int result;
        (stringstream(value) >> result)?_data[datasize] = result:_data[datasize] = INT_MAX;
        datasize++;
    }
}

void Row::UpdateRowData(size_t _nCols, string& csvFile)
{
    int s = _nCols;
    _data = new int[s];
    int datasize = 0;
    int result;
    int BUFFSIZE = 800;
    char buff[BUFFSIZE];
    
    ifstream file(csvFile);
    stringstream ss;

    for(int i=0; i<currentrowcnt; i++)
    {
        file.getline(buff, BUFFSIZE);
    }
    
    ss<<buff;
    
    for(int col=0; col<s; col++)
    {
        ss.getline(buff, 100, ',');
        (stringstream(buff) >> result)?_data[datasize] = result:_data[datasize] = INT_MAX;
        datasize++;
    }
    
    ss<<"";
    ss.clear();
    currentrowcnt++;
}

bool Row::CheckEmptyRow(size_t _nCols, Row& row)
{
    unsigned int zerocnt = 0;
    for(int i=0; i<_nCols; i++)
    {
        if(row[i] == INT_MAX)
        {
            zerocnt++;
        }
    }
    
    if(zerocnt == _nCols)
    {
        return true;
    }
    else
        return false;
}

bool Table::read(const string& csvFile)
{   fstream file;
    file.open(csvFile, ios::in);
    if(!file)
        return false;
    else
        return true;// TODO
}


void Table::Countcol(string& csvFile)
{
    size_t colcnt = 0;
    ifstream file(csvFile);
    string ss;
    
    getline(file, ss);
    
    for(unsigned int i=0; i<= ss.size(); i++)
    {
        if(ss[i]== ',')
        {
            colcnt++;
        }
        
    }
    
    _nCols = colcnt+1;
}


void Table::SetTableData(size_t _nCols, Row& row, string& csvFile)
{
    int s = _nCols;
    row.SetRowdata(s, csvFile);
    
    while(!row.CheckEmptyRow(s,row))
    {
        _rows.push_back(row);
        row.UpdateRowData(s, csvFile);
    }
}

void Table::print()
{
    for(int i=0; i<_rows.size(); i++)
    {
        for(int j=0; j<_nCols; j++)
        {
            if(_rows[i][j] == INT_MAX)
            {
                cout<< setw(4) << right << ".";
            }
            else
                cout<< setw(4) << right << _rows[i][j];
        }
        cout<<endl;
    }
}

void Table::sum(int colnumber)
{
    int result = 0;
    int nullcnt = 0;
    for (int i=0; i < _rows.size(); i++)
    {
        if(_rows[i][colnumber] == INT_MAX)
        {
            nullcnt++;
        }
        else
        result += _rows[i][colnumber];
    }
    
    if (nullcnt == _rows.size())
    {
        cout << "Error: This is a NULL column."<<endl;
    }
    
    else
        cout << "The summation of data in column #"<<colnumber<<" is "<<result<<"."<<endl;
}

void Table::max(int colnumber)
{
    int nullcnt = 0;
    int temp;
    int max;
    for(int k=0; k< _rows.size(); k++)
    {
        if(_rows[k][colnumber] != INT_MAX)
           {
               max = _rows[k][colnumber];
               temp = k;
               break;
           }
        else
            nullcnt++;
           
    }
    for(int i=temp+1; i < _rows.size(); i++)
    {
        if(_rows[i][colnumber] == INT_MAX)
        {
            nullcnt++;
        }
        else if(_rows[i][colnumber] > max)
        {
            max = _rows[i][colnumber];
        }
    }
    if (nullcnt == _rows.size())
    {
        cout << "Error: This is a NULL column."<<endl;
    }
    
    else
        cout << "The maximum of data in column #"<<colnumber<<" is "<<max<<"."<<endl;
}

void Table::min(int colnumber)
{
    int nullcnt = 0;
    int temp;
    int min;
    for(int k=0; k< _rows.size(); k++)
    {
        if(_rows[k][colnumber] != INT_MAX)
        {
            min = _rows[k][colnumber];
            temp = k;
            break;
        }
        else
            nullcnt++;
        
    }
    for(int i=temp+1; i < _rows.size(); i++)
    {
        if(_rows[i][colnumber] == INT_MAX)
        {
            nullcnt++;
        }
        else if(_rows[i][colnumber] < min)
        {
            min = _rows[i][colnumber];
        }
    }
    if (nullcnt == _rows.size())
    {
        cout << "Error: This is a NULL column."<<endl;
    }
    
    else
        cout << "The minimum of data in column #"<<colnumber<<" is "<<min<<"."<<endl;
}

void Table::distinct(int colnumber)
{
    int samecnt = 0;
    int nullcnt = 0;
    
    for(int k=0; k< _rows.size(); k++)
    {
        if(_rows[k][colnumber] == INT_MAX)
        {
            nullcnt++;
        }
        else
        {
            for(int j=k+1; j < _rows.size(); j++)
            {
                if(_rows[j][colnumber] == _rows[k][colnumber])
                {
                    samecnt++;
                }
            }
        }
    }
    if(nullcnt == _rows.size())
    {
        cout << "Error: This is a NULL column."<<endl;
    }
    else
        cout << "The distinct count of data in column #"<<colnumber<<" is "<<_rows.size()-nullcnt-samecnt<<"."<<endl;
}

void Table::ave(int colnumber)
{
    int result = 0;
    int nullcnt = 0;
    double ave;
    for (int i=0; i < _rows.size(); i++)
    {
        if(_rows[i][colnumber] == INT_MAX)
        {
            nullcnt++;
        }
        else
            result += _rows[i][colnumber];
    }
    
    if (nullcnt == _rows.size())
    {
        cout << "Error: This is a NULL column."<<endl;
    }
    
    else
    {
        ave = static_cast<double>(result)/(_rows.size()-nullcnt);
        cout << "The average of data in column #"<<colnumber<<" is "<<fixed<<setprecision(1)<<ave<<"."<<endl;
    }
}

void Table::add(Row& row, size_t _nCols)
{
    row.Setaddedrow(_nCols);
    _rows.push_back(row);
}
