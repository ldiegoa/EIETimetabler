// File solver.cpp
#include "solver.hpp"

void TT_State::Allocate()
{
  Timetable::Allocate();
  room_lectures.resize(fp->Rooms() + 1, vector<unsigned>(fp->Periods()));
  course_daily_lectures.resize(fp->Courses(), vector<unsigned>(fp->Days()));
  working_days.resize(fp->Courses());	
}

ostream& operator<<(ostream& os, const TT_State& as)
{
  unsigned i, j;
  os << "TT_State: " << endl;
  for (i = 0; i < as.T.size(); i++)
    {
      for (j = 0; j < as.T[i].size(); j++)	
	os << setw(3) << as.T[i][j];
      os << endl;
    }
  os << endl;

  os << "Course daily lectures (redundant) " << endl;
  for (i = 0; i < as.course_daily_lectures.size(); i++)
    {
      for (j = 0; j < as.course_daily_lectures[i].size(); j++)	
	os << as.course_daily_lectures[i][j];
      os << endl;
    }
  os << endl;

  os << "Room lectures (redundant) " << endl;
  for (i = 0; i < as.room_lectures.size(); i++)
    {
      for (j = 0; j < as.room_lectures[i].size(); j++)	
	os << as.room_lectures[i][j] << ' ';
      os << endl;
    }
  os << endl;

  os << "Working days (redundant) " << endl;
  for (i = 0; i < as.working_days.size(); i++)
    os << as.working_days[i] << ' ';
  os << endl;
  return os;
}

TT_MoveTime::TT_MoveTime()
{
  course = 0;
  from = 0;
  to = 0;
}

TT_MoveTime::TT_MoveTime(unsigned c, unsigned f, unsigned t)
{
  course = c;
  from = f;
  to = t;
}

bool TT_MoveTime::operator==(const TT_MoveTime& c) const
{
  return course == c.course 
      && from == c.from
      && to == c.to;
}

bool TT_MoveTime::operator!=(const TT_MoveTime& c) const
{
  return course != c.course 
      || from != c.from
      || to != c.to;
}

istream& operator>>(istream& is, TT_MoveTime& c)
{
  char ch; 
  return is >> c.course >> ch >> c.from >> ch >> ch >> c.to;
}

ostream& operator<<(ostream& os, const TT_MoveTime& c)
{
  return os << c.course << ':' << c.from << "->" << c.to;
}

TT_MoveRoom::TT_MoveRoom()
{
  course = 0;
  period = 0;
  old_room = 0;
  new_room = 0;
}

TT_MoveRoom::TT_MoveRoom(unsigned c, unsigned p, unsigned o, unsigned n)
{
  course = c;
  period = p;
  old_room = o;
  new_room = n;
}

bool TT_MoveRoom::operator==(const TT_MoveRoom& c) const
{
  return course == c.course 
    && period == c.period
    && old_room == c.old_room
    && new_room == c.new_room;
}

bool TT_MoveRoom::operator!=(const TT_MoveRoom& c) const
{
  return course != c.course 
    || period != c.period
    || old_room != c.old_room
    || new_room != c.new_room;
}

istream& operator>>(istream& is, TT_MoveRoom& c)
{
  char ch; 
  return is >> ch >> c.course >> ch >> c.period >> ch 
	    >> c.old_room >> ch >> ch >> c.new_room;
}

ostream& operator<<(ostream& os, const TT_MoveRoom& c)
{
  return os << '[' << c.course << ':' << c.period 
	    << ']' << c.old_room << "->" << c.new_room;
}

// ***************************************************************************
//                               Helpers
// ***************************************************************************


// constructor
TT_StateManager::TT_StateManager(Faculty* pin) 
  : StateManager<Faculty,TT_State>(pin) 
{} 


// initial state builder (random rooms)
void TT_StateManager::RandomState(TT_State& as) 
{
  ResetState(as); // make all elements of as equal to 0
  for (unsigned c = 0; c < p_in->Courses(); c++)
    {
      unsigned lectures = p_in->CourseVector(c).Lectures();
      for (unsigned j = 0; j < lectures; j++)
	{
	  unsigned p;
	  do // cycle until the period is free and available
	    p = Random(0,p_in->Periods()-1);
	  while (as(c,p) != 0 || !p_in->Available(c,p));
	  as(c,p) = Random(1,p_in->Rooms());
	}
    }
  UpdateRedundantStateData(as);
} 

