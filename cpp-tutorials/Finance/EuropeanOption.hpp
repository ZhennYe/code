// EuropeanOption.hpp
#include <sstream>

class EuropeanOption
{
private:

	void init();	//initialize default values
	void copy(const EuropeanOption& o2);
	// 'KERNEL' FUNCTIONS FOR OPTION CALCULATION
	double CallPrice() const;
	double PutPrice() const;
	double CallDelta() const;
	double PutDelta() const;

public:
	// public member data for convenience only
	double r;	// interest rate
	double sig;	// volatility
	double K;	//strike price
	double T;	// expiry date (years)
	double U;	// current underlying price
	double b;	// cost of carry

	string optType; // option name (call, put)

public:
	//constructors
	EuropeanOption();	// default call otion
	EuropeanOption(const EuropeanOption& option2); // copy constructor
	EuropeanOption(const string& optionType); // create option type

	// destructor
	virutal ~EuropeanOption();

	// assignment operator
	EuropeanOption& operator = (const EuropeanOption& option2);

	// functions that calculate option price and sensitivities
	double Price() const;
	double Delta() const;

	// modifier functions
	void toggle();	// change option type (C/P, P/C)
};

