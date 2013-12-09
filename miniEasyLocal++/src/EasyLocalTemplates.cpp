/**
   @file EasyLocalTemplates.cpp
   @brief Template implementation

   This file contains all the definition of the EasyLocal++
   template classes/functions.
   
   @author Andrea Schaerf (schaerf@uniud.it), 
           Luca Di Gaspero (digasper@dimi.uniud.it)
   @version 0.2
   @date 15 Jun 20.2
   @note This version works both with MS Visual C++ and the GNU C++ 
         compiler. Yet, it is extensively tested only with the GNU compiler.
 */

#include <cstdlib>
#include <climits>
namespace easylocal {

  /** 
      Computes the distance between two values in the codomain of the
      cost function and returns it as a double precision value.

      @param x first value
      @param y second value
      @return the distance between x and y
  */
  inline double distance(fvalue x, fvalue y)
  {
    return x > y ? x-y : y-x;
  }

  // State Manager functions

  /**
     Looks for the best state out of a given number of sampled
     states.
     
     @param st the best state found
     @param samples the number of sampled states
   */
  template <class Input, class State>
  fvalue StateManager<Input,State>::SampleState(State &st, int samples)
  { 
    int s = 1;
    RandomState(st);
    fvalue cost = CostFunction(st);
    State best_state = st;
    fvalue best_cost = cost;
    while (s < samples)
      { 
	RandomState(st);
	cost = CostFunction(st);
	if (cost < best_cost)
	  { 
	    best_state = st;
	    best_cost = cost;
	  }
	s++;
      }
    st = best_state;
    return best_cost;
  }

  /** 
      Looks for the best state out of a given number of states
      which are obtained by random generation and are improved 
      running a local search algorithm upon them.
      
      @param st the best state found
      @param samples the number of sampled states
      @param r a pointer to a runner to be used to improve the sampled
  */
  template <class Input, class State>
  fvalue StateManager<Input,State>::ImprovedSampleState(State &st, int samples, Runner<Input,State>* r)
  { // same of SampleState, except that each sample is let run using r
    // before comparing its value
    int s = 1;
    RandomState(st);
    r->SetCurrentState(st);
    r->Go();
    st = r->GetCurrentState();
    fvalue cost = CostFunction(st);
    State best_state = st;
    fvalue best_cost = cost;
    do 
      { 
	cost = CostFunction(st);
	if (cost < best_cost)
	  { 
	    best_state = st;
	    best_cost = cost;
	  }
	RandomState(st);
	r->SetCurrentState(st);
	r->Go();
	st = r->GetCurrentState();
	s++;
      }
    while (s <= samples);
    st = best_state;
    return best_cost;
  }

  /** 
      Evaluates the cost function value in a given state.  
      The tentative definition computes a weighted sum of the violation 
      function and the objective function.
     
     @param st the state to be evaluated
     @return the value of the cost function in the given state */
  template <class Input, class State>
  fvalue StateManager<Input,State>::CostFunction(const State& st) const 
  { return HARD_WEIGHT * Violations(st) + Objective(st); } 

  /** 
      Outputs some informations about the state passed as parameter.

      @param st the state to be inspected
  */
  template <class Input, class State>
  void StateManager<Input,State>::PrintState(const State& st) const
  {
    std::cout << st << std::endl;
    std::cout << "Total cost : " << CostFunction(st) << std::endl;
    std::cout << "    Violations : " << Violations(st) << std::endl;
    std::cout << "    Objective : " << Objective(st) << std::endl;
  }

  /**
     A tentative definition of the violation function: 
     by default, it simply returns 0 and outputs a warning message.

     @param st the state to be evaluated
     @return the value of the violations function in the given state
  */
  template <class Input, class State>  
  fvalue StateManager<Input,State>::Violations(const State& st) const
  { 
    std::cout << "Warning: violations function not implemented yet!" << std::endl;
    return 0; 
  }

   /**
     A tentative definition of the objective function: 
     by default, it simply returns 0 and outputs a warning message.

     @param st the state to be evaluated
     @return the value of the violations function in the given state
  */
  template <class Input, class State>  
  fvalue StateManager<Input,State>::Objective(const State& st) const
  { 
    std::cout << "Warning: violations function not implemented yet!" << std::endl;
    return 0; 
  }
 
  /**
     Sets the internal input pointer to the new value passed as parameter.
     
     @param in the new input object
  */
  template <class Input, class State>  
  void StateManager<Input,State>::SetInput(Input* in) 
  { p_in = in; }

  /**
     Returns the input pointer which the object is attached to.
     
     @return the pointer to the input object
  */
  template <class Input, class State>  
  Input* StateManager<Input,State>::GetInput() 
  { return p_in; }

  /**
     Checks wether the object state is consistent with all the related
     objects.
  */
  template <class Input, class State>  
  void StateManager<Input,State>::Check() const
  { assert(p_in != NULL); }

  /**
     Builds a state manager object linked to the provided input.

     @param in a pointer to an input object
  */
  template <class Input, class State>  
  StateManager<Input,State>::StateManager(Input* in) 
    : p_in(in)  
  {}

  // Output Manager functions


  /**
     Reads a state from an input stream.

     @param st the state to be read
     @param is the input stream
  */
  template <class Input, class Output, class State>
  void OutputManager<Input,Output,State>::ReadState(State &st, std::istream &is) const
  {
    Output out(p_in);
    is >> out;
    InputState(st,out);
  }

  /**
     Writes a state on an output stream.

     @param st the state to be written,
     @param os the output stream
  */
  template <class Input, class Output, class State>
  void OutputManager<Input,Output,State>::WriteState(const State &st, std::ostream &os) const
  {
    Output out(p_in);
    OutputState(st,out);  
    os << out;
  }

  /**
     Sets the internal input pointer to the new value passed as parameter.
     
     @param in the new input.
  */
  template <class Input, class Output, class State>
  void OutputManager<Input,Output,State>::SetInput(Input* in) 
  { p_in = in; }

  /**
     Returns the input pointer which the object is attached to.
     
     @return the pointer to the input.
  */
  template <class Input, class Output, class State>
  Input* OutputManager<Input,Output,State>::GetInput() 
  { return p_in; }
  
  /**
     Checks wether the object state is consistent with all the related
     objects.
  */
  template <class Input, class Output, class State>  
  void OutputManager<Input,Output,State>::Check() const
  { 
    assert(p_in != NULL && p_sm->GetInput() == p_in); 
  }

  // Prohibition Manager functions
  // up to now, the only actual prohibition manager is the tabu list manager

  /** 
      Constructs a tabu list manager object which manages a list of 
      the given tenure (i.e., the number of steps a move is considered tabu).
      
      @param min the minimum tabu tenure
      @param max the maximum tabu tenure
  */
  template <class Move>
  TabuListManager<Move>::TabuListManager(int min, int max)
    : min_tenure(min), max_tenure(max), iter(0)
  { }

  
  /**
     Inserts the move in the tabu list and updates the aspiration function.

     @param mv the move to add
     @param mv_cost the move cost
     @param best the best state cost found so far
  */
  template <class Move>
  void TabuListManager<Move>::InsertMove(const Move& mv, fvalue mv_cost, fvalue curr, fvalue best)
  { 
    InsertIntoList(mv); 
    UpdateAspirationFunction(mv_cost,curr,best); 
  }

  /**
     Checks whether the given move is prohibited.

     @param mv the move to check
     @param mv_cost the move cost
     @param curr the current state cost
     @param best the best state cost found so far
     @return true if the move mv is prohibited, false otherwise
  */
  template <class Move>
  bool TabuListManager<Move>::ProhibitedMove(const Move& mv, fvalue mv_cost, fvalue curr, fvalue best) const
  { return ListMember(mv) && !Aspiration(mv,mv_cost,curr,best); }

  /** 
      Deletes all the elements of the tabu list.
  */
  template <class Move>
  void TabuListManager<Move>::Clean()
  { tlist.clear(); }

  /** 
      Checks whether the inverse of a given move belongs to the tabu list.
      
      @param mv the move to check
      @return true if the inverse of the move belongs to the tabu list, 
      false otherwise
  */
  template <class Move>
  bool TabuListManager<Move>::ListMember(const Move& mv) const
  {
    typename std::list<ListItem<Move> >::const_iterator p = tlist.begin();
    while (p != tlist.end())
      {
	if (Inverse(mv,p->elem))
	  return true;
	else
	  p++;
      }
    return false;
  }

  /** 
      Writes the current status of the tabu list on a output stream.
      
      @param os the output stream
      @param tl the tabu list manager to output
  */
  template <class Move>
  std::ostream& operator<<(std::ostream& os, const TabuListManager<Move>& tl)
  {
    typename std::list<ListItem<Move> >::const_iterator p = tl.tlist.begin();
    while (p != tl.tlist.end())
      {
	os << p->elem << " (" << p->out_iter - tl.iter << ")" << std::endl;
	p++;
      }
    return os;
  }

  /**
     Checks whether the aspiration criterion is satisfied for a given move.
     By default, it verifies if the move cost applied to the current state
     gives a value lower than the best state cost found so far.

     @param mv the move
     @param mv_cost the move cost
     @param curr the cost of the current state
     @param best the cost of the best state found so far
     @return true if the aspiration criterion is satisfied, false otherwise
  */
  template <class Move>
  bool TabuListManager<Move>::Aspiration(const Move& mv, fvalue mv_cost, fvalue curr, fvalue best) const 
  { return curr + mv_cost < best; }

  /**
     Inserts the move into the tabu list, and update the list removing
     the moves for which the tenure has elapsed.

     @param mv the move to add
  */
  template <class Move>
  void TabuListManager<Move>::InsertIntoList(const Move& mv)
  {
    int tenure = Random(min_tenure,max_tenure);
    ListItem<Move> li(mv, iter+tenure);
    tlist.push_front(li);
	
    typename std::list<ListItem<Move> >::iterator p = tlist.begin();
    while (p != tlist.end())
      if (p->out_iter == iter)
	p = tlist.erase(p);
      else
	p++;
    iter++;
  }

  // Neighborhood explorer functions

  /**
     Constructs a neighborhood explorer passing a pointer to a state manager 
     and a pointer to the input.
     
     @param sm a pointer to a compatible state manager
     @param in a pointer to an input object
  */
  template <class Input, class State, class Move>  
  NeighborhoodExplorer<Input,State,Move>::NeighborhoodExplorer(StateManager<Input,State>* sm, Input* in) 
    :  p_sm(sm), p_in(in), p_pm(NULL)
  {}

  /**
     Constructs a neighborhood explorer which has a prohibition manager too.
     It accepts a pointer to a state manager, a pointer to the
     prohibition manager, and a pointer to the input.
     
     @param sm a pointer to a compatible state manager
     @param pm a pointer to a compatible prohibition manager
     @param in a pointer to an input object
  */
  template <class Input, class State, class Move>  
  NeighborhoodExplorer<Input,State,Move>::NeighborhoodExplorer(StateManager<Input,State>* sm, ProhibitionManager<Move>* pm, Input* in) 
    :  p_sm(sm), p_in(in), p_pm(pm)
  {}

