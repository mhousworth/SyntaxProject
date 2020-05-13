#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include "stdlib.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "LexerProject/Lexer.h"
#include <stack>

//set up symbols
enum Symbols {
    // the symbols:
    // needs work
    // Terminal symbols:
    TS_ID,
    TS_TYPE,
    TS_NUMBER,
    TS_PLUS,        // +
    TS_MINUS,       // -
    TS_MULTIPLY,    // *
    TS_DIVIDE,      // /
    TS_L_PARENS,	// (
    TS_R_PARENS,	// )
    TS_SEMICOLON,	// ;
    TS_EOS,		    // $
    TS_EQUAL,       // =
    TS_INVALID,	    // invalid token

    // Non-terminal symbols:
    NTS_S,		// S
    NTS_D,      // D
    NTS_A,      // A
    NTS_E,      // E
    NTS_Eprime, // E`
    NTS_T,      // T
    NTS_Tprime, // T`
    NTS_F,		// F
};
  
// Flag for terminal print outs
bool verbose = false;
std::vector<std::string> printOut;

// Symbol Table Data, Key=Identifier, Data=Pair<MemoryAddress, Type>
std::unordered_map<std::string, std::pair<unsigned int, std::string>> sym_table;
unsigned int memory_address = 5000;


std::string get_filename();
void consolePrint();
std::string formatTL(std::vector<std::pair<std::string, std::string>> *list, int *index);

// Part 2 Requirements

bool symbol_check(std::string id);
void symbol_insert(std::string id, std::string type);
void symbol_printAll();

// Part 2 Helper Function

bool symbol_typeMatch(std::string id1, std::string id2);
bool symbol_typeCheck(std::string id, std::string tp);
std::string symbol_getType(std::string id);
unsigned int symbol_getAddress(std::string id);

Symbols stringtoSymbol(std::pair<std::string, std::string> input);

bool S(std::vector<std::pair<std::string, std::string>> *list, int *index);
bool D(std::vector<std::pair<std::string, std::string>> *list, int *index);
bool A(std::vector<std::pair<std::string, std::string>> *list, int *index);
bool E(std::vector<std::pair<std::string, std::string>> *list, int *index);
bool E_P(std::vector<std::pair<std::string, std::string>> *list, int *index);
bool T(std::vector<std::pair<std::string, std::string>> *list, int *index);
bool T_P(std::vector<std::pair<std::string, std::string>> *list, int *index);
bool F(std::vector<std::pair<std::string, std::string>> *list, int *index);
bool NUM(std::vector<std::pair<std::string, std::string>> *list, int *index);
bool TYPE(std::vector<std::pair<std::string, std::string>> *list, int *index);



//
// GLOBAL STACK
std::stack<Symbols>	stack1;



