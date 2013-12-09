/** 
    @file EasyLocal.h
    @brief Class declarations

    This file contains all the class declarations of the EasyLocal++
    framework.   
    
   @author Andrea Schaerf (schaerf@uniud.it), 
            Luca Di Gaspero (digasper@dimi.uniud.it)
   @version 0.2
   @date 18 Jan 2002
   @note This version works both with MS Visual C++ and the GNU C++ 
         compiler. Yet, it is extensively tested only with the GNU compiler.
*/

#ifndef __EASYLOCAL_H
#define __EASYLOCAL_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <cmath>
#include <cassert>
#include <ctime>
#include <cstdio>

/** This function is provided by a bison parser for batch 
    experiment file processing */
extern "C++" int yyparse(); 

/** This is the input file of the ExpSpec language interpreter */
extern FILE *yyin;
/** This is the output file of the ExpSpec language interpreter */
extern FILE *yyout;  

/** The easylocal namespace embeds all the classes of the framework. */
namespace easylocal {
  /** A random number generator. Generates pseudo-random integer values in the
      range [a, b].
      @param a the lower bound of the range
      @param b the upper bound of the range
      @return a value in the range [a, b]
  */
  int Random(int a, int b);

  /** This constant multiplies the value of the Violations function in the
      hierarchical formulation of the Cost function (i.e., 
      CostFunction(s) = HARD_WEIGHT * Violations(s) + Objective(s)).
      @todo The use of the global HARD_WEIGHT is a rough solution, 
      waiting for an idea of a general mechanism for managing cost function 
      weights.
  */
#ifndef HARD_WEIGHT
#define HARD_WEIGHT 1000
#endif
	
  /** These are used by the Tester class for returning 
      a code error to the parser. */
  const int RUNNER_NOT_FOUND = 1, RUNNER_TYPE_MISMATCH = 2;
	
  /** The fvalue definition represent the codomain of the cost and of the 
      objective function. For default it is set to double, but the user 
      can provide its own definition. 
      In this case, the user must define its own double-valued distance 
      function between fvalues.
      @todo Find a more general mechanism.
  */
#ifndef fvalue
#define fvalue double
#endif

  /** It is the precision above which the computed difference of the
      cost function and the expected value should be considered
      different. */
  const double EPS = 1.0E-6;

  /** This function computes the distance between two @c fvalues and
      returns its value as a double.

      @param x the first function value
      @param y the second function value
      @return the distance between x and y
  */
  inline double distance(fvalue x, fvalue y);
		
  /** @defgroup Helpers Helper classes 
      Helper classes perform actions related to some specific aspects of the
      search. For example, the @c NeighborhoodExplorer is responsible
      for everything concerning the neighborhood: candidate move
      selection, update current state by executing a move, and so on.
      Different @c NeighborhoodExplorers may be defined in case of
      composite search, each one handling a specific neighborhood
      relation used by the algorithm.  

      Helper classes cooperate among themselves.  For example, the @c
      NeighborhoodExplorer is not responsible for the move prohibition
      mechanisms (such as maintaining the tabu list), which are
      delegated to another helper, namely the @c ProibitionManager.
  
      Helper classes do not have their own internal data, but they work on the
      internal state of the runners that invoke them, and interact with
      them through function parameters.  
  */

  // forward class tag declaration
  template <class Input, class State> class Runner;

  /** The State Manager is responsible for all operations on the state
      which are independent of the neighborhood definition, such as
      generating a random state, and computing the cost of a state.
      No @c Move template is supplied to it.  
      @ingroup Helpers
  */
  template <class Input, class State>
  class StateManager
  {
  public:
    /** Generates a random state.
	@note @bf To be implemented in the application.
	@param st the state generated */
    virtual void RandomState(State &st) = 0; 
    /** Possibly updates redundant state data. It is used by the
	output manager to make the state consistent.
	@param st the state to be updated
    */
    virtual void UpdateRedundantStateData(State &st) 
    {}
    virtual fvalue SampleState(State &st, int samples);
    virtual fvalue ImprovedSampleState(State &st, int samples, Runner<Input,State>* r);      
		
    // State Evaluation functions
    virtual fvalue CostFunction(const State& st) const;
    virtual fvalue Objective(const State& st) const;
    virtual fvalue Violations(const State& st) const;
				
    // debug functions
    virtual void PrintState(const State& st) const;
    virtual void Check() const;
		
    void SetInput(Input* in);
    Input* GetInput();
  protected:
    StateManager(Input* in = NULL);
    Input* p_in; /**< A pointer to the input object. */
  };

  /** The Output Manager is responsible for translating between
      elements of the search space and output solutions.  It also
      delivers other output information of the search, and stores and
      retrieves solutions from files.  This is the only helper that
      deals with the @c Output class.  All other helpers work only on
      the @c State class, which represents the elements of the search
      space used by the algorithms.  
      @ingroup Helpers
  */	
  template <class Input, class Output, class State>
  class OutputManager
  {
  public:
    /** Transforms the given state in an output object. 
	@param st the state to transform 
	@param out the corresponding output object. */
    virtual void OutputState(const State &st, Output& out) const = 0;
    /** Transforms an output object in a state object. 
	@param st the resulting state
	@param out the output object to transform */
    virtual void InputState(State &st, const Output& out) const = 0;
    virtual void ReadState(State &st, std::istream &is) const;
    virtual void WriteState(const State &st, std::ostream &os) const;
		
