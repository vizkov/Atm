/*
This code runs on GNU's g++ compiler on ubuntu.
To run this on Windows make sure you install the appropriate compiler and have the neccessary file permissions enabled
Reccomended : run this on a Linux distro :p
*/

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

int ui_view();
int ui_ops();
int ui_rec();
int ui_save();
int ui_rback();
int ui_sp();
int ui_update();
int ui_del();
int ui_sel_one();
int ui_commit();

/*

  table column referenced as :
  0:TRANSACTION ID
  1:ACCOUNT NAME
  2:ACCOUNT NUMBER
  3:CREDIT/DEBIT
  4:BALANCE

*/

using namespace std;

class hash_map //class for all row instances
{
public:
  int id, record_no, col_no, spoint;
  string update_val = "";
  hash_map *next_ptr;
  bool del, update;

  hash_map(int x, int y)
  {
    id = x;
    record_no = y;
    update = false;
    del = false;
    spoint = 0;
  }
};

hash_map *arr[9767] = {}; //global array for hash map
vector<hash_map> changes; //global vector to track changes
int cursor = 0;           //a cursor to track save ponits

void clear()
{
  system("cls"); //replace with clear for windows :p
}

int calc_hash(int x)
{
  return x % 9767;
}

string trim(string s) //removes delimeter '@' from input
{
  string a = "";
  for (int i = 0; i < s.length(); ++i)
  {
    if (s[i] != '@')
    {
      a = a + s[i];
    }
  }
  return a;
}

string add(int x, string s) //adds delimeter to output
{
  if (x == 1 && s.length() < 11)
  {

    while (s.length() != 11)
    {
      s = "@" + s;
    }
    return s;
  }
  else if (x == 3 && s.length() < 5)
  {

    string y = s.substr(1, s.length() - 1);

    while (y.length() != 4)
    {
      y = "0" + y;
    }

    y = s[0] + y;
    return y;
  }
  else if (x == 4 && s.length() < 6)
  {

    while (s.length() != 6)
    {
      s = "@" + s;
    }
    return s;
  }
}

void sort_insert(int x) //inserts and sorts 'changes' vector
{

  if (changes.size() == 0)
  {
    changes.push_back(*arr[x]);
  }
  else
  {
    int i = 0;
    vector<hash_map>::iterator it = changes.begin();
    while (changes[i].id <= arr[x]->id && i < changes.size())
    {
      ++i;
    }
    changes.insert(it + i, *arr[x]);
  }
}

void init(hash_map *arr[]) //initializes a hash map on row pointers as keys and primary colm as values
{
  string data;
  int i, fsize, index;
  hash_map *obj_pointer;

  ifstream infile("table.txt");

  infile.seekg(0, infile.end);
  fsize = infile.tellg();

  infile.seekg(0) >> data;
  for (i = 0; i <= (fsize - 54); i = i + 54, infile.seekg(i) >> data)
  {
    data = trim(data);
    index = calc_hash(stoi(data)); //collision ?
    obj_pointer = new hash_map(stoi(data), i);
    arr[index] = obj_pointer;
  }

  infile.close();
}

void view(int x, int y = 0) // y is used to determine the offset for calc the specific row
{
  string data, a, b, c, d, e;
  ifstream infile("table.txt");

  if (x == 0) //0 means view all , 1 means view a specific record
  {
    for (int i = 0; i < 9767; ++i)
    {
      if (arr[i] != NULL)
      {
        infile.seekg(arr[i]->record_no);
        infile >> a >> b >> c >> d >> e;
        a = trim(a);
        b = trim(b);
        e = trim(e);
        cout << "TRANSACTION ID :" << a << endl;
        cout << "ACCOUNT NAME :" << b << endl;
        cout << "ACCOUNT NUMBER :" << c << endl;
        cout << "CREDIT/DEBIT :" << d << endl;
        cout << "BALANCE :" << e << endl;
        cout << "\n\n";
      }
    }
  }
  else
  {
    y = y % 9767;
    if (arr[y] != NULL)
    {
      infile.seekg(arr[y]->record_no);
      infile >> a >> b >> c >> d >> e;
      a = trim(a);
      b = trim(b);
      e = trim(e);
      cout << "TRANSACTION ID :" << a << endl;
      cout << "ACCOUNT NAME :" << b << endl;
      cout << "ACCOUNT NUMBER :" << c << endl;
      cout << "CREDIT/DEBIT :" << d << endl;
      cout << "BALANCE :" << e << endl;
    }
  }
}

void remove(int x) //x is used to get a specific row
{
  x = x % 9767;

  arr[x]->del = true;
  arr[x]->spoint = cursor;
  sort_insert(x);
}

void update(int x, int y, string s) //x is used to get a specific row ,y for col and s for value
{
  x = x % 9767;

  arr[x]->update = true;
  arr[x]->col_no = y;
  s = add(y, s);
  arr[x]->update_val = s;
  arr[x]->spoint = cursor;

  sort_insert(x);
}