void TT_StateManager::ResetState(TT_State& as)
{
  for (unsigned c = 0; c < p_in->Courses(); c++)
    for (unsigned p = 0; p < p_in->Periods(); p++)
      as(c,p) = 0;
}

void TT_StateManager::UpdateRedundantStateData(TT_State& as) const
{
  unsigned p, c, r, d;
  
  for (r = 1; r < p_in->Rooms() + 1; r++)
    for (p = 0; p < p_in->Periods(); p++)
      as.ResetRoomLectures(r,p);

  for (c = 0; c < p_in->Courses(); c++)
    {
      for (p = 0; p < p_in->Periods(); p++)
	{
	  r = as(c,p);
	  if (r != 0)
	    as.IncRoomLectures(r,p);
	}
    }

  for (c = 0; c < p_in->Courses(); c++)
    {
      as.ResetWorkingDays(c);
      for (d = 0; d < p_in->Days(); d++)
      {
	as.ResetCourseDailyLectures(c,d);
	for (p = d * p_in->PeriodsPerDay(); 
	     p < (d+1) * p_in->PeriodsPerDay(); p++)
	  {
	    if (as(c,p) != 0)
	      as.IncCourseDailyLectures(c,d);
	  }
	if (as.CourseDailyLectures(c,d) >= 1)
	  as.IncWorkingDays(c);
      }
    }
} 

// cost function components

// violations
fvalue TT_StateManager::Violations(const TT_State& as) const 
{
  return Conflitcs(as) + RoomOccupation(as);
} 

fvalue TT_StateManager::Objective(const TT_State& as) const 
{
  return RoomCapacity(as) + MinWorkingDays(as);
} 

unsigned TT_StateManager::Conflitcs(const TT_State& as) const
{
  unsigned c1, c2, p, cost = 0;
  for (c1 = 0; c1 < p_in->Courses(); c1++)
    for (c2 = c1+1; c2 < p_in->Courses(); c2++)
      if (p_in->Conflict(c1,c2))
	{
	  for (p = 0; p < p_in->Periods(); p++)
	    if (as(c1,p) != 0 && as(c2,p) != 0)
	      cost++;
	}
  return cost;
}
          
unsigned TT_StateManager::RoomOccupation(const TT_State& as) const
{
  unsigned r, p, cost = 0;
  for (p = 0; p < p_in->Periods(); p++)
    for (r = 1; r <= p_in->Rooms(); r++)
      if (as.RoomLectures(r,p) > 1)
	cost += as.RoomLectures(r,p) - 1;
  return cost;
}

unsigned TT_StateManager::MinWorkingDays(const TT_State& as) const
{
  unsigned c, cost = 0;
  for (c = 0; c < p_in->Courses(); c++)
    if (as.WorkingDays(c) < p_in->CourseVector(c).MinWorkingDays())
      cost += p_in->CourseVector(c).MinWorkingDays() - as.WorkingDays(c);
  return cost;  
}

unsigned TT_StateManager::RoomCapacity(const TT_State& as) const
{
  unsigned c, p, r, cost = 0;
  for (c = 0; c < p_in->Courses(); c++)
    for (p = 0; p < p_in->Periods(); p++)
      {
	r = as(c,p);
	if (r != 0 && p_in->RoomVector(r).Capacity() < p_in->CourseVector(c).Students())
	  cost++;
      }
  return cost;
}


// print cost function components

// violations
void TT_StateManager::PrintViolations(ostream& os, const TT_State& as) const 
{
  PrintConflitcs(os,as);
  PrintRoomOccupation(os,as);
  PrintRoomCapacity(os,as); 
  PrintMinWorkingDays(os,as);
} 

void TT_StateManager::PrintConflitcs(ostream& os, const TT_State& as) const
{
  unsigned c1, c2, p;
  for (c1 = 0; c1 < p_in->Courses(); c1++)
    for (c2 = c1+1; c2 < p_in->Courses(); c2++)
      if (p_in->Conflict(c1,c2))
	{
	  for (p = 0; p < p_in->Periods(); p++)
	    if (as(c1,p) != 0 && as(c2,p) != 0)
	      os << "Courses " << p_in->CourseVector(c1).Name() << " and " 
		 <<  p_in->CourseVector(c2).Name() << " have both a lecture at " 
		 << p_in->PeriodVector(p).Name() << endl;
	}
}
          
