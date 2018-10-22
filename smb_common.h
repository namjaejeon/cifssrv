// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *   Copyright (C) 2018 Samsung Electronics Co., Ltd.
 */

#ifndef __SMB_COMMON_H__
#define __SMB_COMMON_H__

#include <linux/kernel.h>

#include "smb1pdu.h"
#include "smb2pdu.h"

#define CIFSD_SMB1_PROT		0
#define CIFSD_SMB2_PROT		1
#define CIFSD_SMB21_PROT		2
/* multi-protocol negotiate request */
#define CIFSD_SMB2X_PROT		3
#define CIFSD_SMB30_PROT		4
#define CIFSD_SMB302_PROT		5
#define CIFSD_SMB311_PROT		6
#define CIFSD_BAD_PROT		0xFFFF

#define CIFSD_SMB1_VERSION_STRING	"1.0"
#define CIFSD_SMB20_VERSION_STRING	"2.0"
#define CIFSD_SMB21_VERSION_STRING	"2.1"
#define CIFSD_SMB30_VERSION_STRING	"3.0"
#define CIFSD_SMB302_VERSION_STRING	"3.02"
#define CIFSD_SMB311_VERSION_STRING	"3.1.1"

/* Dialects */
#define CIFSD_SMB10_PROT_ID		0x00
#define CIFSD_SMB20_PROT_ID		0x0202
#define CIFSD_SMB21_PROT_ID		0x0210
/* multi-protocol negotiate request */
#define CIFSD_SMB2X_PROT_ID		0x02FF
#define CIFSD_SMB30_PROT_ID		0x0300
#define CIFSD_SMB302_PROT_ID		0x0302
#define CIFSD_SMB311_PROT_ID		0x0311
#define CIFSD_BAD_PROT_ID		0xFFFF

#define IS_SMB2(x) ((x)->vals->protocol_id != CIFSD_SMB10_PROT_ID)

struct cifsd_work;
struct cifsd_tcp_conn;
struct cifsd_tcp_conn;

int cifsd_min_protocol(void);
int cifsd_max_protocol(void);

int get_protocol_idx(char *str);

int check_message(struct cifsd_work *work);
bool is_smb_request(struct cifsd_tcp_conn *conn);

int find_matching_smb1_dialect(int start_index,
				char *cli_dialects,
				__le16 byte_count);
int find_matching_smb2_dialect(int start_index,
				__le16 *cli_dialects,
				__le16 dialects_count);

int negotiate_dialect(void *buf);

void init_smb2_0_server(struct cifsd_tcp_conn *server);
void init_smb2_1_server(struct cifsd_tcp_conn *server);
void init_smb3_0_server(struct cifsd_tcp_conn *server);
void init_smb3_02_server(struct cifsd_tcp_conn *server);
int init_smb3_11_server(struct cifsd_tcp_conn *server);
int is_smb2_neg_cmd(struct cifsd_work *work);
bool is_chained_smb2_message(struct cifsd_work *work);
void init_smb2_neg_rsp(struct cifsd_work *work);
int is_smb2_rsp(struct cifsd_work *work);

#endif /* __SMB_COMMON_H__ */