void save_point() //updates cursor value
{
  cursor++;
}

void rollback(char a) //a is used to get the value of the save point upto which we need to rollback
{
  int x = (int)a;
  int index = changes.size();
  index--;
  x = x - 65;

  while (changes.at(index).spoint != x)
  {
    changes.pop_back();
    index--;
  }
  cursor = x + 1;
}

void commit()
{
  int x; //to print or not to print ; 0 means dont print ;
  string brr[5];
  ifstream infile("table.txt");
  ofstream ofile("temp.txt");

  for (int i = 0; i < 9767; ++i)
  {
    if (arr[i] != NULL)
    {
      x = 1;
      infile.seekg(arr[i]->record_no);
      infile >> brr[0] >> brr[1] >> brr[2] >> brr[3] >> brr[4];

      for (int j = 0; j < changes.size(); ++j)
      {
        if (changes[j].id == arr[i]->id) //match found thus changes have been made to row 'i'
        {
          if (changes[j].del)
          {
            x = 0;
          }
          else if (changes[j].update)
          {
            brr[changes[j].col_no] = changes[j].update_val;
          }
        }
      }

      if (x != 0)
      {
        for (int k = 0; k < 5; ++k)
        {
          if (k == 4)
          {
            ofile << brr[k] << "\n";
          }
          else
          {
            ofile << brr[k] << "    ";
          }
        }
      }
    }
  }

  ofile.close(); //clean-up
  infile.close();
  cursor = 0;
  changes.clear();
  for (int i = 0; i < 9767; ++i)
  {
    delete (arr[i]);
    arr[i] = NULL;
  }
  rename("temp.txt", "table.txt");
  remove("temp.txt");

  init(arr); //get contents of new table
}

int ui_menu()
{
  int x = 0;
  cout << "WELCOME TO ATM TRANSACTIONS :D"
       << "\n"
       << endl;
  cout << "Please choose a value :" << endl;
  cout << "1 : VIEW" << endl;
  cout << "2 : OPERATIONS" << endl;
  cout << "3 : RECOVERY" << endl;
  cout << "4 : SAVE * Save all progress first to view any updates *"
       << "\n"
       << endl
       << "5 : QUIT"
       << "\n"
       << endl;
  cin >> x;
  if (x == 1)
  {
    clear();
    return ui_view();
  }
  else if (x == 2)
  {
    clear();
    return ui_ops();
  }
  else if (x == 3)
  {
    clear();
    return ui_rec();
  }
  else if (x == 4)
  {
    clear();
    return ui_commit();
  }
  else if (x == 5)
  {
    return 0;
  }
}

int ui_commit()
{
  int x;
  char b;
  cout << "Total changes made to file :" << changes.size() << "\n"
       << endl;
  if (changes.size() == 0)
  {
    cout << "No changes made yet ! Please do some operations before writing to file " << endl
         << "\n";
    cout << "Enter 0 to go back to main menu" << endl;
    cin >> x;
    if (x == 0)
    {
      clear();
      return 1;
    }
  }
  else
  {
    cout << "Do you want to write to file ? Type in Y for yes and N for no" << endl;
    cin >> b;
    if (b == 'Y' || b == 'y')
    {
      clear();
      commit();
      return 1;
    }
    else if (b == 'N' || b == 'n')
    {
      clear();
      return 1;
    }
  }
}

int ui_rec()
{
  int x;
  cout << "Please choose a value : " << endl;
  cout << "1 : Save Point" << endl;
  cout << "2 : Rollback" << endl;
  cout << "\n"
       << "3 : Go Back"
       << "\n"
       << endl;
  cin >> x;
  if (x == 1)
  {
    clear();
    return ui_sp();
  }
  else if (x == 2)
  {
    clear();
    return ui_rback();
  }
  else if (x == 3)
  {
    clear();
    return 1;
  }
}
int ui_sp()
{
  int y;
  char b, a = cursor + 65;
  cout << "Current count of save points : " << cursor << "\n"
       << endl;
  if (changes.size() == 0)
  {
    cout << "No changes made yet ! Please do some operations before creating any saves" << endl
         << "\n";
    cout << "Enter 0 to go back to main menu" << endl;
    cin >> b;
    if (y == 0)
    {
      clear();
      return 1;
    }
  }
  else
  {
    cout << "Do you want to create a savepoint '" << a << "' ?"
         << "  Type in 'Y' for yes and 'N' for no" << endl;
    cin >> b;
    if (b == 'Y' && changes.size() != 0)
    {
      save_point();
      clear();
      return 1;
    }
    else if (b == 'N')
    {
      clear();
      return 1;
    }
  }
}