    virtual void Check() const;
    void SetInput(Input* in);
    Input* GetInput();
  protected:
    /** Constructs an output manager by providing it a state manager
     and an input object.
     @param sm a pointer to a state manager
     @param in a pointer to an input object */
    OutputManager(StateManager<Input,State>* sm, Input* in = NULL) 
      :  p_sm(sm), p_in(in) {}
    StateManager<Input,State>* p_sm; /**< A pointer to an attached 
					state manager. */
    Input* p_in;  /**< A pointer to an input object. */
  };
	
  
  /** The Prohibition Manager deals with move prohibition mechanisms
     that prevents cycling and allows for diversification.  
     
     This class is at the top of the hierarchy: we have also a more
     specific prohibition manager, which maintains a list of @c Move
     elements according to the prohibition mechanisms of tabu search.  
     @ingroup Helpers
  */
  template <class Move>
  class ProhibitionManager
  {
  public:
    /** Marks a given move as prohibited, according to the prohibition
	strategy.
	@param mv the move
	@param mv_cost the cost of the move
	@param curr the cost of the current solution
	@param best the cost of the best solution found so far  */
    virtual void InsertMove(const Move& mv, fvalue mv_cost, fvalue curr, fvalue best) = 0;
    /** Checks whether the given move is prohibited, according to the
	prohibition strategy.
	@param mv the move
	@param mv_cost the cost of the move
	@param curr the cost of the current solution
	@param best the cost of the best solution found so far
	@return true if the move is prohibited, false otherwise */
    virtual bool ProhibitedMove(const Move& mv, fvalue mv_cost, fvalue curr, fvalue best) const = 0;
    /** Resets the prohibition manager mechanisms. */
    virtual void Clean() = 0;
    /** Checks whether the state of the prohibition manager is consistent
	with the attached objects. */
    virtual void Check() {}
  };

  // forward class tag declaration
  template <class Move> class ListItem;

  /** The Tabu List Manager handles a list of @c Move elements according
      to the prohibition mechanisms of tabu search.
      Namely it maintains an item in the list for a number of iterations 
      that varies randomly in a given range.
      Each time a new @c Move is inserted in the list, the ones which their 
      iteration count has expired are removed.
      @ingroup Helpers
  */
  template <class Move>
  class TabuListManager : public ProhibitionManager<Move>
  {
#ifndef __GNUC_MINOR__
    friend std::ostream& operator<<(std::ostream&, const TabuListManager<Move>&);
#elif __GNUC_MINOR__ > 7 
    friend std::ostream& operator<< <>(std::ostream&, const TabuListManager<Move>&);
#else 
    friend std::ostream& operator<<(std::ostream&, const TabuListManager<Move>&);
#endif
  public:
    void InsertMove(const Move& mv, fvalue mv_cost, fvalue curr, fvalue best);
    bool ProhibitedMove(const Move& mv, fvalue mv_cost, fvalue curr, 
			fvalue best) const;
    /** Sets the length of the tabu list to be comprised in the range
	[min, max].
	@param min the minimum tabu tenure
	@param max the maximum tabu tenure */
    void SetLength(unsigned int min, unsigned int max) 
    { min_tenure = min; max_tenure = max; }
    void Clean();
    /** Returns the minimum number of iterations a move is considered tabu.
	@return the minimum tabu tenure */
    unsigned int MinTenure() const 
    { return min_tenure; }
    /** Returns the maximum number of iterations a move is considered tabu.
	@return the maximum tabu tenure */
    unsigned int MaxTenure() const 
    { return max_tenure; }
    /** Verifies whether a move is the inverse of another one. Namely it 
	tests whether mv1 is the inverse of mv2 (that will be an element of
	the tabu list).
	@note @bf To be implemented in the application.
	@param mv1 the move to be tested
	@param mv2 the move used for comparison  */
    virtual bool Inverse(const Move& mv1, const Move& mv2) const = 0;
  protected:
    TabuListManager(int min = 0, int max = 0);
    /** Virtual destructor. */
    virtual ~TabuListManager() {}
    virtual bool Aspiration(const Move&, fvalue mv_cost, fvalue curr, fvalue best) const; 
    void InsertIntoList(const Move& mv); 
    /** Updates the function associated with the aspiration criterion. 
	For default it does nothing.
	@param mv_cost the cost of the move
	@param curr the cost of the current solution
	@param best the cost of the best solution found so far */
    void UpdateAspirationFunction(fvalue mv_cost, fvalue curr_cost, fvalue best_cost) 
    {}
    bool ListMember(const Move&) const;
		
    unsigned int min_tenure, /**< The minimum tenure of the tabu list. */
      max_tenure;  /**< The maximum tenure of the tabu list. */
    unsigned long iter; /**< The current iteration. */
    std::list<ListItem<Move> > tlist; /**< The list of tabu moves. */
  };

