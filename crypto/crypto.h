#ifndef SECVARCTL_CRYPTO_H
#define SECVARCTL_CRYPTO_H

#ifdef OPENSSL

#include <openssl/obj_mac.h>
#define CRYPTO_MD_SHA1 NID_sha1
#define CRYPTO_MD_SHA224 NID_sha224
#define CRYPTO_MD_SHA256 NID_sha256 
#define CRYPTO_MD_SHA384 NID_sha384
#define CRYPTO_MD_SHA512 NID_sha512

#elif defined MBEDTLS

#include <mbedtls/md.h>
#define CRYPTO_MD_SHA1 MBEDTLS_MD_SHA1
#define CRYPTO_MD_SHA224 MBEDTLS_MD_SHA224
#define CRYPTO_MD_SHA256 MBEDTLS_MD_SHA256 
#define CRYPTO_MD_SHA384 MBEDTLS_MD_SHA384
#define CRYPTO_MD_SHA512  MBEDTLS_MD_SHA512

#endif
/**====================PKCS7 Functions ====================**/

/* 
 *checks the pkcs7 struct for using SHA256 as the message digest 
 *@param pkcs7 , a pointer to either an openssl or mbedtls pkcs7 struct
 *@return SUCCESS if message digest is SHA256 else return errno
 */
int crypto_pkcs7_md_is_sha256(void *pkcs7);

/*
 *free's the memory allocated for a pkcs7 structure
 *@param pkcs7 , a pointer to either an openssl or mbedtls pkcs7 struct
 */
void crypto_pkcs7_free(void *pkcs7);

/*
 *parses a buffer into a pointer to a pkcs7 struct. struct allocation is done internally to this func, but not dealloc
 *@param buf, buffer of data containg pkcs7 data
 *@param buflen, length of buf
 *@return if successful, a void pointer to either an openssl or mbedtls pkcs7 struct. else returns NULL
 *NOTE: if successful (returns not NULL), remember to call crypto_free_pkcs7 to unalloc. 
 */
void * crypto_pkcs7_parse_der(const unsigned char *buf, const int buflen);

/*
 *returns one signing ceritficate from the PKKCS7 signing certificate chain
 *@param pkcs7 ,  a pointer to either an openssl or mbedtls pkcs7 struct
 *@param cert_num , the index (starts at 0) of the signing certificate to retrieve
 *@return a pointer to either an openssl or mbedtls X509 Struct
 *NOTE: The returned pointer need not be freed, since it is a reference to memory in pkcs7
 */
void* crypto_get_signing_cert(void *pkcs7, int cert_num);

/*
 *determines if signed data in pkcs7 is correctly signed by x509 by signing the hash with the pk and comparing the resulting signature with that in the pkcs7
 *@param pkcs7 , a pointer to either an openssl or mbedtls pkcs7 struct
 *@param x509 , a pinter to either an openssl or mbedtls x509 struct
 *@param hash , the expected hash
 *@param hash_len , the length of expected hash (ex: SHA256 = 32), if 0 then asssumptions are made based on md in pkcs7
 *@return SUCCESS or error number if resulting hashes are not equal
 */
int crypto_pkcs7_signed_hash_verify(void *pkcs7, void *x509, unsigned char *hash, int hash_len);


/**====================X509 Functions ====================**/
int crypto_get_x509_der_len(void *x509);
int crypto_get_tbs_x509_der_len(void *x509);
int crypto_get_x509_version(void *x509);
int crypto_get_x509_sig_len(void *x509);
int crypto_x509_md_is_sha256(void *x509);
int crypto_x509_oid_is_pkcs1_sha256(void *x509);
int crypto_x509_get_pk_bit_len(void *x509);
/*
 *checks the type of the x509 and ensures that it is of type RSA
 *@param x509, a pointer to either an openssl or mbedtls x509 struct
 *@return SUCCESS if RSA or if not, returns the returned type value (differs for openssl and mbedtls)
 */
