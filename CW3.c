#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#define MAX 30

// type definition inclusions
typedef enum { false, true } bool;

typedef struct Operators{ // represents the operators stack
    char data;
    struct Operators *nextNode;
}Operators; // used in infix-postfix and infix-prefix conversions

typedef struct Operands{ // represesnt the operands stack
    char data[MAX];
    struct Operands *nextNode;
}Operands; // used in postfix-infix and prefix-infix conversions

typedef struct Evaluation{ // represesnts the operands stack during evaluation
    int value;
    struct Evaluation *nextNode;
}Evaluation; // used in evaluation function

// global variables initialization
Operators *operators_top = NULL; // stack top
Operands *operands_top = NULL; // stack top
Evaluation *evaluation_top = NULL; // stack top
FILE *defaultPtr, *inPtr, *outPtr; // file pointers
char inMode; // input mode (through file or keyboard)

// function prototypes
char * infixTOpostfix(char inArray[], const int last_element);
char * infixTOprefix(char inArray[], const int last_element);
char * postfixTOinfix(char inArray[], const int last_element);
char * prefixTOinfix(char inArray[], const int last_element);
bool menu(char inArray[]);
bool isOpening(char in);
bool isClosing(char in);
int evaluate(char in[]);
int priority(char in);
void push(char in[], int code);
void pop(int code);

int main(){
    // declare variables
    char input[MAX]={'\0'}, location[MAX];
    int runs=1;

    // output rules
    puts("** RULES:\n\t- For Infix inputs, the expression's parentheses must be\n\t\tbalanced.\n");
    puts("\t- For Postfix and Prefix inputs, the expressions must be\n\t\tparenthesis-free.\n");
    puts("\t- For all inputs, there should be spaces between operators\n\t\tand operands.\n");
    puts("** NOTE: Failure to follow these rules will cause the program to show \n\t\tan error message (and close afterwards in some cases).\n");

    while (1){
        if (runs!=1) // clear screen when it's not the first run
            system("cls"); // clear console screen
        fflush(stdin); // to clear input buffer at the beginning of the iteration
        printf("How would you like to input the expression:\n(A) External file.\n(B) Keyboard.\n? ");
        scanf(" %c", &inMode);
        fflush(stdin); // to clear input buffer before entering the switch statement
        inMode = toupper(inMode); // done to ease comparison
        printf("\n----------------------------------\n");
        switch (inMode){
            case 'A': // if user chose to input through file
                printf("Enter the file location [(-1) to input from c:\\G51PGA\\default.txt]\nYour input: ");
                scanf("%s", location);
                if (strcmp(location, "-1\0")==0) // if user input (-1)
                    strcpy(location, "c:\\G51PGA\\default.txt");
                if ((defaultPtr = fopen(location, "a+")) == NULL){ // open file at the location
                    puts("Could not find file.\nRestart program.\n");
                    return 0;
                }
                else {
                    fgets(input, MAX, defaultPtr); // get input from file
                    printf("\nDetected input expression from file: %s\n\n", input); // show read input on the screen
                }
                if (!menu(input)) // if menu() returns 0, end program. Otherwise, reiterate
                    return 0;
                break;

            case 'B': // if user chose to input through keyboard
                printf("Enter an expression: "); // prompt user for input
                fgets(input, MAX, stdin); // get input from keyboard (stdin)
                inPtr = fopen("c:\\G51PGA\\input.txt", "w"); // open input.txt file
                fprintf(inPtr, "%s\n", input); // save input to input.txt
                puts("[Input was saved to c:\\G51PGA\\input.txt]\n");
                fclose(inPtr); // close file using its pointer
                outPtr = fopen("c:\\G51PGA\\output.txt", "w"); // open output.txt file
                if (!menu(input)) // if menu() returns 0, end program. Otherwise, reiterate
                    return 0;
                break;
            default:
                puts("Invalid choice.\nRestart program.\n");
        } // end switch statement
        runs++;
    } // end while loop
    return 0; // indicates successful execution termination
} // end function main()