  /** The class for a @c Move item in the Tabu List.
      It is simply a compound data made up of the @c Move itself and the 
      iteration at which the element shall leave the list.
  */
  template <class Move> 
  class ListItem
  {
    friend class TabuListManager<Move>;
#ifndef __GNUC_MINOR__
    friend std::ostream& operator<<(std::ostream&, const TabuListManager<Move>&);
#elif __GNUC_MINOR__ > 7 
    friend std::ostream& operator<< <>(std::ostream&, const TabuListManager<Move>&);
#else 
    friend std::ostream& operator<<(std::ostream&, const TabuListManager<Move>&);
#endif
  public:
    /** Creates a tabu list item constituted by a move 
	and the leaving iteration passed as parameters.
	@param mv the move to insert into the list
	@param out the iteration at which the move leaves the list.
    */
    ListItem(Move mv, unsigned long out)
      : out_iter(out) { elem = mv; }
  protected:
    Move elem;              /**< The move stored in the list item. */
    unsigned long out_iter; /**< iteration at which the element 
			       leaves the list */
  };
	
  /** Prints out the current status of the Tabu List Manager. */
  template <class Move>
  std::ostream& operator<<(std::ostream&, const TabuListManager<Move>&);
		 

  /** The Neighborhood Explorer is responsible for the strategy
      exploited in the exploration of the neighborhood, and for 
      computing the variations of the cost function due to a specific
      @c Move. 
      @ingroup Helpers
   */
  template <class Input, class State, class Move>
  class NeighborhoodExplorer
  {
  public:
    /** Virtual destructor. */
    virtual ~NeighborhoodExplorer() {}
		
    // move generating functions
    virtual void FirstMove(const State& st, Move& mv);
    /** Generates the move that follows mv in the exploration of the 
	neighborhood of the state st. 
	It returns the generated move in the same variable mv.
	
	@note @bf To be implemented in the application.
	@param st the start state 
	@param mv the move 
     */
    virtual void NextMove(const State &st, Move& mv) = 0;
    /** Generates a random move in the neighborhood of a given state.
	
	@note @bf To be implemented in the application.
	@param st the start state 
	@param mv the generated move 
    */
    virtual void RandomMove(const State &st, Move& mv) = 0;           
    virtual fvalue BestMove(const State &st, Move& mv);
    virtual fvalue SampleMove(const State &st, Move& mv, int samples);
    virtual fvalue BestNonProhibitedMove(const State &st, Move& mv, fvalue curr, fvalue best);
    virtual fvalue SampleNonProhibitedMove(const State &st, Move& mv, int samples, fvalue curr, fvalue best);
    // end of exploration detection
    virtual bool LastMoveDone(const Move &mv);

    /** States whether a move is feasible or not in a given state. 
	For default it acceptsall the moves as feasible ones, but it can
	be overwritten by the user.

	@param st the start state
	@param mv the move to check for feasibility
	@return true if the move is feasible in st, false otherwise
    */
    virtual bool FeasibleMove(const State &st, const Move mv) 
    { return true; }

    virtual void SetProhibitionManager(ProhibitionManager<Move> *pm);

    /** Modifies the state passed as parameter by applying a given
	move upon it.
	
	@note @bf To be implemented in the application.
	@param st the state to modify
	@param mv the move to be applied
    */
    virtual void MakeMove(State &st, const Move& mv) = 0;
		
    // evaluation function
    virtual fvalue DeltaCostFunction(const State& st, const Move & mv);
		
    // debugging/statistic functions
    virtual void NeighborhoodStatistics(const State &st);
    void PrintMoveInfo(const State &st, const Move& mv, std::ostream& os = std::cout);
    
    /** Prompts for reading a move in the neighborhood of a given state
	from an input stream.
	
	@param st the start state
	@param mv the move read from the input stream
	@param is the input stream
    */
    virtual void InputMove(const State &st, Move& mv, std::istream& is = std::cin) const {}
				
    void SetInput(Input* in);
    Input* GetInput();
    void Check();
  protected:
    NeighborhoodExplorer(StateManager<Input,State>* sm, Input* in = NULL);
    NeighborhoodExplorer(StateManager<Input,State>* sm, ProhibitionManager<Move>* pm, Input* in = NULL);
		
    StateManager<Input,State>* p_sm; /**< A pointer to the attached 
					state manager. */
    Input* p_in; /**< A pointer to the input object. */
    virtual fvalue DeltaObjective(const State& st, const Move & mv);
    virtual fvalue DeltaViolations(const State& st, const Move & mv);
		
    
    Move best_move; /**< The best move found in the exploration of the
		         neighborhood (used from the neighborhood enumerating
			 functions such as BestMove). */

    Move start_move;  /**< The start move in the exploration of 
			 the neighborhood. */
    // the prohibition manager used with memory based strategies
    ProhibitionManager<Move>* p_pm; /**< A pointer to the attached
				       prohibition manager (used in case
				       of memory based strategy. */
  };

  /** @defgroup Parameters Parameter handling classes
      This group of classes handles a general parameter passing mechanism
      to runners.
      It is needed because in principle it is not possible to know neither 
      the number nor the type of paramerter a given runner need.
  */

