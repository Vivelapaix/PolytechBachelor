#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<errno.h>

//#include <crtdbg.h>
//#define _CRTDBG_MAPALLOC
//#define _CRTDBG_MAP_ALLOC

void CheckPrintStr ( char *str, double result, char error );// функция печати строки
int DynamicString ( FILE* file, char **str, char *error, int *enter );//функция приема динамической строки
int IsNumber ( char c ); //функция обработки числа
int IsOperator ( char c ); //функция обработки оператора
int IsFunction ( char c ); //обработка функций
int Priority ( char c );//функция назначения приоритета операциям
int Associativity ( char c );//функция назначения ассоциативности операторам
char *InputToPolsk ( char *str, char *output, char *error, char *variables );//функция,возвращающая строку в обратной польской записи
double calculation ( char *str, double *arrayOfNumbers, char *error, double *numbers, char *variables );//функция вычисления строки в "Обратной польской записи"
double constants( char *str );//обработка констант
int unaryMinus ( char *str, int topOfOutput );// функция обработки унарного минуса
void codesOfErrors ( char error, char *str );//обработка ошибок
int IsConstant ( char c ); // функция обработки константы
int parsingOfString(char *str, char *error);
int parsingOfVariable ( char *str, int indOfToken );
int IsVariable( char c );
int IsVariableTo( char c, char *variables);
void splitOfInput( char *input, char **varExpression, char *error, char *variables, char *output, double *numbers, double *arrayOfNumbers );// разбиение входной строки,если в ней имеются переменные
void SymbolicCalculator( char *str, char *output, char *variables, double *arrayOfNumbers, double *numbers, char *error, int delim );

int main ( int argc, char **argv )
{ 
  char *str = NULL;
  int len = 0, enter = 0;
  char error = 'A';
  double res = 0;
  FILE *file = NULL;
  //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  //_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  //_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
  //_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  //_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
  //_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  //_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
  //_CrtDumpMemoryLeaks();

    if ( argc > 2 )
    {
      printf("ERROR: to many arguments");
      return -1;
    }
    if ( argc == 2 )
    {
       fopen_s ( &file, argv[1], "r" );
       if ( file == NULL )
       {
         printf ( "ERROR: the file is absent" );
         return -1;
       }
    }
    else if ( argc == 1 )
    {
      file=stdin;
    }
   
    while ( DynamicString( file, &str, &error, &enter ) )
    {
	  if ( error == 'A' )
	  {
        len = strlen( str );
        if ( str[len-1] == '\n' )
        {
          str[len-1] = 0;
          enter = 1;
        } 
        else
	    {
		  enter = 0;
	    }
	  } 
	  CheckPrintStr ( str, res, error );
      if ( enter )
	  {
        printf("\n");
	  }
      enter = 0;
	  error = 'A';
    }

    free ( str );
    fclose ( file );

	return 0;
}


void CheckPrintStr ( char *str, double result, char error )
{
  int i = 0, delim = 0, length = ( int )strlen( str );
  char *output = malloc( ( int )3*length*sizeof(char) );
  char *variables = malloc ( ( int )length*sizeof(char) );
  double *arrayOfNumbers = malloc ( ( int )length*sizeof(double) );
  double *numbers = malloc ( ( int )length*sizeof(double) );

  delim = parsingOfString( str, &error);

  if( ( error == 'A' ) || ( error == 'G' )  )
  {
	error = ( ( output != NULL ) && ( arrayOfNumbers != NULL ) && ( variables != NULL ) && ( numbers != NULL ) ) ? 'A' : 'm' ;
  }
  
  if ( error == 'A' )
  {
  while( i < length )
  {
    while(( str[i] == ' ' ) || ( str[i] == '\t' ) )
    {
      i++;
    } 
    if( ( str[i] == '/' ) && ( str[i+1] == '/' ) )
    {
		printf("%s",str);
        break; 
    }
    else if( str[i] == 0 )
    {
      printf("%s",str);
      break;
    }
    else
    { 
      if( delim != 0 )
	  {
        SymbolicCalculator( str, output, variables, arrayOfNumbers, numbers, &error, delim );
	  }
	  else
	  {
	    InputToPolsk ( str, output, &error, variables );
	  }
	  result = ( error == 'A' ) ? calculation ( output, arrayOfNumbers, &error, numbers, variables ) : 0 ;
    
	  if ( error == 'A' )
	  {
		  printf("%s == %g", str, result );
		  error = 'A';
		  errno = 0;
	  }
	  else
	  {
		  codesOfErrors ( error, str );
		  error = 'A';
		  errno = 0;
	  }
      break;
    }
  }
  }
  else
  {
    codesOfErrors ( error, str );
    error = 'A';
    errno = 0;
  }

  free( output );
  free( arrayOfNumbers );
  free( variables );
}


