%{

#ifdef WIN32
#define snprintf _snprintf
#endif

#include <pcap.h>
#include "tme.h"

#ifdef WIN32
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include <key_formatter.h>

class MyFormatter: public yyFlexLexer
{
public:
	MyFormatter(char *key,int key_len,char *output,int output_len)
	{
		my_key=key;
		my_key_len=key_len;
		my_output=output;
		my_output_len=output_len-1;
		output[output_len-1]='\0';
		my_key=key;
	};
	int GetMyKeyLen(){return my_key_len;};

private:
	char *my_key;
	int my_key_len;
	char *my_output;
	int my_output_len;
	int yylex();
};



static uint64 s_to_uint64(int8 *src)
{
	int8 tmp2[8];
	
	tmp2[0]=src[7];
	tmp2[1]=src[6];
	tmp2[2]=src[5];
	tmp2[3]=src[4];
	tmp2[4]=src[3];
	tmp2[5]=src[2];
	tmp2[6]=src[1];
	tmp2[7]=src[0];

	return *(uint64*)tmp2;
}

static int64 s_to_int64(int8 *src)
{
	int8 tmp2[8];
	
	tmp2[0]=src[7];
	tmp2[1]=src[6];
	tmp2[2]=src[5];
	tmp2[3]=src[4];
	tmp2[4]=src[3];
	tmp2[5]=src[2];
	tmp2[6]=src[1];
	tmp2[7]=src[0];

	return *(int64*)tmp2;
}

static uint32 s_to_uint32(int8 *src)
{
	int8 tmp2[4];
	
	tmp2[0]=src[3];
	tmp2[1]=src[2];
	tmp2[2]=src[1];
	tmp2[3]=src[0];

	return *(uint32*)tmp2;
}

static int32 s_to_int32(int8 *src)
{
	int8 tmp2[4];
	
	tmp2[0]=src[3];
	tmp2[1]=src[2];
	tmp2[2]=src[1];
	tmp2[3]=src[0];

	return *(int32*)tmp2;
}

static uint16 s_to_uint16(int8 *src)
{
	int8 tmp2[2];
	
	tmp2[0]=src[1];
	tmp2[1]=src[0];

	return *(uint16*)tmp2;
}

static int16 s_to_int16(int8 *src)
{
	int8 tmp2[2];
	
	tmp2[0]=src[1];
	tmp2[1]=src[0];

	return *(int16*)tmp2;
}

%}

%option outfile="key_formatter.cpp"
%option c++
%option yyclass="MyFormatter"
%option prefix="kf"
%%

"%bu"	{	
			int __read;
			if (my_key_len<1)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%u",*(unsigned char*)my_key); 
				my_key+=1;
				my_key_len-=1; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

"%bd"	{	
			int __read;
			if (my_key_len<1)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%d",*(char*)my_key); 
				my_key+=1;
				my_key_len-=1; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

"%bx"	{	
			int __read;
			if (my_key_len<1)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%x",*(unsigned char*)my_key); 
				my_key+=1;
				my_key_len-=1; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

"%2bx"	{	
			int __read;
			if (my_key_len<1)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%.2x",*(unsigned char*)my_key); 
				my_key+=1;
				my_key_len-=1; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

"%hu"	{	
			int __read;
			if (my_key_len<2)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%u",s_to_uint16(my_key)); 
				my_key+=2;
				my_key_len-=2; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

"%hd"	{	
			int __read;
			if (my_key_len<2)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%d",s_to_int16(my_key)); 
				my_key+=2;
				my_key_len-=2; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

"%hx"	{	
			int __read;
			if (my_key_len<2)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%x",s_to_uint16(my_key)); 
				my_key+=2;
				my_key_len-=2; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

"%4hx"	{	
			int __read;
			if (my_key_len<2)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%.4x",s_to_uint16(my_key)); 
				my_key+=2;
				my_key_len-=2; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

"%lu"	{	
			int __read;
			if (my_key_len<4)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%u",s_to_uint32(my_key)); 
				my_key+=4;
				my_key_len-=4; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

"%ld"	{	
			int __read;
			if (my_key_len<4)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%d",s_to_int32(my_key)); 
				my_key+=4;
				my_key_len-=4; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

"%lx"	{	
			int __read;
			if (my_key_len<4)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%x",s_to_uint32(my_key)); 
				my_key+=4;
				my_key_len-=4; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

"%8lx"	{	
			int __read;
			if (my_key_len<4)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%.8x",s_to_uint32(my_key)); 
				my_key+=4;
				my_key_len-=4; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

"%qu"	{	
			int __read;
			if (my_key_len<8)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%qu",s_to_uint64(my_key)); 
				my_key+=8;
				my_key_len-=8; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

"%qd"	{	
			int __read;
			if (my_key_len<8)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%qd",s_to_int64(my_key)); 
				my_key+=8;
				my_key_len-=8; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

"%qx"	{	
			int __read;
			if (my_key_len<8)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%qx",s_to_uint64(my_key)); 
				my_key+=8;
				my_key_len-=8; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

"%16qx"	{	
			int __read;
			if (my_key_len<8)
				return -1;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%.16qx",s_to_uint64(my_key)); 
				my_key+=8;
				my_key_len-=8; 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}
		
\n		{ 
			int __read;
			if (my_output_len!=0)
			{	
				__read=snprintf(my_output,my_output_len,"\n"); 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

.		{ 
			int __read;
			if (my_output_len!=0)
			{
				__read=snprintf(my_output,my_output_len,"%c",yytext[0]); 
				if (__read<0)
					__read=my_output_len;
				my_output+=__read;
				my_output_len-=__read;
			}
		}

%%
int key_formatter(char *format, char *key, int key_len, char *output, int output_len)
{
	MyFormatter *lexer;
	int rv;
	istrstream my_stream(format);
	lexer=new MyFormatter(key,key_len,output,output_len);

	lexer->switch_streams(&my_stream,0);

	
	((FlexLexer*)lexer)->yylex();
	rv=key_len-lexer->GetMyKeyLen();
	delete lexer;
	return rv;
}

int yywrap()
{
	return 1;
}