int main(){

    // Vector of Tokens and Lexemes
    std::vector<std::pair<std::string, std::string>> lexemeToken;

    Lexer Lex;
    
    // Added Stack of Enum Symbols
	std::stack<Symbols>	stack1;

    Lex.readFile(get_filename());
    Lex.readFile("test.txt");
    Lex.outputList();

    lexemeToken = Lex.getList();

    int length = lexemeToken.size();

    // symbol_insert("a", "integer");
    // symbol_insert("b", "boolean");
    // symbol_insert("c", "integer");

    // bool result = false;
    // result = symbol_typeMatch("a", "c");
    // result = symbol_typeMatch("a", "b");
    // result = symbol_typeCheck("a", "integer");
    // result = symbol_typeCheck("a", "boolean");

    // std::cout << "a type: " + symbol_getType("a") + "\n";;
    // std::cout << "a addr: " + std::to_string(symbol_getAddress("a")) + "\n";
    // std::cout << "b type: " + symbol_getType("b") + "\n";
    // std::cout << "b addr: " + std::to_string(symbol_getAddress("b")) + "\n";;

    // symbol_printAll();

    //INITIALIZE STACK
    //
    //
    stack1.push(TS_EOS);
    stack1.push(NTS_S);
    
    
    //Convert lexemes to ENUM
    
    // std::vector<Symbols> enumRules; //created for holding enums
    // for(int i =0; i<length;i++ ){
    //     //create vector of enums representing the lexemtokens
    //     enumRules.push_back(stringtoSymbol(lexemeToken[i]));
    // }

    //SET UP PARSING TABLE
    //map< Symbols, map<Symbols, > > table;
    std::map< Symbols, std::map<Symbols, int>> table;
    table[NTS_S][TS_ID] = 1;
	table[NTS_S][TS_TYPE] = 2;
	table[NTS_D][TS_TYPE] = 3;
    table[NTS_A][TS_ID] = 4;
    table[NTS_E][TS_ID] = 5;
    table[NTS_E][TS_NUMBER] = 5;
    table[NTS_E][TS_L_PARENS] = 5;
    table[NTS_Eprime][TS_PLUS] = 6;
    table[NTS_Eprime][TS_MINUS] = 7;
    table[NTS_Eprime][TS_R_PARENS] = 14;
    table[NTS_Eprime][TS_SEMICOLON] = 14;
    table[NTS_T][TS_ID] = 8;
    table[NTS_T][TS_NUMBER] = 8;
    table[NTS_T][TS_L_PARENS] = 8;
    table[NTS_Tprime][TS_MULTIPLY] = 9;
    table[NTS_Tprime][TS_DIVIDE] = 10;
    table[NTS_Tprime][TS_PLUS] = 14;
    table[NTS_Tprime][TS_MINUS] = 14;
    table[NTS_Tprime][TS_R_PARENS] = 14;
    table[NTS_Tprime][TS_SEMICOLON] = 14;
    table[NTS_F][TS_ID] = 11;
    table[NTS_F][TS_NUMBER] = 12;
    table[NTS_F][TS_L_PARENS] = 13;



    int index = 0;

    while (stack1.size() > 0){

            if(stringtoSymbol(lexemeToken[index]) == stack1.top()){
                std::cout << "Matched Symbols: " << stringtoSymbol(lexemeToken[index]) << std::endl;
                index++;
                stack1.pop();
            }
            else if (TS_EOS == stack1.top()){
                // TODO: Check if there is more in the lexer to parse
                // For now pop and end
                stack1.pop();
            }
            else
            {
                std::cout << "Rule " << table[stack1.top()][stringtoSymbol(lexemeToken[index])] << std::endl;
                switch (table[stack1.top()][stringtoSymbol(lexemeToken[index])])
                {
                case 1:	// 1. S → A
					stack1.pop();
					stack1.push(NTS_A);
					break;

				case 2:	// 2. S → D
					stack1.pop();
					stack1.push(NTS_D);		
					break;

				case 3:	// 3. D → type id;
					stack1.pop();
                    stack1.push(TS_SEMICOLON);
                    stack1.push(TS_ID);
					stack1.push(TS_TYPE);
					break;
                    
                case 4: // 4. A → id = E;
                    stack1.pop();
                    stack1.push(NTS_E);
                    stack1.push(TS_EQUAL);
					stack1.push(TS_ID);
					break;

                case 5: // 5. E → TE'
                    stack1.pop();
                    stack1.push(NTS_Eprime);
					stack1.push(NTS_T);
					break;

                case 6: // 6. E' → +TE'
                    stack1.pop();
                    stack1.push(NTS_Eprime);
                    stack1.push(NTS_T);
					stack1.push(TS_PLUS);
					break;

                case 7: // 7. E' → -TE'
                    stack1.pop();
                    stack1.push(NTS_Eprime);
                    stack1.push(NTS_T);
					stack1.push(TS_MINUS);
					break;

                case 8: // 8. T → FT'
                    stack1.pop();
                    stack1.push(NTS_Tprime);
					stack1.push(NTS_F);
					break;

                case 9: // 9.  T' → *FT'
                    stack1.pop();
                    stack1.push(NTS_Tprime);
					stack1.push(NTS_F);
                    stack1.push(TS_MULTIPLY);
					break;

                case 10: // 10.  T' → /FT'
                    stack1.pop();
                    stack1.push(NTS_Tprime);
					stack1.push(NTS_F);
                    stack1.push(TS_DIVIDE);
					break;

                case 11: // 11. F → TS_ID
                    stack1.pop();
                    stack1.push(TS_ID);
					break;

                case 12: // 12. F → TS_NUMBER
                    stack1.pop();
                    stack1.push(TS_NUMBER);
					break;

                case 13: // 13. F → (E)
                    stack1.pop();
                    stack1.push(TS_R_PARENS);
                    stack1.push(NTS_E);
                    stack1.push(TS_L_PARENS);
					break;

                case 14: // 14. EPSILON
                    stack1.pop();
					break;
                

				default:
					std::cout << "parsing table defaulted" << std::endl;
					return 0;
					break;
                }
            }
            
    }

    // std::ofstream fout;
    // fout.open("output.txt");

    // //Print out results, can be changed to a file output
    // char output[256];

    // for (int i=0; i < printOut.size(); i++){
    //     fout << printOut[i];
    // }

    // fout.close();

    return true;
}

