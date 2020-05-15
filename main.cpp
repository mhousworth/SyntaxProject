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
Symbols stringtoTerminal(std::pair<std::string, std::string> input);
std::string terminaltoString(Symbols input);

// Part 2 Helper Function

bool symbol_typeMatch(std::string id1, std::string id2);
bool symbol_typeCheck(std::string id, std::string tp);
std::string symbol_getType(std::string id);
unsigned int symbol_getAddress(std::string id);
std::string keywordtoType(std::string keyword);

Symbols stringtoTerminal(std::pair<std::string, std::string> input);

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



// GLOBAL STACK
std::stack<Symbols>	stack1;



int main(){

    // Vector of Tokens and Lexemes
    std::vector<std::pair<std::string, std::string>> lexemeToken;

    Lexer Lex;
    
    // Added Stack of Enum Symbols
	std::stack<Symbols>	stack1;

    Lex.readFile(get_filename());
    // Lex.readFile("test.txt");
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

    // Boolean indicating Declaration
    bool DFLAG = false;
    // Boolean indicating Assignment, used only to get the identifier getting an assignment
    bool AFLAG = false;
    // Primary Identifier and type, either being declared or being assigned a value
    std::pair<std::string, std::string> pSymbol("","");

    while (stack1.size() > 0){

        if(index >= length){
            if (TS_EOS == stack1.top()){
                stack1.pop();
            }
            else{
                // End of file reached with an incomplete Statement
            }
        }
        else if(stringtoTerminal(lexemeToken[index]) == stack1.top()){

            // Printout symbol/terminal matches
            std::cout << "Matched Symbols: ";

            Symbols sym = stringtoTerminal(lexemeToken[index]);
            std::string out = lexemeToken[index].first;
            if (sym == TS_ID){
                std::cout << "Identifier: " << out;
            }
            else if (sym == TS_TYPE){
                std::cout << "Typedef: " << out;
            }
            else if (sym == TS_NUMBER){
                std::cout << "Number: " << out;
            }
            else{
                std::cout << out;
            }
            std::cout << std::endl;

            // Declaration Handling
            if (DFLAG){

                // Assign Symbol's Type
                if(sym == TS_TYPE){
                    pSymbol.second = lexemeToken[index].first;
                }
                // Assign Symbol's Identifier
                else if(sym == TS_ID){
                    pSymbol.first = lexemeToken[index].first;
                }
                // Try to add to Symbol Table
                else if(sym == TS_SEMICOLON){
                    // Check if Declaration has already been made for identifier
                    if(symbol_check(pSymbol.first)){
                        // Error: Multiple Declarations
                        std::cout << "Multiple Declarations\n";
                    }
                    else{
                        symbol_insert(pSymbol.first, keywordtoType(pSymbol.second));
                    }

                    DFLAG = false;
                    pSymbol.first = "";
                    pSymbol.second = "";
                }
            }
            // Assignment Handling
            else if(AFLAG){
                if(sym == TS_ID){
                    // Check if identifier is in the Symbol Table
                    if(symbol_check(lexemeToken[index].first)){
                        pSymbol.first = lexemeToken[index].first;
                        pSymbol.second = symbol_getType(lexemeToken[index].first);
                    }
                    else{
                        // Error: Identifier was not declared
                        std::cout << "Identifier: " << lexemeToken[index].first << " was not declared\n";
                    }
                }

                AFLAG = false;
            }
            // Assignment Handling + Type Checking
            else{
                // Assigning via another identifier
                if(sym == TS_ID){
                    // Check if identifier is in the Symbol Table
                    if(symbol_check(lexemeToken[index].first)){
                        // Check if NOT matching types
                        if(!symbol_typeMatch(pSymbol.first, lexemeToken[index].first)){
                            // Error Different Types
                            std::cout << "Different Types\n";
                        }
                    }
                    else{
                        // Error: Identifier was not declared
                        std::cout << "Identifier: " << lexemeToken[index].first << " was not declared\n";
                    }
                }
                // Assigning via numbers
                else if(sym == TS_NUMBER){
                    // Check if identifier is in the Symbol Table
                    if(symbol_check(lexemeToken[index].first)){
                        // Check Type Match
                        if(!symbol_typeCheck(pSymbol.first, lexemeToken[index].second)){
                            // Error Different Types
                            std::cout << "Different Types\n";
                        }
                    }
                }
                else if(pSymbol.second == "boolean"){
                    if(!(lexemeToken[index].first == "true") || !(lexemeToken[index].first == "false")){
                        // Error Different Types
                        std::cout << "Different Types\n";
                    }
                }
                else if(sym == TS_SEMICOLON){
                    pSymbol.first = "";
                    pSymbol.second = "";
                }
            }

            // Increment lexer and pop from stack
            index++;
            stack1.pop();
        }
        else if (TS_EOS == stack1.top()){
            // TODO: Check if there is more in the lexer to parse
            if (index>=length){
                stack1.pop();
            }
            else
            {
                stack1.push(NTS_S);
            }
        }
        else
        {
            std::cout << "Rule " << table[stack1.top()][stringtoTerminal(lexemeToken[index])] << std::endl;
            switch (table[stack1.top()][stringtoTerminal(lexemeToken[index])]){
                case 1:	// 1. S → A
                    // Set Assignment Flag
                    AFLAG = true;

                    stack1.pop();
                    stack1.push(NTS_A);
                    break;

                case 2:	// 2. S → D
                    // Set Declaration Flag
                    DFLAG = true;

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
                    stack1.push(TS_SEMICOLON);
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

    symbol_printAll();

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

std::string keywordtoType(std::string keyword){
    if(keyword == "int")
        return "integer";
    else if(keyword == "bool")
        return "boolean";
    else if(keyword == "float")
        return keyword;
    else
        return "";
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

Symbols stringtoTerminal(std::pair<std::string, std::string> input){

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
   