void TT_StateManager::PrintRoomOccupation(ostream& os, const TT_State& as) const
{
  unsigned r, p;
  for (p = 0; p < p_in->Periods(); p++)
    for (r = 1; r <= p_in->Rooms(); r++)
      if (as.RoomLectures(r,p) > 1)	
	os << as.RoomLectures(r,p) << " lectures in room " 
	   << p_in->RoomVector(r).Name() << " the period " << p_in->PeriodVector(p).Name() << endl;
}

void TT_StateManager::PrintMinWorkingDays(ostream& os, const TT_State& as) const
{
  unsigned c;
  for (c = 0; c < p_in->Courses(); c++)
    if (as.WorkingDays(c) < p_in->CourseVector(c).MinWorkingDays())
      os << "The course " << p_in->CourseVector(c).Name() << " has only " << as.WorkingDays(c)
	 << " days of lecture" << endl;
}

void TT_StateManager::PrintRoomCapacity(ostream& os, const TT_State& as) const
{
  unsigned c, p, r;
  for (c = 0; c < p_in->Courses(); c++)
    for (p = 0; p < p_in->Periods(); p++)
      {
	r = as(c,p);
	if (r != 0 && p_in->RoomVector(r).Capacity() < p_in->CourseVector(c).Students())
	  os << "Room " << p_in->RoomVector(r).Name() << " too small for course " 
	     << p_in->CourseVector(c).Name() << " the period " 
	     << p_in->PeriodVector(p).Name() << endl;
      }
}

/*****************************************************************************
 * Output Manager Methods
 *****************************************************************************/

void TT_OutputManager::InputState(TT_State& as, const Timetable& tt) const 
{
  for (unsigned i = 0; i < p_in->Courses(); i++)
    for (unsigned j = 0; j < p_in->Periods(); j++)    
      as(i,j) = tt(i,j);
  p_sm->UpdateRedundantStateData(as);
}

void TT_OutputManager::OutputState(const TT_State& as, Timetable& tt) const 
{
  for (unsigned i = 0; i < p_in->Courses(); i++)
    for (unsigned j = 0; j < p_in->Periods(); j++)    
      tt(i,j) = as(i,j);
}