std::string get_filename(){
    std::string file_name;
    std::cout << "Please enter a filename to analyze:" << std::endl;
    std::cin >> file_name;
    file_name.append(".txt");
    return file_name;
}

// Prints out the last entry to printOut vector
void consolePrint(){
    std::cout << printOut[printOut.size()-1];
}

// Formats the current Token/Lexeme Pair into a descriptive string
std::string formatTL(std::vector<std::pair<std::string, std::string>> *list, int *index){

    char output[256] = {0};
    sprintf(output, "Token: %-12s Lexeme: %s\n", (*list)[*index].second.c_str(), (*list)[*index].first.c_str());
    return std::string(output);
}

// Returns true if identifier is already in the symbol table
bool symbol_check(std::string id){

    // Try to access via "at" method, throws OOR if it doesn't exist
    try{
        sym_table.at(id);
    }
    catch (const std::out_of_range& oor){
        return false;
    }

    return true;
}

// Adds identifier to symbol table with the current open address, and increments open address
void symbol_insert(std::string id, std::string type){

    // Set via key "id", pair memory_address and type. Increment memory_address
    sym_table[id] = std::pair<unsigned int, std::string>(memory_address++, type);

}

// Prints all paired identifiers and memory addresses
void symbol_printAll(){

    // Array for sorted keys
    std::string keys[sym_table.size()];

    std::unordered_map<std::string, std::pair<unsigned int, std::string>>::iterator iter = sym_table.begin();

    char output[256];
    sprintf(output, "%29s \n", "Symbol Table");
    std::cout << output;
    sprintf(output, "%s \n", "-------------------------------------------------");
    std::cout << output;
    sprintf(output, "%-16s %-16s %-16s \n", "Identifier", "Memory Address", "Type");
    std::cout << output;
    sprintf(output, "%s \n", "-------------------------------------------------");
    std::cout << output;

    // Bucket sort keys by memory address value
    for (iter; iter != sym_table.end(); iter++){

        keys[iter->second.first - 5000] = iter->first;
    }

    // Print Elements
    for (int i = 0; i < sym_table.size(); i++){

        const char* s1 = keys[i].c_str();
        const char* s2 = std::to_string(sym_table[keys[i]].first).c_str();
        const char* s3 = sym_table[keys[i]].second.c_str();
        sprintf(output, "%-16s %-16s %-16s \n", s1, s2, s3);
        std::cout << output;
    }

}

// Returns true if both identifiers are the same type
bool symbol_typeMatch(std::string id1, std::string id2){
    
    return sym_table[id1].second == sym_table[id2].second;
}

// Returns true if the identifier matches the type
bool symbol_typeCheck(std::string id, std::string tp){

    return sym_table[id].second == tp;
}

// Returns identifier's defined type
std::string symbol_getType(std::string id){

    return sym_table[id].second;
}

// Returns identifier's allocated address
unsigned int symbol_getAddress(std::string id){

    return sym_table[id].first;
}

// Statement
// First(S)={type,id} Follow(S)={$}
bool S(std::vector<std::pair<std::string, std::string>> *list, int *index){

    // Check if index is within list bounds
    if(*index < (*list).size()){
        // Print current token/lexeme
        printOut.push_back(formatTL(list, index));
        if(verbose)
            consolePrint();
    }
    // Error
    else{
        printOut.push_back("Syntax Error: Start of Statement expected a \"type\" or \"identifier\"\n");
        if(verbose)
            consolePrint();

        return false;
    }

    // type
    if (TYPE(list, index)){

        // Print Rule
        printOut.push_back("\tS -> D\n");
        if(verbose)
            consolePrint();
        
        // Call/Expand Non-Terminal, return result
        return D(list, index);
    }
    // id
    else if((*list)[*index].second == "identifier"){
        
        printOut.push_back("\tS -> A\n");
        if(verbose)
            consolePrint();
        
        // Call/Expand Non-Terminal, return result
        return A(list, index);
    }
    // Error
    else{
        printOut.push_back("Syntax Error: Start of Statement expected a type or identifier\n");
        if(verbose)
            consolePrint();

        return false;
    }

}

