#include <list>
#include <vector>
#include <map>
#include "stdlib.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "LexerProject/Lexer.h"

bool verbose = true;
std::vector<std::string> printOut;

std::string get_filename();
void consolePrint();
std::string formatTL(std::vector<std::pair<std::string, std::string>> *list, int *index);

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

int main(){

    // Vector of Tokens and Lexemes
    std::vector<std::pair<std::string, std::string>> lexemeToken;

    Lexer Lex;

    // Lex.readFile(get_filename());
    Lex.readFile("test.txt");
    // Lex.outputList();

    lexemeToken = Lex.getList();

    int index = 0;
    int length = lexemeToken.size();

    while((index < length) && S(&lexemeToken, &index));

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

// Statement
// First(S)={type,id} Follow(S)={$}
bool S(std::vector<std::pair<std::string, std::string>> *list, int *index){

    // Print current token/lexeme
    printOut.push_back(formatTL(list, index));
    if(verbose)
        consolePrint();

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
        printOut.push_back("Syntax Error: Invalid start to statement\n");
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

        // Print current token/lexeme
        printOut.push_back(formatTL(list, index));
        if(verbose)
            consolePrint();

        // id
        if ((*list)[*index].second == "identifier"){
            (*index)++;

            // Print current token/lexeme
            printOut.push_back(formatTL(list, index));
            if(verbose)
                consolePrint();

            // ;
            if ((*list)[*index].first == ";"){
                (*index)++;

                // End of Declaration
                return true;
            }
            else{
            printOut.push_back("Syntax Error: End of Declaration missing \";\" following \"id\"\n");
            if(verbose)
                consolePrint();

            return false;
            }
        }
        else{
            printOut.push_back("Syntax Error: Declaration missing \"id\" following \"type\"\n");
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

        // Print current token/lexeme
        printOut.push_back(formatTL(list, index));
        if(verbose)
            consolePrint();

        // =
        if ((*list)[*index].first == "="){
            (*index)++;

            // Print current token/lexeme
            printOut.push_back(formatTL(list, index));
            if(verbose)
                consolePrint();
            
            // E
            if (E(list, index)){

                // ;
                if ((*list)[*index].first == ";"){
                    (*index)++;

                    // End of Assignment
                    return true;
                }
            }
            else{
                // E Rules will have printed errors
                return false;
            }
        }
        else{
            printOut.push_back("Syntax Error: Assignment missing \"=\" following \"id\"\n");
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
        printOut.push_back("Syntax Error: Invalid start to Expression");
            if(verbose)
                consolePrint();

            return false;
    }
}

// Expression Prime
// First(E')={+,-,epsilon} Follow(E')={;,)}
bool E_P(std::vector<std::pair<std::string, std::string>> *list, int *index){

    // Follow Set
    // +
    if ((*list)[*index].first == "+"){
        (*index)++;

        // Print Rule
        printOut.push_back("\tE' -> +TE'\n");
        if(verbose)
            consolePrint();

        // Print current token/lexeme
        printOut.push_back(formatTL(list, index));
        if(verbose)
            consolePrint();

        // Call/Expand Non-Terminal
        if(T(list, index)){

            // Call/Expand Non-Terminal
            if(E_P(list, index)){

                // End of Expression Prime
                return true;
            }
            // Error handled by recursive Expression Prime, otherwise hard to read
            // else{
            //     printOut.push_back("Syntax Error: Expression Prime missing Expression Prime following Term\n");
            //     if(verbose)
            //         consolePrint();

            //     return false;
            // }
        }
        // Error
        else{
            printOut.push_back("Syntax Error: Expression Prime missing Term following \"+\"\n");
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

        // Print current token/lexeme
        printOut.push_back(formatTL(list, index));
        if(verbose)
            consolePrint();

        // Call/Expand Non-Terminal
        if(T(list, index)){

            // Call/Expand Non-Terminal
            if(E_P(list, index)){

                // End of Expression Prime
                return true;
            }
            // Error handled by recursive Expression Prime, otherwise hard to read
            // else{
            //     printOut.push_back("Syntax Error: Expression Prime missing Expression Prime following Term\n");
            //     if(verbose)
            //         consolePrint();

            //     return false;
            // }
        }
        // Error
        else{
            printOut.push_back("Syntax Error: Expression Prime missing Term following \"-\"\n");
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
        printOut.push_back("Syntax Error: Invalid lexeme while parsing Expression Prime");
        if(verbose)
            consolePrint();

        return false;
    }
}

// Term
// First(T)={(,id,num} Follow(T)={+,-,;,)}
bool T(std::vector<std::pair<std::string, std::string>> *list, int *index){

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

    std::string lexeme = (*list)[*index].first;

    // First of
    // *
    if (lexeme == "*"){
        (*index)++;

        // Print Rule
        printOut.push_back("\tT' -> *FT'\n");
        if(verbose)
            consolePrint();

        // Print current token/lexeme
        printOut.push_back(formatTL(list, index));
        if(verbose)
            consolePrint();

        // Call/Expand Non-Terminal
        if(F(list, index)){

            // Call/Expand Non-Terminal
            if(T_P(list, index)){

                // End of Term Prime
                return true;
            }
            // Error handled by recursive Term Prime, otherwise hard to read
            // else{
            //     printOut.push_back("Syntax Error: Term Prime missing Term Prime following Term\n");
            //     if(verbose)
            //         consolePrint();

            //     return false;
            // }
        }
        // Error
        else{
            printOut.push_back("Syntax Error: Term Prime missing Factor following \"*\"\n");
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

        // Print current token/lexeme
        printOut.push_back(formatTL(list, index));
        if(verbose)
            consolePrint();

        // Call/Expand Non-Terminal
        if(F(list, index)){

            // Call/Expand Non-Terminal
            if(T_P(list, index)){

                // End of Term Prime
                return true;
            }
            // Error handled by recursive Term Prime, otherwise hard to read
            // else{
            //     printOut.push_back("Syntax Error: Term Prime missing Term Prime following Term\n");
            //     if(verbose)
            //         consolePrint();

            //     return false;
            // }
        }
        // Error
        else{
            printOut.push_back("Syntax Error: Term Prime missing Factor following \"/\"\n");
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
        printOut.push_back("Syntax Error: Invalid lexeme while parsing Term Prime");
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

        // Print current token/lexeme
        printOut.push_back(formatTL(list, index));
        if(verbose)
            consolePrint();

        // Call/Expand Non-Terminal
        if (E(list, index)){
            
            // )
            if ((*list)[*index].first == ")"){
                (*index)++;

                // Print current token/lexeme
                printOut.push_back(formatTL(list, index));
                if(verbose)
                    consolePrint();

                // End of Factor
                return true;
            }
            // Error
            else{
                printOut.push_back("Syntax Error: Factor missing \")\" following Expression\n");
                if(verbose)
                    consolePrint();

                return false;
            }
        }
        // Error
        else{
            printOut.push_back("Syntax Error: Factor missing Expression following \"(\"\n");
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

        // Print current token/lexeme
        printOut.push_back(formatTL(list, index));
        if(verbose)
            consolePrint();

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