# Data structures
I decided to keep the same data structures as the design doc suggested and added member functions to them. I created a new one to store the tables map called `Database`, which just has a `unordered_map` of `Table` and serves as a layer to get data about tables.

# Tokenizer
I decided to make more types of tokens to make parsing easier, notably `OPERATOR`, `END_OF_QUERY` and `DATA_TYPE`.
The tokenizer walks through the query string and outputs a `vector` of `Token`.

# Parser
I decided to take the recommended approach and walk through the `Token` vector. I view the first keyword to check which type of query it is and dispatch accordingly. 
I created a few helper functions which were very useful:
- `expect(type, value)` - checks if current token is of type and value and throws error if not, value can be null and then it just checks type.
- `match(type, value)` - same as expect but returns bool instead of throwing error
- `peek()` - returns current token

At the end the parser returns a unique pointer of a statement which is the base class for all kind of statements (`CreateStatement`, `InsertStatement`, ...)

# Engine
I decided to split computation and printing results, this decision came after building the unit test for the engine and realizing the faults with having them conbinedparser expect taking two keywords instead of one