bool menu(char inArray[]){
    // declare variables
    char temp[MAX]={'\0'}, output[MAX]={'\0'}, exp[5]={'\0'}, str[MAX], *out=NULL, choice, conversion_instruction;
    int type=0, i, opening=0, closing=0, j, count;

    /*--------------------- DETERMINE EXPRESSION TYPE ---------------------*/
    // loop to copy the operators and operands without parentheses into the exp[] array
    for (i=0, j=0; i<=strlen(inArray); i++){
        if (inArray[i]!='\0' && inArray[i]!='(' && inArray[i]!=')')
            exp[j++]=inArray[i];
    } // end for loop
    // loop to get a string (str[]) that has values of 'd', 'o' or a space
    // 'd' stands for data. 'o' stands for operator. ' ' is a space
    // note: this is done to ease comparisons and not call isdigit() or isspace() repeatedly
    for (i=0, j=0, count=0; i<=strlen(exp); i++){
        if (isspace(exp[i]) && count==0) // skip spaces before first character
            continue;
        if (exp[i]=='+'||exp[i]=='*'||exp[i]=='/'||exp[i]=='^')
            str[j++] = 'o';
        else if (exp[i]=='-'){ // negative number detection
            if (isalnum(exp[i+1])){ // check the next character
                if (isdigit(exp[i+2])) // check the next character
                    i+=2; // because we already accessed the next two characters
                else
                    i++; // because we already accessed the next character
                str[j++] = 'd'; // this is a negative number
            }
            else // if it's not a negative number, it's an operator
                str[j++] = 'o';
        }
        else {
            if (isalnum(exp[i]))
                str[j++] = 'd';
            else if (isspace(exp[i]))
                str[j++] = ' ';
        } // end nested if-else
        count++; // increment character counter
    } // end for loop
    // loop to determine expression type using str[]
    if (str[0]=='d'){
        if (str[1]=='d'){
            if (str[2]==' ')
                type = (str[3]=='d')? 3 : 1; // type 3 for postfix and 1 for infix
            else
                type = 1; // expression is infix
        }
        else if (str[1]==' ')
            type = (str[2]=='d')? 3 : 1; // type 3 for postfix and 1 for infix
        else
            type = 1; // expression is infix
    }
    else if (str[0]=='o'){
        if (str[1]=='d')
            type == 1; // expression is infix
        else
            type = 2; // expression is prefix
    } // end nested if-else statement
    if (type == 0){ // if expression does not satisfy the preceding criteria
        printf("\nInvalid input.\nRestart program.\n");
        return 0; // informs main() to terminate the program
    }

    /*------------------ CHECK FOR IMBALANCED PARENTHESES ------------------*/
    for (i=0; inArray[i]!='\0'; i++){ // loop to count parentheses
        // if character is an opening parenthesis, increment opening counter
        opening += (isOpening(inArray[i]))? 1 : 0;
        // if character is a closing parenthesis, increment closing counter
        closing += (isClosing(inArray[i]))? 1 : 0;
    } // end for loop
    if (type==1 && opening!=closing){ // if expression is infix and parentheses are imbalanced
        printf("Imbalanced parentheses.\nRestart the program.\n");
        return 0; // informs main() to terminate the program
    }
    else if ((type==2 || type==3) && (opening!=0 || closing!=0)){ // if expression is postfix or prefix and contains parentheses
        printf("Invalid input.\nPostfix and Prefix expressions should be parenthesis-free.\nRestart the program.\n");
        return 0; // informs main() to terminate the program
    } // end if-else statement

    /*----------------------- WRONG CHARACTER DETECTION -----------------------*/
    for (i=0; i<=strlen(inArray); i++){ // loop through input characters
        if (inArray[i]!='\0' && !isalnum(inArray[i]) && !isspace(inArray[i])){
            if (!isOpening(inArray[i]) && !isClosing(inArray[i])){
                if (inArray[i]!='*'&&inArray[i]!='/'&&inArray[i]!='+'&&inArray[i]!='-'&&inArray[i]!='^'){
                    printf("Invalid input characters.\nRestart the program.\n"); // if char is not an operand nor an operator
                    return 0; // informs main() to terminate the program
                } // end if statement
            } // end if statement
        } // end nested if statement
    } // end for loop

    /*--------------- DISPLAY THE MENU ON THE COMMAND PROMPT ---------------*/
    printf("What do you want to do:\n");
    printf("(A) Convert the expression.\n(B) Evaluate the expression.\n(C) Exit.\n? ");
    scanf(" %c", &choice);
    choice = toupper(choice); // done for easier comparisons
    switch (choice){ // outer switch statement
        case 'A' : // if user asked for conversions
            switch (type){ // inner switch statement
                case 1 : // if expression is infix
                    printf("\nThis is an infix expression.\n\nWhat do you want the expression to be converted to:\n");
                    printf("(A) Postfix.\t(B) Prefix.\t(C) Both.\n? ");
                    scanf(" %c", &conversion_instruction); // get user's choice of conversion
                    conversion_instruction = toupper(conversion_instruction); // for easier comparisons
                    if (conversion_instruction == 'A'){ // infix expression to postfix
                        strcpy(output, infixTOpostfix(inArray, strlen(inArray))); // convert and put result in output[]
                        printf("\nPostfix expression: %s\n", output); // output postfix expression
                    }
                    else if (conversion_instruction == 'B'){ // infix expression to prefix
                        strcpy(output, infixTOprefix(inArray, strlen(inArray))); // convert an put result in output[]
                        printf("\nPrefix expression: %s\n", output); // output prefix expression
                    }
                    else if (conversion_instruction == 'C'){ // infix expression to postfix and prefix
                        strcpy(output, infixTOpostfix(inArray, strlen(inArray))); // convert infix to postfix and put result in output[]
                        printf("\nPostfix expression: %s\n", output); // output postfix expression
                        out = infixTOprefix(inArray, strlen(inArray)); // convert infix to prefix and put result in *out
                        printf("Prefix expression: %s\n", out); // output prefix expression
                        sprintf(output + strlen(output), "\n%s", out); // append output with *out contents
                    } // end if-else statement
                    if (inMode=='A'){ // if input was done through a file
                        fprintf(defaultPtr, "\n%s\n", output); // append output to same file (default.txt)
                        puts("[Output was saved to c:\\G51PGA\\default.txt]\n");
                        fclose(defaultPtr); // close file using its pointer
                    }
                    else { // if input was done through the keyboard
                        fprintf(outPtr, "%s\n", output); // save output[] into output.txt
                        puts("[Output was saved to c:\\G51PGA\\output.txt]\n");
                        fclose(outPtr); // close file using its pointer
                    } // end if-else statement
                    break; // break out of the inner switch statement
                case 2 : // if expression is prefix
                    printf("\nThis is a prefix expression.\n\nWhat do you want the expression to be converted to:\n");
                    printf("(A) Infix.\t(B) Postfix.\t(C) Both.\n? ");
                    scanf(" %c", &conversion_instruction); // get user's choice of conversion
                    conversion_instruction = toupper(conversion_instruction); // for easier comparisons
                    if (conversion_instruction == 'A'){ // prefix expression to infix
                        strcpy(output, prefixTOinfix(inArray, strlen(inArray))); // convert and put result in output[]
                        printf("\nInfix expression: %s\n", output); // output infix expression
                    }
                    else if (conversion_instruction == 'B'){ // prefix to postfix
                        strcpy(temp, prefixTOinfix(inArray, strlen(inArray))); // copy to convert infix expression to postfix
                        strcpy(output, infixTOpostfix(temp, strlen(temp)));
                        printf("\nPostfix expression: %s\n", output); // output postfix expression
                    }
                    else if (conversion_instruction == 'C'){ // prefix to infix and postfix
                        strcpy(temp, prefixTOinfix(inArray, strlen(inArray))); // convert prefix to infix and put result in temp[]
                        printf("\nInfix expression: %s\n", temp); // output infix expression
                        out = infixTOpostfix(temp, strlen(temp)); // convert infix to postfix and put result in *out
                        printf("Postfix expression: %s\n", out); // output postfix expression
                        sprintf(output, "%s\n%s", temp, out); // put both results in output[]
                    } // end if-else statement
                    if (inMode=='A'){ // if input was done through file
                        fprintf(defaultPtr, "\n%s\n", output); // append output[] to same file (default.txt)
                        puts("[Output was saved to c:\\G51PGA\\default.txt]\n");
                        fclose(defaultPtr); // close file using its pointer
                    }
                    else { // if input was done through keyboard
                        fprintf(outPtr, "%s\n", output); // save output[] into output.txt
                        puts("[Output was saved to c:\\G51PGA\\output.txt]\n");
                        fclose(outPtr); // close file using its pointer
                    }
                    break; // break out of the inner switch statement
                case 3 : // if expression is postfix
                    printf("\nThis is a postfix expression.\n\nWhat do you want the expression to be converted to:\n");
                    printf("(A) Infix.\t(B) Prefix.\t(C) Both.\n? ");
                    scanf(" %c", &conversion_instruction); // get user's choice of conversion
                    conversion_instruction = toupper(conversion_instruction); // for easier comparisons
                    if (conversion_instruction == 'A'){ // postfix to infix
                        strcpy(output, postfixTOinfix(inArray, strlen(inArray))); // convert and put result in output[]
                        printf("\nInfix expression: %s\n", output); // output infix expression
                    }
                    else if (conversion_instruction == 'B'){ // postfix to prefix
                        strcpy(temp, postfixTOinfix(inArray, strlen(inArray))); // convert postfix to infix and put result in temp[]
                        strcpy(output, infixTOprefix(temp, strlen(temp))); // cconvert infix to prefix and put result in output[]
                        printf("\nPrefix expression: %s\n", output); // output prefix expression
                    }
                    else if (conversion_instruction == 'C'){ // postfix to infix and prefix
                        strcpy(temp, postfixTOinfix(inArray, strlen(inArray))); // convert postfix to infix and put result in temp[]
                        printf("\nInfix expression: %s\n", temp); // output infix expression
                        out = infixTOprefix(temp, strlen(temp)); // convert infix to prefix and put result in *out
                        printf("Prefix expression: %s\n", out); // output prefix expression
                        sprintf(output, "%s\n%s", strrev(temp), out); // put both results in output[]
                    } // end if-else statement
                    if (inMode=='A'){ // if input was done through file
                        fprintf(defaultPtr, "\n%s\n", output); // append output to same file (default.txt)
                        puts("[Output was saved to c:\\G51PGA\\default.txt]\n");
                        fclose(defaultPtr); // close file using its pointer
                    }
                    else { // if input was done through the keyboard
                        fprintf(outPtr, "%s\n", output); // save output[] into output.txt
                        puts("[Output was saved to c:\\G51PGA\\output.txt]\n");
                        fclose(outPtr); // close file using its pointer
                    } // end if-else statement
                    break; // break out of the inner switch statement
            } // end inner switch statement
            break; // break out of the outer switch statement
        case 'B': // if user chose to evaluate the expression
            for (i=0; i<=strlen(inArray); i++){ // loop to make sure there are no letters in the expression
                if (isalpha(inArray[i])){
                    puts("\nCannot perform evaluation.\n");
                    puts("Evaluated expressions should consist of numbers and operators only.\nRestart program.\n");
                    return 0;
                } // end if statement
            } // end for loop
            switch (type){ // inner switch statement
                case 1 : // if expression is infix
                    out = infixTOpostfix(inArray, strlen(inArray)); // convert infix expression to postfix
                    strcpy(temp, out); // copy *out into temp[] because evaluate() takes an array only
                    sprintf(output, "Result: %d", evaluate(temp)); // evaluate and put result in output[]
                    printf("\n%s\n", output); // output the evaluation result
                    break; // break out of the inner switch statement
                case 2 : // if expression is prefix
                    out = prefixTOinfix(inArray, strlen(inArray)); // convert prefix expression to infix
                    strcpy(temp, out); // copy *out into temp[] because evaluate() takes an array only
                    out = infixTOpostfix(temp, strlen(temp)); // convert infix expression to postfix
                    sprintf(output, "Result: %d", evaluate(out)); // evaluate and put result in output[]
                    printf("\n%s\n", output); // output the evaluation result
                    break; // break out of the inner switch statement
                case 3 : // if expression is postfix
                    sprintf(output, "Result: %d", evaluate(inArray)); // evaluate and put result in output[]
                    printf("\n%s\n", output); // output the evaluation result
                    break; // break out of the inner switch statement
            } // end inner switch statement
            if (inMode=='A'){ // if input was done through file
                fprintf(defaultPtr, "\n%s\n", output); // append output to same file (default.txt)
                puts("[Output was saved to c:\\G51PGA\\default.txt]\n");
                fclose(defaultPtr); // close file using its pointer
            }
            else { // if input was done through keyboard
                fprintf(outPtr, "%s\n", output); // save output[] into output.txt
                puts("[Output was saved to c:\\G51PGA\\output.txt]\n");
                fclose(outPtr); // close file using its pointer
            } // end if-else statement
            break; // break out of the outer switch statement
        case 'C' : // if user chose to exit the program
            return 0; // informs main() to terminate the program
        default :
            puts("Invalid input value.\n");
    } // end outer switch statement
    // determine what the user wants to do next
    printf("----------------------------------\n");
    printf("\nWhat you want to do next:\n");
    printf("(A) Enter a new expression.\t(B) End program.\n? ");
    scanf(" %c", &choice);
    if (choice == 'a' || choice == 'A')
        return 1;  // informs main() to reiterate the loop the program
    else{
        puts("\nProgram was terminated.\n");
        return 0;  // informs main() to terminate the program
    } // end if-else statement
} // end function menu()