int ui_rback()
{
  int y = 0;
  char b;

  if (cursor == 0)
  {
    cout << "No saves made yet ! Please create a save point " << endl
         << "\n";
    cout << "Enter 0 to go back to main menu" << endl;
    cin >> y;
    if (y == 0)
    {
      clear();
      return 1;
    }
  }
  else
  {
    cout << "Current save points  : " << endl;
    while (y <= cursor)
    {
      b = y + 65;
      cout << b << " ";
      y++;
    }

    cout << "\n"
         << "Do you want to rollback to a savepoint ?  Type in 'Y' for yes and 'N' for no"
         << endl;
    cin >> b;
    if (b == 'Y')
    {
      cout << "Please enter the point upto which you want to rollback :" << endl;
      cin >> b;
      rollback(b);
      clear();
      return 1;
    }
    else if (b == 'N')
    {
      clear();
      return 1;
    }
  }
}

int ui_ops()
{
  int x;
  cout << "Please choose a value : " << endl;
  cout << "1 : Update" << endl;
  cout << "2 : Delete" << endl;
  cout << "\n"
       << "3 : Go Back"
       << "\n"
       << endl;
  cin >> x;
  if (x == 1)
  {
    clear();
    return ui_update();
  }
  else if (x == 2)
  {
    clear();
    return ui_del();
  }
  else if (x == 3)
  {
    clear();
    return 1;
  }
}

int ui_update()
{
  int x, y;
  string s;

  cout << "Please enter the Transaction id of which you want to change the data of : " << endl;
  cin >> x;
  clear();
  if (arr[x % 9767] != NULL && arr[x % 9767]->del != true)
  {
    cout << "Please choose a value : " << endl;
    cout << "1 : Account Name" << endl;
    cout << "2 : Credit/Debit" << endl;
    cout << "3 : Balance " << endl;
    cout << "\n"
         << "4 : Go Back"
         << "\n"
         << endl;
    cin >> y;
    if (y == 4)
    {
      clear();
      return ui_ops();
    }
    else if (y > 0 && y < 4)
    {
      clear();
      cout << "Please enter a value : " << endl;
      cin >> s;
      if (y == 2) //table has been referenced as 0-based index
      {
        y = 3;
      }
      if (y == 3)
      {
        y = 4;
      }
      update(x, y, s);
      clear();
      return 1;
    }
  }
  else
  {
    cout << "Can't update ! No such id exists or it has been marked for deletion " << endl
         << "\n";
    cout << "Enter 0 to go back to main menu" << endl;
    cin >> y;
    if (y == 0)
    {
      clear();
      return 1;
    }
  }
}

int ui_del()
{
  int y;
  char a;
  cout << "Please enter Transaction id : " << endl;
  cin >> y;
  clear();
  if (arr[y % 9767] != NULL && arr[y % 9767]->del != true)
  {
    cout << "Are you sure you want to delete ? Type in 'Y' for yes and 'N' for no" << endl;
    cin >> a;
    if (a == 'Y')
    {
      remove(y);
      clear();
      return 1;
    }
    else if (a == 'N')
    {
      clear();
      return ui_ops();
    }
  }
  else
  {
    cout << "Can't delete ! No such id exists or it has already been marked for deletion" << endl
         << "\n";
    cout << "Enter 0 to go back to main menu" << endl;
    cin >> y;
    if (y == 0)
    {
      clear();
      return 1;
    }
  }
}

int ui_view()
{
  int x;
  cout << "Please choose a value :" << endl;
  cout << "1 : Select * " << endl;
  cout << "2 : Show a specific row " << endl;
  cout << "\n"
       << "3 : Go Back"
       << "\n"
       << endl;
  cin >> x;
  if (x == 1)
  {
    clear();
    view(0);

    cout << "Enter 0 to go back to main menu" << endl;
    cin >> x;
    if (x == 0)
    {
      clear();
      return 1;
    }
  }
  else if (x == 2)
  {
    clear();
    return ui_sel_one();
  }
  else if (x == 3)
  {
    clear();
    return 1;
  }
}

int ui_sel_one()
{
  int x;
  char a;
  cout << "Please enter a Transaction id : " << endl;
  cin >> x;
  if (arr[x % 9767] != NULL)
  {
    clear();
    view(1, x);
    cout << "\n"
         << "Enter 0 to go back to main menu" << endl;
    cin >> x;
    if (x == 0)
    {
      clear();
      return 1;
    }
  }
  else
  {
    clear();
    cout << "Not a valid Transaction id !" << endl
         << "\n";
    cout << "Enter 0 to go back to main menu" << endl;
    cin >> x;
    if (x == 0)
    {
      clear();
      return 1;
    }
  }
}

int main()
{

  int x = 1; //tracks exit condotion
  clear();
  init(arr);
  while (x == 1)
  {
    x = ui_menu();
  }
  clear();
  return 0;
}