  /**
     Evaluates the variation of the cost function obtainted by applying the
     move to the given state.
     The tentative definition computes a weighted sum of the variation of 
     the violations function and of the difference in the objective function.

     @param st the start state
     @param mv the move
     @return the variation in the cost function
  */
  template <class Input, class State, class Move>  
  fvalue NeighborhoodExplorer<Input,State,Move>::DeltaCostFunction(const State& st, const Move & mv)
  { return HARD_WEIGHT * DeltaViolations(st,mv) + DeltaObjective(st,mv); }

  /**
     Sets the prohibition manager to the value passed as input.
     
     @param pm a pointer to a prohibition manager
  */
  template <class Input, class State, class Move>  
  void NeighborhoodExplorer<Input,State,Move>::SetProhibitionManager(ProhibitionManager<Move>* pm) 
  { p_pm = pm; }


  /**
     Looks for the best move in the exploration of the neighborhood of a given 
     state. (i.e., the one that gives the best improvement in the cost 
     function).
     
     @param st the state
     @param mv the best move in the state st
     @return the cost of move mv
  */
  template <class Input, class State, class Move>  
  fvalue NeighborhoodExplorer<Input,State,Move>::BestMove(const State &st, Move& mv)
  {  
    FirstMove(st,mv); 
    fvalue mv_cost = DeltaCostFunction(st,mv);
    best_move = mv; 
    fvalue best_delta = mv_cost;
    do // look for the best move
      { 
	mv_cost = DeltaCostFunction(st,mv);
#ifdef COST_DEBUG
	std::cerr << mv << ' ' << mv_cost << std::endl;
#endif
	if (mv_cost < best_delta) 
	  { 
	    best_move = mv;
	    best_delta = mv_cost;
	  }
	NextMove(st,mv);
      }
    while (!LastMoveDone(mv));
    mv = best_move;
    return best_delta;
  }

  /**
     Outputs some statistics about the neighborhood of the given state.
     In detail it prints out the number of neighbors, the number of 
     improving/non-improving/worsening moves and their percentages.
     
     @param st the state to inspect
  */
  template <class Input, class State, class Move>  
  void NeighborhoodExplorer<Input,State,Move>::NeighborhoodStatistics(const State &st)
  {  
    unsigned int neighbors = 0, improving_neighbors = 0, 
      worsening_neighbors = 0, non_improving_neighbors = 0;
    Move mv;
    fvalue mv_cost;

    FirstMove(st,mv);
    do 
      { 	
	neighbors++;
        mv_cost = DeltaCostFunction(st,mv);
	if (mv_cost < 0)
	  improving_neighbors++;
	else if (mv_cost > 0)
	  worsening_neighbors++;
	else
	  non_improving_neighbors++;
	NextMove(st,mv);
      }
    while (!LastMoveDone(mv));
    std::cout << "Neighborhood size: " <<  neighbors << std::endl
	      << "   improving moves: " << improving_neighbors << " ("
	      << (100.0*improving_neighbors)/neighbors << "%%)" << std::endl
	      << "   worsening moves: " << worsening_neighbors << " ("
	      << (100.0*worsening_neighbors)/neighbors << "%%)" << std::endl
      	      << "   non-improving moves: " << non_improving_neighbors << " ("
	      << (100.0*non_improving_neighbors)/neighbors << "%%)" << std::endl;
  }

  /** 
      Generates the first move in the exploration of the neighborhood of a 
      given state. 
      By default, it invokes the RandomMove function and records mv as 
      start move.

      @param st the state
  */
  template <class Input, class State, class Move>
  void NeighborhoodExplorer<Input,State,Move>::FirstMove(const State& st, Move& mv) 
  { 
    RandomMove(st,mv); 
    start_move = mv;
  }

  /**
     Returns the best move found out of a number of sampled moves from a given
     state.
     
     @param st the state
     @param mv the best move found
     @param samples the number of sampled moves
     @return the cost of the move mv
  */
  template <class Input, class State, class Move>  
  fvalue NeighborhoodExplorer<Input,State,Move>::SampleMove(const State &st, Move& mv, int samples)
  {  
    int s = 1;
    RandomMove(st,mv);
    fvalue mv_cost = DeltaCostFunction(st,mv);
    best_move = mv;
    fvalue best_delta = mv_cost;
    do // look for the best sampled move
      { 
	mv_cost = DeltaCostFunction(st,mv);
	if (mv_cost < best_delta)
	  { 
	    best_move = mv;
	    best_delta = mv_cost;
	  }
	RandomMove(st,mv);
	s++;
      }
    while (s < samples);
    mv = best_move;
    return best_delta;
  }

  /** 
      Looks for the best move in a given state that is non prohibited.
      
      @param st the state
      @param mv the best non prohibited move in st
      @param curr the cost of the state st
      @param best the cost of the best state found so far
      @return the cost of the move mv
  */
  template <class Input, class State, class Move>  
  fvalue NeighborhoodExplorer<Input,State,Move>::BestNonProhibitedMove(const State &st, Move& mv, fvalue curr, fvalue best) 
  {
    register fvalue mv_cost;
    bool tabu_move;
    bool all_moves_tabu = true;
	
    FirstMove(st,mv); 
    mv_cost = DeltaCostFunction(st,mv);
    best_move = mv;
    fvalue best_delta = mv_cost;
    do  // look for the best non prohibited move 
      { // (if all moves are prohibited, then get the best)
	tabu_move = p_pm->ProhibitedMove(mv,mv_cost,curr,best);      
	if (   (mv_cost < best_delta && !tabu_move)
	       || (mv_cost < best_delta && all_moves_tabu)
	       || (all_moves_tabu && !tabu_move))
	  { 
	    best_move = mv;
	    best_delta = mv_cost;
	  }
	if (!tabu_move) 
	  all_moves_tabu = false;
	NextMove(st,mv);
	mv_cost = DeltaCostFunction(st,mv);
      }
    while (!LastMoveDone(mv));
    mv = best_move;
    return best_delta;
  } 

  
  /**
     Returns the best non prohibited move found out of a number of sampled 
     moves from a given state.
     
      @param st the state
      @param mv the best non prohibited move found
      @param samples the number of sampled moves
      @param curr the cost of the state st
      @param best the cost of the best state found so far
      @return the cost of the move mv
  */
  template <class Input, class State, class Move>  
  fvalue NeighborhoodExplorer<Input,State,Move>::SampleNonProhibitedMove(const State &st, Move& mv, int samples, fvalue curr, fvalue best)
  {  
    int s = 1;
    fvalue mv_cost;
    bool tabu_move;
    bool all_moves_tabu = true;
	
    RandomMove(st,mv);
    mv_cost = DeltaCostFunction(st,mv);
    best_move = mv;
    fvalue best_delta = mv_cost;
    do 
      { 
	tabu_move = p_pm->ProhibitedMove(mv,mv_cost,curr,best);      
	if (   (mv_cost < best_delta && !tabu_move)
	       || (mv_cost < best_delta && all_moves_tabu)
	       || (all_moves_tabu && !tabu_move))
	  { 
	    best_move = mv;
	    best_delta = mv_cost;
	  }
	if (!tabu_move) 
	  all_moves_tabu = false;
	RandomMove(st,mv);
	mv_cost = DeltaCostFunction(st,mv);
	s++;
      }
    while (s < samples);
    mv = best_move;
    return best_delta;
  }


  /**
     Outputs some informations about a move in a given state on a stream.

     @param st the state
     @param mv the move
     @param os an output stream
  */
  template <class Input, class State, class Move>  
  void NeighborhoodExplorer<Input,State,Move>::PrintMoveInfo(const State &st, const Move& mv, std::ostream& os)
  {
    os << "Move : " << mv << std::endl;
    os << "Start state cost : " << p_sm->CostFunction(st) << std::endl;
    os << "\tViolations : " << p_sm->Violations(st) << std::endl;
    os << "\tObjective  : " << p_sm->Objective(st) << std::endl;
	
    os << "Move cost : " << DeltaCostFunction(st,mv) << std::endl;
    os << "\tViolations : " << DeltaViolations(st,mv) << std::endl;
    os << "\tObjective  : " << DeltaObjective(st,mv) << std::endl;
	
    State st1 = st;
    MakeMove(st1,mv);
    os << "Final state cost : " << p_sm->CostFunction(st1) << std::endl;  
    os << "\tViolations : " << p_sm->Violations(st1) << std::endl;
    os << "\tObjective  : " << p_sm->Objective(st1) << std::endl;
	
    os << "Error : " << p_sm->CostFunction(st1) - DeltaCostFunction(st,mv) - p_sm->CostFunction(st) << std::endl;
  }

  /**
     Sets the internal input pointer to the new value passed as parameter.
     
     @param in the new input.
  */
  template <class Input, class State, class Move>
  void NeighborhoodExplorer<Input,State,Move>::SetInput(Input* in) 
  { p_in = in; }

  /**
     Returns the input pointer which the object is attached to.
     
     @return the pointer to the input.
  */
  template <class Input, class State, class Move>
  Input* NeighborhoodExplorer<Input,State,Move>::GetInput() 
  { return p_in; }

  /**
     Checks wether the object state is consistent with all the related
     objects.
  */
  template <class Input, class State, class Move>  
  void NeighborhoodExplorer<Input,State,Move>::Check()
  { assert(p_in != NULL && p_in == p_sm->GetInput()); }

  /** 
      Evaluates the variation of the violations function obtained by 
      performing a move in a given state.
      The tentative definition simply makes the move and invokes the 
      companion StateManager method (Violations) on the initial and on the
      final state.
      
      @param st the state
      @param mv the move to evaluate
      @return the difference in the violations function induced by the move mv
  */
  template <class Input, class State, class Move>  
  fvalue NeighborhoodExplorer<Input,State,Move>::DeltaViolations(const State& st, const Move & mv)
  {
    State st1 = st;
    MakeMove(st1,mv);
    return p_sm->Violations(st1) - p_sm->Violations(st);
  }

  /**
     Evaluates the variation of the objective function obtained by performing
     a move in a given state.
     The tentative definition simply makes the move and invokes the 
      companion StateManager method (Objective) on the initial and on the
      final state.
      
      @param st the state
      @param mv the move to evaluate
      @return the difference in the objective function induced by the move mv
  */
  template <class Input, class State, class Move>  
  fvalue NeighborhoodExplorer<Input,State,Move>::DeltaObjective(const State& st, const Move & mv)
  {
    State st1 = st;
    MakeMove(st1,mv);
    return p_sm->Objective(st1) - p_sm->Objective(st);
  }

