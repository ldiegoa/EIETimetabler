// File solver.hpp
#ifndef TT_SOLVER_HPP
#define TT_SOLVER_HPP

#include "faculty.hpp"
#include <EasyLocal.h>
#include <EasyLocalTemplates.cpp>

using namespace easylocal;

class TT_State : public Timetable
{  friend ostream& operator<<(ostream&, const TT_State&);
public:
  TT_State(Faculty *f = NULL) : Timetable(f) {}

  unsigned RoomLectures(unsigned i, unsigned j) const 
          { return room_lectures[i][j]; }
  void ResetRoomLectures(unsigned i, unsigned j) { room_lectures[i][j] = 0; }
  void IncRoomLectures(unsigned i, unsigned j) { room_lectures[i][j]++; }
  void DecRoomLectures(unsigned i, unsigned j) { room_lectures[i][j]--; }

  unsigned CourseDailyLectures(unsigned i, unsigned j) const 
    { return course_daily_lectures[i][j]; }
  void ResetCourseDailyLectures(unsigned i, unsigned j) 
    { course_daily_lectures[i][j] = 0; }
  void IncCourseDailyLectures(unsigned i, unsigned j) 
    { course_daily_lectures[i][j]++; }
  void DecCourseDailyLectures(unsigned i, unsigned j) 
    { course_daily_lectures[i][j]--; }

  unsigned WorkingDays(unsigned i) const 
          { return working_days[i]; }
  void ResetWorkingDays(unsigned i) { working_days[i] = 0; }
  void IncWorkingDays(unsigned i) { working_days[i]++; }
  void DecWorkingDays(unsigned i) { working_days[i]--; }

 protected:
  void Allocate();

  // redundant data
  vector<vector<unsigned> > room_lectures; // number of lectures per room in the same period (should be 0 or 1)

  vector<vector<unsigned> > course_daily_lectures; // number of lectures per course per day
  vector<unsigned> working_days; // number of days of lecture per course
};

class TT_MoveTime
{
  friend ostream& operator<<(ostream& os, const TT_MoveTime& c);
  friend istream& operator>>(istream& is, TT_MoveTime& c);
 public:
  TT_MoveTime();
  TT_MoveTime(unsigned, unsigned, unsigned);
  bool operator==(const TT_MoveTime&) const;
  bool operator!=(const TT_MoveTime&) const;
  unsigned course, from, to;
};

class TT_MoveRoom
{
  friend ostream& operator<<(ostream& os, const TT_MoveRoom& c);
  friend istream& operator>>(istream& is, TT_MoveRoom& c);
 public:
  TT_MoveRoom();
  TT_MoveRoom(unsigned, unsigned, unsigned, unsigned);
  bool operator==(const TT_MoveRoom&) const;
  bool operator!=(const TT_MoveRoom&) const;
  unsigned course, period, old_room, new_room;
};

/***************************************************************************
 * State Manager 
 ***************************************************************************/

class TT_StateManager : public StateManager<Faculty,TT_State> 
{
public:
  TT_StateManager(Faculty*);
  void RandomState(TT_State&);   // mustdef 
protected:
  fvalue Violations(const TT_State& as) const;   // mayredef 
  fvalue Objective(const TT_State& as) const;    // mayredef 

  void UpdateRedundantStateData(TT_State& as) const;  // mayredef 
  void ResetState(TT_State& as);

  unsigned Conflitcs(const TT_State& as) const;
  unsigned RoomOccupation(const TT_State& as) const;
  unsigned RoomCapacity(const TT_State& as) const;
  unsigned MinWorkingDays(const TT_State& as) const;

  void PrintObjective(ostream& os, const TT_State& as) const {}  // mustdef 
  void PrintViolations(ostream& os, const TT_State& as) const;   // mustdef 

  void PrintConflitcs(ostream& os, const TT_State& as) const;
  void PrintRoomOccupation(ostream& os, const TT_State& as) const;
  void PrintRoomCapacity(ostream& os, const TT_State& as) const;
  void PrintMinWorkingDays(ostream& os, const TT_State& as) const;
}; 

/***************************************************************************
 * Time Neighborhood Explorer:
 ***************************************************************************/

class TT_TimeNeighborhoodExplorer
  : public NeighborhoodExplorer<Faculty,TT_State,TT_MoveTime> 
{
public:
  TT_TimeNeighborhoodExplorer(StateManager<Faculty,TT_State>*, Faculty*);
  void RandomMove(const TT_State&, TT_MoveTime&);          // mustdef 
  bool FeasibleMove(const TT_State&, const TT_MoveTime&);  // mayredef 
  void MakeMove(TT_State&,const TT_MoveTime&);             // mustdef 
protected:
  fvalue DeltaViolations(const TT_State&, const TT_MoveTime&); // mayredef 
  fvalue DeltaObjective(const TT_State&, const TT_MoveTime&);  // mayredef 

  int DeltaConflitcs(const TT_State& as, const TT_MoveTime& mv) const;
  int DeltaRoomOccupation(const TT_State& as, const TT_MoveTime& mv) const;
  int DeltaMinWorkingDays(const TT_State& as, const TT_MoveTime& mv) const;
  void NextMove(const TT_State&,TT_MoveTime&);   // mustdef 
private:
  void AnyNextMove(const TT_State&,TT_MoveTime&);
  void AnyRandomMove(const TT_State&, TT_MoveTime&);
};
  
/***************************************************************************
 * Time Tabu List Manager:
 ***************************************************************************/