  /** The type of values for the parameters of the runners.
      @ingroup Parameters
  */
  union ValueType {
    unsigned long natural; /**< The type for a natural value. */
    unsigned int short_natural; /**< The type for a small natural value. */
    double real; /**< The type for a real value. */
  };
		
  /** The parameter data class maintains the binding between name of the
      parameter, its type and its value.
      @ingroup Parameters
  */
  class ParameterData {
    std::string name;
    std::string type;
    ValueType value;
  public:
    ParameterData(std::string,std::string,ValueType);
    std::string Name() const;
    std::string Type() const;
    ValueType Value() const;  
  };
	
  /** A paramter box maintains a list of parameters that could be passed
      to a runner.
      @ingroup Parameters
  */
  class ParameterBox
  {
  public:
	void Put(std::string name, std::string type, ValueType value);
    void Put(std::string name, unsigned long value);
    void Put(std::string name, unsigned int value);
    void Put(std::string name, double value);
    void Get(std::string name, std::string type, ValueType& value) const;
    void Get(std::string name, unsigned long& value) const;
    void Get(std::string name, unsigned int& value) const;
    void Get(std::string name, double& value) const;
    void Clear();
  protected:
    std::list<ParameterData> parameters; /**< The list of parameters contained
					    in the parameter box. */
  };

  /** @defgroup Runners Runner classes
      Runner classes are the algorithmic core of the framework. They are 
      responsible for performing a run of a local search technique, 
      starting from an initial state and leading to a final one. 
  */

  /** This is the interface for an abstract runner. 
      Each runner has many data objects for representing the state of
      the search (current state, best state, current move, number of
      iterations, ...), and it maintain links to all the helpers,
      which are invoked for performing specific tasks on its own
      data. Example of actual runners are tabu search and 
      simulated annealing.  
      @ingroup Runners
  */
  template <class Input, class State>
  class Runner 
  {  
  public:
    /** Virtual destructor. */
    virtual ~Runner() {}
    Runner(std::string s = "Runner name", std::string t = "Runner type");
    /** Performs a full run of the search method. */
    virtual void Go() = 0;
    /** Performs a given number of steps of the search method.
	@param n the number of steps to make */	
    virtual void Step(unsigned int n) = 0;
    /** Outputs some informations about the runner on a given output stream
	(cout for default).
	@param os the output stream */
    virtual void Print(std::ostream& os = std::cout) const = 0;
    /** Sets the internal state of the runner to be equal to the parameter.
	@param st the state to become the new internal state */ 
    virtual void SetCurrentState(const State& st) = 0;
    /** Gets the internal state of the runner. 
	@return the current state of the runner */
    virtual State GetCurrentState() = 0;
    /** Gets the cost of the internal state of the runner.
	@return the cost of the current state of the runner */
    virtual fvalue CurrentStateCost() = 0; 
    /** Gets the best state found by the runner.
	@return the best state found by the runner */
    virtual State GetBestState() = 0;
    /** Gets the cost of the best state found by the runner.
	@return the cost of the best state found by the runner */
    virtual fvalue BestStateCost() = 0;
    /** Computes the cost of the current state. */
    virtual void ComputeCost() = 0;
    /** Checks whether the lower bound of the cost function has been
	reached.
	@return true if the lower bound of the cost function has reached,
	false otherwise */
    virtual bool LowerBoundReached() = 0;
    /** Gets the number of iterations performed by the runner.
	@return the number of iterations performed */
    virtual unsigned long NumberOfIterations() const = 0;
    /** Reads runner parameters from the standard input and it sets them. */
    virtual void ReadParameters() = 0;
    std::string Name();
    std::string Type();
    void SetName(std::string s);
    /** Sets the internal input pointer to the new value passed as parameter.
     	@param in the new input object  */
    virtual void SetInput(Input* in) = 0;
    /** Returns the input pointer which the object is attached to.
	@return the pointer to the input object */
    virtual Input* GetInput() = 0;
    /** Checks wether the object state is consistent with all the related
	objects. */
    virtual void Check() = 0;
    /** Sets the runner parameters, passed through a parameter box.
	@param pb the object containing the parameter setting 
	for the runner */
    virtual void SetParameters(const ParameterBox& pb) = 0;
  protected:
    std::string name, /**< The name of the runner. */
      type; /**< The type of the runner. */
  };

