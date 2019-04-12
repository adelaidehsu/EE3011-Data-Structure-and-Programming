/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include "limits.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include "dbTable.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
    // TODO: to print out a row.
    // - Data are seperated by a space. No trailing space at the end.
    // - Null cells are printed as '.'
    for(int i=0; i<r.size()-1; i++)
    {
        if(r[i]!=INT_MAX)
        {
            os<<r[i]<<' ';
        }
        else
            os<<'.'<<' ';
    }
    
    int end = r.size()-1;
    if(r[end]!= INT_MAX)
    {
        os<<r[end];
    }
    else
        os<<'.';
    
    return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
    // TODO: to print out a table
    // - Data are seperated by setw(6) and aligned right.
    // - Null cells are printed as '.'
    for(int i=0; i<t.nRows(); i++)
    {
        for(int j=0; j<t.nCols(); j++)
        {
            if(t[i][j]!=INT_MAX)
            {
                os<<setw(6)<<right<<t[i][j];
            }
            else
                os<<setw(6)<<right<<'.';
        }
        os<<endl;
    }
    return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
    // TODO: to read in data from csv file and store them in a table
    // - You can assume the input file is with correct csv file format
    // - NO NEED to handle error file format
    size_t colcnt = 0;
    string ss;
    getline(ifs, ss);
    for(unsigned int i=0; i<= ss.size(); i++)
    {
        if(ss[i]== ',')
        {
            colcnt++;
        }
        
    }
    size_t _nCols = colcnt+1;
    
    ifs.clear();
    ifs.seekg(0, ios::beg);
    DBRow r;
    //int index = 0;
    int cnt = 1;
    r.SetRowdata(_nCols, ifs);
    while(!r.CheckEmptyRow(_nCols,r))
    {
        t.addRow(r);
        //cout<<t[index]<<endl;
        r.reset();
        r.UpdateRowData(_nCols,ifs,cnt);
        //index++;
    }
    
    return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
    // TODO
    _data.erase(_data.begin()+c);
}
void DBRow::SetRowdata(size_t _nCols, ifstream& ifs)
{
    int s = _nCols;
    
    int BUFFSIZE = 800;
    int result;
    char buff[BUFFSIZE];
    
    stringstream ss;
    
    ifs.getline(buff, BUFFSIZE);
    ss<<buff;
    
    for(int col=0; col<s; col++)
    {
        ss.getline(buff, 100, ',');
        (stringstream(buff) >> result)? _data.push_back(result):_data.push_back(INT_MAX);
    }
    ss<<"";
    ss.clear();
    

}
void DBRow::UpdateRowData(size_t _nCols, ifstream& ifs, int cnt)
{
    int s = _nCols;
    int result;
    int BUFFSIZE = 800;
    char buff[BUFFSIZE];
    stringstream ss;
    
    for(int i=0; i<cnt; i++)
    {
        ifs.getline(buff, BUFFSIZE);
    }
    
    ss<<buff;
    
    for(int col=0; col<s; col++)
    {
        ss.getline(buff, 100, ',');
        (stringstream(buff) >> result)?_data.push_back(result):_data.push_back(INT_MAX);
    }
    ss<<"";
    ss.clear();
}
bool DBRow::CheckEmptyRow(size_t _nCols, DBRow& row)
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
/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
    // TODO: called as a functional object that compares the data in r1 and r2
    //       based on the order defined in _sortOrder
    for(int i=0; i<_sortOrder.size(); i++)
    {
        if(r1[_sortOrder[i]] > r2[_sortOrder[i]])
            return false;
        else if(r1[_sortOrder[i]] < r2[_sortOrder[i]])
            return true;
    }
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
    // TODO
    _table.erase(_table.begin(), _table.end());
}

void
DBTable::addCol(const vector<int>& d)
{
    // TODO: add a column to the right of the table. Data are in 'd'.
    for(int i=0; i<_table.size(); i++)
    {
        _table[i].addData(d[i]);
    }
}

void
DBTable::delRow(int c)
{
    // TODO: delete row #c. Note #0 is the first row.
    _table.erase(_table.begin()+c);
}