class TT_TimeTabuListManager
  : public TabuListManager<TT_MoveTime> 
{
public:
  TT_TimeTabuListManager(int min = 0, int max = 0);
protected:
  bool Inverse(const TT_MoveTime&,const TT_MoveTime&) const; // mustdef 
private:
}; 
 
/***************************************************************************
 * Room Neighborhood Explorer:
 ***************************************************************************/
class TT_RoomNeighborhoodExplorer
  : public NeighborhoodExplorer<Faculty,TT_State,TT_MoveRoom> 
{
public:
  TT_RoomNeighborhoodExplorer(StateManager<Faculty,TT_State>*, Faculty*);
  void RandomMove(const TT_State&, TT_MoveRoom&);        // mustdef 
  bool FeasibleMove(const TT_State&, const TT_MoveRoom&);// mayredef 
  void MakeMove(TT_State&,const TT_MoveRoom&);           // mustdef 
protected:
  fvalue DeltaViolations(const TT_State&, const TT_MoveRoom&); // mayredef 
  fvalue DeltaObjective(const TT_State&, const TT_MoveRoom&);  // mayredef 

  int DeltaRoomOccupation(const TT_State& as, const TT_MoveRoom& mv) const;
  int DeltaRoomCapacity(const TT_State& as, const TT_MoveRoom& mv) const;

  void NextMove(const TT_State&, TT_MoveRoom&);  // mustdef 
};
  
/***************************************************************************
 * Room Tabu List Manager:
 ***************************************************************************/
class TT_RoomTabuListManager
  : public TabuListManager<TT_MoveRoom> 
{
public:
  TT_RoomTabuListManager(int min = 0, int max = 0);
protected:
  bool Inverse(const TT_MoveRoom&,const TT_MoveRoom&) const; // mustdef 
}; 
 
/***************************************************************************
 * Output Manager:
 ***************************************************************************/
class TT_OutputManager
  : public OutputManager<Faculty,Timetable,TT_State> 
{
public:
  TT_OutputManager(StateManager<Faculty,TT_State>* psm, Faculty* pin)
    : OutputManager<Faculty,Timetable,TT_State>(psm,pin) {}
  void InputState(TT_State&, const Timetable&) const;  // mustdef 
  void OutputState(const TT_State&, Timetable&) const; // mustdef 
  void PrettyPrintOutput(const Timetable&, string s) const; // mayredef 
private:
  bool GroupLecture(const Timetable & tt, unsigned g, unsigned p, unsigned& c, unsigned& r) const;
  bool Member(unsigned e, const vector<unsigned> & v) const;
}; 

/***************************************************************************
 * Kickers:
 ***************************************************************************/

#ifdef NO_MINI
class TT_TimeRoomKicker : public BimodalKicker<Faculty,TT_State,TT_MoveTime,TT_MoveRoom>
{
public:
  TT_TimeRoomKicker(TT_TimeNeighborhoodExplorer *tnhe, TT_RoomNeighborhoodExplorer *rnhe);
  bool RelatedMoves(const TT_MoveTime &mv1, const TT_MoveTime &mv2); // mustdef 
  bool RelatedMoves(const TT_MoveTime &mv1, const TT_MoveRoom &mv2); // mustdef 
  bool RelatedMoves(const TT_MoveRoom &mv1, const TT_MoveTime &mv2); // mustdef 
  bool RelatedMoves(const TT_MoveRoom &mv1, const TT_MoveRoom &mv2); // mustdef 
};
#endif

/***************************************************************************
 * Time Tabu Search Runner:
 ***************************************************************************/
class TT_TimeTabuSearch
  : public TabuSearch<Faculty,TT_State,TT_MoveTime> 
{
public:
  TT_TimeTabuSearch(StateManager<Faculty,TT_State>* psm,
		NeighborhoodExplorer<Faculty,TT_State,TT_MoveTime>* pnhe, 
		TabuListManager<TT_MoveTime>* ptlm, Faculty* pin);
};

/***************************************************************************
 * Time Hill Climbing Runner:
 ***************************************************************************/
class TT_TimeHillClimbing
  : public HillClimbing<Faculty,TT_State,TT_MoveTime> 
{
public:
  TT_TimeHillClimbing(StateManager<Faculty,TT_State>* psm,
		       NeighborhoodExplorer<Faculty,TT_State,TT_MoveTime>* pnhe,
		       Faculty* pin);
};

/***************************************************************************
 * Room Tabu Search Runner:
 ***************************************************************************/
class TT_RoomTabuSearch
  : public TabuSearch<Faculty,TT_State,TT_MoveRoom> 
{
public:
  TT_RoomTabuSearch(StateManager<Faculty,TT_State>* psm,
		NeighborhoodExplorer<Faculty,TT_State,TT_MoveRoom>* pnhe, 
		TabuListManager<TT_MoveRoom>* ptlm, Faculty* pin);
};
 
/***************************************************************************
 * Room Hill Climbing Runner:
 ***************************************************************************/
class TT_RoomHillClimbing
  : public HillClimbing<Faculty,TT_State,TT_MoveRoom> 
{
public:
  TT_RoomHillClimbing(StateManager<Faculty,TT_State>* psm,
		       NeighborhoodExplorer<Faculty,TT_State,TT_MoveRoom>* pnhe,
		       Faculty* pin);
};

/***************************************************************************
 * The Token Ring Solver
 ***************************************************************************/
class TT_TokenRingSolver
  : public TokenRingSolver<Faculty,Timetable,TT_State> 
{
public:
  TT_TokenRingSolver(StateManager<Faculty,TT_State>*,
		    OutputManager<Faculty,Timetable,TT_State>*,
		    Faculty*,Timetable*);
}; 

#endif