  /** A Move Runner is an instance of the Runner interface which it compel
      with a particular definition of @Move (given as template instantiation).
      It is at the root of the inheritance hierarchy of actual runners.
      @ingroup Runners
  */
  template <class Input, class State, class Move>
  class MoveRunner : public Runner<Input,State>
  {
  public:
    /** Virtual destructor. */
    virtual ~MoveRunner() {};
    void Go();
    void Step(unsigned int n);
    void SetCurrentState(const State& s);
    State GetCurrentState();
    fvalue CurrentStateCost();
    State GetBestState();
    fvalue BestStateCost();
    void ComputeCost(); 
    bool LowerBoundReached();
    unsigned long NumberOfIterations() const;
    unsigned long MaxIteration() const;
    void SetMaxIteration(unsigned long max);
    void SetInput(Input* in);
    Input* GetInput();
    void Print(std::ostream& os = std::cout) const;
    void SetParameters(const ParameterBox& pb);
    /** Sets the output stream which is used to write the plot of the
	cost function during a run.
	@param os the output stream to use (cerr for default).
    */
    void SetPlotStream(std::ostream* os = &std::cerr) 
    { pos = os; }
    void Check();
  protected:
    MoveRunner(StateManager<Input,State>* s, 
	       NeighborhoodExplorer<Input,State,Move>* ne, Input* in = NULL, 
	       std::string name = "Runner name", std::string type = "Move Runner");
    virtual void InitializeRun();
    /** Actions to be performed at the end of the run. */
    virtual void TerminateRun() {};
    virtual void ComputeMoveCost(); 
    virtual void UpdateIterationCounter();
    bool MaxIterationExpired();
    /** Encodes the criterion used to stop the search. */
    virtual bool StopCriterion() = 0;
    /** Encodes the criterion used to select the move at each step. */
    virtual void SelectMove() = 0;
    virtual bool AcceptableMove();
    virtual void MakeMove();
    /** Stores the move and updates the related data. */
    virtual void StoreMove() {}
    virtual void UpdateStateCost();
		
    // input
    Input* p_in; /**< A pointer to the input object. */
		
    // helpers
    StateManager<Input,State>* p_sm; /**< A pointer to the attached 
					state manager. */
    NeighborhoodExplorer<Input,State,Move>* p_nhe; /**< A pointer to the 
						      attached neighborhood 
						      explorer. */
		
    // state data
    State current_state; /**< The current state object. */
    fvalue current_state_cost; /**< The cost of the current state. */
    bool current_state_set; /**< A flag that whether the current state is set.
			       It is so until a new input is given. */
    Move current_move;      /**< The currently selected move. */
    fvalue current_move_cost; /**< The cost of the selected move. */
		
    State best_state;         /**< The best state object. */
    fvalue best_state_cost;   /**< The cost of the best state. */
		
    unsigned long iteration_of_best; /**< The iteration when the best 
					state has found. */
    unsigned long max_idle_iteration; /**< The maximum number of iterations
					 allowed since the best state has
					 been found. */
		
    unsigned long number_of_iterations; /**< The overall number of iterations
					   performed. */
    unsigned long max_iteration;  /**< The overall maximum number 
				     of iterations allowed. */
		
    std::ostream* pos; /**< The output stream used for plotting. */
  };

   /** The Steepest Descent runner performs a simple local search.
      At each step of the search, the best move in the neighborhood of current
      solution is selected and performed.
      It is worth noticing that this algorithm leads straightly to the 
      nearest local minimum of a given state.
      @ingroup Runners
  */	
  template <class Input, class State, class Move>
  class SteepestDescent : public MoveRunner<Input,State,Move>
  {
  public:
    void Print(std::ostream& os = std::cout) const;
    void ReadParameters() {}
  protected: 
    SteepestDescent(StateManager<Input,State>* s, 
		    NeighborhoodExplorer<Input,State,Move>* ne, 
		    Input* in = NULL);
    void InitializeRun();
    void TerminateRun();
    bool StopCriterion();
    bool AcceptableMove();
    void SelectMove();
  };

  /** The Hill Climbing runner considers random move selection. A move
      is then performed only if it does improve or it leaves unchanged
      the value of the cost function.  
      @ingroup Runners 
  */
  template <class Input, class State, class Move>
  class HillClimbing : public MoveRunner<Input,State,Move>
  {public:
    void Print(std::ostream& os = std::cout) const;
    void ReadParameters();
  protected: 
    HillClimbing(StateManager<Input,State>* s, 
		 NeighborhoodExplorer<Input,State,Move>* ne, Input* in = NULL);
    void InitializeRun();
    void TerminateRun();
    bool StopCriterion();
    bool AcceptableMove();
    void StoreMove();
    void SelectMove();
  };
  
  /** The Tabu Search runner explores a subset of the current
      neighborhood. Among the elements in it, the one that gives the
      minimum value of the cost function becomes the new current
      state, independently of the fact whether its value is less or
      greater than the current one.
      
      Such a choice allows the algorithm to escape from local minima,
      but creates the risk of cycling among a set of states.  In order to
      prevent cycling, the so-called tabu list is used, which
      determines the forbidden moves. This list stores the most recently
      accepted moves, and the inverses of the moves in the list are
      forbidden.  
      @ingroup Runners
  */
  template <class Input, class State, class Move>
  class TabuSearch : public MoveRunner<Input,State, Move>
  {public:
    void ReadParameters();
    void Print(std::ostream& os = std::cout) const;
    void SetInput(Input* in);
    void SetParameters(const ParameterBox& pb);
  protected:
    TabuSearch(StateManager<Input,State>* s, 
	       NeighborhoodExplorer<Input,State,Move>* ne,
	       TabuListManager<Move>* tlm, Input* in = NULL);
    void SetTabuListManager(TabuListManager<Move>* tlm,
			    int min_tabu = 0, int max_tabu = 0);
    void InitializeRun();
    bool StopCriterion();
    void SelectMove();
    bool AcceptableMove();
    void StoreMove();
    TabuListManager<Move>* p_pm; /**< A pointer to a tabu list manger. */
  };

