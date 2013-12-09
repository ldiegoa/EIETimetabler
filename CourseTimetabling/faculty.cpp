// File faculty.cpp
#include "faculty.hpp"

ostream& operator<<(ostream& os, const Course& c)
{
  os << c.name << '\t' << c.long_name << '\t'
     << c.teacher << '\t' << c.lectures;
  return os;
}

istream& operator>>(istream& is, Course& c)
{
  char buffer[BUF_SIZE], ch;  
  is.getline(buffer,BUF_SIZE,',');
  c.name = buffer;
  is.getline(buffer,BUF_SIZE,',');
  is >> ch; is.putback(ch); // ignores blanks
  c.long_name = buffer;
  is.getline(buffer,BUF_SIZE,',');
  is >> ch; is.putback(ch); // ignores blanks
  c.teacher = buffer;
  is >> c.lectures >> c.min_working_days >> c.students;
  is.getline(buffer,BUF_SIZE);
  return is;
}

ostream& operator<<(ostream& os, const CourseGroup& g)
{
  return os << g.Name() << "\t" << g.LongName();
}


ostream& operator<<(ostream& os, const Room& r)
{
  return os << r.name << "\t" << r.capacity;
}

istream& operator>>(istream& is, Room& r)
{
  char buffer[BUF_SIZE];
  is >> r.name >> r.capacity;
  is.getline(buffer,BUF_SIZE);
  return is;
}

ostream& operator<<(ostream& os, const Period& p)
{
  return os << p.name;
}

istream& operator>>(istream& is, Period& p)
{
  char buffer[BUF_SIZE];
  is >> p.name;
  is.getline(buffer,BUF_SIZE);
  return is;
}

// **************************************************************
// ********************  TIMETABLE ********************************
// ************************************************************** 

ostream& operator<<(ostream& os, const Timetable& tt)
{
  unsigned i, j;
  os << "Course    (Teacher)           |          Monday              |            Tuesday           |          Wednesday           |           Thursday           |           Friday             |" << endl
     << " - - - - - - - - - - - - - -  |   7   9   13   15   17   19  |   7   9   13   15   17   19  |   7   9   13   15   17   19  |   7   9   13   15   17   19  |   7   9   13   15   17   19  |" << endl
     << " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
  for (i = 0; i < tt.T.size(); i++)
    {
      string s = tt.fp->CourseVector(i).Name();
      for (j = s.size(); j < 10; j++)
	s += ' ';
      s += "(" + tt.fp->CourseVector(i).Teacher() + ") ";
      for (j = s.size(); j < 30; j++)
	s += ' ';
      for (j = 0; j < tt.T[i].size(); j++)
	{
	  if (j % tt.fp->PeriodsPerDay() == 0)
	    s += "|";
	  if (tt.T[i][j] != 0)
	    {
	      string room_name = tt.fp->RoomVector(tt.T[i][j]).Name();
	      s += " ";
	      if (room_name.size() == 1)
		s += " " + room_name + "  ";
	      else if (room_name.size() == 2)
		s += room_name + "  ";
	      else
		s += room_name + " ";
	    }
	  else 
	    s += "  -  ";
	}
      s += "|";
      os << s << endl;
    }
  os  << " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << endl << endl;
  return os; 
}

istream& operator>>(istream& is, Timetable& tt)
{ 
  unsigned i, j, room;
  char buffer[BUF_SIZE];
  string room_name;

  is.getline(buffer,BUF_SIZE);
  is.getline(buffer,BUF_SIZE);
  is.getline(buffer,BUF_SIZE);
  
  for (i = 0; i < tt.T.size(); i++)
    {
      is.getline(buffer,BUF_SIZE,')');
      //      is >> buffer >> buffer;
      for (j = 0; j < tt.T[i].size(); j++)
	{
	  if (j % tt.fp->PeriodsPerDay() == 0)
	    is >> buffer;
	  is >> room_name;
	  if (room_name == "-")
	    tt.T[i][j] = 0;
	  else
	    {
	      room = tt.fp->RoomIndex(room_name);
	      tt.T[i][j] = room;
	    }
	}
      is >> buffer; //is.getline(buffer,BUF_SIZE);
    }
  if (!tt.CheckFeasibility()) 
    throw CTTException("Error: not a feasible solution");

  return is; 
}

