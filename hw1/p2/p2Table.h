/****************************************************************************
  FileName     [ p2Table.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Row and Table ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>

using namespace std;

class Row
{
 public:
    // TODO: define constructor and member functions on your own
    Row() {};
    void SetRowdata(size_t i, string&);
    void UpdateRowData(size_t i, string&);
    bool CheckEmptyRow(size_t i, Row& row);
    void Setaddedrow(size_t i);
    // Basic access functions
    const int operator[] (size_t i) const { return _data[i]; }
    int& operator[] (size_t i) { return _data[i]; }

 private:
    int    *_data; // DO NOT change this definition. Use it to store data.
    int currentrowcnt;
};

class Table
{
 public:
    // TODO: define constructor and member functions on your own
    Table() {};
    bool read(const string&);
    void Countcol(string&);
    void SetTableData(size_t i, Row& row, string& csvFile);
    void print();
    void sum(int i);
    void max(int i);
    void min(int i);
    void distinct(int i);
    void ave(int i);
    void add(Row& row, size_t i);
    
    // Basic access functions
    size_t nCols() const{ return _nCols; }
    const Row& operator[] (size_t i) const { return _rows[i]; }
    Row& operator[] (size_t i) { return _rows[i]; }

 private:
    size_t       _nCols; // You should record the number of columns.
    vector<Row>  _rows;  // DO NOT change this definition. Use it to store rows.
};

#endif // P2_TABLE_H