  /** The Simulated annealing runner relies on a probabilistic local
      search technique whose name comes from the fact that it
      simulates the cooling of a collection of hot vibrating atoms.

      At each iteration a candidate move is generated at random, and
      it is always accepted if it is an improving move.  Instead, if
      the move is a worsening one, the new solution is accepted with
      time decreasing probability.

      @ingroup Runners
   */
  template <class Input, class State, class Move>
  class SimulatedAnnealing : public MoveRunner<Input,State,Move>
  {
  public:
    void ReadParameters();
    void SetParameters(const ParameterBox& pb);
    void Print(std::ostream& os = std::cout) const;
  protected: 
    SimulatedAnnealing(StateManager<Input,State>* s, 
		       NeighborhoodExplorer<Input,State,Move>* ne, Input* in = NULL);
    void InitializeRun();
    void TerminateRun();
    bool StopCriterion();
    void UpdateIterationCounter();
    void SelectMove();
    bool AcceptableMove();
  protected:
    double temperature; /**< The current temperature. */
    double start_temperature; /**< The starting temperature. */
    double min_temperature;   /**< The minimum temperature allowed before 
				 stopping the search. */
    double cooling_rate;  /**< The cooling rate of the temperature 
			     (temperature = temperature * cooling_rate 
			     at regular intervals). */
    unsigned int neighbor_sample; /**< The Number of moves tested 
				     at each temperature. */
  };
	
  /** @defgroup Solvers Solver classes
      Solver classes control the search by generating the initial solutions, 
      and deciding how, and in which sequence,
      runners have to be activated (e.g. tandem, multistart, hybrid
      search). In addition, they communicate with the external
      environment, by getting the input and delivering the output. They
      are linked to one or more runners (for simple or composite
      search, respectively) and to some of the helpers.  
  */
	
  /** An Abstract Solver is an abstract interface for the Solver concept.
      It simply defines the signature for them, independently of the
      problem definition classes.
      @ingroup Solvers
  */
  class AbstractSolver
  {
  public:
    /** Virtual destructor. */
    virtual ~AbstractSolver() {}
    /** Performs a full solving procedure by finding an initial state, 
     running the attached runner and delivering the output. */    
    virtual void Solve() = 0;
    /** Start again a solving procedure, running the attached runner from
	the current internal state. */
    virtual void ReSolve() = 0;
    /** Tries multiple runs on different initial states and records the
	best one.
	@param n the number of trials  */
    virtual void MultiStartSolve(unsigned int n) = 0;
  };
  
  /** A Solver represents the external layer of EasyLocal++; it
      implements the Abstract Solver interface and furthermore is
      parametrized with the Input and Output of the problem.  @ingroup
      Solvers 
  */
  template <class Input, class Output>
  class Solver : public AbstractSolver
  {
  public:
    virtual void Solve() = 0;
    virtual void SetInput(Input* in);
    Input* GetInput();
    Output* GetOutput();
    void SetOutput(Output* out);
  protected:
    Solver(Input* in = NULL, Output* out = NULL);
    /** Updates the output by translating the best state found by the
	solver to an output object. */
    virtual void DeliverOutput() = 0;
    Input* p_in; /**< A pointer to the input object. */
    Output* p_out; /**< A pointer to the output object. */
  };
	
  /** A Local Search Solver has an internal state, and defines the ways for
      dealing with a local search algorithm.
      @ingroup Solvers
  */
  template <class Input, class Output, class State>
  class LocalSearchSolver : public Solver<Input, Output>
  {public:
    void SetInitTrials(int t);
    void Solve();
    /** Uses the final state of previous (Re)Solve execution as 
	the initial one
	This is useful for running on a set of continuously changing 
	instances. */
    virtual void ReSolve();
    virtual void MultiStartSolve(unsigned int n);
    void SetInput(Input* in);
    virtual void Check();
    fvalue InternalStateCost();
  protected:
    LocalSearchSolver(StateManager<Input,State>* sm, 
		      OutputManager<Input,Output,State>* om, Input* in = NULL, Output* out = NULL);
    /**
       Lets all the managed runners Go, and then it collects the best state
       found.
    */
    virtual void Run() = 0; 
    /** Returns the number of iterations performed in the search. */
    virtual unsigned long NumberOfIterations() const = 0;
    void DeliverOutput();
    virtual void FindInitialState();
    void ComputeCost();
    StateManager<Input,State>* p_sm; /**< A pointer to the attached 
					state manager. */
    OutputManager<Input,Output,State>* p_om; /**< A pointer to the attached
						output manager. */
    fvalue internal_state_cost;  /**< The cost of the internal state. */
    State internal_state;        /**< The internal state of the solver. */
    unsigned int number_of_init_trials; /**< Number of different initial 
					   states tested for a run. */
  };
	