char * infixTOpostfix(char inArray[], const int last_element){
     // declare variables
    static char outArray[MAX]={'\0'};
    char temp[2];
    int i, j;

    for (i=0, j=0; i<=last_element; i++){ // loop for conversion
        if (isalpha(inArray[i])){ // if character is an letter
            outArray[j++]=inArray[i]; // push letter into the output array
        }
        else if (isdigit(inArray[i])){ // if character is an integer
            if (isdigit(inArray[i+1])){ // double-digit detection
                sprintf(outArray + j, "%c%c ", inArray[i], inArray[i+1]);
                j+=3; // increment outArray[] element counter
                i++; // because we already accessed the next character
            }
            else { // if it's a single-digit number
                sprintf(outArray + j, "%c ", inArray[i]);
                j+=2; // increment output's element counter
            } // end inner if-else
        }
        else if (isOpening(inArray[i])){ // if character is an opening parenthesis
            strcpy(temp, "(\0"); // change character into a string
            push(temp, 1); // push the parenthesis into the operators stack
        }
        else if (isClosing(inArray[i])){ // if character is a closing parenthesis
            while (operators_top!=NULL && !isOpening(operators_top->data)){ // pop intil it reaches the opening parenthesis
                sprintf(outArray + j, "%c ", operators_top->data); // append outArray[] with stack top
                pop(1); // pop the top of the stack
                j+=2; // increment outArray[] element counter
            }
            pop(1); // pop the opening parenthesis from the stack
        }
        else if (inArray[i]=='+'||inArray[i]=='-'||inArray[i]=='*'||inArray[i]=='/'||inArray[i]=='^'){ // if character is an operator
            if (inArray[i]=='-'){ // if character is a minus sign
                if (isdigit(inArray[i+1])){ // negative number detection
                    if (isdigit(inArray[i+2])){ // negative double-digit number
                        sprintf(outArray + j, "%c%c%c ", inArray[i], inArray[i+1], inArray[i+2]);
                        j+=4; // increment outArray[] element counter
                        i+=2; // because we already accessed the next two characters
                    }
                    else { // negative single-digit
                        sprintf(outArray + j, "%c%c ", inArray[i], inArray[i+1]);
                        j+=3; // increment outArray[] element counter
                        i++; // because we already accessed the next character
                    } // end inner if-else
                    continue; // in order not to get to the next while loop
                } // end if
            } // end if
            while (operators_top!=NULL && priority(operators_top->data) >= priority(inArray[i]) && !isOpening(operators_top->data)){
                // pop while the top has a higher priority than the current operator
                sprintf(outArray + j, "%c ", operators_top->data); // append outArray[] with stack top
                pop(1); // pop the top of the stack
                j+=2; // increment outArray[] element counter
            } // end while loop
            temp[0] = inArray[i]; // convert operator into a string
            temp[1] = '\0';
            push(temp, 1); // push the operator into the operators stack
        } // end nested if-else statement
    } // end for loop

    while (operators_top!=NULL){  // pop all the operators left in the stack
        sprintf(outArray + j, "%c ", operators_top->data);
        pop(1);
    } // end while loop

    return outArray; // return result to caller (menu())
} // end function infixTOpostfix()