// Declaration
// First(D)={type} Follow(D)={$}
bool D(std::vector<std::pair<std::string, std::string>> *list, int *index){

    // type
    if (TYPE(list, index)){
        (*index)++;

        // Print Rule
        printOut.push_back("\tD -> type id;\n");
        if(verbose)
            consolePrint();

        // Check if index is within list bounds
        if(*index < (*list).size()){
            // Print current token/lexeme
            printOut.push_back(formatTL(list, index));
            if(verbose)
                consolePrint();
        }
        // Error
        else{
            printOut.push_back("Syntax Error: EOF: Declaration expected \"id\" and \";\" following \"type\"\n");
            if(verbose)
                consolePrint();

            return false;
        }

        // id
        if ((*list)[*index].second == "identifier"){
            (*index)++;

            // Check if index is within list bounds
            if(*index < (*list).size()){
                // Print current token/lexeme
                printOut.push_back(formatTL(list, index));
                if(verbose)
                    consolePrint();
            }
            // Error
            else{
                printOut.push_back("Syntax Error: EOF: Declaration expected \";\" following Expression\n");
                if(verbose)
                    consolePrint();

                return false;
            }

            // ;
            if ((*list)[*index].first == ";"){
                (*index)++;

                // End of Declaration
                return true;
            }
            else{
            printOut.push_back("Syntax Error: Declaration expected \";\" following \"id\"\n");
            if(verbose)
                consolePrint();

            return false;
            }
        }
        else{
            printOut.push_back("Syntax Error: Declaration expected \"id\" following \"type\"\n");
            if(verbose)
                consolePrint();

            return false;
        }
    }
    else{
        printOut.push_back("Fatal Error: Unknown interuption at the start of Declaration\n");
        if(verbose)
            consolePrint();

        return false;
    }
}

// Assignment
// First(A)={id} Follow(A)={$}
bool A(std::vector<std::pair<std::string, std::string>> *list, int *index){

    // id
    if((*list)[*index].second == "identifier"){
        (*index)++;

        // Print Rule
        printOut.push_back("\tA -> id = E;\n");
        if(verbose)
            consolePrint();

        // Check if index is within list bounds
        if(*index < (*list).size()){
            // Print current token/lexeme
            printOut.push_back(formatTL(list, index));
            if(verbose)
                consolePrint();
        }
        // Error
        else{
            printOut.push_back("Syntax Error: EOF: Assignment expected \"=\" and an Expression following \"id\"\n");
            if(verbose)
                consolePrint();

            return false;
        }

        // =
        if ((*list)[*index].first == "="){
            (*index)++;

            // Check if index is within list bounds
            if(*index < (*list).size()){
                // Print current token/lexeme
                printOut.push_back(formatTL(list, index));
                if(verbose)
                    consolePrint();
            }
            // Error
            else{
                printOut.push_back("Syntax Error: EOF: Assignment expected an Expression following \"=\"\n");
                if(verbose)
                    consolePrint();

                return false;
            }
            
            // E
            if (E(list, index)){

                // Check if index is within list bounds
                if(*index >= (*list).size()){
                    printOut.push_back("Syntax Error: EOF: Assignment expected \";\" following Expression\n");
                    if(verbose)
                        consolePrint();

                    return false;
                }

                // ;
                if ((*list)[*index].first == ";"){
                    (*index)++;

                    // End of Assignment
                    return true;
                }
                else{
                    printOut.push_back("Syntax Error: Assignment expected \";\" following Expression\n");
                    if(verbose)
                        consolePrint();

                    return false;
                }
            }
            else{
                // E Rules will have printed errors
                return false;
            }
        }
        else{
            printOut.push_back("Syntax Error: Assignment expected \"=\" following \"id\"\n");
            if(verbose)
                consolePrint();

            return false;
        }
    }
    else{
        printOut.push_back("Fatal Error: Unknown interuption at the start of Assignment\n");
        if(verbose)
            consolePrint();

        return false;
    }

}