void TT_OutputManager::PrettyPrintOutput(const Timetable& tt, string dir_name) const
{
  unsigned i, j, c, r, p, days = p_in->Periods()/ p_in->PeriodsPerDay();
  string day_names[5] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
  string file_name;
  ofstream os;
  
  //  os << "<center><h1 class=\"title\">Orario studenti</h1></center>" << endl;
  for (i = 0; i < p_in->Groups(); i++)
    {
      file_name = dir_name + '/' + p_in->GroupVector(i).Name() + ".html";
      os.open(file_name.c_str());
      assert(!os.fail());
      os <<  "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n<html>\n<head>\n"
	 << "<link rel=\"stylesheet\" href=\"tt_style.css\" type=\"text/css\"></link>\n"
	 << "</head>\n\n  <body>\n<br><br>\n"     
	 << "<table BORDER CELLPADDING=6 WIDTH = \"100%\">\n<tr>\n<td COLSPAN=\"6\" class=\"header\">" 
	 << "<p class=\"course_group\"> " << p_in->GroupVector(i).LongName() << "</p>"
	 << "</td>\n</tr>\n\n"
	 << "<tr>" << endl
	 << "<td width = \"15%\" class=\"header\"><div class=\"hours\" align=\"right\">periods</div>"
	 << "<div class=\"days\" align=\"left\">days</div></td>\n"
	 << "<td width = \"20%\" class=\"header\"><center class=\"hours\">9-11</center></td>\n"
	 << "<td width = \"20%\" class=\"header\"><center class=\"hours\">11-13</center></td>\n"
	 << "<td width = \"5%\" class=\"header\">&nbsp;</td>\n"
	 << "<td width = \"20%\" class=\"header\"><center class=\"hours\">14.30-16.30</center></td>\n"
	 << "<td width = \"20%\" class=\"header\"><center class=\"hours\">16.30-18.30</center></td> \n</tr>";      
      for (j = 0; j < days; j++)
	{
	  os << "<tr>\n<td class=\"header\"><center class=\"days\">" + day_names[j] + "</center></td>\n";
	  for (p = j * p_in->PeriodsPerDay(); p < (j+1) * p_in->PeriodsPerDay(); p++)
	    {
	      if (p % p_in->PeriodsPerDay() == 2)
		os << "<td class=\"empty\">&nbsp;</td>\n";
	      if (GroupLecture(tt,i,p,c,r)) 
		{
		  os << "<td class=\"full\"><p class=\"subject\">" 
		     << p_in->CourseVector(c).LongName() << "<p class=\"teacher\">(" 
		     << p_in->CourseVector(c).Teacher() << ")<div class=\"room\">\n" 
		     << p_in->RoomVector(r).Name() << "</div></td>\n";
		}
	      else
		os << "<td class=\"empty\">&nbsp;</td>\n";
	    }
	  os << "</tr>\n\n";
	}
      os << "</table><br></br>\n</body>\n</html>\n";
      os.close();
    }

  for (r = 1; r <= p_in->Rooms(); r++)
    {
      file_name = dir_name + '/' + "Room" + p_in->RoomVector(r).Name() + ".html";
      os.open(file_name.c_str());
      assert(!os.fail());
      os <<  "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n<html>\n<head>\n"
	 << "<link rel=\"stylesheet\" href=\"tt_style.css\" type=\"text/css\"></link>\n"
	 << "</head>\n\n  <body>\n<br><br>\n"     
	 << "<table BORDER CELLPADDING=6 WIDTH = \"100%\">\n<tr>\n<td COLSPAN=\"6\" class=\"header\">" 
	 << "<p class=\"room\"> Room " << p_in->RoomVector(r).Name() << " (" << p_in->RoomVector(r).Capacity() << " seats)</p>"
	 << "</td>\n</tr>\n\n"
	 << "<tr>" << endl 
	 << "<td width = \"15%\" class=\"header\"><div class=\"hours\" align=\"right\">periods</div><div class=\"days\" align=\"left\">days</div></td>\n"
	 << "<td width = \"20%\" class=\"header\"><center class=\"hours\">9-11</center></td>\n"
	 << "<td width = \"20%\" class=\"header\"><center class=\"hours\">11-13</center></td>\n"
	 << "<td width = \"5%\" class=\"header\">&nbsp;</td>\n"
	 << "<td width = \"20%\" class=\"header\"><center class=\"hours\">14.30-16.30</center></td>\n"
	 << "<td width = \"20%\" class=\"header\"><center class=\"hours\">16.30-18.30</center></td> \n</tr>";      
      for (j = 0; j < days; j++)
	{
	  os << "<tr>\n<td class=\"header\"><center class=\"days\">" + day_names[j] + "</center></td>\n";
	  for (p = j * p_in->PeriodsPerDay(); p < (j+1) * p_in->PeriodsPerDay(); p++)
	    {
	      if (p % p_in->PeriodsPerDay() == 2)
		os << "<td class=\"empty\">&nbsp;</td>\n";
	      for (c = 0; c < p_in->Courses(); c++)
		if (tt(c,p) == r)
		{
		  os << "<td class=\"full\"><p class=\"subject\">" 
		     << p_in->CourseVector(c).LongName() << "<p class=\"teacher\">(" 
		     << p_in->CourseVector(c).Teacher() << ")"
		     << "</td>\n";
		  break;
		}
	      if (c == p_in->Courses()) // no lecture found
		os << "<td class=\"empty\">&nbsp;</td>\n";
	    }
	  os << "</tr>\n\n";
	}
      os << "</table><br></br>";
      os << "</body>\n</html>\n";
      os.close();
    }

  // file of empty rooms
  file_name = dir_name + "/EmptyRooms.html";
  os.open(file_name.c_str());
  assert(!os.fail());
  os <<  "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n<html>\n<head>\n"
     << "<link rel=\"stylesheet\" href=\"tt_style.css\" type=\"text/css\"></link>\n"
     << "</head>\n\n  <body>\n<br><br>\n"     
     << "<table BORDER CELLPADDING=6 WIDTH = \"100%\">\n<tr>\n<td COLSPAN=\"6\" class=\"header\">" 
     << "<p class=\"room\"> Free rooms</p>"
     << "</td>\n</tr>\n\n"
     << "<tr>" << endl 
     << "<td width = \"15%\" class=\"header\"><div class=\"hours\" align=\"right\">periods</div><div class=\"days\" align=\"left\">days</div></td>\n"
     << "<td width = \"20%\" class=\"header\"><center class=\"hours\">9-11</center></td>\n"
     << "<td width = \"20%\" class=\"header\"><center class=\"hours\">11-13</center></td>\n"
     << "<td width = \"5%\" class=\"header\">&nbsp;</td>\n"
     << "<td width = \"20%\" class=\"header\"><center class=\"hours\">14.30-16.30</center></td>\n"
     << "<td width = \"20%\" class=\"header\"><center class=\"hours\">16.30-18.30</center></td> \n</tr>";      
  for (j = 0; j < days; j++)
    {
      os << "<tr>\n<td class=\"header\"><center class=\"days\">" + day_names[j] + "</center></td>\n";
      for (p = j * p_in->PeriodsPerDay(); p < (j+1) * p_in->PeriodsPerDay(); p++)
	{
	  if (p % p_in->PeriodsPerDay() == 2)
	    os << "<td class=\"empty\">&nbsp;</td>\n";
	  os << "<td class=\"full\">";
	  unsigned count = 0;
	  for (r = 1; r <= p_in->Rooms(); r++)
	    {	    
	      for (c = 0; c < p_in->Courses(); c++)
		if (tt(c,p) == r)
		  break;
	      if (c == p_in->Courses()) // the room r is free
		{
		  os << p_in->RoomVector(r).Name() << " ";
		  count++;
		}
	    }
	  if (count == 0)
	    os << "&nbsp; ";
	  os << "</td>";
	}
      os << "</tr>\n\n";
    }
  os << "</table><br></br>";
  os << "</body>\n</html>\n";
  os.close();
}