int DynamicString ( FILE* file, char **str, char *error, int *enter )
{
  int counter = 0;
  char *tmp = NULL;
  int c;
  int memory = 1;
  while( ( ( c = fgetc( file ) ) != EOF ) )
  {
    if( memory )
	{
      tmp = ( char* )realloc( ( *str ), ( counter+2 )*sizeof( char ) );
	}
    if( ( tmp != NULL ) && memory )
    {
      (*str) = tmp;
      (*str)[counter] = ( unsigned char ) c;
      counter++;
	  if ( c == '\n' )
	  {
	    break;
	  }
    }
    else
    {
      memory = 0;
	  *error = 'm';
	  if( counter == 0 )
      {
	    counter++;
      }
      if ( c == '\n' )
	  {	  
        *enter = 1;
	    break;
	  }
    }
  }
   if( ( *error == 'A' ) && ( tmp != NULL ) )
   {
    (*str)[counter] = 0;
   }
   memory = 1;

  return counter;
}



 int IsNumber ( char c )
 {
	if( ( c >= '0' ) && ( c <= '9' ))
	  return 1;
	else
      return 0;
 }
 
 int IsOperator ( char c )
 {
	 if( ( c == '+' ) || ( c == '-' ) || ( c == '*' ) || ( c == '/' ) || ( c == '^' ) )
	   return 1;
	 else
	   return 0;
 }

 int IsFunction ( char c )
 {
	 if( ( c == 'a' )  || ( c == 'c' ) || ( c == 's' ) || ( c == 't' ) || ( c == 'l' ) || ( c == 'f' ) )
       return 1;
	 else
	   return 0;
 }

 int IsConstant ( char c )
 {
	 if( ( c == 'p' )  || ( c == 'e' ) )
       return 1;
	 else
	   return 0;
 }
  
 int IsVariable ( char c )
 {
   if( (( c >= 'A' ) && ( c <= 'Z' ) || ( c >= 'a' ) && ( c <= 'z' )) && ( c != 'e' ) && ( c != 'E' ) )
	  return 1;
   else
      return 0;
 }

 int Priority ( char c )
 {
   switch(c)
   {
   case '+':
   case '-':
	 return 1;

   case '*':
   case '/':
     return 2;

   case 's':
   case 'c':
   case 't':
   case 'g':
   case 'S':
   case 'C':
   case 'T':
   case 'q':
   case 'l':
   case 'o':
   case 'i':
   case 'L':
	 return 4;

   case 'u':
	 return 3;

   case'^':
	 return 3;
   }
   return 0;
 }

 int Associativity ( char c )
 {
    //правая ассоциативность
   switch(c)
   {
   case '^':
   case 'u':
     return 1;
	//левая ассоциативность
   case '+':
   case '/':
   case '*':
   case '-':
   case 's':
   case 'c':
   case 't':
   case 'g':
   case 'S':
   case 'C':
   case 'T':
   case 'q':
   case 'l':
   case 'o':
   case 'i':
   case 'L':
	 return 0;	  
   }
  
   return 0;
  }