  /** The Simple Local Search solver handles a simple local search algorithm
      encapsulated in a runner.
      @ingroup Solvers
  */
  template <class Input, class Output, class State>
  class SimpleLocalSearch : public LocalSearchSolver<Input,Output,State>
  {
  public:
    void SetRunner(Runner<Input,State> *r);
  protected:
    SimpleLocalSearch(StateManager<Input,State>* sm, 
		      OutputManager<Input,Output,State>* om,
		      Runner<Input,State>* r, Input* in = NULL, Output* out = NULL);
    SimpleLocalSearch(StateManager<Input,State>* sm, 
		      OutputManager<Input,Output,State>* om, Input* in = NULL, Output* out = NULL);
    void Run(); 
    unsigned long NumberOfIterations() const;
    Runner<Input,State>* p_runner; /**< A pointer to the managed runner. */				   
  };
	
	
  /** A Multi Runner solver handles a set of runners.
      @ingroup Solvers
  */
  template <class Input, class Output, class State>
  class MultiRunnerSolver : public LocalSearchSolver<Input,Output,State>
  {
  public:
    void ClearRunners();
    void AddRunner(Runner<Input,State> *r);
    void SetRunner(Runner<Input,State> *r, unsigned int i);		
  protected:
    MultiRunnerSolver(StateManager<Input,State>* sm, 
		      OutputManager<Input,Output,State>* om, Input* in = NULL, Output* out = NULL);
    unsigned long NumberOfIterations() const;
    unsigned long total_iterations; /**< The overall number of iterations
				       performed by all the managed 
				       runners. */
    unsigned int start_runner;      /**< The index of the runner to
				       start with. */
    std::vector<Runner<Input,State>* > runners; /**< The vector of
						   the linked runners. */
  };
	
  /** A Comparative Solver applies different runners to the same instances
      (and the same initial solutions).
      @ingroup Solvers
  */
  template <class Input, class Output, class State>
  class ComparativeSolver : public MultiRunnerSolver<Input,Output,State>
  {
  protected:
    /** Constructs a comparative solver by providing it links to
     a state manager, an output manager, an input, and an output object.

     @param sm a pointer to a compatible state manager
     @param om a pointer to a compatible output manager
     @param in a pointer to an input object
     @param out a pointer to an output object
    */
    ComparativeSolver(StateManager<Input,State>* sm, 
		      OutputManager<Input,Output,State>* om, 
		      Input* in = NULL, Output* out = NULL) 
      : MultiRunnerSolver<Input,Output,State>(sm,om,in,out) {}
    void Run();  
    State start_state; /**< The start state is equal for each runner used
			  and is kept in this variable. */
  };
	
  /** The Token-ring Solver alternates n runners for a number of
      rounds.   
      @ingroup Solvers
  */
  template <class Input, class Output, class State>
  class TokenRingSolver : public MultiRunnerSolver<Input,Output,State>
  {public:
    void SetRounds(unsigned int r);
    void SetStartRunner(unsigned int sr);
    void Check();
    void Print(std::ostream& os = std::cout) const;
  protected:
    TokenRingSolver(StateManager<Input,State>* sm, 
		    OutputManager<Input,Output,State>* om, Input* in = NULL, Output* out = NULL); 
    // Run all runners circularly on the same thread
    void Run();
    int max_idle_rounds; /**< Maximum number of runs without improvement 
			     allowed. */
    //    unsigned int start_runner;
  };
	
  /** @defgroup Testers Tester classes
      Tester classes represent a simple predefined interface of the user
      program. They help the user in debugging the code, adjusting the
      techniques, and tuning the parameters.  Testers also allow the
      user to instruct the system to perform massive batch
      experiments, and collect the results in aggregated form.  
  */

  /** The Abstract Move Tester is an interface for a tester that handles
      moves.
      @ingroup Testers
  */	
  template <class Input, class Output, class State>
  class AbstractMoveTester 
  {
  public:
    AbstractMoveTester(std::string s);
    /** Virtual destructor. */
    virtual ~AbstractMoveTester() {}
    std::string Name();
    /** The method shall execute the test menu on a given state 
	@param st the state
     */
    virtual void RunTestMenu(State& st) = 0;
    /** The method shall set the input of the linked objects according
	to the parameter.
	@param in a pointer to the input object
    */
    virtual void SetInput(Input* in) = 0;
  protected:
    std::string name; /**< The name of the move tester */
  };
	
  /** A Move Tester allows to test the behavior of a given
      neighborhood explorer.
      @ingroup Testers
  */
  template <class Input, class Output, class State, class Move>
  class MoveTester : public AbstractMoveTester<Input,Output,State>
  {
  public:
    MoveTester(StateManager<Input,State>* sm, OutputManager<Input,Output,State>* om, NeighborhoodExplorer<Input,State,Move>* ne, std::string nm, Input* in = NULL);
    void RunTestMenu(State& st);
    void SetInput(Input *in);
  protected:
    void ShowMenu();
    void ExecuteChoice(State& st);
    StateManager<Input,State>* p_sm; /**< A pointer to the attached 
					state manager. */
    NeighborhoodExplorer<Input,State,Move>* p_nhe; /**< A pointer to the
						      attached neighborhood
						      explorer. */
    OutputManager<Input,Output,State>* p_om; /**< A pointer to the attached
						output manager. */
    Input* p_in;  /**< A pointer to the input object. */
    Output out;   /**< The output object. */
    int choice;   /**< The option currently chosen from the menu. */
  };
  