bool TT_OutputManager::Member(unsigned e, const vector<unsigned> & v) const
{
  for (unsigned i = 0; i < v.size(); i++)
    if (v[i] == e)
      return true;
  return false;
}

bool TT_OutputManager::GroupLecture(const Timetable & tt, unsigned g, unsigned p, unsigned& c, unsigned& r) const
  // find the lecture (ie. course c and room r) of the group i in this period
  // return false if it does not exist. Used by PrettyPrintOutput
{
  const CourseGroup& group = p_in->GroupVector(g);
  for (unsigned i = 0; i < group.Size(); i++)
    if (tt(group[i],p) != 0)
      {
	c = group[i];
	r = tt(group[i],p);
	return true;
      }
  return false;
}


/*****************************************************************************
 * Time Neighborhood Explorer Methods
 *****************************************************************************/

// constructor
TT_TimeNeighborhoodExplorer::TT_TimeNeighborhoodExplorer(StateManager<Faculty,TT_State>* psm, 
							   Faculty* pin)
  : NeighborhoodExplorer<Faculty,TT_State,TT_MoveTime>(psm, pin) 
{} 

// initial move builder
void TT_TimeNeighborhoodExplorer::RandomMove(const TT_State& as, TT_MoveTime& mv) 
{
  do 
    AnyRandomMove(as,mv);
  while (!FeasibleMove(as,mv));
}

// initial move builder
void TT_TimeNeighborhoodExplorer::AnyRandomMove(const TT_State& as, TT_MoveTime& mv) 
{
  mv.course = Random(0,p_in->Courses() - 1);
  mv.from = Random(0,p_in->Periods() - 1);
  mv.to = Random(0,p_in->Periods() - 1);
} 

// check move feasibility
bool TT_TimeNeighborhoodExplorer::FeasibleMove(const TT_State& as, const TT_MoveTime& mv) 
{
  return  as(mv.course,mv.from) != 0 
    && as(mv.course,mv.to) == 0
    && !p_in->Available(mv.course,mv.to);
} 