  /**
     Checks whether the whole neighborhood has been explored.
     The tentative definition verifies is the move passed as parameter 
     coincides with the start move.

     @param mv the move to check
     @return true if the whole neighborhood has been explored, false otherwise
  */
  template <class Input, class State, class Move>  
  bool NeighborhoodExplorer<Input,State,Move>::LastMoveDone(const Move &mv) 
  { return mv == start_move; } 

  // Runner functions

  /**
     Constructs a runner and associates a name and a type to it.

     @param s the name of the runner
     @param t the type of the runner (used for parameter setting)
  */
  template <class Input, class State>
  Runner<Input,State>::Runner(std::string s, std::string t)
    : name(s), type(t) 
  {}

  /** 
      Inspects the name of the runner.
      
      @return the name of the runner
  */
  template <class Input, class State>
  std::string Runner<Input,State>::Name() 
  { return name; }

  /** 
      Inspects the type of the runner.
      
      @return the type of the runner
  */
  template <class Input, class State>
  std::string Runner<Input,State>::Type()
  { return type; }

  /**
     Sets the name of the runner to the given parameter.
     
     @param s the name to give to the runner
  */
  template <class Input, class State>
  void Runner<Input,State>::SetName(std::string s) 
  { name = s; } 

  /**
     Creates a move runner and links it to a given state manager, neighborhood
     explorer and input objects. In addition, it sets its name and type to
     the given values.

     @param sm a pointer to a compatible state manager
     @param ne a pointer to a compatible neighborhood explorer
     @param in a pointer to the input object
     @param name the name of the runner
     @param type the type of the runner
  */
  template <class Input, class State, class Move>
  MoveRunner<Input,State,Move>::MoveRunner(StateManager<Input,State>* sm, NeighborhoodExplorer<Input,State,Move>* ne, Input* in, std::string name, std::string type)
    : Runner<Input,State>(name,type), p_in(in), p_sm(sm), p_nhe(ne)
  { 
    if (in != NULL)
      current_state.SetInput(in);
    number_of_iterations = 0;
    max_iteration = ULONG_MAX;
    current_state_set = false; 
  }

  /**
     Sets the internal input pointer to the new value passed as parameter.
     
     @param in the new input.
  */
  template <class Input, class State, class Move>
  void MoveRunner<Input,State,Move>::SetInput(Input* in) 
  { 
    p_in = in; 
    current_state.SetInput(in);
    current_state_set = false;
    p_nhe->SetInput(in);
  }

  /**
     Returns the input pointer which the object is attached to.
     
     @return the pointer to the input.
  */
  template <class Input, class State, class Move>
  Input* MoveRunner<Input,State,Move>::GetInput() 
  { return p_in; }

  /**
     Outputs the name of the runner on a given output stream.

     @param os the output stream
  */
  template <class Input, class State, class Move>
  void MoveRunner<Input,State,Move>::Print(std::ostream& os) const
  { os << this->name << " : " << this->type << std::endl; }

  /**
     Checks wether the object state is consistent with all the related
     objects.
  */
  template <class Input, class State, class Move>
  void MoveRunner<Input,State,Move>::Check() 
  { 
    assert(p_in != NULL);
    assert(p_in == p_sm->GetInput());  
    assert(p_in == p_nhe->GetInput());
  }


  /**
     Sets the internal state of the runner to the value passed as parameter.

     @param s the state to become the current state of the runner
  */
  template <class Input, class State, class Move>
  void MoveRunner<Input,State,Move>::SetCurrentState(const State& s) 
  { 
    current_state = s; 
    current_state_set = true; 
    current_state_cost = p_sm->CostFunction(current_state); 
  }

  /**
     Retrieves the internal state of the runner.
     
     @return the current state of the runner
  */
  template <class Input, class State, class Move>
  State MoveRunner<Input,State,Move>::GetCurrentState() 
  { return current_state; }

  /**
     Returns the cost of the internal state.

     @return the cost of the current state
  */
  template <class Input, class State, class Move>
  fvalue MoveRunner<Input,State,Move>::CurrentStateCost() 
  { return current_state_cost; }

  /**
     Returns the best state found so far by the runner.
     
     @return the best state found
  */
  template <class Input, class State, class Move>
  State MoveRunner<Input,State,Move>::GetBestState() 
  { return best_state; }

  /** 
      Returns the cost of the best state found so far by the runner.

      @return the cost of the best state found
  */
  template <class Input, class State, class Move>
  fvalue MoveRunner<Input,State,Move>::BestStateCost() 
  { return best_state_cost; }

  /** 
      Computes explicitely the cost of the current state (used 
      at the beginning of a run for consistency purpose).
  */
  template <class Input, class State, class Move>
  void MoveRunner<Input,State,Move>::ComputeCost()  
  { current_state_cost = p_sm->CostFunction(current_state); }

  /**
     Checks whether the lower bound of the cost function has been reached.
     The tentative definition verifies whether the current state cost is
     equal to zero.
  */
  template <class Input, class State, class Move>
  bool MoveRunner<Input,State,Move>::LowerBoundReached() 
  { return current_state_cost == 0; }

  /**
     Returns the number of iterations elapsed.

     @return the number of iterations performed by the runner
  */
  template <class Input, class State, class Move>
  unsigned long MoveRunner<Input,State,Move>::NumberOfIterations() const 
  { return number_of_iterations; }

  /**
     Returns the maximum value of iterations allowed for the runner.

     @return the maximum value of iterations allowed
  */
  template <class Input, class State, class Move>
  unsigned long MoveRunner<Input,State,Move>::MaxIteration() const 
  { return max_iteration; }

  /**
     Sets a bound on the maximum number of iterations allowed for the runner.
     
     @param max the maximum number of iterations allowed */
  template <class Input, class State, class Move>
  void MoveRunner<Input,State,Move>::SetMaxIteration(unsigned long max)  
  { max_iteration = max; }

  /**
     Sets the runner parameters, passed through a parameter box.

     @param pb the object containing the parameter setting for the algorithm
  */
  template <class Input, class State, class Move>
  void MoveRunner<Input,State,Move>::SetParameters(const ParameterBox& pb)
  {   
    pb.Get("max idle iteration",max_idle_iteration);
    pb.Get("max iteration", max_iteration);
  }

  /**
     Performs a full run of a local search method.
   */
  template <class Input, class State, class Move>
  void MoveRunner<Input,State,Move>::Go()
  { 
    assert(current_state_set);
    InitializeRun();
    while (!MaxIterationExpired() && !StopCriterion() && !LowerBoundReached())
      { 
	UpdateIterationCounter();
	SelectMove();
#ifdef TRACE_MOVES
	Print(); std::cerr << "press any key ... "; std::cin.get();
#endif
	if (AcceptableMove())
	  { 
	    MakeMove();
	    UpdateStateCost();
	    StoreMove();
	  }
      }
    TerminateRun();
  }

  /**
     Actually performs the move selected by the local search strategy.
   */
  template <class Input, class State, class Move>
  void MoveRunner<Input,State,Move>::MakeMove() 
  { 
#ifdef TRACE_MOVES
    p_nhe->PrintMoveInfo(current_state,current_move,std::cerr);
#endif
#ifdef COST_DEBUG
    fvalue ocost = current_state_cost;
    State previous_state = current_state;
#endif
    p_nhe->MakeMove(current_state,current_move); 
#ifdef COST_DEBUG
    fvalue ncost = p_sm->CostFunction(current_state);
    if (distance(ncost,(ocost+current_move_cost)) > EPS)
      {
	std::cerr << "Error in computing delta_cost: " 
		  << ncost-(ocost+current_move_cost) << std::endl;
	std::cerr << "Current iteration : " << number_of_iterations << std::endl;
	std::cerr << "Previous state : " << std::endl;
	std::cerr << previous_state << std::endl;
	std::cerr << "Current state : " << std::endl;
	std::cerr << current_state << std::endl;
	p_nhe->PrintMoveInfo(previous_state,current_move,std::cerr);
	char s[3];
	std::cout << "Press enter to continue...";
	std::cin.getline(s,3);
      }
#endif
#ifdef PLOT_DATA
    assert(pos); // the plot output stream must be set
    *pos << number_of_iterations << "\t" << current_state_cost << std::endl;
#endif
  }

  /**
     Performs a given number of steps of the local search strategy.

     @param n the number of steps
  */
  template <class Input, class State, class Move>
  void MoveRunner<Input,State,Move>::Step(unsigned int n)
  { 
    assert(current_state_set);
    for (unsigned int i = 0; i < n; i++) 
      { 
	UpdateIterationCounter();
	SelectMove();
	if (AcceptableMove())
	  { MakeMove();
	  UpdateStateCost();
	  StoreMove();
	  if (LowerBoundReached())
	    break;
	  }
      }
  }

  /**
     Computes the cost of the selected move; it delegates this task to the
     neighborhood explorer.
  */
  template <class Input, class State, class Move>
  void MoveRunner<Input,State,Move>::ComputeMoveCost() 
  { current_move_cost = p_nhe->DeltaCostFunction(current_state,current_move); }

  /**
     Updates the counter that tracks the number of iterations elapsed.
  */
  template <class Input, class State, class Move>
  void MoveRunner<Input,State,Move>::UpdateIterationCounter() 
  { number_of_iterations++; }

  /**
     Verifies whether the upper bound on the number of iterations
     allowed for the strategy has been reached.

     @return true if the maximum number of iteration has been reached, false
     otherwise
  */
  template <class Input, class State, class Move>
  bool MoveRunner<Input,State,Move>::MaxIterationExpired() 
  { return number_of_iterations > max_iteration; }

  /** 
      Checks whether the selected move can be performed.
      Its tentative definition simply returns true
  */
  template <class Input, class State, class Move>
  bool MoveRunner<Input,State,Move>::AcceptableMove() 
  { return true; }

  /**
     Updates the cost of the internal state of the runner.
  */
  template <class Input, class State, class Move>
  void MoveRunner<Input,State,Move>::UpdateStateCost() 
  { current_state_cost += current_move_cost; }

  /**
     Initializes all the runner variable for starting a new run.
  */
  template <class Input, class State, class Move>
  void MoveRunner<Input,State,Move>::InitializeRun()
  {
    number_of_iterations = 0;
    iteration_of_best = 0;
    ComputeCost();
    best_state = current_state;
    best_state_cost = current_state_cost;
  }

  // Actual Runners


  // Hill Climbing
  

  /**
     Constructs a hill climbing runner by linking it to a state manager, 
     a neighborhood explorer, and an input object.

     @param s a pointer to a compatible state manager
     @param ne a pointer to a compatible neighborhood explorer
     @param in a poiter to an input object
  */
  template <class Input, class State, class Move>
  HillClimbing<Input,State,Move>::HillClimbing(StateManager<Input,State>* s, NeighborhoodExplorer<Input,State,Move>* ne, Input* in)
    : MoveRunner<Input,State,Move>(s, ne, in, "Runner name", "Hill Climbing")
  { 
    //  max_idle_iteration = 0; 
  }