// Expression
// First(E)={(,id,num} Follow(E)={;,)}
bool E(std::vector<std::pair<std::string, std::string>> *list, int *index){

    // Check if index is within list bounds
    if(*index >= (*list).size()){
        printOut.push_back("Syntax Error: EOF: Start of Expression expected \"(\", \"id\", or \"num\"\n");
        if(verbose)
            consolePrint();

        return false;
    }

    // (, id, num
    if (((*list)[*index].first == "(") || ((*list)[*index].second == "identifier") || NUM(list, index)){

        // Print Rule
        printOut.push_back("\tE -> TE\'\n");
        if(verbose)
            consolePrint();
        
        // Call/Expand Non-Terminal
        if (T(list, index)){

            // Call/Expand Non-Terminal
            if (E_P(list, index)){

                //End of Expression
                return true;
            }
        }
        // Error Handled by Term
        else
            return false;
    }
    // Error
    else{
        printOut.push_back("Syntax Error: Start of Expression expected \"(\", \"id\", or \"num\"\n");
            if(verbose)
                consolePrint();

            return false;
    }
}

// Expression Prime
// First(E')={+,-,epsilon} Follow(E')={;,)}
bool E_P(std::vector<std::pair<std::string, std::string>> *list, int *index){

    // Check if index is within list bounds
    if(*index >= (*list).size()){
        printOut.push_back("Syntax Error: EOF: Expression Prime expected \"+\", \"-\", \";\", or \")\"\n");
        if(verbose)
            consolePrint();

        return false;
    }

    // First Set
    // +
    if ((*list)[*index].first == "+"){
        (*index)++;

        // Print Rule
        printOut.push_back("\tE' -> +TE'\n");
        if(verbose)
            consolePrint();

        // Check if index is within list bounds
        if(*index < (*list).size()){
            // Print current token/lexeme
            printOut.push_back(formatTL(list, index));
            if(verbose)
                consolePrint();
        }
        // Error
        else{
            printOut.push_back("Syntax Error: EOF: Expression Prime expected Term following \"+\"\n");
            if(verbose)
                consolePrint();

            return false;
        }

        // Call/Expand Non-Terminal
        if(T(list, index)){

            // Call/Expand Non-Terminal
            if(E_P(list, index)){

                // End of Expression Prime
                return true;
            }
            // Error handled by recursive Expression Prime, otherwise hard to read
            // else{
            //     printOut.push_back("Syntax Error: Expression Prime expected Expression Prime following Term\n");
            //     if(verbose)
            //         consolePrint();

            //     return false;
            // }
        }
        // Error
        else{
            printOut.push_back("Syntax Error: Expression Prime expected Term following \"+\"\n");
            if(verbose)
                consolePrint();

            return false;
        }
    }
    else if ((*list)[*index].first == "-"){
        (*index)++;

        // Print Rule
        printOut.push_back("\tE' -> -TE'\n");
        if(verbose)
            consolePrint();

        // Check if index is within list bounds
        if(*index < (*list).size()){
            // Print current token/lexeme
            printOut.push_back(formatTL(list, index));
            if(verbose)
                consolePrint();
        }
        // Error
        else{
            printOut.push_back("Syntax Error: EOF: Expression Prime expected Term following \"-\"\n");
            if(verbose)
                consolePrint();

            return false;
        }

        // Call/Expand Non-Terminal
        if(T(list, index)){

            // Call/Expand Non-Terminal
            if(E_P(list, index)){

                // End of Expression Prime
                return true;
            }
            // Error handled by recursive Expression Prime, otherwise hard to read
            // else{
            //     printOut.push_back("Syntax Error: Expression Prime expected Expression Prime following Term\n");
            //     if(verbose)
            //         consolePrint();

            //     return false;
            // }
        }
        // Error
        else{
            printOut.push_back("Syntax Error: Expression Prime expected Term following \"-\"\n");
            if(verbose)
                consolePrint();

            return false;
        }
    }
    // Follow Set
    // ;, )
    else if (((*list)[*index].first == ";") || ((*list)[*index].first == ")")){

        // Print Rule
        printOut.push_back("\tE' -> epsilon\n");
        if(verbose)
            consolePrint();

        // End of Expression Prime
        return true;
    }
    // Error
    else{
        printOut.push_back("Syntax Error: Expression Prime expected \"+\", \"-\", \";\", or \")\"\n");
        if(verbose)
            consolePrint();

        return false;
    }
}