// update the state according to the move 
void TT_TimeNeighborhoodExplorer::MakeMove(TT_State& as, const TT_MoveTime& mv) 
{
     // update the state matrix
  unsigned room = as(mv.course,mv.from);
  as(mv.course,mv.to) = room;
  as(mv.course,mv.from) = 0;

     // update the redundant data
  unsigned from_day = mv.from / p_in->PeriodsPerDay();
  unsigned to_day= mv.to / p_in->PeriodsPerDay();
  as.DecRoomLectures(room,mv.from);
  as.IncRoomLectures(room,mv.to);

  if (from_day != to_day)
    {
      as.DecCourseDailyLectures(mv.course,from_day);
      as.IncCourseDailyLectures(mv.course,to_day);
      if (as.CourseDailyLectures(mv.course,from_day) == 0)
	as.DecWorkingDays(mv.course);
      if (as.CourseDailyLectures(mv.course,to_day) == 1)
	as.IncWorkingDays(mv.course);  
    }
}


// compute the next move in the exploration of the neighborhood
void TT_TimeNeighborhoodExplorer::NextMove(const TT_State& as, TT_MoveTime& mv) 
{
  do 
    AnyNextMove(as,mv);
  while (!FeasibleMove(as,mv));
}

void TT_TimeNeighborhoodExplorer::AnyNextMove(const TT_State& as, TT_MoveTime& mv) 
{
  if (mv.to < p_in->Periods() - 1)
    mv.to++;
  else
    if (mv.from < p_in->Periods() - 1)
      {
	mv.from++;
	mv.to = 0;
      }
  else
    {    
      mv.course = (mv.course + 1) % p_in->Courses();
      mv.from = 0;
      mv.to = 1;
    }
}


fvalue TT_TimeNeighborhoodExplorer::DeltaViolations(const TT_State& as, const TT_MoveTime& mv) //const
{
  return DeltaConflitcs(as,mv) + DeltaRoomOccupation(as,mv);
} 

fvalue TT_TimeNeighborhoodExplorer::DeltaObjective(const TT_State& as, const TT_MoveTime& mv) //const
{
  // no change in room capacity
  return DeltaMinWorkingDays(as,mv);
} 

int TT_TimeNeighborhoodExplorer::DeltaConflitcs(const TT_State& as, const TT_MoveTime& mv) const
{
  unsigned c;
  int cost = 0;
 
  for (c = 0; c < p_in->Courses(); c++)
    {
      if (c == mv.course) continue;
      if (p_in->Conflict(c,mv.course))
	{
	  if (as(c,mv.from) != 0)
	    cost--;
	  if (as(c,mv.to) != 0)
	    cost++;
	}
    }
  return cost;
}
          
int TT_TimeNeighborhoodExplorer::DeltaRoomOccupation(const TT_State& as, const TT_MoveTime& mv) const
{
  unsigned r;
  int cost = 0;

  r = as(mv.course,mv.from);
  if (as.RoomLectures(r,mv.from) > 1)
    cost--;
  if (as.RoomLectures(r,mv.to) > 0)
    cost++;
  return cost;
}

int TT_TimeNeighborhoodExplorer::DeltaMinWorkingDays(const TT_State& as, 
						    const TT_MoveTime& mv) const
{
  unsigned from_day = mv.from / p_in->PeriodsPerDay();
  unsigned to_day = mv.to / p_in->PeriodsPerDay();

  if (from_day == to_day)
    return 0;
  if (as.WorkingDays(mv.course) <= p_in->CourseVector(mv.course).MinWorkingDays()
      && as.CourseDailyLectures(mv.course,from_day) == 1
      && as.CourseDailyLectures(mv.course,to_day) >= 1)
    return 1;
  if (as.WorkingDays(mv.course) < p_in->CourseVector(mv.course).MinWorkingDays()
      && as.CourseDailyLectures(mv.course,from_day) > 1
      && as.CourseDailyLectures(mv.course,to_day) == 0)
    return -1;
  return 0;
}


/*****************************************************************************
 * Time Tabu List Manager Methods
 *****************************************************************************/

// constructor
TT_TimeTabuListManager::TT_TimeTabuListManager(int min, int max)
  : TabuListManager<TT_MoveTime>(min,max) 
{}

