// SPDX-License-Identifier: Apache-2.0
/* Copyright 2021 IBM Corp.*/
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <mbedtls/x509_crt.h> // for printCertInfo
#include "external/extraMbedtls/include/generate-pkcs7.h"
#include "secvar/include/general_secvar_cmds.h"

/**
 *checks to see if string is a valid variable name
 *@param var variable name
 *@return SUCCESS or error code
 */
int isVariable(const char * var)
{
    for (size_t i = 0; i < current_backend->sb_variables_count; i++) {
        if (strcmp(var, current_backend->sb_variables[i]) == 0)
            return SUCCESS;
    }

    return INVALID_VAR_NAME;
}

/**
* uses current backend to generate a string that contains the names of all of the variables supported
*@param outString , the returned string
*@param var_count, the number of variables to print, helpful if only printing first n vars
*@return SUCCESS if string generated correctly
*NOTE: outString STILL NEEDS TO BE DEALLOCATED OUTSIODE OF FUNCTION
*/
int getVariableString(char **outString, size_t var_count) 
{
    size_t len;
    //length of string = (max length of any of the variables + seperator length) * number of variables
    len = (strlen("dbx") + strlen("'', ")) * var_count; 
    *outString = calloc(len, 1);
    if (!outString) {
        prlog(PR_ERR, "ERROR: failed to allocate memory\n");
        return ALLOC_FAIL;
    }
    for (size_t i = 0;  i < var_count; i++) {
        strcat(*outString, "'");
        strcat(*outString, current_backend->sb_variables[i]);
        if (i < var_count - 1)
            strcat(*outString, "', ");
        else
             strcat(*outString, "'");
    }
    return SUCCESS;
}
/**
 *parses x509 certficate buffer (PEM or DER) into certificate struct
 *@param x509, returned x509, expected to be allocated mbedtls_x509_crt struct,
 *@param certBuf pointer to certificate data
 *@param buflen length of certBuf
 *@return CERT_FAIL if certificate cant be parsed
 *@return SUCCESS if certificate is valid
 */
int parseX509(mbedtls_x509_crt *x509, const unsigned char *certBuf, size_t buflen) 
{
    int failures;
    unsigned char *generatedDER = NULL;
    size_t generatedDERSize;
    if ((ssize_t)buflen <= 0) {
        prlog(PR_ERR, "ERROR: Certificate has invalid length %zd, cannot validate\n", buflen);
        return CERT_FAIL;
    }
    mbedtls_x509_crt_init(x509);
    // puts cert data into x509_Crt struct and returns number of failed parses
    failures = mbedtls_x509_crt_parse(x509, certBuf, buflen); 
    if (failures) {
        prlog(PR_INFO, "Failed to parse cert as DER mbedtls err#%d, trying PEM...\n", failures);
        // if failed, maybe input is PEM and so try converting PEM to DER, if conversion fails then we know it was DER and it failed
        if (convert_pem_to_der(certBuf, buflen, &generatedDER, &generatedDERSize)) {
            prlog(PR_ERR, "Parsing x509 as PEM format failed mbedtls err#%d \n", failures);
            return CERT_FAIL;
        }
        // if success then try to parse into x509 struct again
        failures = mbedtls_x509_crt_parse(x509, generatedDER, generatedDERSize); 
        if (failures) {
            prlog(PR_ERR, "Parsing x509 from PEM failed with MBEDTLS exit code: %d \n", failures);
            return CERT_FAIL;
        }
    }
    if (generatedDER) 
        free(generatedDER);

    return SUCCESS;
}