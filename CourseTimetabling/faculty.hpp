// File Faculty.hpp
#ifndef FACULTY_HPP
#define FACULTY_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cassert>
 
const int BUF_SIZE = 200;

using namespace std;

class CTTException
  : public exception
{
public:
  CTTException(const string& msg) 
    : message(msg) {}
  const char* what() const throw()
  { return message.c_str(); }
  const string& toString() const throw()
  { return message; }
  virtual ~CTTException() throw()
  {}
protected:
  string message;
};

class Course
{
  friend ostream& operator<<(ostream&, const Course&);
  friend istream& operator>>(istream&, Course&);
public:
  Course() {} // data are initialized by the operator >>
  string Name() const { return name; }
  string LongName() const { return long_name; }
  string Teacher() const { return teacher; }
  unsigned Students() const { return students; }
  void AddStudents(unsigned s) {  students += s; }
  unsigned Lectures() const { return lectures; }
  unsigned MinWorkingDays() const { return min_working_days; }
protected:
  string name, long_name, teacher;
  unsigned lectures, students, min_working_days;
};

class CourseGroup
{  
friend ostream& operator<<(ostream&, const CourseGroup&);
public:
  string LongName() const { return longname; }
  string Name() const { return name; }
  void SetLongName(string ln)  { longname = ln; }
  void SetName(string n) { name = n; }
  unsigned Size() const { return members.size(); }
  void Clear() { members.clear(); }
  void AddMember(unsigned e) { members.push_back(e); }
  unsigned operator[](unsigned i) const { return members[i]; }
protected:
  string longname;
  string name;
  vector<unsigned> members;
};

class Room
{
  friend ostream& operator<<(ostream&, const Room&);
  friend istream& operator>>(istream&, Room&);
public:
  Room() {}
  Room(string n, unsigned c) { name = n; capacity = c; }
  string Name() const { return name; }
  unsigned Capacity() const { return capacity; }
protected:
  string name;
  unsigned capacity;
};

class Period
{
  friend ostream& operator<<(ostream&, const Period&);
  friend istream& operator>>(istream&, Period&);
public:
  string Name() const { return name; }
protected:
  string name;
};

class Faculty
{
  friend ostream& operator<<(ostream&, const Faculty&);
public:
  void Load(string instance) throw(CTTException); // reads an instance from file(s)

  unsigned Courses() const { return courses; }
  unsigned Rooms() const { return rooms; }
  unsigned Periods() const { return periods; }
  unsigned PeriodsPerDay() const { return periods_per_day; }
  unsigned Days() const { return periods/periods_per_day; }

  bool  Available(unsigned c, unsigned p) const 
  { return availability[c][p]; } // availability matrix access
  bool Conflict(unsigned c1, unsigned c2) const 
  { return conflict[c1][c2]; } // conflict matrix access
  const Course& CourseVector(int i) const { return course_vect[i]; }
  const Room& RoomVector(int i) const { return room_vect[i]; }
  const Period& PeriodVector(int i) const { return period_vect[i]; }

  const CourseGroup& GroupVector(int i) const { return group_vect[i]; }

  unsigned RoomIndex(string) const throw(CTTException); 
  unsigned CourseIndex(string) const throw(CTTException);
  unsigned GroupIndex(string) const throw(CTTException);
  unsigned PeriodIndex(string) const throw(CTTException);
  unsigned Groups() const { return groups; }
  string DirName() { return dir_name; }
  string Name() const { return name; }
protected:
  string dir_name; // name of the directory containing the input files
  string name; 
  unsigned rooms, courses, periods, periods_per_day, groups;

  // data objects
  vector<Course> course_vect;
  vector<Period> period_vect;
  vector<Room> room_vect;

  // availability and conflicts constraints
  vector<vector<bool> > availability;
  vector<vector<bool> > conflict;

  // course groups
  vector<CourseGroup> group_vect;

  // auxiliary functions
  const char* CreatePath(const string& file_name) const;
};

class Timetable
{
  friend ostream& operator<<(ostream&, const Timetable&);
  friend istream& operator>>(istream&, Timetable&);
public:
  Timetable(Faculty * f = NULL); 
  virtual ~Timetable() {} 
  void SetInput(Faculty *);

    // matrix access functions (const and non-const)
  unsigned operator()(unsigned i, unsigned j) const { return T[i][j]; }
  unsigned& operator()(unsigned i, unsigned j) { return T[i][j]; }
  bool CheckFeasibility(); // checks whether a timetable (read from file) is feasible
 protected:
  virtual void Allocate();
  Faculty* fp;  
  vector<vector<unsigned> > T;  // (courses X periods) timetable matrix
};

#endif