  /**
     Reads the hill climbing parameters from the standard input.
  */
  template <class Input, class State, class Move>   
  void HillClimbing<Input,State,Move>::ReadParameters() 
  { 
    std::cout << "HILL CLIMBING -- INPUT PARAMETERS" << std::endl;
    std::cout << "Number of idle iterations: ";
    std::cin >> this->max_idle_iteration;
  }
  
  /**
     The select move strategy for the hill climbing simply looks for a
     random move.
  */
  template <class Input, class State, class Move>   
  void HillClimbing<Input,State,Move>::SelectMove() 
  { 
    this->p_nhe->RandomMove(this->current_state,this->current_move); 
    this->ComputeMoveCost();
  }

  /**
     The hill climbing initialization simply invokes 
     the superclass companion method.
  */
  template <class Input, class State, class Move>
  void HillClimbing<Input,State,Move>::InitializeRun()
  { 
    MoveRunner<Input,State,Move>::InitializeRun(); 
    assert(this->max_idle_iteration > 0);
  }

  /**
     At the end of the run, the best state found is set with the last visited
     state (it is always a local minimum).
  */
  template <class Input, class State, class Move>
  void HillClimbing<Input,State,Move>::TerminateRun()
  { 
    this->best_state = this->current_state;
    this->best_state_cost = this->current_state_cost;
  }

  /**
     The stop criterion for the hill climbing strategy is based on the number
     of iterations elapsed from the last strict improving move performed.
  */
  template <class Input, class State, class Move>
  bool HillClimbing<Input,State,Move>::StopCriterion()
  { return this->number_of_iterations - this->iteration_of_best >= this->max_idle_iteration; }

  /**
     A move is accepted if it is non worsening (i.e., it improves the cost
     or leaves it unchanged).
  */
  template <class Input, class State, class Move>
  bool HillClimbing<Input,State,Move>::AcceptableMove()
  { return this->current_move_cost <= 0; }

  /**
     The store move for hill climbing simply updates the variable that
     keeps track of the last improvement.
  */
  template <class Input, class State, class Move>
  void HillClimbing<Input,State,Move>::StoreMove()
  { 
    if (this->current_move_cost < -EPS)
      {
	this->iteration_of_best = this->number_of_iterations;
      }
  }

  /**
     Outputs some hill climbing statistics on a given output stream.

     @param os the output stream
  */
  template <class Input, class State, class Move>
  void HillClimbing<Input,State,Move>::Print(std::ostream & os) const
  { 
    MoveRunner<Input,State,Move>::Print(os);
    os << "PATAMETERS: " << std::endl;
    os << "  Max idle iteration : " << this->max_idle_iteration << std::endl;
    os << "  Max iteration : " << this->max_iteration << std::endl;
    os << "RESULTS : " << std::endl;
    os << "  Number of iterations : " << this->number_of_iterations << std::endl;
    os << "  Iteration of best : " << this->iteration_of_best << std::endl;
    os << "  Current state [cost: " 
       << this->current_state_cost << "] " << std::endl;
    os << this->current_state << std::endl;
    os << std::endl;
  }

  // Steepest Descent

  /**
     Constructs a steepest descent runner by linking it to a state manager, 
     a neighborhood explorer, and an input object.

     @param s a pointer to a compatible state manager
     @param ne a pointer to a compatible neighborhood explorer
     @param in a poiter to an input object
  */
  template <class Input, class State, class Move>
  SteepestDescent<Input,State,Move>::SteepestDescent(StateManager<Input,State>* s, NeighborhoodExplorer<Input,State,Move>* ne, Input* in)
    : MoveRunner<Input,State,Move>(s, ne, in, "Runner name", "Steepest Descent")
  {}

  /**
     Selects always the best move in the neighborhood.
  */
  template <class Input, class State, class Move>   
  void SteepestDescent<Input,State,Move>::SelectMove() 
  { this->current_move_cost = this->p_nhe->BestMove(this->current_state,this->current_move); }

  /**
     Invokes the companion superclass method, and initializes the move cost
     at a negative value for fulfilling the stop criterion the first time
  */     
  template <class Input, class State, class Move>
  void SteepestDescent<Input,State,Move>::InitializeRun()
  { 
    MoveRunner<Input,State,Move>::InitializeRun(); 
    this->current_move_cost = -1; // needed for passing the first time 
                            // the StopCriterion test
  }

  /**
     The search is stopped when no (strictly) improving move has been found.
  */
  template <class Input, class State, class Move>
  bool SteepestDescent<Input,State,Move>::StopCriterion()
  { return this->current_move_cost >= 0; }

  /**
     A move is accepted if it is an improving one.
  */
  template <class Input, class State, class Move>
  bool SteepestDescent<Input,State,Move>::AcceptableMove()
  { return this->current_move_cost < 0; }

    /**
     At the end of the run, the best state found is set with the last visited
     state (it is always a local minimum).
  */
  template <class Input, class State, class Move>
  void SteepestDescent<Input,State,Move>::TerminateRun()
  { 
    this->best_state = this->current_state;
    this->best_state_cost = this->current_state_cost;
  }
  
  /**
     Outputs some steepest descent statistics on a given output stream.

     @param os the output stream
  */
  template <class Input, class State, class Move>
  void SteepestDescent<Input,State,Move>::Print(std::ostream & os) const
  {  
    MoveRunner<Input,State,Move>::Print(os);
    os << "PATAMETERS: " << std::endl;
    os << "  Max iteration : " << this->max_iteration << std::endl;
    os << "RESULTS : " << std::endl;
    os << "  Number of iterations : " << this->number_of_iterations << std::endl;
    os << "  Current state [cost: " 
       << this->current_state_cost << "] " << std::endl;
    os << this->current_state << std::endl;
    os << std::endl;
  }

  // Tabu Search
  
  /**
     Constructs a tabu search runner by linking it to a state manager, 
     a neighborhood explorer, a tabu list manager, and an input object.

     @param s a pointer to a compatible state manager
     @param ne a pointer to a compatible neighborhood explorer
     @param tlm a pointer to a compatible tabu list manager
     @param in a poiter to an input object
  */
  template <class Input, class State, class Move>
  TabuSearch<Input,State,Move>::TabuSearch(StateManager<Input,State>* s, NeighborhoodExplorer<Input,State,Move>* ne, TabuListManager<Move>* tlm, Input* in)
    : MoveRunner<Input,State,Move>(s, ne, in, "Runner name", "Tabu Search")
  { 
    if (in != NULL)
      this->best_state.SetInput(in);
	
    SetTabuListManager(tlm); 
    p_pm = tlm;
    this->p_nhe->SetProhibitionManager(p_pm);
  }

  /**
     Sets the internal input pointer to the new value passed as parameter.
     
     @param in the new input.
  */
  template <class Input, class State, class Move>
  void TabuSearch<Input,State,Move>::SetInput(Input* in) 
  { 
    MoveRunner<Input,State,Move>::SetInput(in);
    this->best_state.SetInput(in); 
  }

  /**
     Reads the tabu search parameters from the standard input.
  */
  template <class Input, class State, class Move>
  void TabuSearch<Input,State,Move>::ReadParameters()
  {
    int min_tabu, max_tabu;
    std::cout << "TABU SEARCH -- INPUT PARAMETERS" << std::endl;
    std::cout << "Length of the tabu list (min,max): ";
    std::cin >> min_tabu >> max_tabu;
    p_pm->SetLength(min_tabu,max_tabu);
    std::cout << "Number of idle iterations: ";
    std::cin >> this->max_idle_iteration;
  }

  /**
     Sets the tabu list manager according to the one passed as parameter.
     
     @param tlm a pointer to a compatible tabu list manager
     @param min_tabu the minimum tabu tenure for a move
     @param max_tabu the maximum tabu tenure for a move
  */
  template <class Input, class State, class Move>
  void TabuSearch<Input,State,Move>::SetTabuListManager(TabuListManager<Move>* tlm, int min_tabu, int max_tabu)
  { 
    p_pm = tlm;
    if (max_tabu != 0) // if min_tabu and max_tabu are properly set
      p_pm->SetLength(min_tabu,max_tabu);
    this->p_nhe->SetProhibitionManager(p_pm);
  }


  /**
     Initializes the run by invoking the companion superclass method, and
     cleans the tabu list.
  */
  template <class Input, class State, class Move>
  void TabuSearch<Input,State,Move>::InitializeRun()
  {
    MoveRunner<Input,State,Move>::InitializeRun(); 
    assert(this->max_idle_iteration > 0);
    p_pm->Clean();
  }

  /** 
      Selects always the best move that is non prohibited by the tabu list 
      mechanism.
  */
  template <class Input, class State, class Move>
  void TabuSearch<Input,State,Move>::SelectMove() 
  { this->current_move_cost = this->p_nhe->BestNonProhibitedMove(this->current_state, this->current_move, this->current_state_cost, this->best_state_cost); }

  /**
     The stop criterion is based on the number of iterations elapsed from
     the last strict improvement of the best state cost.
  */
  template <class Input, class State, class Move>  
  bool TabuSearch<Input,State,Move>::StopCriterion()
  { return this->number_of_iterations - this->iteration_of_best >= this->max_idle_iteration; }

  /** 
      In tabu search the selected move is always accepted.
      That is, the acceptability test is replaced by the 
      prohibition mechanism which is managed inside the selection.
  */
  template <class Input, class State, class Move>  
  bool TabuSearch<Input,State,Move>::AcceptableMove() { return true; }

  
  /**
     Sets the tabu search parameters, passed through a parameter box.

     @param pb the object containing the parameter setting for the algorithm
  */
  template <class Input, class State, class Move>  
  void TabuSearch<Input,State,Move>::SetParameters(const ParameterBox& pb)
  {
    unsigned int min_tabu, max_tabu;
    MoveRunner<Input,State,Move>::SetParameters(pb);
    pb.Get("min tenure", min_tabu);
    pb.Get("max tenure", max_tabu);
    p_pm->SetLength(min_tabu,max_tabu);
  }

  /**
     Stores the move by inserting it in the tabu list, if the state obtained
     is better than the one found so far also the best state is updated.
  */
  template <class Input, class State, class Move>  
  void TabuSearch<Input,State,Move>::StoreMove()
  {
    p_pm->InsertMove(this->current_move, this->current_move_cost, this->current_state_cost, this->best_state_cost);
    if (this->current_state_cost + EPS < this->best_state_cost)
      { 
	this->iteration_of_best = this->number_of_iterations;
	this->best_state = this->current_state;
	this->best_state_cost = this->current_state_cost;
      }
  }