char parsingOfFunction( char *str, char *endOfFunction )
{
	if ( ( *str == 's' ) && ( *( str + 1 )== 'i' ) && ( *( str + 2 ) == 'n' ) && ( *( str + 3 ) == '(' ) )
	{
		*endOfFunction = '(';
		return 's';
	}
    else if ( ( *str == 'c' ) && ( *( str + 1 )== 'o' ) && ( *( str + 2 ) == 's' ) && ( *( str + 3 ) == '(' ) )
	{
		*endOfFunction = '(';
		return 'c';
	}
    else if ( ( *str == 't' ) && ( *( str + 1 )== 'g' ) && ( *( str + 2 ) == '(' ) )
	{
		*endOfFunction = '(';
		return 't';
	}
    else if ( ( *str == 'c' ) && ( *( str + 1 )== 't' ) && ( *( str + 2 ) == 'g' ) && ( *( str + 3 ) == '(' ) )
	{
		*endOfFunction = '(';
		return 'g';
	}
    else if ( ( *str == 'a' ) && ( *( str + 1 )== 'r' ) && ( *( str + 2 ) == 'c' ) && ( *( str + 3 )== 's' ) && ( *( str + 4 ) == 'i' ) && ( *( str + 5 ) == 'n' ) && ( *( str + 6 ) == '(' ) )
	{
		*endOfFunction = '(';
		return 'S';
	}
    else if ( ( *str == 'a' ) && ( *( str + 1 )== 'r' ) && ( *( str + 2 ) == 'c' ) && ( *( str + 3 )== 'c' ) && ( *( str + 4 ) == 'o' ) && ( *( str + 5 ) == 's' ) && ( *( str + 6 ) == '(' ) )
	{
		*endOfFunction = '(';
		return 'C';
	}
    else if ( ( *str == 'a' ) && ( *( str + 1 )== 'r' ) && ( *( str + 2 ) == 'c' ) && ( *( str + 3 )== 't' ) && ( *( str + 4 ) == 'g' ) && ( *( str + 5 ) == '(' ) )
	{
		*endOfFunction = '(';
		return 'T';
	}
	else if ( ( *str == 's' ) && ( *( str + 1 )== 'q' ) && ( *( str + 2 ) == 'r' ) && ( *( str + 3 ) == 't' ) && ( *( str + 4 ) == '(' ) )
	{
		*endOfFunction = '(';
		return 'q';
	}
	else if ( ( *str == 'l' ) && ( *( str + 1 )== 'n' ) && ( *( str + 2 ) == '(' ) )
	{
		*endOfFunction = '(';
		return 'l';
	}
	else if ( ( *str == 'f' ) && ( *( str + 1 )== 'l' ) && ( *( str + 2 ) == 'o' ) && ( *( str + 3 ) == 'o' ) && ( *( str + 4 ) == 'r' ) && ( *( str + 5 ) == '(' ) )
	{
		*endOfFunction = '(';
		return 'o';
	}
	else if ( ( *str == 'c' ) && ( *( str + 1 )== 'e' ) && ( *( str + 2 ) == 'i' ) && ( *( str + 3 ) == 'l' ) && ( *( str + 4 ) == '(' ) )
	{
		*endOfFunction = '(';
		return 'i';
	}
	else if ( ( *str == 'l' ) && ( *( str + 1 )== 'o' ) && ( *( str + 2 ) == 'g' ) && ( *( str + 3 ) == '(' ) )
	{
		*endOfFunction = '(';
		return 'L';
	}
	else 
	{
		return 'f';
	}

}

 char parsingOfConstants ( char *str, int beginOfConstant, char *endOfConstant )
 {
	int j = 0;
	j = beginOfConstant;
    if ( ( str[j] == 'p' ) && ( str[j+1]== 'i' ) )
	{
	 *endOfConstant = str[j+2];
	 j+=2;
	 if ( ( str[j] == ' ' ) || ( str[j] == '\t' ) )
	 {
	   while( ( str[j] == ' ' ) || ( str[j] == '\t' ) )
	   {
		 j++;
	   }
	 }
	 if ( IsOperator ( str[j] ) || ( str[j] == 0 ) || ( str[j] == ')') || ( str[j] == ',') )
	 {
		j = 0;
		return 'A';
	 }
     else
	 {
	    j = 0;
	    return 'O';
	 }
	}
	else if ( str[j] == 'e' ) 
	{
	  *endOfConstant = str[j+1];
	  j++;
	 if ( ( str[j] == ' ' ) || ( str[j] == '\t' ) )
	 {
	   while( ( str[j] == ' ' ) || ( str[j] == '\t' ) )
	   {
		 j++;
	   }
	 }
	 if ( IsOperator ( str[j] ) || ( str[j] == 0 ) || ( str[j] == ')') || ( str[j] == ',') )
	 {
		j = 0;
		return 'A';
	 }
     else
	 {
	    j = 0;
	    return 'O';
	 }
	}
	else
	{
		return 'y';
	}
 }

 char *InputToPolsk ( char *str, char *output, char *error, char *variables )
 {
  int j = 0, topOfStack = 0, lengthOfString = ( int )strlen( str ), 
  topOfOutput = 0, tmpOfStack = 0, brackets = 0, counter = 0, topOfVariables = 0;
  double number = 0, result = 0;
  char *operator_stack = malloc( lengthOfString*sizeof( char ) );
  char *endOfNumber = NULL; 
  char tmpError = 'A', endOfFunction = 'A', endOfConstant = 'A', tmpToken;

  if( operator_stack == NULL )
  {
    *error = 'm';
  }

  if( *error == 'A' )
  {
    while( j < lengthOfString )
	{  
	  if ( ( str[j] == ' ' ) || ( str[j] == '\t' ) )
	  {
		;
	  }
	  else if ( IsOperator( str[j] ) )
	  { 
        tmpToken = str[j];
	    if( tmpToken == '-' )
	    {
		  if( unaryMinus ( str, j ) ) 
		  {
            tmpToken = 'u';
		  }
	    }
	    if( tmpToken != 'u' )
	    {
		  output[topOfOutput++] = ' ';
	    }
	    while ( topOfStack > 0 )
	    {   
		  tmpOfStack = topOfStack-1;
		  if( ( Priority( operator_stack[tmpOfStack] ) >= Priority ( tmpToken ) ) && !Associativity( tmpToken ) 
			|| Associativity( tmpToken )&&( Priority( operator_stack[tmpOfStack] ) > Priority ( tmpToken ) ) )
		  {
	        output[topOfOutput++] = operator_stack[tmpOfStack];
	        output[topOfOutput++] = ' ';
		    operator_stack[tmpOfStack] = ' ';
		    topOfStack--;
		  }
		  else
		  {
		    break;
		  }	
	    }	
	    operator_stack[topOfStack++] = tmpToken;
	  }
      else if (  IsNumber( str[j] ) || ( str[j] == '.' ) )
	  {
		number = strtod( str + j, &endOfNumber );
		if( ( errno == ERANGE ) || ( endOfNumber == ( str + j ) ) )
		{
		  *error = 'n';
		  errno = 0;
		}
		else
		{
		  while ( ( str + j ) != endOfNumber )
		  {
			output[topOfOutput++] = str[j];
			j++;
		  }
          output[topOfOutput++] = ' ';
		  j--;
		}
        number = 0;	
	    errno = 0;
	  }
	  else if( str[j]=='(' )
	  {
        operator_stack[topOfStack++] = str[j];
	    counter = 0;
      }
	  else if ( str[j] == ')' )
	  {  
        output[topOfOutput++] = ' ';
	    if ( topOfStack > 0 )
	    {
		  topOfStack--;
	    }
	    else
	    {
	      *error = 'O';
		  break;
	    } 
	    while ( topOfStack >= 0 )
	    {
		  if( operator_stack[topOfStack] == '(' )
		  {
		    brackets = 1;
		    break;
		  }
		  else
		  {
		    output[topOfOutput++] = operator_stack[topOfStack];
		    operator_stack[topOfStack] = ' ';
		    if( topOfStack == 0 )
		    {
			  break;
		    }
	        topOfStack--;
		  }
		}
	    if( counter == 0 )
	    {
	      brackets = 0;
	    }
	    if( !brackets )
	    {
		  *error = 'B';
	    }
	    brackets = 0;
	    counter = 0;
	    operator_stack[topOfStack] = ' ';
	  }
	  else if ( IsFunction ( str[j] ) )
	  {
		if ( ( tmpError = parsingOfFunction( str + j, &endOfFunction ) ) == 'f')
		{
		  *error = tmpError;
          if( IsVariableTo( str[j], variables ) > -1)
		  {
		    output[topOfOutput++] = str[j];
			output[topOfOutput++] = '#';
            *error = 'A';
		  }
		  else
		  {
		    break;
		  }
		}
		else
		{
		  operator_stack[topOfStack++] = tmpError;
		  while( str[j] != endOfFunction )
		  {
		    j++;
		  }
		  tmpError = 'A';
		  j--;
		}
		
	  }
	  else if ( IsConstant ( str[j] ) )
	  {
	    if ( ( tmpError = parsingOfConstants( str, j, &endOfConstant ) ) == 'A')
	    {
		  while( str[j] != endOfConstant )
		  {
		    output[topOfOutput++] = str[j];
		    j++;
		    tmpError = 'A';
	        j--;
		  }
		}
	    else
	    {
		  *error = tmpError;
          if( IsVariableTo( str[j], variables ) > -1 )
		  {
		    output[topOfOutput++] = str[j];
			output[topOfOutput++] = '#';
            *error = 'A';
     	  }
		  else
		  {
		    break;
		  }
		}
	  }
	  else if ( str[j] == ',' )
	  {
	    output[topOfOutput++] = ' ';
	    if ( topOfStack > 0 )
	    {
		  topOfStack--;
	    }
	    else
	    {
	      *error = 'O';
		  break;
	    }
	    while ( topOfStack >= 0 )
	    {
		  if( operator_stack[topOfStack] == '(' )
		  {
		    brackets = 1;
		    break;
		  }
		  else
		  {
		    output[topOfOutput++] = operator_stack[topOfStack];
		    operator_stack[topOfStack] = ' ';
		    if( topOfStack == 0 )
		    {
			  break;
		    }
	        topOfStack--;
		  }
		}
	    if( !brackets )
	    {
		  *error = 'B';
	    }
	    brackets = 0;
	    topOfStack++;
	  }
	  else
	  {
		*error = 'y';
        if( IsVariableTo( str[j], variables ) > -1 )
		{
		  output[topOfOutput++] = str[j];
		  output[topOfOutput++] = '#';
          *error = 'A';
		}
	    else
		{
		  break;
	    }
	  }
	
      if ( ( j+1 ) == lengthOfString )
      { 
	    if( ( output[topOfOutput-1] == ' ' ) || ( output[topOfOutput-1] == '\t' ) )
	    {
		  ;
	    }
	    else
	    {
		  output[topOfOutput++] = ' ';
	    }
	    topOfStack--;
        while ( topOfStack >= 0 )
	    {
	      if ( ( operator_stack[topOfStack] == '(' ) || ( operator_stack[topOfStack] == ')' ) )
		  {
		    *error = 'B';
		    break;
		  }
		  output[topOfOutput++] = operator_stack[topOfStack];
          output[topOfOutput++] = ' ';
		  operator_stack[topOfStack] = ' ';
	      topOfStack--;
		}
	    topOfStack++;
	    output[topOfOutput-1] = '\0';
	  }
	  if ( *error == 'A' )
	  {
	    if( ( str[j] != ' ' ) && ( str[j] != '\t' ) && ( str[j] != '(' ) )
	    {
	      counter++;
	    }
	    j++;
	  }
	  else
	  {
	    break;
	  }
  }
}
 free( operator_stack );
 return output;
}



