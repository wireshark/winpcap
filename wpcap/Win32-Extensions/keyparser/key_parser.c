/*
 * Copyright (c) 1999 - 2003
 * NetGroup, Politecnico di Torino (Italy)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Politecnico di Torino nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <pcap.h>
#include "key_parser.h"
#include "key_formatter.h"
#define MAGIC_MON_CONFIG 0x26031977

static uint32 legend_by_table(bpf_keyparser *kp);
static uint32 legend_by_formatter(bpf_keyparser *kp);
static uint32 descriptions_by_formatter(bpf_keyparser *kp);

typedef struct 
{
	char filter[100];
	char filter_desc[128];
	struct bpf_program program;
}
	tf;

int init_legend(legend_type *legend,uint32 length)				
{
	legend_type tmp;

	tmp.elements=(legend_element*)malloc(sizeof(legend_element)*length);

	if (tmp.elements==NULL)
		return -1;

	tmp.total_length=length;
	tmp.used_length=0;
	*legend=tmp;
	return 1;
}
void destroy_legend(legend_type *legend)
{
	if (legend==NULL)
		return;

	if (legend->elements!=NULL)
		free(legend->elements);
	
	legend->elements=NULL;
	legend->total_length=0;
	legend->used_length=0;
	return;
}
int init_ft_legend(ft_legend_type *ft_legend, uint32 length)
{
	ft_legend_type tmp;

	tmp.elements=(ft_element*)malloc(sizeof(ft_element)*length);
	
	if (tmp.elements==NULL)
		return -1;
	*ft_legend=tmp;
	ft_legend->length=length;
	return 1;

}

void destroy_ft_legend(ft_legend_type *ft_legend)
{
	if (ft_legend==NULL)
		return;

	if (ft_legend->elements!=NULL)
		free(ft_legend->elements);
	
	ft_legend->elements=NULL;
	ft_legend->length=0;
	return;

}

static uint32 legend_by_table(bpf_keyparser *kp)
{
	uint32 i,j;

	for (i=0;i<kp->ft_legend.length;i++)
	{
		for(j=0;(j<kp->key_len)&&(kp->curr_pointer[j]==kp->ft_legend.elements[i].key[j]);j++);
		
		if (j==kp->key_len)
		{
			strncpy(kp->legend.elements[0].description,kp->ft_legend.elements[i].description,MAX_DESCRIPTION_LENGTH);
			strncpy(kp->legend.elements[0].value,kp->ft_legend.elements[i].value,MAX_VALUE_LENGTH);
			kp->legend.used_length=1;
			return 1;
		}
	}
	kp->legend.elements[0].value[0]='\0';
	kp->legend.elements[0].description[0]='\0';
	kp->legend.total_length=1;
	return 0;
}

static uint32 legend_by_formatter(bpf_keyparser *kp)
{
	uint8 *key=kp->curr_pointer;
	uint32 key_len=kp->key_len;
	uint32 tmp,i,length;

	length=(kp->ft_legend.length<kp->legend.total_length)?kp->ft_legend.length:kp->legend.total_length;

	for (i=0;i<length;i++)
	{
		strncpy(kp->legend.elements[i].description,kp->ft_legend.elements[i].description,MAX_DESCRIPTION_LENGTH);
		kp->legend.elements[i].value[0]='\0';
		tmp=key_formatter(kp->ft_legend.elements[i].format,key,key_len,kp->legend.elements[i].value,MAX_VALUE_LENGTH);
		key_len-=tmp;
		key+=tmp;
	}
	kp->legend.used_length=length;
	return 1;
}

int pcap_keyparser_init(bpf_keyparser *kp, const struct pcap_pkthdr *header, const u_char *data)
{
	uint32 blocks=header->caplen/kp->block_size;

	if(blocks==0)
		return 1;

	kp->curr_pointer=(uint8*) data;
	kp->last_valid_pointer=(uint8*)data+(blocks-1)*kp->block_size;

	return 1;
}
void *pcap_keyparser(bpf_keyparser *kp, legend_type *legend)
{
	int8 *tmp;	

	if (kp->curr_pointer==NULL)
		return NULL;
	tmp=kp->curr_pointer;

	if (kp->type==LEGEND_FORMATTER)
		legend_by_formatter(kp);
	else
		legend_by_table(kp);

	*legend=kp->legend;

	if(tmp!=kp->last_valid_pointer)
		kp->curr_pointer+=kp->block_size;
	else
		kp->curr_pointer=NULL;
	
	return (tmp+kp->key_len);
}

uint32 crc_exor(void *buffer, uint32 length)
{
	uint32 *tmp=(uint32*)buffer;
	uint32 mis=0;
	uint32 crc=0;
	uint32 i=length/4;
	uint32 j;


	for(j=0;j<i;j++)
		crc^=tmp[j];

	switch (length%4)
	{
	case 0:
		return crc;

	case 1:
		((uint8*)&mis)[0]=((uint8*)buffer)[length-1];
		crc^=mis;
		return crc;

	case 2:
		((uint8*)&mis)[0]=((uint8*)buffer)[length-2];
		((uint8*)&mis)[1]=((uint8*)buffer)[length-1];
		crc^=mis;
		return crc;

	case 3:
		((uint8*)&mis)[0]=((uint8*)buffer)[length-3];
		((uint8*)&mis)[1]=((uint8*)buffer)[length-2];
		((uint8*)&mis)[2]=((uint8*)buffer)[length-1];
		crc^=mis;
		return crc;
	}

	return 0;

}
int32 save_monitor_config(char *filename, bpf_keyparser *kp, struct bpf_program *prog)
{
	FILE *fp;
	uint32 magic= MAGIC_MON_CONFIG;
	uint32 crc;
	int32 bytes_written;
	bpf_keyparser kp_temp=*kp;
	kp_temp.curr_pointer=0;
	kp_temp.last_valid_pointer=0;
	kp_temp.ft_legend.elements=0;
	kp_temp.legend.elements=0;

	fp=fopen(filename,"wb");

	if (fp==NULL)
		return -1;

	if (fwrite((void*)&magic,4,1,fp)<1)
	{
		fclose(fp);
		return -2;
	}
		
	if (fwrite((void*)&prog->bf_len,4,1,fp)<1)
	{
		fclose(fp);
		return -3;
	}
		
	if (fwrite((void*)prog->bf_insns,sizeof(struct bpf_insn),prog->bf_len,fp)<prog->bf_len)
	{
		fclose(fp);
		return -4;
	}

	crc=crc_exor((void*)prog->bf_insns,sizeof(struct bpf_insn)*prog->bf_len);
	
	if (fwrite((void*)&crc,4,1,fp)<1)
	{
		fclose(fp);
		return -5;
	}

	if (fwrite((void*)&kp_temp,sizeof(bpf_keyparser),1,fp)<1)
	{
		fclose(fp);
		return -6;
	}

	if (fwrite((void*)kp->ft_legend.elements,sizeof(ft_element),kp->ft_legend.length,fp)<kp->ft_legend.length)
	{
		fclose(fp);
		return -7;
	}

	crc=crc_exor((void*)kp->ft_legend.elements,sizeof(ft_element)*kp->ft_legend.length);

	if (fwrite((void*)&crc,4,1,fp)<1)
	{
		fclose(fp);
		return -8;
	}
		
	bytes_written=ftell(fp);
		
	fclose(fp);
		
	return bytes_written;
}

int32 load_monitor_config(char *filename, bpf_keyparser *kp, struct bpf_program *prog)
{
	FILE *fp;
	uint32 magic;
	uint32 crc_read,crc_calc;

	if ((kp==NULL) || (prog==NULL))
		return -1;

	fp=fopen(filename,"rb");

	if (fp==NULL)
		return -2;

	if (fread((void*)&magic,4,1,fp)<1)
	{
		fclose(fp);
		return -3;
	}

	if (magic!=MAGIC_MON_CONFIG)
	{
		fclose(fp);
		return -4;
	}

	if (fread((void*)&prog->bf_len,4,1,fp)<1)
	{
		fclose(fp);
		return -5;
	}
	
	prog->bf_insns=(struct bpf_insn*)malloc(sizeof(struct bpf_insn)*prog->bf_len);

	if (prog->bf_insns==NULL)
	{
		fclose(fp);
		return -6;
	}

	if (fread((void*)prog->bf_insns,sizeof(struct bpf_insn),prog->bf_len,fp)<prog->bf_len)
	{
		free(prog->bf_insns);
		fclose(fp);
		return -7;
	}

	if (fread((void*)&crc_read,4,1,fp)<1)
	{
		free(prog->bf_insns);
		fclose(fp);
		return -8;
	}

	crc_calc=crc_exor((void*)prog->bf_insns,prog->bf_len*sizeof(struct bpf_insn));

	if(crc_calc!=crc_read)
	{
		free(prog->bf_insns);
		fclose(fp);
		return -9;
	}

	if (fread((void*)kp,sizeof(bpf_keyparser),1,fp)<1)
	{
		free(prog->bf_insns);
		fclose(fp);
		return -10;
	}

	kp->ft_legend.elements=(ft_element*)malloc(sizeof(ft_element)*kp->ft_legend.length);

	if (kp->ft_legend.elements==NULL)
	{
		free(prog->bf_insns);
		fclose(fp);
		return -11;
	}

	if (fread((void*)kp->ft_legend.elements,sizeof(ft_element),kp->ft_legend.length,fp)<kp->ft_legend.length)
	{
		free(prog->bf_insns);
		free(kp->ft_legend.elements);
		fclose(fp);
		return -12;
	}

	if (fread((void*)&crc_read,4,1,fp)<1)
	{
		free(prog->bf_insns);
		free(kp->ft_legend.elements);
		fclose(fp);
		return -13;
	}
	
	fclose(fp);

	crc_calc=crc_exor((void*)kp->ft_legend.elements,sizeof(ft_element)*kp->ft_legend.length);

	if (crc_calc!=crc_read)
	{
		free(prog->bf_insns);
		free(kp->ft_legend.elements);
		return -14;
	}

	kp->legend.elements=(legend_element*)malloc(sizeof(legend_element)*kp->legend.total_length);

	if (kp->legend.elements==NULL)
	{
		free(prog->bf_insns);
		free(kp->ft_legend.elements);
		return -15;
	}

	return 1;

}

void pcap_destroy_keyparser(bpf_keyparser *kp)
{
	if(kp==NULL)
		return;
	
	destroy_legend(&kp->legend);
	destroy_ft_legend(&kp->ft_legend);
}

int pcap_keyparser_descriptions(bpf_keyparser *kp, legend_type *legend)
{
	if (kp->type==LEGEND_FORMATTER)
		descriptions_by_formatter(kp);
	else
		return -1;

	*legend=kp->legend;

	return 0;
}

static uint32 descriptions_by_formatter(bpf_keyparser *kp)
{
	uint32 i,length;

	length=(kp->ft_legend.length<kp->legend.total_length)?kp->ft_legend.length:kp->legend.total_length;

	for (i=0;i<length;i++)
		strncpy(kp->legend.elements[i].description,kp->ft_legend.elements[i].description,MAX_DESCRIPTION_LENGTH);

	kp->legend.used_length=length;
	return 1;
}

int decode_cond(const char* origin, char *filter, char *desc)
{
	char *first,*second;

	first=strchr(origin,'\"');

	if (first==NULL)
	{
		strcpy(filter,origin);
		strcpy(desc,origin);
		return 0;
	}

	second=strchr(first+1,'\"');

	if (second==NULL)
	{
		strncpy(filter,origin,first-origin);
		filter[first-origin]='\0';
		strcpy(desc,filter);
		return -1;
	}

	strncpy(filter,origin,first-origin);
	filter[first-origin]='\0';

	strncpy(desc,first+1,second-first-1);
	desc[second-first-1]='\0';
	
	return 0;

}

int
pcap_compile_multiple(pcap_t *p, struct bpf_program *program,
	     char *buf_ori, int optimize, bpf_u_int32 mask, bpf_keyparser *kp )
{
	tf s_filters[64];
	tf pre_filter;
	int flag=0;
	
	struct bpf_insn JUMP={BPF_JMP|BPF_JA,0,0,0};

	struct bpf_insn LOOKUP={BPF_MISC|BPF_TME|BPF_LOOKUP,0,0,0};

	struct bpf_insn EXECUTE={BPF_MISC|BPF_TME|BPF_EXECUTE,0,0,0};

	struct bpf_insn LDA={BPF_LD|BPF_IMM,0,0,0};

	struct bpf_insn STA={BPF_ST|BPF_MEM_EX_IMM,0,0,0};
	struct bpf_insn RET={BPF_RET|BPF_IMM,0,0,0};
	struct bpf_insn init[]=
	{		
		BPF_SEPARATION,					0,	0,	0,		

		BPF_MISC|BPF_TME|BPF_SET_MEMORY, 0,0,	1500000, // allocation of some memory
		BPF_MISC|BPF_TME|BPF_INIT,		0,	0,	0,  //initialization of block #0
													//with default values
		BPF_LD|BPF_IMM,					0,	0,	1,  //key length is 12 bytes
		BPF_MISC|BPF_TME|BPF_SET_REGISTER_VALUE,	0,	0,	TME_KEY_LEN,

		BPF_LD|BPF_IMM,					0,	0,32,  //key length is 12 bytes
		BPF_MISC|BPF_TME|BPF_SET_REGISTER_VALUE,	0,	0,	TME_BLOCK_SIZE,

		
		BPF_LD|BPF_IMM,					0,	0,	0,
		BPF_MISC|BPF_TME|BPF_VALIDATE,	0,	0,	64	//validation/activation of block #0
													//starting at byte 64
	};
	
	char *switch_ptr,*other,*tmpPtr;
	unsigned int i,total_filters,total_insns,other_len;

	char *buf;

	int j;

	int ended =FALSE;

	buf=(char*)malloc(sizeof(char)*(strlen(buf_ori)+1));

	if (buf==NULL) 
		return -1;

	strcpy(buf,buf_ori);

	_strlwr(buf);

	switch_ptr=strstr(buf,"switch");

	if (switch_ptr==NULL)
	{
		free(buf);
		return -1;
	}

	strcpy(buf,buf_ori);
	
	strncpy(pre_filter.filter,buf,switch_ptr-buf);
	pre_filter.filter[switch_ptr-buf]='\0';
	other=switch_ptr+strlen("switch");

	other_len=strlen(other);

	for(i=0;i<other_len;i++)
		if(other[i]=='(')
			break;

	if(i==other_len)
	{
		free(buf);
		return -1;
	}

	other+=i+1;

	other_len=strlen(other);

	for(i=other_len-1;i>=0;i--)
		if(other[i]==')')
			break;

	if (i<0)
	{
		free(buf);
		return -1;
	}

	other[i]='\0';

	if (strlen(other)==0)
	{
		free(buf);
		return -1;
	}

	i=0;

	while(!ended)
	{
		tmpPtr=strchr(other,';');
		if (tmpPtr==NULL)
		{
			ended=TRUE;
			if (decode_cond(other,s_filters[i].filter,s_filters[i].filter_desc)<0)
			{
				free(buf);
				return -1;
			}
			i++;
		}
		else
		{
			*tmpPtr='\0';
			if (decode_cond(other,s_filters[i].filter,s_filters[i].filter_desc)<0)
			{
				free(buf);
				return -1;
			}
			i++;
			other=tmpPtr+1;
		}

	}

	total_filters=i;
	
	if (init_ft_legend(&kp->ft_legend, total_filters)<0)
	{
		free(buf);
		return -1;
	}

	kp->type=LEGEND_TABLE;
	
	if (init_legend(&kp->legend,1)<0)
	{
		destroy_ft_legend(&kp->ft_legend);
		free(buf);
		return -1;	
	}
	kp->key_len=4;
	kp->block_size=32;
	
	total_insns=0;

	for(j=0;j<(int)total_filters;j++)
	{
		char tmp1[210]="";
	
		if (flag==1)
			_snprintf(tmp1,210,"%s and %s",pre_filter.filter,s_filters[j].filter);
		else
			_snprintf(tmp1,210,"%s",s_filters[j].filter);

	
		if (pcap_compile(p,&s_filters[j].program,tmp1,optimize,mask)<0)
		{
			destroy_ft_legend(&kp->ft_legend);
			destroy_legend(&kp->legend);

			j--;
			while(j>=0)
			{
				pcap_freecode(&(s_filters[j].program));
				j--;
			}
			free(buf);
			return -1;
		}
		
		strncpy(kp->ft_legend.elements[j].description,s_filters[j].filter_desc,MAX_DESCRIPTION_LENGTH);
		
		kp->ft_legend.elements[j].value[0]='\0';
		
		kp->ft_legend.elements[j].key[0]=((j)>>24);
		kp->ft_legend.elements[j].key[1]=((j)>>16)%256;
		kp->ft_legend.elements[j].key[2]=((j)>>8)%256;
		kp->ft_legend.elements[j].key[3]=(j)%256;
		
		total_insns+=(s_filters[j].program.bf_len+4);
	}

	total_insns++;
	total_insns+=sizeof(init)/sizeof(struct bpf_insn);

	total_insns+=total_filters*3;

	total_insns++;
	
	i=0;
	
	program->bf_insns=(struct bpf_insn*)malloc(sizeof(struct bpf_insn)*total_insns);
	
	if (program->bf_insns==NULL)
	{
		destroy_ft_legend(&kp->ft_legend);
		destroy_legend(&kp->legend);
		
		for(i=0;i<total_filters;i++)
			pcap_freecode(&(s_filters[i].program));
		free(buf);
		return -1;
	}

	program->bf_len=0;
	
	for (i=0;i<total_filters;i++)
	{
		for (j=0;j<(signed)s_filters[i].program.bf_len;j++)
			if ((s_filters[i].program.bf_insns[j].code&BPF_RET)==BPF_RET)
			{
				if (s_filters[i].program.bf_insns[j].k>0)
					JUMP.k=s_filters[i].program.bf_len-j-1;
				else
					JUMP.k=s_filters[i].program.bf_len-j+3;

				s_filters[i].program.bf_insns[j]=JUMP;
			}
			
		memcpy(&program->bf_insns[program->bf_len],
			s_filters[i].program.bf_insns,
			sizeof(struct bpf_insn)*s_filters[i].program.bf_len);
		program->bf_len+=s_filters[i].program.bf_len;

		LDA.k=i;
		program->bf_insns[program->bf_len]=LDA;
		program->bf_insns[program->bf_len+1]=STA;
		program->bf_insns[program->bf_len+2]=LOOKUP;
		program->bf_insns[program->bf_len+3]=EXECUTE;
		
		program->bf_len+=4;
	}

	program->bf_insns[program->bf_len]=RET;
	program->bf_len++;

	memcpy(&program->bf_insns[program->bf_len],&init,sizeof(init));
	
	program->bf_len+=sizeof(init)/sizeof(struct bpf_insn);
	
	for (i=0;i<total_filters;i++)
	{
		program->bf_insns[program->bf_len]=LDA;
		program->bf_insns[program->bf_len].k=i;
		program->bf_len++;
		program->bf_insns[program->bf_len]=STA;
		program->bf_len++;
		program->bf_insns[program->bf_len]=LOOKUP;
		program->bf_len++;
	}
	
	RET.k=1;
	program->bf_insns[program->bf_len]=RET;
	program->bf_len++;

	for(i=0;i<total_filters;i++)
		pcap_freecode(&(s_filters[i].program));
	free(buf);
	return 0;
}

