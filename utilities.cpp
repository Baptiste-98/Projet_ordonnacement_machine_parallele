//==============================================================================
// SOME USEFUL FUNCTIONS - FUNCTION BODIES "UTILITIES.CPP".             C. PRINS
//==============================================================================  
#include <time.h>
#include "utilities.h"
using namespace std;

//------------------------------------------------------------------------------
// Chrono: chronometer function in seconds
//------------------------------------------------------------------------------
// Example: double Beg = Chrono(); ...; cout << "Duration" << Chrono() - Beg;
//------------------------------------------------------------------------------

double Chrono () {
	return double(clock()) / CLOCKS_PER_SEC;
}

//------------------------------------------------------------------------------
// DelIntMat/DelRealMat: delete a matrix created by NewIntMat/NewRealMat.
//------------------------------------------------------------------------------
// The memory manager does not remember the number of rows (m) and if rows and
// columns are indexed from 0 or 1 (i1 = true/false). You have to recall them.
//------------------------------------------------------------------------------

void DelIntMat (int **A, int m, bool i1) {
	if (i1) m++;
	for (int i = 0; i < m; i++) delete [] A[i];
   delete [] A;
}

void DelRealMat (double **A, int m, bool i1) {
	if (i1) m++;
	for (int i = 0; i < m; i++) delete [] A[i];
   delete [] A;
}

//------------------------------------------------------------------------------
// DelIntVect/DelRealVect: delete a vector created by NewIntVect/NewRealVect.
//------------------------------------------------------------------------------
// You don't need to recall the size, the memory manager remembers it.
//------------------------------------------------------------------------------

void DelIntVect (int* T) { 
	delete [] T; 
}

void DelRealVect (double* T) { 
	delete [] T; 
}

//------------------------------------------------------------------------------
// EI : edit integer i on w positions with spaces before (sign "-" included)
//------------------------------------------------------------------------------
// - Useful to write columns with given width: cout << EI(i,6) << endl;
// - Width w is an optional parameter, default value 1.
// - Subtlety: int w=1 must be specified only for the prototype (.h file).
//------------------------------------------------------------------------------

string EI (int i, int w) {
	return JR (to_string(i), w, ' ');
}

//------------------------------------------------------------------------------
// ER : edit real x on w positions, including d decimals, "-" included
//------------------------------------------------------------------------------
// - Useful to write columns with given width: cout << ER(x,6,4) << endl;
//------------------------------------------------------------------------------

string ER (double x, int w, int d) {  
   // Get sequence of digits to display, rounded, without sign and "."
   int y = round(fabs(x) * pow(10.0,d));
   string S = to_string(y);
   // Add leading zeros to fractional part if needed, using function JL
   if (int(x) == 0 && S.length() < d) S = JL("0",d-S.length(),'0') + S;
   // Just add "0." before sequence if integer part of x is null
	if (int(x) == 0) S = "0." + S;
	// Otherwise integer part contains the |S|-d first characters of S
	// and decimal part contains the d other characters 
   else S = S.substr(0,S.length()-d) + "." + S.substr(S.length()-d,d);
   // Add sign if x is negative
   if (x < 0) S = "-" + S;
   return JR(S, w, ' ');
}

//------------------------------------------------------------------------------
// Error: display error message Msg, wait for CR, and stop application
//------------------------------------------------------------------------------
// Example: ifstream F "toto.txt"; if (F.fail()) Error ("File not found!");
//------------------------------------------------------------------------------

void Error (string Msg) {
	std::cout << "Error! " << Msg;
	getchar();
	exit(1);
}

//-----------------------------------------------------------------------------
// JL : function to left-justify string S on w positions
//-----------------------------------------------------------------------------
// - Complete with character c whose default value is space. 
// - Truncate S if it has more than w characters.
// - E.g., if S = "37", then "cout << JR(S,5,' ') << endl;" displays "37   ".
// - Subtlety: char c=' ' must be specified only in the prototype (.h file)
//-----------------------------------------------------------------------------

string JL (string S, int w, char c) {
	if (S.length() >= w) return S.substr(0,w);
	else {
		string F(w - S.length(), c);  // String constructor
		return S + F;
	}
}

//------------------------------------------------------------------------------
// JR : function to right justify string S on w positions.               
//------------------------------------------------------------------------------
// - Complete with character c whose default value is space. 
// - Do nothing if S has more than w characters.
// - E.g., if S = "37", then "cout << JR(S,5,'0') << endl;" displays "00037".
// - Subtlety: char c=' ' must be specified only in the prototype (.h file)
//------------------------------------------------------------------------------

string JR (string S, int w, char c) {
	if (S.length() >= w) return S;
	else {
		string F(w - S.length(), c);  // String constructor
		return F + S;
	}
}

//------------------------------------------------------------------------------
// Max: functions returning the maximum of two integers or reals (one overload)
//------------------------------------------------------------------------------

int    Max (int i, int j)       { return i > j ? i : j; }
double Max (double i, double j) { return i > j ? i : j; }