double calculation ( char *str, double *arrayOfNumbers, char *error, double *numbers, char *variables )
{
    int lengthOfString = 0, j = 0, i = 0, counterOfNumbers = 0, position;
	char *opt;
	double result = 0;
    lengthOfString = strlen( str );

    if( *error == 'A' )
	{
	while ( j < lengthOfString ) 
    {
	  if ( str[j] == ' ' || str[j] == '\t')
	  {
		j++;
	  }
	  if ( IsNumber( str[j] ) || ( str[j] == '.' ) )
	  {
		arrayOfNumbers[i++] = strtod( str+j, &opt );
		errno = 0;
		counterOfNumbers++;
        while ( str[j] != *opt )
	    {
		  j++;
	    }
	  }
	  else if ( IsVariable ( str[j] ) && ( str[j+1] == '#' ) )
	  {
	    position = IsVariableTo( str[j], variables );
	    if( position != -1 )
		{
		  arrayOfNumbers[i++] = numbers[position];
		  j++;
		}
		else
		{
		  *error = 'O';
		}
	  }
	  else if ( IsConstant( str[j] ) )
	  {
        if ( str[j] == 'e' )
		{
			arrayOfNumbers[i++] = 2.71828182845904523536;
			j++;
		}
		else
		{
			arrayOfNumbers[i++] = 3.14159265358979323846;
			j+= 2;
		}
	  }
      else if ( IsOperator ( str[j] ) || ( str[j] == 's' ) || ( str[j] == 'c' ) || ( str[j] == 't' ) || ( str[j] == 'g' ) || ( str[j] == 'S' ) || ( str[j] == 'C' ) || ( str[j] == 'T' ) || ( str[j] == 'q' ) || ( str[j] == 'l' ) || ( str[j] == 'u' ) || ( str[j] == 'o' ) || ( str[j] == 'i' ) || ( str[j] == 'L' ))
	  {
		switch ( str[j] ) 
	    {
		case '+':
	    if ( ( i - 2 ) >= 0)
		{
		  if ( ( arrayOfNumbers[i-2] + arrayOfNumbers[i-1] ) < pow(10,308) )
		  {
			arrayOfNumbers[i-2] = arrayOfNumbers[i-2] + arrayOfNumbers[i-1];
			i--;
		  }
		  else
		  {
		    *error = 'b';
		  }
		}
		else
		{
          *error = 'O';
		}
        break;
        case '-':
		if ( ( i - 2 ) >= 0)
		{
		  if ( ( arrayOfNumbers[i-2] - arrayOfNumbers[i-1] ) < pow(10,308) )
		  {
			arrayOfNumbers[i-2] = arrayOfNumbers[i-2] - arrayOfNumbers[i-1];
			i--;
		  }
		  else
		  {
		    *error = 'b';
		  }
		}
		else
		{
          *error = 'O';
		}
        break;
        case '*':
		if ( ( i - 2 ) >= 0)
		{
		  if ( ( arrayOfNumbers[i-2] * arrayOfNumbers[i-1] ) <= pow(10,308) )
		  {
			arrayOfNumbers[i-2] = arrayOfNumbers[i-2] * arrayOfNumbers[i-1];
			i--;
		  }
		  else
		  {
		    *error = 'b';
		  }
		}
		else
		{
          *error = 'O';
		}
        break;
        case '/':
		if ( ( i - 2 ) >= 0)
		{
		  if( arrayOfNumbers[i-1] != 0 )
		  {
		    if ( ( arrayOfNumbers[i-2] / arrayOfNumbers[i-1] ) <= pow(10,308) )
		    {
			  arrayOfNumbers[i-2] = arrayOfNumbers[i-2] / arrayOfNumbers[i-1];
			  i--;
			}
		    else
		    {
		     *error = 'b';
		    }
		  }
		  else
		  {
		    *error = 'd';
		  }
		}
		else
		{
          *error = 'O';
		}
        break;
	    case '^':
		if ( ( i - 2 ) >= 0)
		{
			if ( pow ( arrayOfNumbers[i-2], arrayOfNumbers[i-1] ) <= pow(10,308) )
		    {
			  arrayOfNumbers[i-2] = pow ( arrayOfNumbers[i-2], arrayOfNumbers[i-1] );
			  i--;
			}
		    else
		    {
		    *error = 'b';
		    }
		}
		else
		{
          *error = 'O';
		}
        break;
	    case 's':
		if ( ( i - 1 ) >= 0)
		{
		  if ( ( arrayOfNumbers[i-1] > (-1)*( pow(10,308) ) ) &&  ( arrayOfNumbers[i-1] < ( pow(10,308) ) ) )
		  {
		    arrayOfNumbers[i-1] = sin ( arrayOfNumbers[i-1] );
		  }
		  else
		  {
		    *error = 'b';
		  }
		}
		else
		{
          *error = 'O';
		}
        break;
        case 'c':
		if ( ( i - 1 ) >= 0)
		{
          if ( ( arrayOfNumbers[i-1] > (-1)*( pow(10,308) ) ) &&  ( arrayOfNumbers[i-1] < ( pow(10,308) ) ) )
		  {
		   arrayOfNumbers[i-1] = cos( arrayOfNumbers[i-1] );
          }
          else
		  {
		    *error = 'b';
		  }
		}
		else
		{
          *error = 'O';
		}
        break;
        case 't':
		if ( ( i - 1 ) >= 0)
		{
		  if ( ( arrayOfNumbers[i-1] > (-1)*( pow(10,308) ) ) &&  ( arrayOfNumbers[i-1] < ( pow(10,308) ) ) )
		  {
		    if( ceil( ( arrayOfNumbers[i-1]-3.14159265358979323846/2 )/3.14159265358979323846 ) != ( arrayOfNumbers[i-1]-3.14159265358979323846/2 )/3.14159265358979323846 )
		    {
		     arrayOfNumbers[i-1] = tan( arrayOfNumbers[i-1] );
		    }
		    else
		    {
		    *error = 'o';
		    }
		  }
          else
		  {
		    *error = 'b';
		  }
		}
		else
		{
          *error = 'O';
		}
        break;
        case 'g':
		if ( ( i - 1 ) >= 0)
		{
		  if ( ( arrayOfNumbers[i-1] > (-1)*( pow(10,308) ) ) &&  ( arrayOfNumbers[i-1] < ( pow(10,308) ) ) )
		  {
		    if( ceil( arrayOfNumbers[i-1] /3.14159265358979323846 ) != arrayOfNumbers[i-1] /3.14159265358979323846 )
		    {
		     arrayOfNumbers[i-1] = 1/tan( arrayOfNumbers[i-1] );
		    }
		    else
		    {
		    *error = 'o';
		    }
		  }
          else
		  {
		    *error = 'b';
		  }
		}
		else
		{
          *error = 'O';
		}
        break;
        case 'S':
		if ( ( i - 1 ) >= 0)
		{
		  if( ( arrayOfNumbers[i-1] >= -1 ) && ( arrayOfNumbers[i-1] <= 1 ) )
		  {
			arrayOfNumbers[i-1] = asin ( arrayOfNumbers[i-1] );
		  }
		  else
		  {
		    *error = 'o';
		  }
		}
		else
		{
          *error = 'O';
		}
        break;
        case 'C':
		if ( ( i - 1 ) >= 0)
		{
		  if( ( arrayOfNumbers[i-1] >= -1 ) && ( arrayOfNumbers[i-1] <= 1 ) )
		  {
			arrayOfNumbers[i-1] = acos ( arrayOfNumbers[i-1] );
		  }
		  else
		  {
		    *error = 'o';
		  }
		}
		else
		{
          *error = 'O';
		}
        break;
        case 'T':
		if ( ( i - 1 ) >= 0)
		{
		  if ( ( arrayOfNumbers[i-1] > (-1)*( pow(10,308) ) ) &&  ( arrayOfNumbers[i-1] < ( pow(10,308) ) ) )
		  {
		   arrayOfNumbers[i-1] = atan ( arrayOfNumbers[i-1] );
		  }
		  else
		  {
		    *error = 'b';
		  }
        }
		else
		{
          *error = 'O';
		}
        break;
		case 'q':
		if ( ( i - 1 ) >= 0)
		{
		  if( arrayOfNumbers[i-1] >= 0 )
		  {
		    if ( ( arrayOfNumbers[i-1] > (-1)*( pow(10,308) ) ) &&  ( arrayOfNumbers[i-1] < ( pow(10,308) ) ) )
		    {
			arrayOfNumbers[i-1] = sqrt ( arrayOfNumbers[i-1] );
			}
		    else
		    {
		    *error = 'b';
		    }
		  }
		  else
		  {
		    *error = 'o';
		  }
		}
		else
		{
          *error = 'O';
		}
        break;
        case 'l':
		if ( ( i - 1 ) >= 0)
		{
		  if( ( arrayOfNumbers[i-1] > 0 ) &&  ( arrayOfNumbers[i-1] <  pow(10,308) ) )
		  {
			arrayOfNumbers[i-1] = log ( arrayOfNumbers[i-1] );
		  }
		  else
		  {
		    *error = 'o';
		  }
		}
		else
		{
          *error = 'O';
		}
        break;
		case 'o':
		if ( ( i - 1 ) >= 0)
		{
		  if ( ( arrayOfNumbers[i-1] > (-1)*( pow(10,308) ) ) &&  ( arrayOfNumbers[i-1] < ( pow(10,308) ) ) )
		  {
		    arrayOfNumbers[i-1] = floor ( arrayOfNumbers[i-1] );
		  }
		  else
		  {
		    *error = 'b';
		  }
		}
		else
		{
          *error = 'O';
		}
        break;
        case 'i':
		if ( ( i - 1 ) >= 0)
		{
		  if ( ( arrayOfNumbers[i-1] > (-1)*( pow(10,308) ) ) &&  ( arrayOfNumbers[i-1] < ( pow(10,308) ) ) )
		  {
		    arrayOfNumbers[i-1] = ceil ( arrayOfNumbers[i-1] );
		  }
		  else
		  {
		    *error = 'b';
		  }
		}
		else
		{
          *error = 'O';
		}
        break;
		case 'u':
		if ( ( i - 1 ) >= 0)
		{
		  if ( counterOfNumbers > 0 )
		  {
			arrayOfNumbers[i-1] = (-1) * ( arrayOfNumbers[i-1] );
		  }
		  else
		  {
          *error = 'r';
		  } 
		}
		else
		{
          *error = 'O';
		}
        break;
		case 'L':
		if ( ( i - 2 ) >= 0)
		{
		  if( ( arrayOfNumbers[i-1] > 0 ) && ( arrayOfNumbers[i-1] <  1e+308 ) && ( arrayOfNumbers[i-2] > 0 ) && ( arrayOfNumbers[i-2] != 1 ) && ( arrayOfNumbers[i-2] <  1e+308 ) )
		  {
			arrayOfNumbers[i-2] = ( log ( arrayOfNumbers[i-1] ) / log ( arrayOfNumbers[i-2] ) );
			i--;
		  }
		  else
		  {
		    *error = 'o';
		  }
		}
		else
		{
          *error = 'O';
		}
		break;
	  } 
	}
	if( *error == 'A' )
	{
	  j++;
	}
	else
	{
	  arrayOfNumbers[0] = 0;
	  break;
	}
 } 
 }
 if( *error == 'A' )
 {
   if( i != 1 )
   {
     *error = 'O';
     result = 0;
   }
   else
   {
     result = arrayOfNumbers[0];
   }
 }
 counterOfNumbers = 0;

 return result;
}