bool Timetable::CheckFeasibility()
{
  unsigned p, c, r, lectures;
  for (c = 0; c < fp->Courses(); c++)
    {
      lectures = 0;
      for (p = 0; p < fp->Periods(); p++)
	{
	  r = T[c][p];
	  if (r != 0)
	    {
	      lectures++;
	      if (!fp->Available(c,p))
		{
		  cerr << "Lecture of " << fp->CourseVector(c).Name() << " at "
		       << fp->PeriodVector(p).Name() << endl;
		  return false;
		}
	    }
	}
      if (lectures != fp->CourseVector(c).Lectures())
	{
	  cerr << "Wrong number of lectures for " << fp->CourseVector(c).Name() << endl;
	  return false;
	}
    }
  return true;
}

Timetable::Timetable(Faculty *f)
{
  fp = f;
  if (fp != NULL)
    Allocate(); 
}

void Timetable::SetInput(Faculty * f)
{
  if (fp != f)
    {
      fp = f;
      Allocate(); 
    }
}

void Timetable::Allocate()
{ T.resize(fp->Courses(), vector<unsigned>(fp->Periods(),0)); }

// **************************************************************
// ********************  FACULTY ********************************
// ************************************************************** 

void Faculty::Load(string dir) throw(CTTException)
{
  string group, group_name, course_name, room_name, period_name, 
    teacher_name, priority;
  string course_name1, course_name2;

  unsigned group_size;

  dir_name = dir;
  char buffer[BUF_SIZE], ch;
  ifstream is(CreatePath("config.dat"));
  if (is.fail())
    throw CTTException("Error while trying to load file config.dat; could not open the file");
  unsigned i;
  is.getline(buffer,BUF_SIZE);
  name = buffer;

  is >> buffer >> courses;
  is >> buffer >> rooms;
  is >> buffer >> periods >> ch >> periods_per_day;

  // **********************************
  // Allocate vectors and matrices
  // **********************************
  course_vect.resize(courses);
  period_vect.resize(periods);
  // location 0 of room_vect is not used (teaching in room 0 means NOT TEACHING)
  room_vect.resize(rooms + 1);

  availability.resize(courses, vector<bool>(periods,true));
  conflict.resize(courses, vector<bool>(courses));

  is.close();

  // **********************************
  // Read courses
  // **********************************

  is.open(CreatePath("courses.dat"));
  is.getline(buffer,BUF_SIZE);
  i = 0;

  while (i < courses)
    {
      do
	{
	  ch = is.peek();
	  if (ch == '#')
	    is.getline(buffer,BUF_SIZE);      
	  if (ch == ' ' || ch == '\n')
	    is.get();
	}
      while (ch == '#' || ch == ' ' || ch == '\n');      
      if (ch == EOF)
	{
	  if (i == 0)
	    cerr << "Error at the beginning of file" << endl;
	  else
	    cerr << "Error after the course " << course_vect[i-1].Name() << endl;
	  if (ch == EOF)
	    throw CTTException("Errore: premature end of the file \"courses.dat\"");
	}
      is >> course_vect[i];
      i++;
    }
  is.close();

  // **********************************
  // Read periods
  // **********************************


  is.open(CreatePath("periods.dat"));
  is.getline(buffer,BUF_SIZE);
  for (i = 0; i < periods; i++)
    {
      do
	{
	  ch = is.peek();
	  if (ch == '#')
	    is.getline(buffer,BUF_SIZE);      
	}
      while (ch == '#');      
      if (ch == EOF)
	throw CTTException("Error: Premature end of the file \"periods.dat\"");
      is >> period_vect[i];
    }
  is.close();

  // **********************************
  // Read rooms
  // **********************************

  is.open(CreatePath("rooms.dat"));
  is.getline(buffer,BUF_SIZE);
  for (i = 1; i <= rooms; i++)
    {
      do
	{
	  ch = is.peek();
	  if (ch == '#')
	    is.getline(buffer,BUF_SIZE);      
	}
      while (ch == '#');      
      if (ch == EOF)
	throw CTTException("Error: premature end of the file \"rooms.dat\"");
      is >> room_vect[i];
    }
  is.close();

  // **********************************
  // Read curricula
  // **********************************

  is.open(CreatePath("curricula.dat"));
  groups = 0;
  CourseGroup dummy_group;
  while (true)
    {
      dummy_group.Clear();
      do
	{
	  ch = is.peek();
	  if (ch == '#' || ch == '\n')
	    is.getline(buffer,BUF_SIZE);      
	}
      while (ch == '#' || ch == '\n');      
      if (is.eof()) 
	break;
      is.getline(buffer,BUF_SIZE,',');
      dummy_group.SetName(buffer);
      is >> ch; is.putback(ch); // read comma and ignore blacks
      is.getline(buffer,BUF_SIZE,',');
      dummy_group.SetLongName(buffer);
      groups++;
      is >> group_size;
      unsigned i1, i2;
      for (i1 = 0; i1 < group_size; i1++)
	{
	  unsigned c1, c2;
	  is >> course_name;
	  c1 = CourseIndex(course_name);
	  dummy_group.AddMember(c1);
	  for (i2 = 0; i2 < i1; i2++)
	    {
	      c2 = dummy_group[i2];
	      conflict[c1][c2] = true;
	      conflict[c2][c1] = true;
	    }
	}
      group_vect.push_back(dummy_group);
      is.getline(buffer,BUF_SIZE); // discards the rest of the line
    }
  is.close();

  // **********************************
  // Read constraints
  // **********************************

  is.open(CreatePath("constraints.dat"));
  unsigned c, p;
  
  // Courses -- Periods
  while (!is.eof())
    {
      is >> course_name;
      if (course_name == "###")
	break;    
      if (course_name[0] == '#' || course_name == "")
	{
	  is.getline(buffer,BUF_SIZE); 
	  continue;
	}
      is >> period_name;
      p = PeriodIndex(period_name);
      c = CourseIndex(course_name);
      availability[c][p] = false;
    }

  // **********************************
  // Add same-teacher constraints
  // **********************************

  for (unsigned c1 = 0; c1 < courses - 1; c1++)
    for (unsigned c2 = c1+1; c2 < courses; c2++)
      if (course_vect[c1].Teacher() == course_vect[c2].Teacher())
	{
	  conflict[c1][c2] = true;
	  conflict[c2][c1] = true;
	}
}

