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

#ifndef __key_parser
#define __key_parser

#include <pcap.h>
#include "tme.h"

#define MAX_DESCRIPTION_LENGTH	128
#define MAX_VALUE_LENGTH	128
#define MAX_KEY_LENGTH		128
#define MAX_FORMAT_LENGTH	128
#define LEGEND_FORMATTER	0
#define	LEGEND_TABLE		1

typedef struct 
{
	int8 description[MAX_DESCRIPTION_LENGTH];
	int8 value[MAX_VALUE_LENGTH];
} legend_element;

typedef struct 
{
	int8 description[MAX_DESCRIPTION_LENGTH];
	int8 key[MAX_KEY_LENGTH];
	int8 format[MAX_FORMAT_LENGTH];
	int8 value[MAX_VALUE_LENGTH];
} ft_element;

typedef struct 
{
	legend_element *elements;
	uint32	total_length;
	uint32	used_length;
} legend_type;

typedef struct
{
	ft_element *elements;
	uint32 length;
} ft_legend_type;

typedef struct
{
	uint32 type;
	ft_legend_type ft_legend;
	
	legend_type legend;
	
	
	uint32 key_len;
	uint32 block_size;

	int8 *curr_pointer;
	int8 *last_valid_pointer;
} bpf_keyparser;

#ifdef __cplusplus
extern "C" {
#endif

int pcap_keyparser_init(bpf_keyparser *, const struct pcap_pkthdr *, const u_char *);
void *pcap_keyparser(bpf_keyparser *, legend_type *);
int32 load_monitor_config(char *filename, bpf_keyparser *kp, struct bpf_program *prog);
int32 save_monitor_config(char *filename, bpf_keyparser *kp, struct bpf_program *prog);
int init_legend(legend_type *legend,uint32 length);				
void destroy_legend(legend_type *legend);
int init_ft_legend(ft_legend_type *ft_legend, uint32 length);
void destroy_ft_legend(ft_legend_type *ft_legend);
int pcap_compile_multiple(pcap_t *,struct bpf_program*,char*,int,bpf_u_int32,bpf_keyparser*);
void pcap_destroy_keyparser(bpf_keyparser *kp);
int pcap_keyparser_descriptions(bpf_keyparser *, legend_type *);

#ifdef __cplusplus
}
#endif

#endif