  /**
     Outputs some tabu search statistics on a given output stream.

     @param os the output stream
  */
  template <class Input, class State, class Move>
  void TabuSearch<Input,State,Move>::Print(std::ostream & os) const
  {  
    MoveRunner<Input,State,Move>::Print(os);
    os << "PATAMETERS: " << std::endl;
    os << "  Max idle iteration : " << this->max_idle_iteration << std::endl;
    os << "  Max iteration : " << this->max_iteration << std::endl;
    os << "  Tenure : " << p_pm->MinTenure() << '-' << p_pm->MaxTenure() << std::endl;
    os << "RESULTS : " << std::endl;
    os << "  Number of iterations : " << this->number_of_iterations << std::endl;
    os << "  Iteration of best : " << this->iteration_of_best << std::endl;
    os << "  Current state [cost: " 
       << this->current_state_cost << "] " << std::endl
       << this->current_state << std::endl;
    os << "  Best State    [cost: " 
       << this->best_state_cost << "] " << std::endl
       << this->best_state << std::endl << std::endl;
    os << "Tabu list : " << std::endl;
    // os << *p_pm; 
    os << std::endl << std::endl;
  }

  // Simulated Annealing

  /**
     Constructs a simulated annealing runner by linking it to a state manager, 
     a neighborhood explorer, and an input object.cd ../

     @param s a pointer to a compatible state manager
     @param ne a pointer to a compatible neighborhood explorer
     @param in a poiter to an input object
  */
  template <class Input, class State, class Move>
  SimulatedAnnealing<Input,State,Move>::SimulatedAnnealing(StateManager<Input,State>* s, NeighborhoodExplorer<Input,State,Move>* ne, Input* in)
    : MoveRunner<Input,State,Move>(s, ne, in, "Runner name", "Simulated Annealing")
  { 
    min_temperature = 0.0001;
  }

  /**
     Reads the simulated annealing parameters from the standard input.
  */  
  template <class Input, class State, class Move>
  void SimulatedAnnealing<Input,State,Move>::ReadParameters()
  {
    std::cout << "SIMULATED ANNEALING -- INPUT PARAMETERS" << std::endl;
    std::cout << "Start temperature: ";
    std::cin >> start_temperature;
    std::cout << "Cooling rate: ";
    std::cin >> cooling_rate;
    std::cout << "Neighbors sampled at each temperature : ";
    std::cin >> neighbor_sample;
  }

  /**
     Initializes the run by invoking the companion superclass method, and
     setting the temperature to the start value.
  */
  template <class Input, class State, class Move>
  void SimulatedAnnealing<Input,State,Move>::InitializeRun()
  {
    MoveRunner<Input,State,Move>::InitializeRun(); 
    assert(start_temperature > 0 && cooling_rate > 0 && neighbor_sample > 0);
    temperature = start_temperature;
  }

  /**
     Stores the current state as best state (it is obviously a local minimum).
  */
  template <class Input, class State, class Move>
  void SimulatedAnnealing<Input,State,Move>::TerminateRun()
  { 
    this->best_state = this->current_state;
    this->best_state_cost = this->current_state_cost;
  }

  /**
     A move is randomly picked.
  */
  template <class Input, class State, class Move>
  void SimulatedAnnealing<Input,State,Move>::SelectMove() 
  { 
    this->p_nhe->RandomMove(this->current_state, this->current_move); 
    this->ComputeMoveCost(); 
  }

  /**
     Sets the simulated annealing  parameters, passed through a parameter box.

     @param pb the object containing the parameter setting for the algorithm
  */
  template <class Input, class State, class Move>
  void SimulatedAnnealing<Input,State,Move>::SetParameters(const ParameterBox& pb)
  {
    pb.Get("start temperature", start_temperature);
    pb.Get("cooling rate", cooling_rate);
    pb.Get("neighbors sampled", neighbor_sample);
    pb.Get("max iteration", this->max_iteration);
  }

  /**
     Outputs some simulated annealing statistics on a given output stream.

     @param os the output stream
  */
  template <class Input, class State, class Move>
  void SimulatedAnnealing<Input,State,Move>::Print(std::ostream & os) const
  {
    MoveRunner<Input,State,Move>::Print(os);
    os << "PATAMETERS: " << std::endl;
    os << "  Start temperature : " << start_temperature << std::endl;
    os << "  Cooling rate : " << cooling_rate << std::endl;
    os << "  Neighbor sample : " << neighbor_sample << std::endl;
    os << "  Max iteration : " << this->max_iteration << std::endl;
    os << "RESULTS : " << std::endl;
    os << "  Number of iterations : " << this->number_of_iterations << std::endl;
    os << "  Current state [cost: " 
       << this->current_state_cost << "] " << std::endl
       << this->current_state << std::endl;
  }

  /**
     The search stops when a low temperature has reached.
   */
  template <class Input, class State, class Move>
  bool SimulatedAnnealing<Input,State,Move>::StopCriterion()
  { return temperature <= min_temperature; }

  /**
     At regular steps, the temperature is decreased 
     multiplying it by a cooling rate.
  */
  template <class Input, class State, class Move>
  void SimulatedAnnealing<Input,State,Move>::UpdateIterationCounter() 
  { this->number_of_iterations++; 
  if (this->number_of_iterations % neighbor_sample == 0)
    temperature *= cooling_rate;
  }

  /** A move is surely accepted if it improves the cost function
      or with exponentially decreasing probability if it is 
      a worsening one.
  */
  template <class Input, class State, class Move>
  bool SimulatedAnnealing<Input,State,Move>::AcceptableMove()
  { return (this->current_move_cost <= 0)
      || (((float)rand())/RAND_MAX < exp(-(this->current_move_cost)/temperature)); }

  /**
     Sets the internal input pointer to the new value passed as parameter.
     
     @param in a pointer to the new input object
  */
  template <class Input, class Output>
  void Solver<Input,Output>::SetInput(Input* in) 
  { p_in = in; }

  /**
     Sets the internal output pointer to the new value passed as parameter.
     
     @param out a pointer to the new output object
  */
  template <class Input, class Output>
  void Solver<Input,Output>::SetOutput(Output* out) 
  { p_out = out; }

  /** 
      Constructs a solver by providing it an input and an output objects.

      @param in a pointer to an input object
      @param out a pointer to an output object
  */
  template <class Input, class Output>
  Solver<Input,Output>::Solver(Input* in, Output* out)
    : p_in(in), p_out(out) 
  {}

  /**
     Returns the input pointer which the object is attached to.
     
     @return the pointer to the input
  */
  template <class Input, class Output>
  Input* Solver<Input,Output>::GetInput()
  { return p_in; }

  /**
     Returns the output pointer which the object is attached to.
     
     @return the pointer to the output
  */
  template <class Input, class Output>
  Output* Solver<Input,Output>::GetOutput()
  { return p_out; }

  /**
     Set the number of states which should be tried in 
     the initialization phase.
  */
  template <class Input, class Output, class State>
  void LocalSearchSolver<Input,Output,State>::SetInitTrials(int t) 
  { number_of_init_trials = t; }

  /**
     Sets the internal input pointer to the new value passed as parameter.
     
     @param in the new input.
  */
  template <class Input, class Output, class State>
  void LocalSearchSolver<Input,Output,State>::SetInput(Input* in) 
  { 
    this->p_in = in;
    internal_state.SetInput(in); 
  }

  /** 
      Returns the cost of the internal state.
      
      @return the cost of the internal state
  */
  template <class Input, class Output, class State>
  fvalue LocalSearchSolver<Input,Output,State>::InternalStateCost() 
  { return internal_state_cost; }

  /**
     Constructs a local search solver by providing it a state manager,
     an output manager, an input object, and an output object.

     @param sm a pointer to a compatible state manager
     @param om a pointer to a compatible output manager
     @param in a pointer to an input object
     @param out a pointer to an output object
  */
  template <class Input, class Output, class State>
  LocalSearchSolver<Input,Output,State>::LocalSearchSolver(StateManager<Input,State>* sm, OutputManager<Input,Output,State>* om, Input* in, Output* out) 
    : Solver<Input, Output>(in,out), p_sm(sm),  p_om(om), 
    number_of_init_trials(1)
  {
    if (in != NULL)
      internal_state.SetInput(in); 
  }

  /**
     The output is delivered by converting the final internal state
     to an output object by means of the output manager.
  */
  template <class Input, class Output, class State>
  void LocalSearchSolver<Input,Output,State>::DeliverOutput() 
  { p_om->OutputState(internal_state,*this->p_out); }

  /**
     The initial state is generated by delegating this task to 
     the state manager. The function invokes the SampleState function.
  */
  template <class Input, class Output, class State>
  void LocalSearchSolver<Input,Output,State>::FindInitialState() 
  { internal_state_cost = p_sm->SampleState(internal_state,number_of_init_trials); }

  /**
     The cost of the internal state is computed by the state manager.
  */
  template <class Input, class Output, class State>
  void LocalSearchSolver<Input,Output,State>::ComputeCost() 
  { internal_state_cost = p_sm->CostFunction(internal_state); }


  /**
     Checks wether the object state is consistent with all the related
     objects.
  */
  template <class Input, class Output, class State>
  void LocalSearchSolver<Input,Output,State>::Check()
  {
    assert(this->p_in != NULL);
    assert(this->p_in == p_sm->GetInput());
    assert(this->p_in == p_om->GetInput());
  }

  /**
     Sets the runner employed for solving the problem to the one passed as
     parameter.

     @param r the new runner to be used
  */
  template <class Input, class Output, class State>
  void SimpleLocalSearch<Input,Output,State>::SetRunner(Runner<Input,State> *r) 
  { p_runner = r; }


  /**
     Constructs a simple local search solver by providing it links to
     a state manager, an output manager, a runner, an input,
     and an output object.

     @param sm a pointer to a compatible state manager
     @param om a pointer to a compatible output manager
     @param r a pointer to a compatible runner
     @param in a pointer to an input object
     @param out a pointer to an output object
  */
  template <class Input, class Output, class State>
  SimpleLocalSearch<Input,Output,State>::SimpleLocalSearch(StateManager<Input,State>* sm, OutputManager<Input,Output,State>* om, Runner<Input,State>* r, Input* in, Output* out)
    : LocalSearchSolver<Input,Output,State>(sm,om,in,out) 
  { p_runner = r; }


  /**
     Constructs a simple local search solver by providing it links to
     a state manager, an output manager, an input, and an output object.

     @param sm a pointer to a compatible state manager
     @param om a pointer to a compatible output manager
     @param in a pointer to an input object
     @param out a pointer to an output object
  */
  template <class Input, class Output, class State>
  SimpleLocalSearch<Input,Output,State>::SimpleLocalSearch(StateManager<Input,State>* sm, OutputManager<Input,Output,State>* om, Input* in, Output* out)
    : LocalSearchSolver<Input,Output,State>(sm,om,in,out) 
  { p_runner = NULL; }


  /** 
      Returns the number of iterations performed by the attached runner.
      
      @return the number of iterations elapsed.
  */
  template <class Input, class Output, class State>
  unsigned long SimpleLocalSearch<Input,Output,State>::NumberOfIterations() const 
  { return p_runner->NumberOfIterations(); }