  /** The State Tester allows to test a State Manager.
      @ingroup Testers
  */
  template <class Input, class Output, class State>
  class StateTester
  {
  public:
    StateTester(StateManager<Input,State>* s, 
		OutputManager<Input,Output,State>* o, Input* in = NULL);
    /** Virtual destructor. */
    virtual ~StateTester() {} 
    void RunTestMenu(State& s);
    void RunInputMenu(State& s);
    void SetInput(Input *in);
  protected:
    void ShowMenu();
    void ShowReducedMenu();
    void ExecuteChoice(State& st);
    StateManager<Input,State>* p_sm; /**< A pointer to the attached 
					state manager. */
    OutputManager<Input,Output,State>* p_om; /**< A pointer to the attached
						output manager. */
    Input* p_in; /**< A pointer to an input object. */
    Output out;  /**< The output objecct. */
    int choice;  /**< The option currently chosen from the menu. */
  };

  /** This class represent the interface between the tester 
      and the interpreter of the batch experiment language. 
      @ingroup Testers
  */
  class AbstractTester
  {
  public:
    /** Virtual destructor. */
    virtual ~AbstractTester() {}
    /** Loads an input instance identified by the string passed as parameter.
	@param id the instance identifier. */
    virtual void LoadInstance(std::string id) = 0;
    /** Adds the runner identified by the name and the type passed as 
	parameter to the attached solver.
	@param name the name of the runner to add
	@param type the type of the runner to add
    */
    virtual int AddRunnerToSolver(std::string name, std::string type) = 0;
    void SetSolverTrials(unsigned int t);
    void SetLogFile(std::string s);
    void SetOutputPrefix(std::string s);
    void SetPlotPrefix(std::string s);
    /** Starts the solving procedure and collects the results. */
    virtual void StartSolver() = 0;
    /** Sets the parameter of the runner specified by the pair (name, type).
	They will be passed as a parameter box that contains them.
	@param name the name of the runner to set
	@param type the type of the runner to set
	@param pb a parameter box containing the parameters to set */
    virtual void SetRunningParameters(std::string name, std::string type, const ParameterBox& pb) = 0; 
  protected:
    unsigned int trials; /**< Number of trials the solver will be run */
    std::ostream* logstream; /**< An output stream where to write running information. */
    std::string output_file_prefix; /**< The file prefix to be used for 
				         writing the outcome of each trial. */
    std::string plot_file_prefix;  /**< The prefix to be used for
				      writing the progress of each run in a plot file. */
  };
	
  /** A Tester collects a set of basic testers (move, state, ...) and
      allows to access them through sub-menus. It represent the external
      user interface provided by the framework.
      @ingroup Testers
  */
  template <class Input, class Output, class State>
  class Tester : public AbstractTester
  {
  public:
    Tester(StateManager<Input,State>* sm, OutputManager<Input,Output,State>* om, Input* in = NULL);
    /** Virtual destructor. */
    virtual ~Tester() {}
    void RunMainMenu();
    void SetInput(Input *in);
    void SetMoveTester(AbstractMoveTester<Input,Output,State>* p_amt, int i);
    void AddMoveTester(AbstractMoveTester<Input,Output,State>* p_amt);
    void CleanMoveTesters();
    void SetStateTester(StateTester<Input,Output,State>* p_st);
    void CleanRunners();
    void SetRunner(Runner<Input,State>* p_ru, unsigned int i);
    void AddRunner(Runner<Input,State>* p_ru);
    void SetSolver(TokenRingSolver<Input,Output,State>* p_so);
    void SetSolverParameters(unsigned int rounds, unsigned int start_runner = 0); 
    void LoadInstance(std::string id);
    int AddRunnerToSolver(std::string name, std::string type);
    void SetRunningParameters(std::string name, std::string type, const ParameterBox& pb);
    void StartSolver();
    void ProcessBatch(std::string filename);
    void CleanSolver();
    void Check();
    void Print(std::ostream& os = std::cout) const;
  protected:
    void ShowMainMenu();
    void ShowMovesMenu();
    void ShowRunMenu();
    void ShowDebuggingMenu();
    void ExecuteMainChoice();
    void ExecuteMovesChoice();
    void ExecuteRunChoice();
    void ExecuteDebuggingMenu();
    std::vector<AbstractMoveTester<Input,Output,State>* > move_testers;
    /**< The set of attached move testers. */
    std::vector<Runner<Input,State>* > runners; /**< The set of attached 
						   runners. */
    TokenRingSolver<Input,Output,State>* solver; /**< A token ring solver
						    to be used for batch
						    file processing. */
    StateTester<Input,Output,State>* state_tester;  /**< A state tester. */
    StateManager<Input,State>* p_sm;  /**< A pointer to a state manager. */
    OutputManager<Input,Output,State>* p_om; /**< A pointer to an output producer. */
    State test_state; /**< The current state managed by the tester. */
    Input* p_in; /**< A pointer to the input object. */
    Output out; /**< The output object. */
    int choice, /**< The option currently chosen from the menu. */
      sub_choice; /** The suboption currently chosen from the menu. */
  };
}; // end namespace easylocal

#endif // define __EASYLOCAL_H
