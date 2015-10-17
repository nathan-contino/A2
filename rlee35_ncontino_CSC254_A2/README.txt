Ryan Lee (rlee35@u.rochester.edu)
Nathan Contino (ncontino@u.rochester.edu)
CSC 254 -- Fall 2015
A2: Syntax Error Recovery


Files Submitted:
	Makefile
	README.txt
	parse.cpp
	scan.cpp
	scan.h

	test_results directory
		results of running the make testX where x is a integer 0-7
	tests directory
		holds 80 test programs to test teh limits of the parser

Run Instructions:
	Compile using the "make" command
		Creates executable parse

	Run parse on specific test program files via "./parse < test.txt"
		where test.txt is a text input file and the program within test.txt will be parsed to a syntax tree

	Run parse on batches of 10 prewritten test program (found in directory 'tests')
		make test0 -- 00-09
		make test1 -- 10-19
		make test2 -- 20-29
		make test3 -- 30-39
		make test4 -- 40-49
		make test5 -- 50-59
		make test6 -- 60-69
		make test7 -- 70-79


Overview:
	Input via stdin:
		A simple program, even if not syntactically correct.

	Output via stdout:
		A syntax tree of the inputted program after recovery from syntax errors.  The syntax tree is built in the process of building the parse tree, never saving the intermediate step.

	Implemented Grammar (given in assignment):
		P	→ 	SL $$
		SL	→ 	S SL  |  ε
		S	→ 	id := E  |  read id  |  write E  |  if C SL end  |  while C SL end
		C	→ 	E ro E
		E	→ 	T TT
		T	→ 	F FT
		F	→ 	( E )  |  id  |  lit
		TT	→ 	ao T TT  |  ε
		FT	→ 	mo F FT  |  ε
		ro	→ 	==  |  !=  |  <  |  >  |  <=  |  >=
		ao	→ 	+  |  -
		mo	→ 	*  |  /

	Syntax error recovery includes the use of:
		Detailed error messages
		Invalid token detection and handling
		Token insertion (In response to failed match)
		Token deletion (PANIC Mode)
		Context sensitive follow sets

	Heavily modified the original C source code to C++ with large additions for error messages and error recovery


Specifics:
1. Invalid token detection and handling
	Done via a modification of the scanner, a scanner wrapper method, a new token t_error, and the token_error method.  We'll redefine the old scan method as scan_raw(), which will attempt to read in tokens.  If scan_raw sees an invalid token, it will return the new t_error which will be caught by the newly defined scan() method.  The new scan() will recursively call itself to rerun scan_raw() until a valid token is scanned, with a error message is printed by token_error() in each iteration.  The error message created and printed to stdout by the token_error() method specifies what was scanned that triggered the error and the action taken to deal with the error.

	The actual cases of invalid tokens is far more complex than it initially appears because of the behaviour of reading from standard in at end-of-file and the desire to be as frugal as possible.  Frugality is necessary because "::" could be read in and return the appropriate t_error token, but the second ':' character could be part of a ":=" token.  The ability to reuse the second portion of that token requires additional error handling and cases within the scanner.  The ability to use the first character and ignore an intermediary character -- i.e ":l=" => ":=" -- does not exist.  In C++, reading from standard in is done via "cin >> noskipws >> c" reading one character at a time even if that character is whitespace.  At the end-of-file, "cin >> noskipws >> c" will repeatedly read the last character before the end-of-file and create false positive and false negative tokens. For example, if the end of the input is "=$$" where '$$' is the eof, the '=' will be read and then reread if not handled correctly.  The subsequent reads will create a valid "==" t_equal token even though "==" was not actually part of input, a false positive.  The opposite of false negatives is true for ">$$" and "<$$", where the '<' and '>' are legal by themselves but the scanner would interpret them as invalid ">>" and "<<" tokens.  Catching these edge cases around the end-of-file character requires several extra sets of if statements, but is not overly complex to implement.


2. Token Insertion
	The simplest method of error recovery, when the parser attempts and fails to match a specific token the token is simply inserted.  This is  straightforward because there are so few cases where this can actually happen.  In order for a match to fail, we must be looking for a terminal character that is not the first character in the non-terminal character we are constructing.  This narrows the possibilities to the ":=" in "S -> id := E", the "id" in "S -> read id", the ")" in "F -> (E)", and the "end" in "S -> if C SL end | while C SL end".  While we could expand this to the relative operator "ro" in "C -> E ro E", this is unnecessary when the panic mode deletion is effective and an insertion would introduce more ambiguity to our error recovery.  Although all of these are terminal character, id can be any id.  To handle this special case of terminal characters, we insert the id "MISSING_ID" in cases where we insert an ID.

	Detailed and specific error messages for token insertion are handled by the match_error method.  The error message states what the match expected, what the match found, and what the match inserted.  This error and it's handling do not read a new token in, keeping the token that failed to match.  This allows recovery and correction for simples failures like not properly closing a parenthesis with a right-paren and forgetting the ":=" get operator after the read symbol.


3. Token Deletion
	The other simple method of error recovery, when the parse cannot find a satisfiable use for the next token -- either in the first of the current non-terminal character or, if the current non-terminal can go to epsilon, in the follow of the current non-terminal character -- it deletes tokens until it finds a satisfiable token.  This is commonly known as panic mode and for each iteration of an unsatisfied token, the parse_error() method is ran and a relevant error message is printed to standard out.  The error message indicates which parse method had the error and gives a brief explanation of why it failed.  The failure explanation detects if the non-terminal character can go to epsilon and changes the error message to reflect it.  Also within the parse_error() method the next token is scanned in, effectively deleting the previous token.  After the parse_error method completes, the parse method that the error occured in is reran.

	This version of error recovery introduces a few issues, including cascading failures -- which the context sensitive follow set handles -- and end-of-file failures -- handled by the eof_error() method and inline detection.


