/* $Cambridge: exim/src/src/pdkim/pdkim.h,v 1.1.2.4 2009/02/27 17:04:20 tom Exp $ */
/* pdkim.h */

#include "sha1.h"
#include "sha2.h"
#include "rsa.h"
#include "base64.h"

#define PDKIM_SIGNATURE_VERSION "1"
#define PDKIM_MAX_BODY_LINE_LEN 1024
#define PDKIM_DEBUG
#define PDKIM_DEFAULT_SIGN_HEADERS "From:Sender:Reply-To:Subject:Date:"\
                             "Message-ID:To:Cc:MIME-Version:Content-Type:"\
                             "Content-Transfer-Encoding:Content-ID:"\
                             "Content-Description:Resent-Date:Resent-From:"\
                             "Resent-Sender:Resent-To:Resent-Cc:"\
                             "Resent-Message-ID:In-Reply-To:References:"\
                             "List-Id:List-Help:List-Unsubscribe:"\
                             "List-Subscribe:List-Post:List-Owner:List-Archive"


/* Function success / error codes */
#define PDKIM_OK              0
#define PDKIM_FAIL            -1
#define PDKIM_ERR_OOM         -100
#define PDKIM_ERR_RSA_PRIVKEY -101
#define PDKIM_ERR_RSA_SIGNING -102
#define PDKIM_ERR_LONG_LINE   -103

/* Main verification status */
#define PDKIM_VERIFY_NONE      0
#define PDKIM_VERIFY_INVALID   1
#define PDKIM_VERIFY_FAIL      2
#define PDKIM_VERIFY_PASS      3

/* Extended verification status */
#define PDKIM_VERIFY_FAIL_NONE    0
#define PDKIM_VERIFY_FAIL_BODY    1
#define PDKIM_VERIFY_FAIL_MESSAGE 2







#ifdef PDKIM_DEBUG
void pdkim_quoteprint(FILE *, char *, int, int);
#endif

typedef struct pdkim_stringlist {
  char *value;
  void *next;
} pdkim_stringlist;
pdkim_stringlist *pdkim_append_stringlist(pdkim_stringlist *, char *);


#define PDKIM_STR_ALLOC_FRAG 256
typedef struct pdkim_str {
  char         *str;
  unsigned int  len;
  unsigned int  allocated;
} pdkim_str;
pdkim_str *pdkim_strnew (char *);
char      *pdkim_strcat (pdkim_str *, char *);
char      *pdkim_strncat(pdkim_str *, char *, int);
void       pdkim_strfree(pdkim_str *);

#define PDKIM_QUERYMETHOD_DNS_TXT 0
/* extern char *pdkim_querymethods[]; */

#define PDKIM_ALGO_RSA_SHA256 0
#define PDKIM_ALGO_RSA_SHA1   1
/* extern char *pdkim_algos[]; */

#define PDKIM_CANON_SIMPLE   0
#define PDKIM_CANON_RELAXED  1
/* extern char *pdkim_canons[]; */


/* -------------------------------------------------------------------------- */
/* Public key as (usually) fetched from DNS */
typedef struct pdkim_pubkey {
  char *version;                  /* v=  */
  char *granularity;              /* g=  */

  int num_hash_algos;
  int **hash_algos;               /* h=  */

  int keytype;                    /* k=  */
  int srvtype;                    /* s=  */

  char *notes;                    /* n=  */
  char *key;                      /* p=  */

  int testing;                    /* t=y */
  int no_subdomaining;            /* t=s */
} pdkim_pubkey;

/* -------------------------------------------------------------------------- */
/* Signature as it appears in a DKIM-Signature header */
typedef struct pdkim_signature {

  /* Bits stored in a DKIM signature header ------ */
  int version;                    /* v=   */
  int algo;                       /* a=   */
  int canon_headers;              /* c=x/ */
  int canon_body;                 /* c=/x */
  int querymethod;                /* q=   */

  char *selector;                 /* s=   */
  char *domain;                   /* d=   */
  char *identity;                 /* i=   */

  unsigned long created;          /* t=   */
  unsigned long expires;          /* x=   */
  unsigned long bodylength;       /* l=   */

  char *headernames;              /* h=   */
  char *copiedheaders;            /* z=   */

  char *sigdata;                  /* b=   */
  char *bodyhash;                 /* bh=  */

  int   sigdata_len;
  int   bodyhash_len;

  /* Signing specific ---------------------------- */
  char *rsa_privkey;     /* Private RSA key */
  char *sign_headers;    /* To-be-signed header names */

  /* Verification specific ----------------------- */
  pdkim_pubkey pubkey;   /* Public key used to verify this signature. */
  int headernames_pos;   /* Current position in header name list */
  char *rawsig_no_b_val; /* Original signature header w/o b= tag value. */
  void *next;            /* Pointer to next signature in list. */
  int verify_status;     /* Verification result */
  int verify_ext_status; /* Extended verification result */

  /* Per-signature helper variables -------------- */
  sha1_context sha1_body;
  sha2_context sha2_body;
  unsigned long signed_body_bytes;
  pdkim_stringlist *headers;
} pdkim_signature;


/* -------------------------------------------------------------------------- */
/* Context to keep state between all operations */

#define PDKIM_MODE_SIGN     0
#define PDKIM_MODE_VERIFY   1
#define PDKIM_INPUT_NORMAL  0
#define PDKIM_INPUT_SMTP    1

typedef struct pdkim_ctx {

  /* PDKIM_MODE_VERIFY or PDKIM_MODE_SIGN */
  int mode;

  /* PDKIM_INPUT_SMTP or PDKIM_INPUT_NORMAL */
  int input_mode;

  /* One (signing) or several chained (verification) signatures */
  pdkim_signature *sig;

  /* Coder's little helpers */
  pdkim_str *cur_header;
  char       linebuf[PDKIM_MAX_BODY_LINE_LEN];
  int        linebuf_offset;
  int        seen_lf;
  int        seen_eod;
  int        past_headers;
  int        num_buffered_crlf;

#ifdef PDKIM_DEBUG
  /* A FILE pointer. When not NULL, debug output will be generated
    and sent to this stream */
  FILE *debug_stream;
#endif

} pdkim_ctx;


int   header_name_match       (char *, char *, int);
char *pdkim_relax_header      (char *, int);

int   pdkim_update_bodyhash   (pdkim_ctx *, char *, int);
int   pdkim_finish_bodyhash   (pdkim_ctx *);

int   pdkim_bodyline_complete (pdkim_ctx *);
int   pdkim_header_complete   (pdkim_ctx *);

int   pdkim_feed              (pdkim_ctx *, char *, int);
int   pdkim_feed_finish       (pdkim_ctx *, char **);

char *pdkim_create_header     (pdkim_signature *, int);

pdkim_ctx
     *pdkim_init_sign         (char *, char *, char *);

pdkim_ctx
     *pdkim_init_verify       (void);

int   pdkim_set_optional      (pdkim_ctx *,
                               int,
                               char *, char *,
                               int, int,
                               unsigned long, int,
                               unsigned long,
                               unsigned long);

void  pdkim_free_sig          (pdkim_signature *);
void  pdkim_free_ctx          (pdkim_ctx *);


#ifdef PDKIM_DEBUG
void  pdkim_set_debug_stream  (pdkim_ctx *, FILE *);
#endif