int unaryMinus ( char *str, int topOfOutput )
{
	int i = 0, j = 0, unary1 = 0;
    i = topOfOutput;

	if( i == 0 )
	{
       return 1;
	}
	else
	{
	  while ( j < i )
	  {
		if ( ( str[j] == ' ' ) || ( str[j] == '\t' ) )
	    {
		  ;
	    }
		else
		{
		  j = 0;
		  unary1 = 0;
		  break;
		}
	    if ( j + 1 == i )
	    {
		  j = 0;
		  return 1;
	    }
		j++;
	  }
	}
	i--;
	if ( unary1 == 0 )
	{
	  while ( ( str[i] == ' ' ) || ( str[i] == '\t' ) ) 
	  {
	    i--;
	  }
	  if ( IsOperator ( str[i] ) || ( str[i] == '(' ) || ( str[i] == ',') )
	  {
        return 1;
      }
	  else 
	  {
		return 0;
	  }

	}
  return 0;
}

void codesOfErrors ( char error, char *str )
{
	switch ( error )
	{
	case 'f':
    printf("%s == ERROR: an incorrect entry function", str );
    break;
    case 'b':
    printf("%s == ERROR: buffer of double overflowed", str );
    break;
	case 'n':
    printf("%s == ERROR: an incorrect entry number", str );
    break;
	case 'O':
    printf("%s == ERROR: not enough operands", str );
    break;
	case 'y':
    printf("%s == ERROR: incorrect symbol", str );
    break;
	case 'B':
    printf("%s == ERROR: not enough brackets", str );
    break;
	case 'd':
    printf("%s == ERROR: dividing by a zero is impossible", str );
    break;
	case 'o':
    printf("%s == ERROR: the range of allowable values violated", str );
    break;
    case 'r':
    printf("%s == ERROR: a meaningless expression", str );
    break;
	case 'm':
    printf("ERROR: low memory" );
    break;
	}
}