ostream& operator<<(ostream& os, const Faculty& f)
{
  unsigned i, j;
  os << f.name << endl;
  os << "Courses : " << f.courses << endl;
  os << "Rooms : " << f.rooms << endl;
  os << "Periods : " << f.periods << " (" 
     << f.periods_per_day << " per day)" << endl;
  os << endl;

  os << "Courses: " << endl;
  for (i = 0; i < f.course_vect.size(); i++)
    os << f.course_vect[i] << endl;
  os << endl;
    
  os << "Rooms: " << endl;
  for (i = 1; i < f.room_vect.size(); i++)
    os << f.room_vect[i] << endl;
  os << endl;

  os << "Periods: " << endl;
  for (i = 0; i < f.period_vect.size(); i++)
    os << f.period_vect[i] << endl;
  os << endl;

  os << "Curricula: " << endl;
  for (i = 0; i < f.groups; i++)
    {
      os << f.group_vect[i].LongName() << " (" << f.group_vect[i].Name() << ") :  ";
      for (j = 0; j < f.group_vect[i].Size(); j++)
	os << f.course_vect[f.group_vect[i][j]].Name() << " ";
      os << endl;
    }
  os << endl;

  os << "Conflicts: " << endl;
  for (i = 0; i < f.conflict.size(); i++)
    {
      for (j = 0; j < f.conflict[i].size(); j++)
	if (f.conflict[i][j])
	  os << "YES";
	else 
	  os << "NO ";
      os << endl;
    }
  os << endl;
  
  os << "Course <--> Period Constraint: " << endl;
  for (i = 0; i < f.availability.size(); i++)
    {
      for (j = 0; j < f.availability[i].size(); j++)
	{
	  if (f.availability[i][j])
	    os << "NO ";
	  else
	    os << "YES";
	}
      os << endl;
    }
  os << endl;
  return os;
}

const char* Faculty::CreatePath(const string& file_name) const
{
  string s;
  s = dir_name + "/" + file_name;
  return s.c_str();
}

unsigned Faculty::CourseIndex(string name) const throw(CTTException)
{
  for (unsigned i = 0; i < course_vect.size(); i++)
    if (course_vect[i].Name() == name)
      return i;
  throw CTTException("Course " + name + " does not exists");
  return 0;    
}

unsigned Faculty::GroupIndex(string name) const throw(CTTException)
{
  for (unsigned i = 0; i < group_vect.size(); i++)
    if (group_vect[i].Name() == name)
      return i;
  throw CTTException("Group " + name + " does not exists");
  return 0;    
}


unsigned Faculty::PeriodIndex(string name) const throw(CTTException)
{
  for (unsigned i = 0; i < period_vect.size(); i++)
    {
      if (period_vect[i].Name() == name)
	return i;
    }
  throw CTTException("Period " + name + " does not exists");
  return 0;
}

unsigned Faculty::RoomIndex(string name) const throw(CTTException)
{
  for (unsigned i = 0; i < room_vect.size(); i++)
    if (room_vect[i].Name() == name)
      return i;
  throw CTTException("Room " + name + " does not exists");
  return 0;
}