char * infixTOprefix(char inArray[], const int last_element){
    // declare variables
    static char outArray[MAX]={'\0'};
    char tempArray[MAX]={'\0'}, temp[2];
    int i, m, n, j;

    strrev(inArray); // reverse input

    for (i=0, j=0; i<=last_element; i++){ // loop through reversed input
        if (isalpha(inArray[i])){ // if character is an operand
            outArray[j++]=inArray[i]; // push the operand into the output array
        }
        else if (isdigit(inArray[i])){ // negative number detection
            if (isdigit(inArray[i+1])){ // negative single-digit
                if (inArray[i+2]=='-'){ // negative double-digit
                    sprintf(outArray + j, "%c%c%c ", inArray[i], inArray[i+1], inArray[i+2]);
                    j+=4; // increment outArray[] element counter
                    i+=2; // because we already accessed the next two characters
                    continue; // in order not to reach the next sprintf()
                } // end if
                 // positive double-digit
                sprintf(outArray + j, "%c%c ", inArray[i], inArray[i+1]);
                j+=3; // increment outArray[] element counter
                i++; // because we already accessed the next character
            }
            else { // positive single-digit
                sprintf(outArray + j, "%c ", inArray[i]);
                j+=2; // increment outArray[] element counter
            } // end if-else statement
        }
        else if (isClosing(inArray[i])){ // if character is a closing parenthesis
            strcpy(temp, ")\0"); // convert parenthesis into a string
            push(temp, 1); // push the parenthesis into the operators stack
        }
        else if (isOpening(inArray[i])){ // if character is an opening parenthesis
            while (operators_top!=NULL && !isClosing(operators_top->data)){ // pop until it reaches closing parenthesis
                sprintf(outArray + j, "%c ", operators_top->data); // append outArray with stack top
                pop(1); // pop the top of the stack
                j+=2; // increment outArray[] element counter
            } // end while loop
            pop(1); // pop the closing parenthesis
        }
        else if (inArray[i]=='+'||inArray[i]=='-'||inArray[i]=='*'||inArray[i]=='/'||inArray[i]=='^'){ // if character is an operator
            while (operators_top!=NULL && priority(operators_top->data) >= priority(inArray[i]) && !isClosing(operators_top->data)){
                // pop while stack top has higher priority than the current operator
                sprintf(outArray + j, "%c ", operators_top->data); // append outArray[] with stack top
                pop(1); // pop the top of the stack
                j+=2; // increment outArray[] element counter
            }
            temp[0]=inArray[i]; // convert operator into a string
            temp[1]='\0';
            push(temp, 1); // push operator into operators stack
        } // end nested if-else statement
    } // end for loop

    while (operators_top!=NULL){ // pop all the operators left in the stack
        sprintf(outArray + j, "%c ", operators_top->data);
        pop(1);
        j+=2; // pop all the operators left in the stack
    } // end while loop

    return strrev(outArray); // return result to caller (menu())
} // end function infixTOprefix()

