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
	test.cpp


Run Instructions:
	Compile using the "make" command
		Creates executable parse
	run parse via "./parse < in.txt"
		where in.txt is a text input file


Overview:
	Input via stdin:
		A simple program, even if not syntactically correct.

	Output via stdout:
		A syntax tree of the inputted program after recovery from syntax errors.  The syntax tree is built in the process of building the parse tree, never saving the intermediate step.

	Implemented Grammar:
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



6. Building the Syntax Tree
	The syntax tree is built as the parse tree is traversed even though the actual parse is never created.  In previous iterations of the code the parse tree was generated correctly, the various measures to improve error recovery made the parse tree virtually unrecognizable.  Each parse method returns a string, representing a subtree of the syntax tree.  These subtrees are concatenated together and the program parse method's output is printed to standard out if the parse completes.  If the parse has a fatal error, only the errors are printed to standard out -- the syntax tree is not.


Test Cases:


