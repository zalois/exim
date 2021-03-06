/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) Jeremy Harris 1995 - 2018 */
/* See the file NOTICE for conditions of use and distribution. */

/* Signing and hashing routine selection for PDKIM */

#include "../exim.h"
#include "../sha_ver.h"


#ifdef USE_GNUTLS
# include <gnutls/gnutls.h>

# if GNUTLS_VERSION_NUMBER >= 0x030000
#  define SIGN_GNUTLS
#  if GNUTLS_VERSION_NUMBER >= 0x030600
#   define SIGN_HAVE_ED25519		/*MMMM*/
#  endif
# else
#  define SIGN_GCRYPT
# endif

#else
# define SIGN_OPENSSL
#endif