char * postfixTOinfix(char inArray[], const int last_element){
    // declare variables
    static char outArray[MAX]={'\0'};
    char operand1[MAX]={'\0'}, operand2[MAX]={'\0'}, temp[MAX]={'\0'};
    int i, operand_count=0;

    for (i=0; i<=last_element; i++){ // loop through characters
        if (isalpha(inArray[i])){ // if operand is a letter
            sprintf(temp, "%c \0", inArray[i]); // change operand into a string
            push(temp, 2); // push operand into the operands stack
            operand_count++;
        }
        else if (isdigit(inArray[i])){ // double-digit detection
            if (isdigit(inArray[i+1])){ // double-digit
                sprintf(temp, "%c%c \0", inArray[i], inArray[i+1]); // change both digits into one string
                i++; // because we already accessed the next character
            }
            else { // single-digit
                sprintf(temp, "%c \0", inArray[i]); // change digit into a string
            } // end inner if-else
            push(temp, 2); // push operand into the operands stack
            operand_count++;
        }
        else if (inArray[i]=='+'||inArray[i]=='-'||inArray[i]=='*'||inArray[i]=='/'||inArray[i]=='^'){ // if character is an operator
            if (inArray[i]=='-'){ // negative double-digit detection
                if (isdigit(inArray[i+1])){
                    if (isdigit(inArray[i+2])){ // negative double-digit
                        sprintf(temp, "%c%c%c \0", inArray[i], inArray[i+1], inArray[i+2]); // put characters in a string
                        i+=2; // because we already accessed the next two characters
                    }
                    else { // negative single-digit
                        sprintf(temp, "%c%c \0", inArray[i], inArray[i+1]); // change both digits into one string
                        i++; // because we already accessed the next character
                    } // end inner if-else
                    push(temp, 2); // push the operand into the operands stack
                    operand_count++;
                    continue; // in order not to reach the next if statement
                }// end if
            } // end if
            if (operand_count>=2){ // if there are two consecutive operands in the stack
                strcpy(operand2, operands_top->data); // copy stack top
                pop(2); // pop operands stack top
                strcpy(operand1, operands_top->data); // copy new stack top
                pop(2); // pop operands new stack top
                // put the operator between the operands and surround them with parentheses
                sprintf(temp, "( %s%c %s) ", operand1, inArray[i], operand2);
                push(temp, 2); // push the new operand into operands stack
                operand_count--;
            } // end if
        } // end nested if-else
    } // end for loop
    while (operands_top!=NULL){ // pop all the operands left in the stack
        strcat(outArray, operands_top->data); // append outArray[] with stack top
        pop(2); // pop operands stack top
    } // end while loop

    return outArray; // return result to caller (menu())
}