void
DBTable::delCol(int c)
{
    // delete col #c. Note #0 is the first row.
    for (size_t i = 0, n = _table.size(); i < n; ++i)
       _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
    // TODO: get the max data in column #c
    int nullcnt = 0;
    size_t colnumber = c;
    int temp;
    bool tempcheck = false;
    int max;
    for(int k=0; k< _table.size(); k++)
    {
        if(_table[k][colnumber] != INT_MAX)
        {
            max = _table[k][colnumber];
            temp = k;
            tempcheck = true;
            break;
        }
        else
            nullcnt++;
        
    }
    if(tempcheck)
    {
        for(int i=temp+1; i < _table.size(); i++)
        {
            if(_table[i][colnumber] == INT_MAX)
            {
                nullcnt++;
            }
            else if(_table[i][colnumber] > max)
            {
                max = _table[i][colnumber];
            }
        }
    }

    if (nullcnt == _table.size())
    {
        return NAN;
    }
    
    else
        return static_cast<float>(max);
}

float
DBTable::getMin(size_t c) const
{
    // TODO: get the min data in column #c
    int nullcnt = 0;
    size_t colnumber = c;
    int temp;
    bool tempcheck = false;
    int min;
    for(int k=0; k< _table.size(); k++)
    {
        if(_table[k][colnumber] != INT_MAX)
        {
            min = _table[k][colnumber];
            temp = k;
            tempcheck = true;
            break;
        }
        else
            nullcnt++;
        
    }
    if(tempcheck)
    {
        for(int i=temp+1; i < _table.size(); i++)
        {
            if(_table[i][colnumber] == INT_MAX)
            {
                nullcnt++;
            }
            else if(_table[i][colnumber] < min)
            {
                min = _table[i][colnumber];
            }
        }
    }

    if (nullcnt == _table.size())
    {
        return NAN;
    }
    
    else
        return static_cast<float>(min);
}

float 
DBTable::getSum(size_t c) const
{
    // TODO: compute the sum of data in column #c
    int result = 0;
    int nullcnt = 0;
    size_t colnumber = c;
    for (int i=0; i < _table.size(); i++)
    {
        if(_table[i][colnumber] == INT_MAX)
        {
            nullcnt++;
        }
        else
            result += _table[i][colnumber];
    }
    
    if (nullcnt == _table.size())
    {
        return NAN;
    }
    
    else
        return static_cast<float>(result);
}

int
DBTable::getCount(size_t c) const
{
    // TODO: compute the number of distinct data in column #c
    // - Ignore null cells
    int samecnt = 0;
    int nullcnt = 0;
    size_t colnumber = c;
    
    for(int k=0; k< _table.size(); k++)
    {
        if(_table[k][colnumber] == INT_MAX)
        {
            nullcnt++;
        }
        else
        {
            for(int j=k+1; j < _table.size(); j++)
            {
                if(_table[j][colnumber] == _table[k][colnumber])
                {
                    samecnt++;
                    break;
                }
            }
        }
    }
    
    return static_cast<float>(_table.size()-nullcnt-samecnt);
}

float
DBTable::getAve(size_t c) const
{
    // TODO: compute the average of data in column #c
    int result = 0;
    int nullcnt = 0;
    size_t colnumber = c;
    double ave;
    for (int i=0; i < _table.size(); i++)
    {
        if(_table[i][colnumber] == INT_MAX)
        {
            nullcnt++;
        }
        else
            result += _table[i][colnumber];
    }
    
    if (nullcnt == _table.size())
    {
        return NAN;
    }
    
    else
    {
        ave = static_cast<double>(result)/(_table.size()-nullcnt);
        return ave;
    }
}

void
DBTable::sort(const struct DBSort& s)
{
    // TODO: sort the data according to the order of columns in 's'
    ::sort(_table.begin(), _table.end(), s);
}

void
DBTable::printCol(size_t c) const
{
    // TODO: to print out a column.
    // - Data are seperated by a space. No trailing space at the end.
    // - Null cells are printed as '.'
    for(int i=0; i<_table.size()-1; i++)
    {
        if(_table[i][c] == INT_MAX)
        {
            cout<<'.'<<' ';
        }
        else
            cout<<_table[i][c]<<' ';
    }
    
    int end = _table.size()-1;
    if(_table[end][c] == INT_MAX)
    {
        cout<<'.';
    }
    else
        cout<<_table[end][c];
    
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