// the inverse move definition
bool TT_TimeTabuListManager::Inverse(const TT_MoveTime& m1, const TT_MoveTime& m2) const 
{
  return m1.course == m2.course && (m1.from == m2.to || m2.from == m1.to); 
} 

/*****************************************************************************
 * Room Neighborhood Explorer Methods
 *****************************************************************************/

// constructor
TT_RoomNeighborhoodExplorer::TT_RoomNeighborhoodExplorer(StateManager<Faculty,TT_State>* psm, 
							   Faculty* pin)
  : NeighborhoodExplorer<Faculty,TT_State,TT_MoveRoom>(psm, pin) 
{} 

// initial move builder
void TT_RoomNeighborhoodExplorer::RandomMove(const TT_State& as, TT_MoveRoom& mv) 
{
  mv.course = Random(0,p_in->Courses() - 1);

  do 
    mv.period = Random(0,p_in->Periods() - 1);
  while (as(mv.course,mv.period) == 0);
  mv.old_room = as(mv.course,mv.period);
  
  do 
    mv.new_room = Random(1,p_in->Rooms());
  while (mv.new_room == mv.old_room);
} 

bool TT_RoomNeighborhoodExplorer::FeasibleMove(const TT_State& as, const TT_MoveRoom& mv) 
{
  return as(mv.course,mv.period) == mv.old_room;
}

// update the state according to the move 
void TT_RoomNeighborhoodExplorer::MakeMove(TT_State& as, const TT_MoveRoom& mv) 
{
  assert (as(mv.course,mv.period) == mv.old_room);
  as(mv.course,mv.period) = mv.new_room;
  as.DecRoomLectures(mv.old_room,mv.period);
  as.IncRoomLectures(mv.new_room,mv.period);
} 

// compute the next move in the exploration of the neighborhood
void TT_RoomNeighborhoodExplorer::NextMove(const TT_State& as, TT_MoveRoom& mv) 
{
  mv.new_room++;
  if (mv.new_room == mv.old_room)
     mv.new_room++;
  if (mv.new_room <= p_in->Rooms()) return;
  do 
    mv.period++;
  while (as(mv.course,mv.period) == 0 && mv.period < p_in->Periods());
  if (mv.period < p_in->Periods())
    {
      mv.old_room = as(mv.course,mv.period);
      mv.new_room = 1;
      if (mv.new_room == mv.old_room)
	mv.new_room++;
      return;
    }
  mv.course = (mv.course + 1) % p_in->Courses();
  mv.period = 0;

  while (as(mv.course,mv.period) == 0)
    mv.period++;
  mv.old_room = as(mv.course,mv.period);
  mv.new_room = 1;
  if (mv.new_room == mv.old_room)
    mv.new_room++;
} 

// local variation components of the cost function 

// violations
fvalue TT_RoomNeighborhoodExplorer::DeltaViolations(const TT_State& as, const TT_MoveRoom& mv) //const
{
  return DeltaRoomOccupation(as,mv);
} 

fvalue TT_RoomNeighborhoodExplorer::DeltaObjective(const TT_State& as, const TT_MoveRoom& mv) //const
{
  return DeltaRoomCapacity(as,mv);
} 

int TT_RoomNeighborhoodExplorer::DeltaRoomOccupation(const TT_State& as, const TT_MoveRoom& mv) const
{
  int cost = 0;
  if (as.RoomLectures(mv.old_room,mv.period) > 1)
    cost--;
  if (as.RoomLectures(mv.new_room,mv.period) > 0)
    cost++;
  return cost;
}

int TT_RoomNeighborhoodExplorer::DeltaRoomCapacity(const TT_State& as, const TT_MoveRoom& mv) const
{
  int cost = 0;
  if (p_in->RoomVector(mv.old_room).Capacity() < p_in->CourseVector(mv.course).Students())
    cost--;
  if (p_in->RoomVector(mv.new_room).Capacity() < p_in->CourseVector(mv.course).Students())
    cost++;
  return cost;
}

/*****************************************************************************
 * Room Tabu List Manager Methods
 *****************************************************************************/

// constructor
TT_RoomTabuListManager::TT_RoomTabuListManager(int min, int max)
  : TabuListManager<TT_MoveRoom>(min,max) 
{}