char * prefixTOinfix(char inArray[], const int last_element){
    // declare variables
    static char outArray[MAX]={'\0'};
    char operand1[MAX]={'\0'}, operand2[MAX]={'\0'}, temp[MAX]={'\0'};
    int i, operand_count=0;

    strrev(inArray); // reverse input array

    for (i=0; i<=last_element; i++){ // loop through reversed input
        if (isalpha(inArray[i])){ // if operand is a letter
            sprintf(temp, "%c \0", inArray[i]); // convert it into a string
            push(temp, 2); // push operand into operands stack
            operand_count++;
        }
        else if (isdigit(inArray[i])){ // if character is a digit
            if (isdigit(inArray[i+1])){ // double-digit operand
                if (inArray[i+2]=='-'){ // negative double-digit
                    sprintf(temp, "%c%c%c \0", inArray[i+2], inArray[i+1], inArray[i]); // put characters in a string
                    i+=2; // because we already accessed the next two characters
                }
                else { // positive double-digit
                    sprintf(temp, "%c%c \0", inArray[i+1], inArray[i]); // put both digits in one string
                    i++; // because we already accessd the next character
                } // end if-else
            }
            else { // single-digit operand
                sprintf(temp, "%c \0", inArray[i]); // convert digit into a string
            } // end if-else statement
            push(temp, 2); // push operand into operands stack
            operand_count++;
        }
        else if (inArray[i]=='+'||inArray[i]=='-'||inArray[i]=='*'||inArray[i]=='/'||inArray[i]=='^'){ // if character is an operator
            if (operand_count>=2){ // if there are two operands in the stack
                strcpy(operand2, operands_top->data); // copy operands stack top
                pop(2); // pop operands stack top
                strcpy(operand1, operands_top->data); // copy new operands stack top
                pop(2); // pop new operands stack top
                // put the operator between the operands and surround them with parentheses
                sprintf(temp, "( %s%c %s) ", operand2, inArray[i], operand1);
                push(temp, 2); // push the new operand into the operands stack
                operand_count--;
            } // end if
        } // end nested if-else statement
    } // end for loop
    while (operands_top!=NULL){ // pop all the operands left in the stack
        strcat(outArray, operands_top->data); // copy stack top into outArray[]
        pop(2); // pop stack top
    } // end while loop

    return outArray; // return result to caller (menu())
} // end function prefixTOinfix()