int crypto_x509_is_RSA(void *x509);

/*
 *returns a short string describing the x509 message digest and encryption algorithms
 *@param x509, a pointer to either an openssl or mbedtls x509 struct
 *@param short_desc ,  already alloc'd pointer to output string
 *@param max_len   , number of bytes allocated to short_desc arg
 */
void crypto_x509_get_short_info(void *x509, char *short_desc, size_t max_len);

/*
 *parses the x509 struct into a human readable informational string
 *@param x509_info , already alloc-d pointer to output string
 *@param max_len , number of bytes allocated to x509_info
 *@param delim  , eachline will start with this, usually indent, when using openssl, the length of this value is the number of 8 spaced tabs
 *@param x509 ,  a pointer to either an openssl or mbedtls x509 struct
 *@return number of bytes written to x509_info
 */
int crypto_x509_get_long_desc(char *x509_info, size_t max_len, char *delim, void *x509);

/*
 *parses a data buffer into an x509 struct 
 *@param x509 , output, a pointer to either an openssl or mbedtls x509 struct, should have already been allocated
 *@param data , pointer to data buffer containing an x509 in DER format
 *@param data_lem , length of data buffer
 */
/*
 *parses a buffer into a pointer to an x509 struct. struct allocation is done internally to this func, but not dealloc
 *@param buf, buffer of data containing x509 data in DER
 *@param buflen, length of buf
 *@return if successful, a void pointer to either an openssl or mbedtls x509 struct. else returns NULL
 *NOTE: if successful (returns not NULL), remember to call crypto_x509_free to unalloc. 
 */
void *crypto_x509_parse_der(const unsigned char *data, size_t data_len);

/*
 *unallocates x509 struct and memory
 *@param x509 ,  a pointer to either an openssl or mbedtls x509 struct, should have already been allocated
 */
void crypto_x509_free(void *x509);


/**====================General Functions ====================**/
/*
 *attempts to convert PEM data buffer into DER data buffer
 *@param input , PEM data buffer
 *@param ilen , length of input data
 *@param output , pointer to output DER data, not yet allocated
 *@param olen , pointer to length of output data
 *@return SUCCESS or errno if conversion failed
 *Note: Remember to unallocate the output data!
 */
int crypto_convert_pem_to_der(const unsigned char *input, size_t ilen, unsigned char **output, size_t *olen);

/*
 *accepts an error code from either mbedtls or openssl and returns a string describing it
 *@param rc , the error code, from mbedtls or openssl
 *@param out_str , an already allocated string, will be filled with string describing the error code
 *@out_max_len , the number of bytes allocated to out_str
 */
void crypto_strerror(int rc, char *out_str, size_t out_max_len);


/**====================Hashing Functions ====================**/
/*
 *Initializes and returns hashing context for the hashing function identified
 *@param ctx , the returned hashing context
 *@param md_id , the id of the hahsing function see above for possible values (CRYPTO_MD_xxx )
 *@return SUCCESS or err if the digest context setup failed
 */
int crypto_md_ctx_init(void **ctx, int md_id);

/*
 *can be repeatedly called to add data to be hashed by ctx
 *@param ctx , a pointer to either an mbedtls or openssl hashing context
 *@param data , data to be hashed
 *@param data_len , length of data to be hashed
 *@return SUCCESS or err if additional data could not be added to context
 */
int crypto_md_update(void *ctx, const unsigned char *data, size_t data_len);

/*
 *runs the hash over the supplied data (given with crypto_md_update) and returns it in hash
 *@param  ctx , a pointer to either an mbedtls or openssl hashing context
 *@param hash, an allocated data blob where the returned hash will be stored
 *@return SUCCESS or err if the hash generation was successful
 */
int crypto_md_finish(void *ctx, unsigned char *hash);

/*
 *frees the memory alloacted for the hashing context
 *@param ctx , a pointer to either an mbedtls or openssl hashing context
 */
void crypto_md_free(void *ctx); 
#endif