/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2018 */
/* See the file NOTICE for conditions of use and distribution. */

#define DELIVER_BUFFER_SIZE 4096

#define PENDING          256
#define PENDING_DEFER   (PENDING + DEFER)
#define PENDING_OK      (PENDING + OK)


/* Private structure for the private options and other private data. */

typedef struct {
  uschar *hosts;
  uschar *fallback_hosts;
  host_item *hostlist;
  host_item *fallback_hostlist;
  uschar *authenticated_sender;
  uschar *helo_data;
  uschar *interface;
  uschar *port;
  uschar *protocol;
  uschar *dscp;
  uschar *serialize_hosts;
  uschar *hosts_try_auth;
  uschar *hosts_require_auth;
  uschar *hosts_try_chunking;
#ifdef EXPERIMENTAL_DANE
  uschar *hosts_try_dane;
  uschar *hosts_require_dane;
#endif
  uschar *hosts_try_fastopen;
#ifndef DISABLE_PRDR
  uschar *hosts_try_prdr;
#endif
#ifndef DISABLE_OCSP
  uschar *hosts_request_ocsp;
  uschar *hosts_require_ocsp;
#endif
  uschar *hosts_require_tls;
  uschar *hosts_avoid_tls;
  uschar *hosts_verify_avoid_tls;
  uschar *hosts_avoid_pipelining;
  uschar *hosts_avoid_esmtp;
#ifdef SUPPORT_TLS
  uschar *hosts_nopass_tls;
  uschar *hosts_noproxy_tls;
#endif
  int     command_timeout;
  int     connect_timeout;
  int     data_timeout;
  int     final_timeout;
  int     size_addition;
  int     hosts_max_try;
  int     hosts_max_try_hardlimit;
  BOOL    address_retry_include_sender;
  BOOL    allow_localhost;
  BOOL    authenticated_sender_force;
  BOOL    gethostbyname;
  BOOL    dns_qualify_single;
  BOOL    dns_search_parents;
  dnssec_domains dnssec;
  BOOL    delay_after_cutoff;
  BOOL    hosts_override;
  BOOL    hosts_randomize;
  BOOL    keepalive;
  BOOL    lmtp_ignore_quota;
  uschar *expand_retry_include_ip_address;
  BOOL    retry_include_ip_address;
#ifdef SUPPORT_SOCKS
  uschar *socks_proxy;
#endif
#ifdef SUPPORT_TLS
  uschar *tls_certificate;
  uschar *tls_crl;
  uschar *tls_privatekey;
  uschar *tls_require_ciphers;
  uschar *tls_sni;
  uschar *tls_verify_certificates;
  int     tls_dh_min_bits;
  BOOL    tls_tempfail_tryclear;
  uschar *tls_verify_hosts;
  uschar *tls_try_verify_hosts;
  uschar *tls_verify_cert_hostnames;
#endif
#ifndef DISABLE_DKIM
  struct ob_dkim dkim;
#endif
} smtp_transport_options_block;

/* smtp connect context */
typedef struct {
  uschar *		from_addr;
  address_item *	addrlist;
  host_item *		host;
  int			host_af;
  int			port;
  uschar *		interface;

  BOOL verify:1;
  BOOL lmtp:1;
  BOOL smtps:1;
  BOOL ok:1;
  BOOL setting_up:1;
  BOOL esmtp:1;
  BOOL esmtp_sent:1;
#ifndef DISABLE_PRDR
  BOOL prdr_active:1;
#endif
#ifdef SUPPORT_I18N
  BOOL utf8_needed:1;
#endif
  BOOL dsn_all_lasthop:1;
#if defined(SUPPORT_TLS) && defined(EXPERIMENTAL_DANE)
  BOOL dane:1;
  BOOL dane_required:1;
#endif
  BOOL pending_MAIL:1;
  BOOL pending_BDAT:1;
  BOOL good_RCPT:1;
  BOOL completed_addr:1;
  BOOL send_rset:1;
  BOOL send_quit:1;

  int		max_rcpt;
  int		cmd_count;

  uschar	peer_offered;
  uschar	avoid_option;
  uschar *	igquotstr;
  uschar *	helo_data;
#ifdef EXPERIMENTAL_DSN_INFO
  uschar *	smtp_greeting;
  uschar *	helo_response;
#endif

  address_item *	first_addr;
  address_item *	next_addr;
  address_item *	sync_addr;

  smtp_inblock  inblock;
  smtp_outblock outblock;
  uschar	buffer[DELIVER_BUFFER_SIZE];
  uschar	inbuffer[4096];
  uschar	outbuffer[4096];

  transport_instance *			tblock;
  smtp_transport_options_block *	ob;
} smtp_context;

extern int smtp_setup_conn(smtp_context *, BOOL);
extern int smtp_write_mail_and_rcpt_cmds(smtp_context *, int *);


/* Data for reading the private options. */

extern optionlist smtp_transport_options[];
extern int smtp_transport_options_count;

/* Block containing default values. */

extern smtp_transport_options_block smtp_transport_option_defaults;

/* The main, init, and closedown entry points for the transport */

extern BOOL smtp_transport_entry(transport_instance *, address_item *);
extern void smtp_transport_init(transport_instance *);
extern void smtp_transport_closedown(transport_instance *);



extern int     smtp_auth(uschar *, unsigned, address_item *, host_item *,
		 smtp_transport_options_block *, BOOL,
		 smtp_inblock *, smtp_outblock *);
extern BOOL    smtp_mail_auth_str(uschar *, unsigned,
		 address_item *, smtp_transport_options_block *);

#ifdef SUPPORT_SOCKS
extern int     socks_sock_connect(host_item *, int, int, uschar *,
	         transport_instance *, int);
#endif

/* End of transports/smtp.h */