int evaluate(char in[]){ // this function evaluates postfix expressions
    // declare variables
    char temp[5];
    int op1=0, op2=0, i;

    for (i=0; i<=strlen(in); i++){ // loop through input characters
        if (isdigit(in[i])){ // if character is an operand
            if (isdigit(in[i+1])){ // if operand is a double-digit
                sprintf(temp, "%c%c\0", in[i], in[i+1]); // put both digits into ont string
                push(temp, 3); // push operand into evaluation stack
                i++; // because we already accessed the next character
            }
            else { // if operand is a single-digit
                sprintf(temp, "%c\0", in[i]); // convert it into a string
                push(temp, 3); // push operand into evaluation stack
            } // end inner if-else
        }
        else if (in[i]=='+' || in[i]=='-' || in[i]=='*' || in[i]=='/' || in[i]=='^'){ // if character is an operator
            if (in[i]=='-'){ // negative number detection
                if (isdigit(in[i+1])){
                    if (isdigit(in[i+2])){ // negative double-digit operand
                        sprintf(temp, "%c%c%c\0", in[i], in[i+1], in[i+2]); // put the three characters into a string
                        i+=2; // because we already accessed the next two characters
                    }
                    else{ // negative single-digit operand
                        sprintf(temp, "%c%c\0", in[i], in[i+1]); // put both characters into one string
                        i+=1; // because we already accessed the next character
                    } // end if-else statement
                    push(temp, 3); // push new operand into evaluation stack
                    continue; // in order not to reach the next pop operations
                } // end if
            } // end if
            op1 = evaluation_top->value; // copy evaluation stack top into op1
            pop(3); // pop evaluation stack top
            op2 = evaluation_top->value; // copy new evaluation stack top into op2
            pop(3); // pop new evaluation stack top
            switch (in[i]){ // determine the arithmetic operation to be carried out
                case '+': // if addition, add the operands and push the result into the stack
                    sprintf(temp, "%d\0", (op1 + op2));
                    push(temp, 3);
                    break;
                case '-': // if subtraction, subtract op1 from op2 and push the result into the stack
                    sprintf(temp, "%d\0", (op2 - op1));
                    push(temp, 3);
                    break;
                case '*': // if multiplication, multiply the operands and push the result into the stack
                    sprintf(temp, "%d\0", (op2 * op1));
                    push(temp, 3);
                    break;
                case '/': // if division, divide op2 by op1 and push the result into the stack
                    sprintf(temp, "%d\0", (op2 / op1));
                    push(temp, 3);
                    break;
                case '^': // if power, raise op2 to the power of op1 and push the result into the stack
                    sprintf(temp, "%d\0", pow(op2,op1));
                    push(temp, 3);
                    break;
            } // end switch statement
        } // nested if-else statement
    } // end for loop

    return evaluation_top->value; // return stack top (result) to the caller (menu())
} // end function evaluate()