// Term
// First(T)={(,id,num} Follow(T)={+,-,;,)}
bool T(std::vector<std::pair<std::string, std::string>> *list, int *index){

    // Check if index is within list bounds
    if(*index >= (*list).size()){
        printOut.push_back("Syntax Error: EOF: Start of Term expected \"(\", \"id\", or \"num\"\n");
        if(verbose)
            consolePrint();

        return false;
    }

    // (, id, num
    if(((*list)[*index].first == "(") || ((*list)[*index].second == "identifier") || NUM(list, index)){
        
        // Print Rule
        printOut.push_back("\tT -> FT\'\n");
        if(verbose)
            consolePrint();

        // Call/Expand Non-Terminal
        if (F(list, index))
            
            // Call/Expand Non-Terminal
            if (T_P(list, index)){
                
                // End of Term
                return true;
            }
            else{
                return false;
            }
        else
            return false;
    }
    // Error
    else{
        printOut.push_back("Fatal Error: Unknown interuption at the start of Term\n");
        if(verbose)
            consolePrint();

        return false;
    }

}

// Term Prime
// First(T')={*,/,epsilon} Follow(T')={+.-.;,)}
bool T_P(std::vector<std::pair<std::string, std::string>> *list, int *index){

    // Check if index is within list bounds
    if(*index >= (*list).size()){
        printOut.push_back("Syntax Error: EOF: Term Prime expected \"*\", \"/\", \"+\", \"-\", \";\", or \")\"\n");
        if(verbose)
            consolePrint();

        return false;
    }

    std::string lexeme = (*list)[*index].first;

    // First of
    // *
    if (lexeme == "*"){
        (*index)++;

        // Print Rule
        printOut.push_back("\tT' -> *FT'\n");
        if(verbose)
            consolePrint();

        // Check if index is within list bounds
        if(*index < (*list).size()){
            // Print current token/lexeme
            printOut.push_back(formatTL(list, index));
            if(verbose)
                consolePrint();
        }
        // Error
        else{
            printOut.push_back("Syntax Error: EOF: Term Prime expected Factor following \"*\"\n");
            if(verbose)
                consolePrint();

            return false;
        }

        // Call/Expand Non-Terminal
        if(F(list, index)){

            // Call/Expand Non-Terminal
            if(T_P(list, index)){

                // End of Term Prime
                return true;
            }
            // Error handled by recursive Term Prime, otherwise hard to read
            // else{
            //     printOut.push_back("Syntax Error: Term Prime expected Term Prime following Term\n");
            //     if(verbose)
            //         consolePrint();

            //     return false;
            // }
        }
        // Error
        else{
            printOut.push_back("Syntax Error: Term Prime expected Factor following \"*\"\n");
            if(verbose)
                consolePrint();

            return false;
        }
    }
    // /
    else if (lexeme == "/"){
        (*index)++;

        // Print Rule
        printOut.push_back("\tT' -> /FT'\n");
        if(verbose)
            consolePrint();

        // Check if index is within list bounds
        if(*index < (*list).size()){
            // Print current token/lexeme
            printOut.push_back(formatTL(list, index));
            if(verbose)
                consolePrint();
        }
        // Error
        else{
            printOut.push_back("Syntax Error: EOF: Term Prime expected Factor following \"/\"\n");
            if(verbose)
                consolePrint();

            return false;
        }

        // Call/Expand Non-Terminal
        if(F(list, index)){

            // Call/Expand Non-Terminal
            if(T_P(list, index)){

                // End of Term Prime
                return true;
            }
            // Error handled by recursive Term Prime, otherwise hard to read
            // else{
            //     printOut.push_back("Syntax Error: Term Prime expected Term Prime following Term\n");
            //     if(verbose)
            //         consolePrint();

            //     return false;
            // }
        }
        // Error
        else{
            printOut.push_back("Syntax Error: Term Prime expected Factor following \"/\"\n");
            if(verbose)
                consolePrint();

            return false;
        }
    }
    // Follow of
    // +, -, ;, )
    else if ((lexeme == "+") || (lexeme == "-") || (lexeme == ";") || (lexeme == ")")){

        // Print Rule
        printOut.push_back("\tT' -> epsilon\n");
        if(verbose)
            consolePrint();

        // End of Term Prime
        return true;
    }
    // Error
    else{
        printOut.push_back("Syntax Error: Term Prime expected \"*\", \"/\", \"+\", \"-\", \";\", or \")\"\n");
        if(verbose)
            consolePrint();

        return false;
    }
}

