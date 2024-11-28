#include<stdio.h>
#include"analyzer.h"
#include<string.h>
#include<ctype.h>


int do_analyze(int argc, char *argv[])
{
    FILE *fp=fopen(argv[1],"r");
    if(fp==NULL)
    {
        printf("File not found ");
        return -1;
    }
    char ch;
    char buffer[100];
    int count=0;
    long int var;
   
    while((fscanf(fp, "%c", &ch))!=EOF)
    {
        if(ch=='#')        //checking for header and macros
        {
            while((ch = fgetc(fp))  != '\n');
        }
        else if (ch == '/') // Check for '//' comment
        {
            if ((ch = fgetc(fp)) == '/') // Handle '//' comment
            {
                 while ((ch = fgetc(fp)) != EOF && ch != '\n'); // Skip to the end of the line
            } 
            else  // If not '//' push back the character for further processing
            {
                fseek(fp, -1, SEEK_CUR); 
            }
        }
        else if(isalpha(ch)|| ch == '_' )
        {
            var=ftell(fp);
            //printf("--------------------->%ld\n",var);
            buffer[count++]=ch;
            while((isalnum(ch = fgetc(fp)) || ch == '_'))
            {
                 buffer[count++]=ch;
            }
            var=ftell(fp);
            //printf("--------------------->%ld\n",var);
            fseek(fp, -1, SEEK_CUR);  // Push back the non-operator character
            buffer[count] = '\0';
            var=ftell(fp);
            //printf("--------------------->%ld\n",var);
            if(is_keyword(buffer))
            {
                printf("\033[31mKeyword  : %s\033[0m\n",buffer);
            }
            else
            {
                printf("\033[34mVariable  : %s\033[0m\n",buffer);
            }
            count = 0;
        }
        else if(ch == '\'')
        {
            buffer[count++]=ch;
            ch=fgetc(fp);
            buffer[count++]=ch;
            ch=fgetc(fp);
            buffer[count++]=ch;
            buffer[count]='\0';
            if(ch=='\'')
            {
                printf("Char Constant : %s\n",buffer);
                count=0;
            }
            else
            {
                printf("Error: in Char constants %s\n",buffer);
                break;
            }
        }
          // Handle operators
        else if (is_operator(ch))
        {
            
            buffer[count++]=ch;
              // Read additional operator characters (like ++, +=, etc.)
            if (strchr("+-=<>!", ch))   // Use strchr for cleaner checks
            {
                if(((fscanf(fp, "%c", &ch))!=EOF))
                {
                    if (strchr("+-=", ch))      
                    {
                        buffer[count++] = ch;  // Add to buffer if it's a valid operator
                    } 
                    else 
                    {
                        fseek(fp, -1, SEEK_CUR);  // If not, push back the character
                    }
                }
            }
            else if(ch=='{')   // Check for unmatched braces
            {
                var=ftell(fp);
               // printf("--------------------->%ld\n",var);
                char temp;
                int temp_count=0;
                while((temp=fgetc(fp))!=EOF)    //run loop upto reach } bace
                {
                    temp_count++;
                    if(temp=='}')
                    {
                        break;
                    }      
                }
                if (temp == EOF)
                {
                    printf("Error: Improper operator '}' encountered, missing matching '{'\n");
                    fseek(fp, -temp_count, SEEK_CUR);
                } else {
                    fseek(fp, -temp_count, SEEK_CUR); // Rewind to the position after the last read
                }
                //var=ftell(fp);
               // printf("--------------------->%ld\n",var);
            }
             else if(ch=='(')   // Check for unmatched braces
            {
                var=ftell(fp);
               // printf("--------------------->%ld\n",var);
                char temp;
                int temp_count=0;
                while((temp=fgetc(fp))!=EOF)    //run loop upto reach } bace
                {
                    temp_count++;
                    if(temp==')')
                    {
                        break;
                    }      
                }
                if (temp == EOF)
                {
                    printf("Error: Improper operator ')' encountered, missing matching '()'\n");
                    fseek(fp, -temp_count, SEEK_CUR);
                } else {
                    fseek(fp, -temp_count, SEEK_CUR); // Rewind to the position after the last read
                }
                //var=ftell(fp);
               // printf("--------------------->%ld\n",var);
            }
            
            buffer[count] = '\0';
            printf("\033[33mOperator   :  %s\033[0m\n", buffer);
            count = 0;
           // printf("-%c\n",ch);
        }  
     
        else if(isdigit(ch))
        {
            buffer[count]=ch;
            count++;
            if(ch =='0')        
            {   //printf("-------------->%c\n",ch);
                if((ch=fgetc(fp))=='b')     //cheking for binary number 
                {
                    buffer[count]=ch;
                    count++;
                    while(isdigit(ch=fgetc(fp)))
                    {
                        if((ch== '0' || ch == '1' ))
                        {
                            buffer[count++]=ch;
                        }   
                        else
                        {
                            buffer[count++]=ch;
                            buffer[count]='\0';
                            printf("ERROR : Invalid binary value %s\n",buffer);           
                            return 0;
                        }
                    }
                }
                else if((ch=='X'||ch=='x'))
                {
                    while(isalnum(ch=fgetc(fp)))
                    {
                        if((ch>= 0 &&ch <= 9 )|| (ch>='A' && ch <='F')|| (ch>='a' && ch <='f'))
                        {
                            buffer[count++]=ch;
                        }   
                        else
                        {
                            buffer[count++]=ch;
                            buffer[count]='\0';
                            printf("ERROR : In hexa value %s\n",buffer);           
                            return 0;
                        }
                    }
                }
                else                //checking for octa
                {
                     while(isdigit(ch=fgetc(fp)))
                    {
                            if((ch>= '0' &&ch <= '7' ))
                            {
                            // printf("-------------->%c\n",ch);
                                buffer[count++]=ch;
                            }
                            else if(ch >= '8')
                            {
                                //printf("-------------->%c\n",ch);
                                buffer[count++]=ch;
                                buffer[count]='\0';
                                printf("ERROR : In Octa value %s\n",buffer);     
                                return 0;
                            }
                    }

                }
            }
            else
            {
                while(isdigit(ch=fgetc(fp)))
                {
                        buffer[count++]=ch;
                }
        
                if(isalpha(ch))
                {    buffer[count++]=ch;
                        while((isalnum(ch = fgetc(fp)) || ch == '_'))
                        {
                            buffer[count++]=ch;
                        }
                        buffer[count]='\0';
                        printf("Error: Invalid Variable Declaration %s\n",buffer);
                        return 0;
                }

            }
           
           fseek(fp, -1, SEEK_CUR);       // Push back the non-digit character
            buffer[count]='\0';
            printf("\033[35mConstant  :%4s\033[0m\n",buffer); 
            count=0;
        }      
        else if(ch=='"')  
        {
            
            buffer[count]=ch;
            int temp= count;
            count++;
           while((ch=fgetc(fp))!='"' &&ch!=EOF)
           {
                buffer[count++]=ch;
           }
            buffer[count++]=ch;
            buffer[count]='\0';
            if (ch == EOF) 
            {
                // Handle error: unclosed string
                // Null-terminate the buffer
                printf("\033[31mError: Unclosed string literal %c\033[0m\n",buffer[temp]);
                return 0;
            }
            else
                printf("\033[35mStrings  :%4s\033[0m\n",buffer); 
            count=0;
        }
        else if(ispunct(ch))
        {
            printf("Special Symbols : %c\n",ch);

        }
       
    }
   
    fclose(fp);

}

int is_keyword(char * buffer)
{
    const char *keywords[] ={"auto","break","case","char","const","continue","default","do","double",
                    "else","enum","extern","float","for","goto","if","int","long","register",
                    "return","short","signed","sizeof","static","struct","switch","typedef",
                    "union","unsigned","void","volatile","while",NULL};
    for(int i=0;keywords[i]!=NULL;i++)
    {
        if(strcmp(keywords[i],buffer)==0)
        {
            return 1;
        }
    }
    return 0; // If Not a keyword
}
int is_operator(char ch)
{
    if(  ch == '+' || ch == '-'|| ch == '*' || ch == '/' || ch == ','
            || ch == ';' || ch == '%' || ch == '>' || ch == '<' || ch == '=' || ch == '('
            || ch == ')' || ch == '[' || ch == ']'|| ch == '{' || ch == '}'||ch=='&'||ch=='!')
            return 1;
    return 0;
}