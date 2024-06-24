/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

/* Headerfile to be included by all QDMI backend implementations */

#ifndef QDMI_BACKEND_H
#define QDMI_BACKEND_H

#include <stdio.h>
#include <stdlib.h>

#include <qdmi.h>
    
/*----------------------------------------*/
/* Backend-only constants */

#define QINFO_INTERNAL_SPACEGRANULARITY 10


/*----------------------------------------*/
/* Backend-only types */

typedef struct QInfo_value_space_d
{
    QInfo_value value;
    int         occupied;
    char        *name;
} QInfo_value_space_t;



/*----------------------------------------*/
/* Backend-only Function defintions */

#ifdef __cplusplus
extern "C" {
#endif

/* Functions in the QDMI library to be called by the backend libraries */

int QDMI_core_register_belib(char *uri, void *regpointer);
int QDMI_core_unregister_belib(char *uri);

/* Functions that the backend libraries need to provide */

int QDMI_backend_init(QInfo info);
typedef int (*QDMI_backend_init_t)(QInfo info);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