void push(char in[], int code){
    // declare variables
    Operators *tempOperatorPtr;
    Operands *tempOperandPtr;
    Evaluation *tempEvalPtr;
    int int_val, i, j;
    char exp[5]={'\0'};

    switch (code){ // determine the stack to push input into
        case 1: // push an operator (character) into the operators stack
            tempOperatorPtr=(Operators *)malloc(sizeof(Operators)); // allocate enough memory for new struct
            tempOperatorPtr->data = in[0]; // set new struct data to input
            if (operators_top==NULL) // if stack is empty
                tempOperatorPtr->nextNode = NULL; // point to nothing
            else // if stack is not empty
                tempOperatorPtr->nextNode = operators_top; // point to stack top
            operators_top = tempOperatorPtr; // overwrite operators top's address with the new struct's address
            break;
        case 2: // push an operand (array) into the operands stack
            tempOperandPtr=(Operands *)malloc(sizeof(Operands)); // allocate enough memory for new struct
            strcpy(tempOperandPtr->data, in); // copy input into new struct's data
            if (operands_top==NULL) // if stack is empty
                tempOperandPtr->nextNode = NULL; // point to nothing
            else // if stack is not empty
                tempOperandPtr->nextNode = operands_top; // point to stack top
            operands_top = tempOperandPtr; // overwrite operands top's address with the new struct's address
            break;
        case 3: // push an operand (integer) into the evaluation stack
            for (i=0, j=0; i<=strlen(in); i++){ // loop to discard any character other than digit ot minus sign
                if (isdigit(in[i]) || in[i]=='-')
                    exp[j++] = in[i]; // put character in exp[]
            } // end for loop
            int_val = atoi(exp); // convert character sting exp[] into an integer
            if (evaluation_top==NULL){ // if stack is empty
                evaluation_top=(Evaluation *)malloc(sizeof(Evaluation)); // allocate enough memory for new struct
                evaluation_top->value = int_val; // set new struct's value to input integer value
                evaluation_top->nextNode = NULL; // point to nothing
            }
            else{ // if stack is not empty
                tempEvalPtr = evaluation_top; // set new struct's address to stack top's address
                evaluation_top = (Evaluation *)malloc(sizeof(Evaluation)); // allocate enough memory for new struct
                evaluation_top->value = int_val; // set new stack top's value to input integer value
                evaluation_top->nextNode = tempEvalPtr; // point to old stack top
            } // end if-else statement
            break;
    } // end switch statement
} // end function push()

void pop(int code){
    // declare variables
    Operators *tempOperatorPtr;
    Operands *tempOperandPtr;
    Evaluation *tempEvalPtr;

    switch (code){ // determine the stack to pop its top
        case 1: // pop operators stack's top
            if (operators_top->nextNode == NULL) // if stack is empty
                operators_top = NULL; // nullify the top
            else { // if stack is not empty
                tempOperatorPtr = operators_top; // hold old stack top's address in temp
                operators_top = operators_top->nextNode; // new stack top points to old top's next node
                free(tempOperatorPtr); // free the old stack top struct
            } // end if-else statement
            break;
        case 2: // pop oprands stack's top
            if (operands_top != NULL){ // make sure stack is not empty
                tempOperandPtr = operands_top; // hold old stack top's address in temp
                operands_top = tempOperandPtr->nextNode; // new stack top points to old top's next node
                free(tempOperandPtr); // free the old stack top struct
            } // end if
            break;
        case 3: // pop evaluation stack's top
            if (evaluation_top->nextNode == NULL){ // if stack is empty
                evaluation_top = NULL; // nullify the top
            }
            else { // if stack is not empty
                tempEvalPtr = evaluation_top; // hold old stack top's address in temp
                evaluation_top = tempEvalPtr->nextNode; // new stack top points to old top's next node
                free(tempEvalPtr); // free the old stack top struct
            } // end if-else statement
            break;
    } // end switch statement
} // end function pop()

int priority(char in){ // sets the priority of its input symbol and returns it as an integer
    switch(in){
    case '(':
        return 1;
    case '+':
    case '-':
        return 2;
    case '*':
    case '/':
        return 3;
    case '^':
        return 4;
    } // end switch statement
} // end function priority()

bool isOpening(char in){ // checks if a specific character of the input is an opening parenthesis
    // if it's an opening parenthesis, return 1. Otherwise, return 0
    return (in=='('||in=='['||in=='{')? 1 : 0;
}

bool isClosing(char in){ // checks if a specific character of the input is a closing parenthesis
    // if it's a closing parenthesis, return 1. Otherwise, return 0
    return (in==')'||in==']'||in=='}')? 1 : 0;
}