//------------------------------------------------------------------------------
// Min: function returning the minimum of two integers or reals (one overload)
//------------------------------------------------------------------------------
// Example, search the minimum m of one integer n-vector int T[n] :
// int m = BigInt; for (int i = 0, i < n; i++) m = Min (m,T[i]);
//------------------------------------------------------------------------------

int    Min(int i, int j) { return i < j ? i : j; }
double Min(double i, double j) { return i < j ? i : j; }

//------------------------------------------------------------------------------
// NewIntVect/NewRealVect: dynamic allocation of a vector T of n integers/reals.
//------------------------------------------------------------------------------
// - n number of elements. Vector created with indexes 0...n-1 by default.
// - if i1=true (index from 1), created with 0...n to be used from index 1.
// - Example of creation, use and deletion : 
//   int* T = NewIntVect (T,10,true); 
//   for (int i = 1; i <= 10; i++) { T[i] = i; cout << T[i] << endl; };
//   DelIntVect (T);
// - T must be defined as int*, but then T can be used normally, e.g., T[i] = 1;
//------------------------------------------------------------------------------

int* NewIntVect (int n, bool i1) {
	if (i1) n++;
	return new int[n];
}

double* NewRealVect (int n, bool i1) {
	if (i1) n++;
	return new double[n];
}

//------------------------------------------------------------------------------
// NewIntMat/NewRealMat : dynamic allocation of a matrix m x n of integers/reals
//------------------------------------------------------------------------------
// - Rows indexed by 0...m-1 and columns in 0...n-1 by default (C++ usual mode).
// - if i1=true, rows indexed in 0..m & columns in 0..n (for use from index 1).
// - Example of creation, use and deletion of an m x n integer matrix: 
//   int** A = NewIntMat (4,3,true); 
//   for (int i = 1; i <= 10; i++) for (int j = 1; j < n; j++) 
//   { A[i][j] = i + j; cout << A[i][j] << endl; };
//   DelIntMat (A, m, i1);
// - A must be defined as int**, but then A is used as usual, e.g., A[i][j] = 1;
//------------------------------------------------------------------------------

int** NewIntMat (int m, int n, bool i1) {
	int** A;
	if (i1) { m++; n++; }
    A = new int* [m];
	for (int i = 0; i < m; i++) A[i] = new int[n];    
	return A;    
}

double** NewRealMat (int m, int n, bool i1) {
	double** A;
	if (i1) { m++; n++; }
   A = new double* [m];
	for (int i = 0; i < m; i++) A[i] = new double[n];    
	return A;    
}

//------------------------------------------------------------------------------
// Pause : display message Msg and wait for CR
//------------------------------------------------------------------------------
// Example : Pause ("Data have been loaded. CR...");
//------------------------------------------------------------------------------

void Pause(string Msg) {
	std::cout << Msg;
	getchar();
}

//------------------------------------------------------------------------------
// Init: function initializing a generator of class RandGen
//------------------------------------------------------------------------------
// The generator is already initialized bu each program run will yields the same
// random sequence. It is recommend to use a big odd 32-bit integer for seed :
// RandGen.R; R.RandInit (279381);
//------------------------------------------------------------------------------

void RandGen::Init (int seed) {
	time_t seconds;
	_X =time(&seconds);
	//srand((unsigned int) seconds);
	//_X = seed;
}

//------------------------------------------------------------------------------
// RandReal: function returning a random real in [a,b[ from a generator
//------------------------------------------------------------------------------

double RandGen::RandReal (double a, double b) {  
	_X = (_X * _A) % _M;
    _P = (double) _X / _M;
    return a + (b - a) * _P;
}

//------------------------------------------------------------------------------
// RandInt: function returning a random integer in [a,b] from a generator
//------------------------------------------------------------------------------

int RandGen::RandInt (int a, int b) {
	_X = (_X * _A) % _M;
    _P = (double) _X / _M;
  	return floor(a + (b - a + 1) * _P);
}

//------------------------------------------------------------------------------
// Trim: function removing leading/trailing spaces and tabs (char 9) in S
//------------------------------------------------------------------------------
// Example : string S = "  toto "; cout << Trim(S); --> display "toto".
//------------------------------------------------------------------------------

string Trim (string S) {
	return TrimR(TrimL(S));
}

//------------------------------------------------------------------------------
// TrimL: function removing leading spaces and tabs (char 9) in S
//------------------------------------------------------------------------------

string TrimL (string S) {
	while (S > "" && (S[0] == ' ' || S[0] == '\t')) S.erase (0, 1);
	return S;
}

//------------------------------------------------------------------------------
// TrimR: function removing trailing spaces and tabs (char 9) in S
//------------------------------------------------------------------------------

string TrimR (string S) {
	while (S > "" && (S.back() == ' ' || S.back() == '\t')) S.pop_back();
	return S;
}

//------------------------------------------------------------------------------
// UC: function converting string S in uppercase
//------------------------------------------------------------------------------
// - Example, ask a question with an answer yes/no in a string S.
// - The problem is that the user may type Yes, YES, yEs etc.
// - Solution : if (UC(S) != "YES" && UC(S) != "NO") Error ("Invalid answer");
//------------------------------------------------------------------------------

string UC(string S) {
	for (int i = 0; i < S.length(); i++) S[i] = toupper(S[i]);
	return S;
}