  /**
     Lets the runner Go, and then collects the best state found.
  */
  template <class Input, class Output, class State>
  void SimpleLocalSearch<Input,Output,State>::Run()
  {
    p_runner->SetCurrentState(this->internal_state);
    p_runner->Go();
    this->internal_state = p_runner->GetBestState();
    this->internal_state_cost = p_runner->BestStateCost();
  }

  /**
     Performs a full solving procedure by finding an initial state, 
     running the attached runner and delivering the output.
  */
  template <class Input, class Output, class State>
  void LocalSearchSolver<Input,Output,State>::Solve()
  {
    FindInitialState();
    Run();
    DeliverOutput();
  }
  
  /**
     Start again a solving procedure, running the attached runner from
     the current internal state.
  */
  template <class Input, class Output, class State>
  void LocalSearchSolver<Input,Output,State>::ReSolve()
  {
    Run();
    DeliverOutput();
  }

  /**
     Tries multiple runs on different initial states and records the
     best one.
     
     @param n the number of trials
   */
  template <class Input, class Output, class State>
  void LocalSearchSolver<Input,Output,State>::MultiStartSolve(unsigned int n)
  { 
    State best_state;
    fvalue best_state_cost = 0; // we assign it a value only to prevent
                                // warnings from "smart" compilers
    for (unsigned int i = 0; i < n; i++)
      { 
	FindInitialState();
	Run();
	if (i == 0 || internal_state_cost < best_state_cost)
	  { 
	    best_state = internal_state;
	    best_state_cost = internal_state_cost;
	  }
      }
    internal_state = best_state;
    internal_state_cost = best_state_cost;
    DeliverOutput();
  }

  /**
     In the case of multi-runner solvers, the number of iterations is the
     overall number of iterations performed by any runner.
  */
  template <class Input, class Output, class State>
  unsigned long MultiRunnerSolver<Input,Output,State>::NumberOfIterations() const 
  { return total_iterations; }


  /**
     Constructs a multi runner solver by providing it links to
     a state manager, an output manager, an input, and an output object.

     @param sm a pointer to a compatible state manager
     @param om a pointer to a compatible output manager
     @param in a pointer to an input object
     @param out a pointer to an output object
  */
  template <class Input, class Output, class State>
  MultiRunnerSolver<Input,Output,State>::MultiRunnerSolver(StateManager<Input,State>* sm, OutputManager<Input,Output,State>* om, Input* in, Output* out)
    : LocalSearchSolver<Input,Output,State>(sm,om,in,out), 
    total_iterations(0)
  { runners.clear(); }

  /**
     Sets the i-th runner managed by the solver to the passed parameter.

     @param r a pointer to a compatible runner
     @param i the runner position to set with r
  */
  template <class Input, class Output, class State>
  void MultiRunnerSolver<Input,Output,State>::SetRunner(Runner<Input,State> *r, unsigned int i) 
  { 
    assert(i < runners.size());
    runners[i] = r; 
  }

  /**
     Adds the given runner to the list of the managed runners.

     @param r a pointer to a compatible runner to add
   */
  template <class Input, class Output, class State>
  void MultiRunnerSolver<Input,Output,State>::AddRunner(Runner<Input,State> *r)
  { runners.push_back(r); }

  /**
     Clears the vector of runners managed by the solver.
  */
  template <class Input, class Output, class State>
  void MultiRunnerSolver<Input,Output,State>::ClearRunners()
  { runners.clear(); }

  /**
     Lets all the managed runners Go, and then it collects the best state
     found.
   */
  template <class Input, class Output, class State>
  void ComparativeSolver<Input,Output,State>::Run()
  { 
    int i;
    start_state = this->internal_state;
    this->runners[0]->SetCurrentState(start_state);
    this->runners[0]->Go();
    this->runners[0]->ComputeCost();
    this->internal_state = this->runners[0]->GetBestState();
    this->internal_state_cost = this->runners[0]->BestStateCost();
	
    for (i = 1; i < this->runners.size(); i++)
      {
	this->runners[i]->SetCurrentState(start_state);
	this->runners[i]->Go();
	this->runners[i]->ComputeCost();
	this->total_iterations += this->runners[i]->NumberOfIterations();
	if (this->runners[i]->BestStateCost() < this->internal_state_cost)
	  {
	    this->internal_state = this->runners[i]->GetBestState();
	    this->internal_state_cost = this->runners[i]->BestStateCost();
	  }
      }
  }

  /**
     Sets the number of rounds to the given value.
     
     @param r the number of rounds.
  */
  template <class Input, class Output, class State>
  void TokenRingSolver<Input,Output,State>::SetRounds(unsigned int r) 
  { max_idle_rounds = r; }

  /**
     Starts the token-ring from the i-th runner.
     
     @param i the runner which to start form
  */
  template <class Input, class Output, class State>
  void TokenRingSolver<Input,Output,State>::SetStartRunner(unsigned int i)
  { this->start_runner = i; }


  /**
     Constructs a token-ring runner solver by providing it links to
     a state manager, an output manager, an input, and an output object.

     @param sm a pointer to a compatible state manager
     @param om a pointer to a compatible output manager
     @param in a pointer to an input object
     @param out a pointer to an output object
  */
  template <class Input, class Output, class State>
  TokenRingSolver<Input,Output,State>::TokenRingSolver(StateManager<Input,State>* sm, OutputManager<Input,Output,State>* om, Input* in, Output* out) 
    : MultiRunnerSolver<Input,Output,State>(sm,om,in,out), 
    max_idle_rounds(1) 
  {}

  /**
     Checks wether the object state is consistent with all the related
     objects.
  */
  template <class Input, class Output, class State>
  void TokenRingSolver<Input,Output,State>::Check()
  {
    LocalSearchSolver<Input,Output,State>::Check();
    for (unsigned int i = 0; i < this->runners.size(); i++)
      {
	this->runners[i]->Check();
	assert(this->runners[i]->GetInput() == this->p_in);
      }
  }

  /**
     Outputs the solver state on a given output stream.

     @param os the output stream
  */
  template <class Input, class Output, class State>
  void TokenRingSolver<Input,Output,State>::Print(std::ostream& os) const
  {
    os << "Solver State" << std::endl;
    for (unsigned int i = 0; i < this->runners.size(); i++)
      {
	os << "Runner " << i << std::endl;
	this->runners[i]->Print(os);
      }
  }

  /**
     Runs all the managed runners one after another till no improvement
     has produced in a given number of rounds
  */
  template <class Input, class Output, class State>
  void TokenRingSolver<Input,Output,State>::Run()
  {
    assert(this->start_runner < this->runners.size());
  	
    // i is the current runner, j is the previous one;
    unsigned int i = this->start_runner, j = (this->start_runner >= 1) ?
      (this->start_runner - 1) : this->runners.size() - 1;
    int idle_rounds = 0;
    bool interrupt_search = false;
    bool improvement_found = false;
	
    this->ComputeCost(); // Set internal_state_cost
    // internal_state_cost is used to check
    // whether a full round has produces improvements or not
    this->runners[i]->SetCurrentState(this->internal_state);
	
    while (idle_rounds < max_idle_rounds && !interrupt_search)
      { 
        do
	  {
	    this->runners[i]->Go();
	    if (this->runners[i]->BestStateCost() < this->internal_state_cost) 
	      { 
	        this->internal_state = this->runners[i]->GetBestState(); 
	        this->internal_state_cost = this->runners[i]->BestStateCost(); 
	        improvement_found = true;
	      } 
	    this->total_iterations += this->runners[i]->NumberOfIterations();
	    if (this->runners[i]->LowerBoundReached() || this->runners.size() == 1)
	      {
	        interrupt_search = true;
	        break;
	      }
	    j = i;
	    i = (i + 1) % this->runners.size();
	    this->runners[i]->SetCurrentState(this->runners[j]->GetBestState());
  	  }
        while (i != this->start_runner);
		
        if (!interrupt_search)
	  {
	    if (improvement_found)
	      idle_rounds = 0;
	    else
  	      idle_rounds++;
  	    improvement_found = false;
  	  }
      }
  }

  // Abstract Move Tester

  /**
     Constructs an abstract move tester and assign it a name passed 
     as parameter.
     @param s the name of the tester
  */
  template <class Input, class Output, class State>
  AbstractMoveTester<Input,Output,State>::AbstractMoveTester(std::string s) 
  { name = s; }

  /**
     Gets the name of the tester.
     @returns the name of the tester
  */
  template <class Input, class Output, class State>
  std::string AbstractMoveTester<Input,Output,State>::Name() 
  { return name; }

  // Move Tester

  /**
     Constructs a move tester by providing it links to
     a state manager, an output manager, a neighborhood explorer, a name and 
     an input object.

     @param sm a pointer to a compatible state manager
     @param om a pointer to a compatible output manager
     @param ne a pointer to a compatible neighborhood explorer
     @param nm the name of the move tester
     @param in a pointer to an input object
  */
  template <class Input, class Output, class State, class Move>
  MoveTester<Input,Output,State,Move>::MoveTester(StateManager<Input,State>* sm, OutputManager<Input,Output,State>* om, NeighborhoodExplorer<Input,State,Move>* ne, std::string nm, Input* in)
    : AbstractMoveTester<Input,Output,State>(nm)
  { 
    if (in != NULL)
      out.SetInput(in);
    p_in = in; 
    p_sm = sm; 
    p_om = om;
    p_nhe = ne; 
  }  

  /**
     Sets the internal input pointer to the new value passed as parameter.
     
     @param in the new input.
  */
  template <class Input, class Output, class State, class Move>
  void MoveTester<Input,Output,State,Move>::SetInput(Input* in)
  {
    p_in = in;
    out.SetInput(in);
    p_nhe->SetInput(in);
  }

  /**
     Manages the move tester menu for the given state.
     
     @param st the state to test
  */
  template <class Input, class Output, class State, class Move>
  void MoveTester<Input,Output,State,Move>::RunTestMenu(State& st)
  {
    do
      {
	ShowMenu();
	if (choice != 0)
	  {
	    clock_t start_t = clock();
	    ExecuteChoice(st);
	    clock_t end_t = clock();
	    double eltime = (end_t - start_t)/(double)CLOCKS_PER_SEC;
	    p_om->OutputState(st,out);
	    std::cout << "CURRENT SOLUTION " << std::endl << out << std::endl;
	    std::cout << "CURRENT COST : " << p_sm->CostFunction(st) << std::endl;
	    std::cout << "ELAPSED TIME : " << eltime << "s" << std::endl;
	  }
      }
    while (choice != 0);
    std::cout << "Leaving move menu" << std::endl;
  }