// the inverse move definition
bool TT_RoomTabuListManager::Inverse(const TT_MoveRoom& m1, const TT_MoveRoom& m2) const 
{
  return m1.course == m2.course && m1.period == m2.period;
} 

#ifdef NO_MINI
TT_TimeRoomKicker::TT_TimeRoomKicker(TT_TimeNeighborhoodExplorer *tnhe, 
				   TT_RoomNeighborhoodExplorer *rnhe)
    : BimodalKicker<Faculty,TT_State,TT_MoveTime,TT_MoveRoom>(tnhe,rnhe,5) 
{}

bool TT_TimeRoomKicker::RelatedMoves(const TT_MoveTime &mv1, const TT_MoveTime &mv2)
{
  //  cerr << mv1 << "   " << mv2 << endl;
  return // mv1.from == mv2.to || 
    mv1.to == mv2.from;
}

bool TT_TimeRoomKicker::RelatedMoves(const TT_MoveTime &mv1, const TT_MoveRoom &mv2)
{
  //  cerr << mv1 << "   " << mv2 << endl;
  return mv1.to == mv2.period;
}

bool TT_TimeRoomKicker::RelatedMoves(const TT_MoveRoom &mv1, const TT_MoveTime &mv2)
{
  //  cerr << mv1 << "   " << mv2 << endl;
  return mv1.period == mv2.from;
}

bool TT_TimeRoomKicker::RelatedMoves(const TT_MoveRoom &mv1, const TT_MoveRoom &mv2)
{
  //cerr << mv1 << "   " << mv2 << endl;
  return mv1.period == mv2.period && mv1.new_room == mv2.old_room
       && mv1.course != mv2.course;
}
#endif


// ***************************************************************************
//                               Runners
// ***************************************************************************

/*****************************************************************************
 * Time Tabu Search Runner Methods
 *****************************************************************************/

// constructor
TT_TimeTabuSearch::TT_TimeTabuSearch(StateManager<Faculty,TT_State>* psm, NeighborhoodExplorer<Faculty,TT_State,TT_MoveTime>* pnhe, TabuListManager<TT_MoveTime>* ptlm, Faculty* pin)
  : TabuSearch<Faculty,TT_State,TT_MoveTime>(psm,pnhe,ptlm,pin) 
{
  SetName("TS-Timetabler");
}

/*****************************************************************************
 * Time Hill Climbing Runner Methods
 *****************************************************************************/

// constructor
TT_TimeHillClimbing::TT_TimeHillClimbing(StateManager<Faculty,TT_State>* psm, NeighborhoodExplorer<Faculty,TT_State,TT_MoveTime>* pnhe, Faculty* pin)
  : HillClimbing<Faculty,TT_State,TT_MoveTime>(psm,pnhe,pin) 
{
  SetName("HC-Timetabler");
}

/*****************************************************************************
 * Room Tabu Search Runner Methods
 *****************************************************************************/

// constructor
TT_RoomTabuSearch::TT_RoomTabuSearch(StateManager<Faculty,TT_State>* psm, 
				   NeighborhoodExplorer<Faculty,TT_State,TT_MoveRoom>* pnhe, 
				   TabuListManager<TT_MoveRoom>* ptlm, Faculty* pin)
  : TabuSearch<Faculty,TT_State,TT_MoveRoom>(psm,pnhe,ptlm,pin) 
{
   SetName("TS-Roomtabler");
}


/*****************************************************************************
 * Room Hill Climbing Runner Methods
 *****************************************************************************/

// constructor
TT_RoomHillClimbing::TT_RoomHillClimbing(StateManager<Faculty,TT_State>* psm, NeighborhoodExplorer<Faculty,TT_State,TT_MoveRoom>* pnhe, Faculty* pin)
  : HillClimbing<Faculty,TT_State,TT_MoveRoom>(psm,pnhe,pin) 
{
   SetName("HC-Roomtabler");
}

/*****************************************************************************
 * Token Ring Solver Methods
 *****************************************************************************/

TT_TokenRingSolver::TT_TokenRingSolver(StateManager<Faculty,TT_State>* psm, 
						 OutputManager<Faculty,Timetable,TT_State>* pom, 
						 Faculty* pin, Timetable* pout)
  :  TokenRingSolver<Faculty,Timetable,TT_State>(psm,pom,pin,pout) 
{

}

