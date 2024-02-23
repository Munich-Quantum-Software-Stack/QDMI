#ifndef QDMIBACKENDWMI_H
#define QDMIBACKENDWMI_H

#include <qdmi_backend.h>
#include "qdmi_internal.h"

const char *gate_set[] = 
{
    "__quantum__qis__barrier__body", 
    "__quantum__qis__cz__body", 
    "__quantum__qis__rz__body",      
    "__quantum__qis__x__body",    
    "__quantum__qis__sx__body",   
    "__quantum__qis__y__body",
    "__quantum__qis__z__body",       
};

#endif // QDMIBACKENDWMI_H