  /**
     Outputs the menu options.
  */
  template <class Input, class Output, class State, class Move>
  void MoveTester<Input,Output,State,Move>::ShowMenu()
  { 
    std::cout << "Move Menu: " << std::endl
	      << "     (1)  Best" << std::endl	   
	      << "     (2)  Random" << std::endl
	      << "     (3)  Input" << std::endl
	      << "     (4)  Print Neighborhood statistics" << std::endl
	      << "     (5)  Check Move Info" << std::endl
	      << "     (0)  Return to Main Menu" << std::endl
	      << " Your choice: ";
    std::cin >> choice;
  }

  /**
     Execute the menu choice on the given state.

     @param st the current state
  */
  template <class Input, class Output, class State, class Move>
  void MoveTester<Input,Output,State,Move>::ExecuteChoice(State& st)
  {
    Move mv;
    switch(choice)
      {	
      case 1: 
	p_nhe->BestMove(st,mv);
	break;
      case 2: 
	p_nhe->RandomMove(st,mv);
	break;
      case 3:      
	std::cout << "Input move : ";
	std::cin >> mv;
	break;
      case 4: 
	p_nhe->NeighborhoodStatistics(st);
	break;
      case 5: 
	{
	  char ch;
	  std::cout << "Random move (y/n)? ";
	  std::cin >> ch;
	  if (ch == 'y' || ch == 'Y')
	    p_nhe->RandomMove(st,mv);
	  else
	    {
	      std::cout << "Input move : ";
	      std::cin >> mv;
	    }
	  std::cout << "Move info" << std::endl;
	  p_nhe->PrintMoveInfo(st,mv,std::cout);
	  break;
	}
      default:
	std::cout << "Invalid choice" << std::endl;
      }
    if (choice == 1 || choice == 2 || choice == 3)
      {
	std::cout << "Move : " << mv << std::endl;
	if (p_nhe->FeasibleMove(st,mv))
	  p_nhe->MakeMove(st,mv);
	else
	  std::cout << "Infeasible move!" << std::endl;
      }
  }

  // State Tester

  /**
     Constructs a state tester by providing it links to
     a state manager, an output manager, and an input object.

     @param sm a pointer to a compatible state manager
     @param om a pointer to a compatible output manager
     @param in a pointer to an input object
  */
  template <class Input, class Output, class State>
  StateTester<Input,Output,State>::StateTester(StateManager<Input,State>* s, OutputManager<Input,Output,State>* o, Input* in)
    : p_sm(s), p_om(o), p_in(in)
  {
    if (in != NULL)
      out.SetInput(in); 
  }

  /**
     Sets the internal input pointer to the new value passed as parameter.
     
     @param in the new input.
  */
  template <class Input, class Output, class State>
  void StateTester<Input,Output,State>::SetInput(Input* in)
  {
    p_in = in;
    out.SetInput(in);
  }

  /**
     Manages the state tester menu for the given state.
     
     @param st the state to test
  */  
  template <class Input, class Output, class State>
  void StateTester<Input,Output,State>::RunTestMenu(State& st)
  {
    ShowMenu();    
    clock_t start_t = clock();
    ExecuteChoice(st);
    clock_t end_t = clock();
    double eltime = (end_t - start_t)/(double)CLOCKS_PER_SEC;
    if (choice != 4 && choice != 5 && choice != 6 && choice != 7) // choices 4-7 do not change the state
      {
	p_om->OutputState(st,out);
	std::cout << "CURRENT SOLUTION " << std::endl << out << std::endl;
	std::cout << "CURRENT COST : " << p_sm->CostFunction(st) << std::endl;
	std::cout << "ELAPSED TIME : " << eltime << "s" << std::endl;
      }
  }

  /**
     Manages an adbridged menu for building the initial state.
  */
  template <class Input, class Output, class State>
  void StateTester<Input,Output,State>::RunInputMenu(State& st)
  {
    ShowReducedMenu();
    clock_t start_t = clock();
    ExecuteChoice(st);
    clock_t end_t = clock();
    double eltime = (end_t - start_t)/(double)CLOCKS_PER_SEC;
    p_om->OutputState(st,out);
    std::cout << "INITIAL SOLUTION " << std::endl << out << std::endl;
    std::cout << "INITIAL COST : " << p_sm->CostFunction(st) << std::endl;
    std::cout << "ELAPSED TIME : " << eltime << "s" << std::endl;
  }

  /**
     Outputs the menu options.
  */
  template <class Input, class Output, class State>
  void StateTester<Input,Output,State>::ShowMenu()
  {
    std::cout << "State Menu: " << std::endl
	      << "    (1) Random state " << std::endl
	      << "    (2) Sample state" << std::endl
	      << "    (3) Read from file" << std::endl
	      << "    (4) Write to file" << std::endl
	      << "    (5) Show state" << std::endl
	      << "    (6) Show input" << std::endl
	      << "    (7) Show cost function components" << std::endl
	      << "Your choice : ";
    std::cin >> choice;
  }

  /**
     Outputs a reduced set of options for the initial state building.
  */
  template <class Input, class Output, class State>
  void StateTester<Input,Output,State>::ShowReducedMenu()
  {
    std::cout << "Initial State Menu: " << std::endl
	      << "    (1) Random state " << std::endl
	      << "    (2) Sample state" << std::endl
	      << "    (3) Read from file" << std::endl
	      << "Your choice : ";
    std::cin >> choice;
    if (choice == 4 || choice == 5 || choice == 6) choice = -1; // choices 4 and 5 do not belong to the reduces menu
  }

  /**
     Execute the menu choice on the given state.
     
     @param st the current state
  */
  template <class Input, class Output, class State>
  void StateTester<Input,Output,State>::ExecuteChoice(State& st)
  {
    unsigned long start_t, end_t;
    start_t = end_t = 0;
    switch(choice)
      {
      case 1: 
	start_t = clock();
	p_sm->RandomState(st);
	end_t = clock();
	break;
      case 2: 
	{
	  int samples;
	  std::cout << "How many samples : ";
	  std::cin >> samples;
	  start_t = clock();
	  p_sm->SampleState(st,samples);
	  end_t = clock();
	  break;
	}
      case 3:
	{
	  std::string file_name;
	  std::cout << "File name : ";
	  std::cin >> file_name;
	  std::ifstream is(file_name.c_str());
	  p_om->ReadState(st,is);
	  break;
	}
      case 4:
	{
	  std::string file_name;
	  std::cout << "File name : ";
	  std::cin >> file_name;
	  std::ofstream os(file_name.c_str());
	  p_om->WriteState(st,os);
	  break;
	}
      case 5:
	{
	  p_sm->PrintState(st); 
	  break;
	}
      case 6:
	{
	  std::cout << *p_in;
	  break;
	}
      case 7:
	{
	  std::cout << std::endl << "Violations: " << std::endl;
	  p_sm->Violations(st);
	  std::cout << std::endl << "Objective: " << std::endl;
	  p_sm->Objective(st);
	  break;
	}
      default:
	std::cout << "Invalid choice" << std::endl;
      }
    if (choice == 1 || choice == 2)
      std::cout << "Time: " << (end_t - start_t)/(float)CLOCKS_PER_SEC << " secs" << std::endl;
  }

  // Tester

  /**
     Sets the i-th move tester as the passed parameter.

     @param p_amt a pointer to a move tester
     @param i the position
  */
  template <class Input, class Output, class State>  
  void Tester<Input, Output, State>::SetMoveTester(AbstractMoveTester<Input,Output,State>* p_amt, int i)
  { assert(i < move_testers.size()); move_testers[i] = p_amt; }

  /**
     Adds a move tester.

     @param p_amt a pointer to a move tester
  */
  template <class Input, class Output, class State>  
  void Tester<Input, Output, State>::AddMoveTester(AbstractMoveTester<Input,Output,State>* p_amt)
  { move_testers.push_back(p_amt); }

  /**
     Removes all the move tester.
  */
  template <class Input, class Output, class State>  
  void Tester<Input, Output, State>::CleanMoveTesters()
  { move_testers.clear(); }

  /**
     Sets the state tester as the passed parameter.

     @param p_st a pointer to a state tester
  */
  template <class Input, class Output, class State>  
  void Tester<Input, Output, State>::SetStateTester(StateTester<Input,Output,State>* p_st)
  { state_tester = p_st; }

  /**
     Constructs a tester by providing it links to
     a state manager, an output manager, and an input object.

     @param sm a pointer to a compatible state manager
     @param om a pointer to a compatible output manager
     @param in a pointer to an input object
  */
  template <class Input, class Output, class State>
  Tester<Input, Output, State>::Tester(StateManager<Input,State>* sm, 
				       OutputManager<Input,Output,State>* om,
				       Input *in)
    : move_testers(0), runners(0), p_in(in)
  {
    if (in != NULL)
      {
	test_state.SetInput(in); 
	out.SetInput(in);  
      }
    p_sm = sm;
    p_om = om;
    state_tester = NULL;
    solver = NULL;
    logstream = &std::cerr;
    output_file_prefix = "";
    plot_file_prefix = "";
  }

  /**
     Sets the internal input pointer to the new value passed as parameter.
     
     @param in the new input.
  */
  template <class Input, class Output, class State>
  void Tester<Input,Output,State>::SetInput(Input* in)
  {
    unsigned int i;

    p_in = in; 
    p_sm->SetInput(in);
    p_om->SetInput(in);
    test_state.SetInput(in);
    state_tester->SetInput(in);
    out.SetInput(in);
    if (solver != NULL) 
      solver->SetInput(in);
    for (i = 0; i < runners.size(); i++)
      if (runners[i] != NULL) 
	runners[i]->SetInput(in);
    for (i = 0; i < move_testers.size(); i++)
      move_testers[i]->SetInput(in);
  }

  /**
     Checks wether the object state is consistent with all the related
     objects.
  */
  template <class Input, class Output, class State>
  void  Tester<Input,Output,State>::Check()
  {
    assert(p_sm->GetInput() == p_in);
    assert(p_om->GetInput() == p_in);
    solver->Check();
    assert(solver->GetInput() == p_in);
    for (unsigned int i = 0; i < runners.size(); i++)
      {
	runners[i]->Check();
	assert(runners[i]->GetInput() == p_in);
      }
  }

  /**
     Outputs the state of the tester on a given output stream.

     @param os the output stream
  */
  template <class Input, class Output, class State>
  void  Tester<Input,Output,State>::Print(std::ostream& os) const
  {
    os << "Tester State" << std::endl;
    for (unsigned int i = 0; i < runners.size(); i++)
      {
	os << "Runner " << i << std::endl;
	runners[i]->Print(os);
      }
    solver->Print(os);
  }

  /**
     Sets the token-ring solver associated to the tester.

     @param p_so a pointer to a compatible token-ring solver
  */
  template <class Input, class Output, class State>
  void Tester<Input, Output, State>::SetSolver(TokenRingSolver<Input,Output,State>* p_so)
  { 
    solver = p_so; 
    if (p_in != NULL && p_in != solver->GetInput())
      solver->SetInput(p_in);
    if (&out != solver->GetOutput())
      solver->SetOutput(&out);
  }