4. Context Sensitive Follow sets
	An improvement over token deletion "panic mode", non-terminals will only be satisfied by values in their context-sensitive follow set.  This avoids cases where a non-terminal character goes to epsilon because the next token is in the follow set, even though that token is not legal given the current context.  For example, "SL" can be followed by "end" or "eof" but "end" is only in the follow set when "SL" is used within the context of "if C SL end" or "while C SL end".  This behaviour can be described as toning down the greedy nature of epsilon transitions.

	The context sensitive set is passed from method to method and built up as we traverse deeper into the parse tree.  All parse methods take an array of tokens called cs_follow, the context sensitive follow set, as one of their arguments except for the rel_op, add_op, and mul_op parse methods.  These 3 parse methods are very close to literal characters and don't have a way to call something that can go to epsilon, rendering a follow set unnecessary.  To build the context sensitive follow set, we look at a few distinct cases which can be extrapolated to a generalized rule.

	A) Case of a non-terminal followed by a terminal character (for this case rel_op, add_op, and mul_op count as terminal character)
		C -> E1 ro E2
		"E1" is passed the first set of "ro"

	B) Case of a non-terminal character followed by another non-terminal character which can go to epsilon
		FT1 -> mo F FT2
		"F" is passed the first set of "FT" (first of "mo") concatenated with the context sensitive follow set of "FT2"

	C) Case of a non-terminal character at the end of the case
		"E2" is passed the context sensitive follow set that was passed to "C"
		"FT2" is passed the context sensitive follow set that was passed to "FT1"

	The one exception to these rules is the "end" terminal character in "S -> if C SL end" and "S -> while C SL end".  The "C" and "SL" is given the ability to accept end-of-file, which allows match to insert the "end" token if it was not included.


5. End-Of-File Errors
	In the process of writing the code, we found there were a specific set of errors that required special attention -- the End-Of-File Errors.  A program will recover from errors and continue to parse forward, but isn't quite able to close out the program completely.  This is especially true when the error occurs right at the end of the program -- missing "end" statements, parenthesis set closure to match left parens, and an extra token or two that make up a portion of a non-terminal character.  For the purpose of clarity, we'll look at what causes most of the issues and how they are handled separately.

	End-Of-File errors come in 2 classes, cascading and invalid ending set.  Cascading errors are the most drastic, often deleting the majority of the program while in the modified panic mode, and are usually the result of a missed or improperly placed relational operator (==/!=/>/>=/</<=).  For example, an "if" statement begins and tries to parse a "Comparison" non-terminal character.  The "Comparison" non-terminal see's a relational operator as the next token, but does not have an expression to match to the left part of the relational operator.  The non-terminal character deletes tokens until it sees an appropriate relational operator, which it rarely does before hitting end-of-file.  Invalid ending sets are a result of extra tokens or missing tokens right before the end-of-file.  Besides the missing tokens described earlier, it's very common to have an extra mathematical symbol at the end.  The multiplication or addition operator will trigger the start of a factor and term tail respectively, but will run out of input before completing the parse.

There are 3 different ways these are handled.

	A) Cascading
		By far the most complex to recover from, cascading errors will almost always reach the end of file unless there is a major logical rework of the grammar and parser.  When they inevitably do, switch statements near the parse_error() methods detect it and run the eof_error() method.  Based on what method was being ran when it reached end of file, a customized error message will be printed and state that there was a "fatal" error.  The entire process will be quit, via a call to exit(1) and the syntax tree will not be printed to standard out as a result.

	B) Invalid Ending -- Extra Tokens
		The easiest to detect, but hard to handle.  There are several locations an invalid token can be detected and the failure can happen in several locations.  For example, "FT1 -> mo F FT2", the invald token could happen while F parses, before F is parsed but after mo matches, right after F is parsed, and in F2.  Depending on the location, a hard exit similar to the cascading error handling is required or the mul_op can simply be ignored and the program can bubble up to the calling FT or T.

	C) Invalid Ending -- Missing Tokens
		Discussed in token insertion before, by allowing certin non-terminals to evaluate as epsilon when end-of-file is seen -- even though end-of-file is not in the context sensitive follow set -- more tokens can be inserted via failed match operations.



6. Building the Syntax Tree
	The syntax tree is built as the parse tree is traversed even though the actual parse is never created.  In previous iterations of the code the parse tree was generated correctly, the various measures to improve error recovery made the parse tree virtually unrecognizable.  Each parse method returns a string, representing a subtree of the syntax tree.  These subtrees are concatenated together and the program parse method's output is printed to standard out if the parse completes.  If the parse has a fatal error, only the errors are printed to standard out -- the syntax tree is not.


Test Cases:
	test##.txt 00 -- 19:
		Fairly simple programs meant to test and demonstrate basic functionality

	test##.txt 20 -- 79:
		Targeted programs and a obfuscated of the n-primes program given on the assignment page with lines randomly deleted and invalid tokens inserted
		
		Targeted programs -- meant to test the boundaries of the parser and how it deals with specific edge cases
		Modified n-primes program -- meant to test the power of the parser and how well it is able to deal with certain edge cases

	NOTE:
		The "make test0" and other batch testing commands should work correctly, but any other batch testing may fail.  There are several cases within end-of-file errors where code exits if the given error is seen.  This exit is a "hard" exit, exiting the task AND the entire process.  Although this functionality is good in some cases, it makes batch testing significantly harder.  More information on these cases can be found in Specifics Part 5.



