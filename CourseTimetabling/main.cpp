// File main.cpp
#include "faculty.hpp"
#include "solver.hpp"


using namespace easylocal;

extern AbstractTester* main_tester;

int main(int argc, char** argv)
{

  Faculty in; 
  if (argc == 1)
    {
      char term;
      string data_dir = "Data"; 
      do
	{
	  cout << "Which term (only term 1 is available) ? : ";      
	  cin >> term;
	  if (term != '1')
	    cout << "Data of term " << term << " are  not available " << endl;
	}
      while (term != '1');
      data_dir += term; 
      try 
	{
	  in.Load(data_dir);
	} 
      catch (CTTException& e) 
	{
	  cerr << e.what() << endl;
	  exit(-1);
	}
      cout << "Load data of term " << term << " (done)" << endl;
    }

  Timetable out(&in);

    // helpers
  TT_StateManager sm(&in);
  TT_TimeTabuListManager ttlm;
  TT_TimeNeighborhoodExplorer tnhe(&sm,&in);
  TT_RoomTabuListManager rtlm;
  TT_RoomNeighborhoodExplorer rnhe(&sm,&in);
  TT_OutputManager om(&sm,&in);
#ifdef NO_MINI
  TT_TimeRoomKicker trk(&tnhe,&rnhe);
#endif

   // runners 
  TT_TimeHillClimbing thc(&sm,&tnhe,&in); 
  TT_TimeTabuSearch tts(&sm,&tnhe,&ttlm,&in);

  TT_RoomHillClimbing rhc(&sm,&rnhe,&in); 
  TT_RoomTabuSearch rts(&sm,&rnhe,&rtlm,&in);

   // solvers
  TT_TokenRingSolver trs(&sm,&om,&in,&out);

   // testers
  StateTester<Faculty,Timetable,TT_State> state_test(&sm,&om,&in);
  MoveTester<Faculty,Timetable,TT_State,TT_MoveTime> time_move_test(&sm,&om,&tnhe,"Change Time",&in);
  MoveTester<Faculty,Timetable,TT_State,TT_MoveRoom> room_move_test(&sm,&om,&rnhe,"Change Room",&in);
#ifdef NO_MINI
  BimodalKickTester<Faculty,Timetable,TT_State,TT_MoveTime,TT_MoveRoom> time_room_kick_test(&sm,&om,&trk,"Kick Time/Room",&in);
#endif

  Tester<Faculty,Timetable,TT_State> tester(&sm,&om,&in);

  tester.SetStateTester(&state_test);
  tester.AddMoveTester(&time_move_test);
  tester.AddMoveTester(&room_move_test);
#ifdef NO_MINI
  tester.AddMoveTester(&time_room_kick_test);
#endif

  tester.AddRunner(&thc); 
  tester.AddRunner(&tts);
  tester.AddRunner(&rhc); 
  tester.AddRunner(&rts);

  tester.SetSolver(&trs);
  tester.SetInput(&in);

  main_tester = &tester;


  assert(in.PeriodsPerDay() == 6);

  if (argc == 1)
    tester.RunMainMenu();
  else
    tester.ProcessBatch(argv[1]);
}