// Factor
// First(F)={(,id,num} Follow(F)={*,/,+.-,;,)}
bool F(std::vector<std::pair<std::string, std::string>> *list, int *index){

    // (, id, num
    if((*list)[*index].first == "("){
        (*index)++;

        // Print Rule
        printOut.push_back("\tF -> (E)\n");
        if(verbose)
            consolePrint();

        // Check if index is within list bounds
        if(*index < (*list).size()){
            // Print current token/lexeme
            printOut.push_back(formatTL(list, index));
            if(verbose)
                consolePrint();
        }
        // Error
        else{
            printOut.push_back("Syntax Error: EOF: Factor expected Expression following \"(\"\n");
            if(verbose)
                consolePrint();

            return false;
        }

        // Call/Expand Non-Terminal
        if (E(list, index)){

            // )
            if ((*list)[*index].first == ")"){
                (*index)++;

                // Check if index is within list bounds
                if(*index < (*list).size()){
                    // Print current token/lexeme
                    printOut.push_back(formatTL(list, index));
                    if(verbose)
                        consolePrint();
                }
                // Error Handled Elsewhere

                // End of Factor
                return true;
            }
            // Error
            else{
                printOut.push_back("Syntax Error: Factor expected \")\" following Expression\n");
                if(verbose)
                    consolePrint();

                return false;
            }
        }
        // Error
        else{
            printOut.push_back("Syntax Error: Factor expected Expression following \"(\"\n");
            if(verbose)
                consolePrint();

            return false;
        }
    }
    else if ((*list)[*index].second == "identifier"){
        (*index)++;

        // Print Rule
        printOut.push_back("\tF -> id\n");
        if(verbose)
            consolePrint();

        // Check if index is within list bounds
        if(*index < (*list).size()){
            // Print current token/lexeme
            printOut.push_back(formatTL(list, index));
            if(verbose)
                consolePrint();
        }
        // Error Handled elsewhere

        // End of Factor
        return true;
    }
    else if (NUM(list, index)){
        (*index)++;

        // Print Rule
        printOut.push_back("\tF -> num\n");
        if(verbose)
            consolePrint();
        
        // Print current token/lexeme
        printOut.push_back(formatTL(list, index));
        if(verbose)
            consolePrint();

        // End of Factor
        return true;
    }
    // Error
    else{
        printOut.push_back("Fatal Error: Unknown interuption at the start of Factor\n");
        if(verbose)
            consolePrint();

        return false;
    }
}

// Number type check rather than a rule
bool NUM(std::vector<std::pair<std::string, std::string>> *list, int *index){

    std::string token = (*list)[*index].second;

    if((token == "integer") || (token == "float"))
        return true;

    return false;
}

// Type check rather than a rule
bool TYPE(std::vector<std::pair<std::string, std::string>> *list, int *index){

    // Types are keywords
    if((*list)[*index].second == "keyword"){

        if((*list)[*index].first == "int"){
            return true;
        }
        else if((*list)[*index].first == "float"){
            return true;
        }
        else if((*list)[*index].first == "bool"){
            return true;
        }
    }
    
    return false;

}


/*set up symbols
    enum Symbols {
        // the symbols:
        //needs work
        // Terminal symbols:
        TS_ID,
        TS_TYPE,
        TS_NUMBER,
        TS_PLUS,
        TS_MINUS,
        TS_MULTIPLY,
        TS_DIVIDE,
        TS_L_PARENS,	// (
        TS_R_PARENS,	// )
        TS_SEMICOLON,		// a
        TS_EOS,		// $, in this case corresponds to '\0'
        TS_INVALID,	// invalid token

        // Non-terminal symbols:
        NTS_S,		// S
        NTS_D,
        NTS_A,
        NTS_E,
        NTS_Eprime,
        NTS_T,
        NTS_Tprime,
        NTS_F,		// F
    };
    */

Symbols stringtoSymbol(std::pair<std::string, std::string> input){

    if (input.second == "identifier"){
        return TS_ID;
    }
    else if ((input.first == "int") || (input.first == "float") || (input.first == "bool")){
        return TS_TYPE;
    }
    else if ((input.second == "integer") || (input.second == "float")){
        return TS_NUMBER;
    }
    else{
        if (input.first == "+")
            return TS_PLUS;
        else if (input.first == "-")
            return TS_MINUS;
        else if (input.first == "*")
            return TS_MULTIPLY;
        else if (input.first == "/")
            return TS_DIVIDE;
        else if (input.first == "(")
            return TS_L_PARENS;
        else if (input.first == ")")
            return TS_R_PARENS;
        else if (input.first == "=")
            return TS_EQUAL;
        else if (input.first == ";")
            return TS_SEMICOLON;
    }

}
   