int IsVariableTo( char c, char *variables )
{
  int i = 0, length = strlen( variables );
  int available = -1;

  while( i < length )
  {
    if( variables[i] == c )
	{
	  available = i;
	  break;
	}
   i++;
  }
  
 return available;
}



int parsingOfString(char *str, char *error)
{
 int i = 0;
 int delim = 0, assign = 0, correct = 0; 

 while( str[i] != 0 )
 {
   if( str[i] == '=' )
   {
	 assign++;
   }
   if( str[i] == ';' )
   {
	 delim++;
   }
   i++;
 }
 if( delim != assign )
 {
   correct = -1;
   *error = 'y';
 }
 else
 {
   if( ( delim > 0 ) && ( assign > 0 ) )
   {
     correct = delim;
	 *error = 'G';
   }
   else
   {
     correct = 0;
     *error = 'A';
   }
 }
  return correct;
}

void SymbolicCalculator( char *str, char *output, char *variables, double *arrayOfNumbers, double *numbers, char *error, int delim )
{
  char *tmpSTR = NULL;
  int i = 0, j = 0, k = 0, topOfVariables = 1, topOfNumbers = 0, position = 0;
  int length = strlen( str );
  char **varExpression = NULL;

  varExpression = malloc( ( delim + 2 )*sizeof( char* ) );
  tmpSTR = malloc( 3*length*sizeof( char ) );
  
  if ( varExpression == NULL )
  {
    *error = 'm';
  }
  else
  {
    for( i = 0; i < ( delim + 2 ); i++ )
    {
      varExpression[i] = malloc( 3*length*sizeof(int) );
	  if( varExpression[i] == NULL )
	  {
	    *error = 'm';
	    break;
	  }
    }
  }
  
  if( *error == 'A' )
  {
	splitOfInput( str, varExpression, error, variables, output, numbers, arrayOfNumbers  );
  }
 
  i = 0;
  for( i = 0; i < ( delim + 2 ); i++ )
  {
    free( varExpression[i] );
  }

  free( varExpression );
  free( tmpSTR ); 
}