  /**
     Sets the parameters of the token-ring solver associated to the tester.
     
     @param rounds the maximum number of non improving rounds
     @param start_runner the runner used to start a round
  */
  template <class Input, class Output, class State>
  void Tester<Input, Output, State>::SetSolverParameters(unsigned int rounds, unsigned int start_runner) 
  {
    assert(solver != NULL);
    solver->SetRounds(rounds);
    solver->SetStartRunner(start_runner);
  }  

  /**
     Removes all the runners attached to the tester.
  */
  template <class Input, class Output, class State>
  void Tester<Input, Output, State>::CleanRunners()
  { 
    runners.clear();
  }

  /** 
      Sets the i-th runner attached to the tester to the one passed 
      as parameter.
      
      @param p_ru a pointer to a compatible runner
      @param i the position of the runner
  */
  template <class Input, class Output, class State>
  void Tester<Input, Output, State>::SetRunner(Runner<Input,State>* p_ru, unsigned int i)
  { 
    runners[i] = p_ru; 
    if (p_in != NULL)
      runners[i]->SetInput(p_in);
  }

  /**
     Adds a runner to the tester.
     
     @param p_ru a pointer to a compatible runner
  */
  template <class Input, class Output, class State>
  void Tester<Input, Output, State>::AddRunner(Runner<Input,State>* p_ru)
  { 
    assert(p_ru != NULL);
    if (p_in != p_ru->GetInput())
      p_ru->SetInput(p_in);
    runners.push_back(p_ru);
  }

  /**
     Removes all the runners attached to the solver
  */
  template <class Input, class Output, class State>
  void Tester<Input, Output, State>::CleanSolver() 
  { 
    assert(solver != NULL); 
    solver->ClearRunners(); 
  }

  /** 
      Attaches the runner whose name and type are passed as parameter to 
      the solver.

      @param name the name of the runner to be attached
      @param type the type of the runner to be attached
  */
  template <class Input, class Output, class State>
  int Tester<Input, Output, State>::AddRunnerToSolver(std::string name, std::string type)
  {
    assert(solver != NULL);
    bool found = false, type_mismatch;
    for (unsigned int i = 0; i < runners.size(); i++)
      if (name == runners[i]->Name())
	{
	  found = true;
	  if (type == runners[i]->Type()) 
	    {
	      type_mismatch = false;
	      solver->AddRunner(runners[i]);	  
	    }
	  else
	    type_mismatch = true;
	  break;
	}
    if (found)
      if (!type_mismatch)
	return 0;
      else
	return RUNNER_TYPE_MISMATCH;
    else
      return RUNNER_NOT_FOUND;
  }

  /**
     Sets the parameters of the runner specified by name and type.

     @param name the name of the runner
     @param type the type of the runner
     @param pb the parameterbox that contains the parameters to
     be passed
  */
  template <class Input, class Output, class State>
  void Tester<Input, Output, State>::SetRunningParameters(std::string name, std::string type, const ParameterBox& pb)
  {
    bool found = false;
    for (unsigned int i = 0; i < runners.size(); i++) 
      if (name == runners[i]->Name()) 
	{
	  runners[i]->SetParameters(pb);
	  found = true;
	  break;
	}
    assert(found);
  }
  
  /**
     Starts the solver and collects the results.
  */
  template <class Input, class Output, class State>
  void Tester<Input, Output, State>::StartSolver()
  { 
    double avgtime = 0;
    fvalue avgcost = 0, avgviol = 0, avgobj = 0;
    assert(solver != NULL);
    *logstream << "Run\t" << "elapsed time\t" << "cost \t" << "violations\t" << "objective\t" << std::endl;
    *logstream << "--------------------------------------------------------------------------" << std::endl;
    for (unsigned int i = 1; i <= trials; i++)
      {
	unsigned long start_t = clock();
	solver->Solve();
	unsigned long end_t = clock();      
	double eltime = (end_t - start_t)/(double)CLOCKS_PER_SEC;
	p_om->InputState(test_state,out);
	// writing the output in a output file
	if (output_file_prefix != "")
	  {
	    std::string fn(output_file_prefix);
	    char* trial = new char[(int)ceil(log10(i))+1];
	    sprintf(trial,"%d",i);
	    fn += '-' + std::string(trial) + std::string(".out");
	    std::ofstream os(fn.c_str(),std::ios::out);
	    os << out;
	    delete trial;
	  }      
	fvalue cost = p_sm->CostFunction(test_state);
	fvalue viol = p_sm->Violations(test_state);
	fvalue obj = p_sm->Objective(test_state);
	*logstream << i << "\t" << eltime << "\t\t" 
		   << cost << "\t" << viol << "\t\t" << obj << std::endl;      
	avgtime += eltime;
	avgcost += cost;
	avgviol += viol;
	avgobj += obj;
      }
	
    avgtime /= trials; avgcost /= trials; avgviol /= trials; avgobj /= trials;
    *logstream << "--------------------------------------------------------------------------" << std::endl;
    *logstream << "Avg:\t" << avgtime << "\t\t" << avgcost << "\t" << avgviol << "\t\t" << avgobj << std::endl;
  }

  /**
     Loads a problem instance specified by a name.

     @param id the identifier of the instance to be loaded
  */
  template <class Input, class Output, class State>
  void Tester<Input, Output, State>::LoadInstance(std::string id) 
  {
    *logstream << "Instance: " << id << std::endl;
    CleanSolver();  
    p_in->Load(id);
    SetInput(p_in);
  }

  /**
     Processes a given batch file of experiments.

     @param filename the name of the batch file
  */
  template <class Input, class Output, class State>
  void Tester<Input, Output, State>::ProcessBatch(std::string filename) 
  { 
    if (yyin = fopen(filename.c_str(), "r"))
      {
	yyout = stdout;
	yyparse();
	fclose(yyin);
      }
    else
      std::cerr << "Error: " << filename << " file not found" << std::endl;
  }

  
  /**
     Manages the tester main menu.     
  */  

  template <class Input, class Output, class State>
  void Tester<Input, Output, State>::RunMainMenu()
  {
    unsigned int i;
    assert(state_tester != NULL);
    for (i = 0; i < move_testers.size(); i++)
      assert(move_testers[i] != NULL);
    for (i = 0; i < runners.size(); i++)
      assert(runners[i] != NULL);
	
    state_tester->RunInputMenu(test_state);
	
    do
      {
	ShowMainMenu();
	if (choice != 0)
	  ExecuteMainChoice();
      }
    while (choice != 0);
    std::cout << "Bye bye..." << std::endl;
  }

  /**
     Outputs the main menu options.
  */
  template <class Input, class Output, class State>
  void Tester<Input,Output, State>::ShowMainMenu()
  {
    std::cout << "MAIN MENU:" << std::endl
	      << "   (1) Move menu" << std::endl
	      << "   (2) Run menu" << std::endl
	      << "   (3) State menu" << std::endl
	      << "   (4) Process batch file" << std::endl
	      << "   (5) Debugging" << std::endl
	      << "   (0) Exit" << std::endl
	      << " Your choice: ";
    std::cin >> choice;
  }

  /**
     Outputs a debugging menu.
  */
  template <class Input, class Output, class State>
  void Tester<Input,Output, State>::ShowDebuggingMenu()
  {
    std::cout << "DEBUGGING MENU:" << std::endl
	      << "   (1) Print tester status" << std::endl
	      << "   (2) Check tester status" << std::endl
	      << " Your choice: ";
    std::cin >> choice;
  }

  /**
     Execute a choice made in the debugging menu.
  */
  template <class Input, class Output, class State>
  void Tester<Input, Output,State>::ExecuteDebuggingMenu()
  {
    switch(choice)
      {
      case 1: 
	Print();
	Check();
	break;
      case 2: 
	Check();
	break;
      default:
	std::cout << "Invalid choice" << std::endl;
      }
  }

  /**
     Execute a choice made in the main menu.
  */
  template <class Input, class Output, class State>
  void Tester<Input, Output,State>::ExecuteMainChoice()
  {
    switch(choice)
      {
      case 1: 
	ShowMovesMenu();
	ExecuteMovesChoice();
	break;
      case 2: 
	ShowRunMenu();
	ExecuteRunChoice();
	break;
      case 3: 
	state_tester->RunTestMenu(test_state);
	break;
      case 4:
	{
	  std::string file_name;
	  std::cout << "Insert Batch File name : ";
	  std::cin >> file_name;
	  ProcessBatch(file_name);
	  break;
	}
      case 5:
	ShowDebuggingMenu();
	ExecuteDebuggingMenu();	
      case 0:
	break;
      default:
	std::cout << "Invalid choice" << std::endl;
      }
  }

  /**
     Outputs the menu for the move testers.
  */
  template <class Input, class Output, class State>
  void Tester<Input, Output,State>::ShowMovesMenu()
  {
    unsigned int i;
    std::cout << "MOVES MENU: " << std::endl;
    for (i = 0; i < move_testers.size(); i++)
      std::cout << "   (" << i+1 << ") " << move_testers[i]->Name() << std::endl;
    std::cout << "   (0) Return to Main Menu" << std::endl;
    std::cout << " Your choice: ";
    std::cin >> sub_choice;
  }

  /**
     Outputs the menu for the runners.
  */
  template <class Input, class Output, class State>
  void Tester<Input,Output, State>::ShowRunMenu()
  {
    unsigned int i;
    std::cout << "RUN MENU: " << std::endl;
    for (i = 0; i < runners.size(); i++)
      std::cout << "   (" << i+1 << ") " << runners[i]->Name() << " [" << runners[i]->Type() << "]" << std::endl;
    std::cout << "   (0) Return to Main Menu" << std::endl;
    std::cout << " Your choice: ";
    std::cin >> sub_choice;
  }

  /**
     Execute a choice from the move testers menu.
  */
  template <class Input, class Output, class State>
  void Tester<Input,Output, State>::ExecuteMovesChoice()
  {
    if (sub_choice > 0)
      move_testers[sub_choice-1]->RunTestMenu(test_state);
  }

  /**
     Execute a choice from the runners menu.
  */
  template <class Input, class Output, class State>
  void Tester<Input,Output, State>::ExecuteRunChoice()
  {
    if (sub_choice > 0)
      {
	runners[sub_choice-1]->ReadParameters();
	runners[sub_choice-1]->SetCurrentState(test_state);
	clock_t start_t = clock();
	runners[sub_choice-1]->Go();
	clock_t end_t = clock();
	double eltime = (end_t - start_t)/(double)CLOCKS_PER_SEC;
	test_state = runners[sub_choice-1]->GetBestState();
	p_om->OutputState(test_state,out);
	std::cout << "CURRENT SOLUTION " << std::endl << out << std::endl;
	std::cout << "CURRENT COST : " << runners[sub_choice-1]->BestStateCost() << std::endl;
	std::cout << "ELAPSED TIME : " << eltime << "s" << std::endl;
      }
  }
} // end of namespace easylocal