void splitOfInput( char *input, char **varExpression, char *error, char *variables, char *output, double *numbers, double *arrayOfNumbers )
{
   int i = 0, j = 0, k = 0, topOfNumbers = 0, head = 0;
   int length = ( int )strlen( input );
   
   while( i <= length )
   {
      if( ( input[i] == ';' ) || ( i == length ) )
      {
        while( head <= i )
	    {
          if( ( input[head] != ' ' ) && ( input[head] != '\t' ) )
	      {
		    varExpression[j][k] = ( input[head] == ';' ) ? '\0' : input[head];
		    k++;
	      }
	      head++; 
		}
	    k = 0;
		if( varExpression[j][1] == '=' )
		{
		  InputToPolsk ( varExpression[j]+2, output, error, variables );
		  numbers[topOfNumbers] = ( *error == 'A' ) ? calculation ( output, arrayOfNumbers, error, numbers, variables ) : 0;
		  if( *error == 'A' )
		  {
		   variables[topOfNumbers++] = varExpression[j][0];
		  }
		  else
		  {
		    break;
		  }
		}
		else
		{
		  variables[topOfNumbers] = '\0';
		  InputToPolsk ( varExpression[j], output, error, variables );
		  numbers[topOfNumbers] = ( *error == 'A' ) ? calculation ( output, arrayOfNumbers, error, numbers, variables ) : 0;
		  if( *error != 'A' )
		  {
		    break;
		  }
		
		}
	    j++;	 
	  }
     i++;
   }
}