#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>

#include "private/qdmi_internal.h"


/*----------------------------------------*/
/* Internal Routines: Startup and Shutdown */

/*.....................................*/
/* read configuration and load libraries */

int QDMI_load_libraries(QInfo sesioninfo)
{
    char *configfilename = NULL;
    FILE *configfile;
    char *line;
    size_t  length;
    int  readlen;
    //char *separator,*key,*param;
    QDMI_Library newlib,runlib,prevlib;
    double dval;
    long   lval;
    QInfo_topic topic;
    QInfo_value value;
    int retval,initerr,err;
    

    /* Determine location of configfile */
    
    configfilename = getenv(QDMI_CONFIG_FILE);
    if (configfilename==NULL)
        configfilename=strdup(QDMI_CONFIG_FILE_DEFAULT);
    
    /* Read configuration file */
   
    configfile=fopen(configfilename,"r");
    if (configfile==NULL)
    {
        return QDMI_ERROR_NOCFGFILE;
    }

    do 
    {
        line=NULL;
        readlen=getline(&line,&length,configfile);
        if (readlen<0)
        {
            if (line!=NULL) 
                free(line);

            if (feof(configfile))
                break;
            else
                return QDMI_ERROR_CFGFILE;
        }
        
        /* no error, no feof, i.e., line is valid */
        
        if (line[0]!='#')
        {
            /* not a comment */
            
            const char *separator = strchr(line,'=');
            
            if (separator==NULL)
            {
                int err;
                
                /* must be a starter for a new library */
                
                /* create library object */
                
                newlib=(QDMI_Library) calloc(sizeof(QDMI_Library_impl_t),1);
                if (newlib==NULL)
                {
                    if (line!=NULL) free(line);
                    return QDMI_ERROR_OUTOFMEM;
                }
                
                /* setup basic objects in library object */
                
                err=QInfo_create(&(newlib->info));
                if (err!=QINFO_SUCCESS)
                {
                    if (line!=NULL) free(line);
                    return qdmi_internal_translate_qinfo_error(err);
                }

                char *newline = strrchr(line, '\n');
                if (newline != NULL)
                {
                    char *nullterminatedline = malloc(newline - line + 1);
                    strncpy(nullterminatedline, line, newline - line);
                    nullterminatedline[newline - line /*- 1*/] = '\0';
                    newlib->libname = nullterminatedline;
                }
                else
                    newlib->libname = line;

                newlib->libhandle=NULL;
            
                /* add new library to list */
                
                newlib->next=qdmi_library_list;
                qdmi_library_list=newlib;
            }
            else
            {
                int err;
                
                /* must be a parameter for an already started library */
                
                if (qdmi_library_list == NULL)
                {
                    /* no library started */
                    if (line != NULL) 
			            free(line);

                    return QDMI_ERROR_CFGFILE;
                }
                
                /* get the strings from the file */
                
		        size_t paramLength = separator - line;
                char *param = (char *)malloc(paramLength + 1);

                strncpy(param, line, paramLength);
                param[paramLength] = '\0';

                /* check type */
                
		        if (param != NULL)
                {
                    const char *valueString = separator + 1;

                    if (strchr(valueString, '.') != NULL)
                    {
                        value.value_double = strtod(valueString, NULL);
            
                        /* we have a double */
            
                        value.value_long = 0;
                        value.value_string = NULL;
                
                        err = QInfo_topic_add(newlib->info, param, QINFO_TYPE_DOUBLE, &topic);
                        if (err != QINFO_SUCCESS)
                        {
                            if (line != NULL)
                                free(line);

                            return qdmi_internal_translate_qinfo_error(err);
                        }
                        
                        err = QInfo_topic_set(newlib->info, topic, &value);
                        if (err != QINFO_SUCCESS)
                        {
                            if (line != NULL) 
                                free(line);

                            return qdmi_internal_translate_qinfo_error(err);
                        }
                    }
                    else if( isdigit(*valueString) )
                    {
                        char *endptr;
                        value.value_long = strtol(valueString, &endptr, 10);

                        if (*endptr == '\0')
                        {
                            /* now we have a long */

                            value.value_double = 0.0;
                            value.value_string = NULL;

                            err = QInfo_topic_add(newlib->info, param, QINFO_TYPE_LONG, &topic);
                            if (err != QINFO_SUCCESS)
                            {
                                if (line != NULL) 
                                free(line);

                                return qdmi_internal_translate_qinfo_error(err);
                            }
                            
                            err = QInfo_topic_set(newlib->info, topic, &value);
                            if (err != QINFO_SUCCESS)
                            {
                                if (line != NULL) 
                                    free(line);

                                return qdmi_internal_translate_qinfo_error(err);
                            }
                        }
                    }
                    else
                    {
                        /* now we have a string */
                        
                        value.value_double = 0.0;
                        value.value_long = 0;
                        value.value_string = strdup(valueString);

                        if (value.value_string == NULL)
                        {
                            if (line!=NULL) 
                                free(line);

                            return QDMI_ERROR_OUTOFMEM;
                        }

                        err = QInfo_topic_add(newlib->info , param, QINFO_TYPE_STRING, &topic);
                        if (err != QINFO_SUCCESS)
                        {
                            if (line != NULL) 
                                free(line);

                            free(value.value_string);

                            return qdmi_internal_translate_qinfo_error(err);
                        }

                        err = QInfo_topic_set(newlib->info, topic, &value);
                        if (err != QINFO_SUCCESS)
                        {
                            if (line != NULL) 
                                free(line);

                            free(value.value_string);

                            return qdmi_internal_translate_qinfo_error(err);
                        }
                    }
                }

                free(param);
                free(line);
            }
        }
    } while (1);
    
    /* the entire file is read and parsed into library list */
    /* now load all backend libraries and grab fct pointers */
   
    newlib=qdmi_library_list;
    retval=QDMI_SUCCESS;
    
    while (newlib!=NULL)
    {
        /* open library */

        newlib->libhandle = dlopen(newlib->libname, RTLD_LAZY);
        if (newlib->libhandle==NULL)
        {
            /* opening failed, remove that */
            
            runlib=qdmi_library_list;
            prevlib=NULL;
            
            while (runlib!=newlib)
            {
                prevlib=runlib;
                runlib=runlib->next;
            }
            
            if (runlib==NULL)
            {
                /* can't find the library anymore, this should never happen */
                return QDMI_ERROR_FATAL;
            }

            /* remove library from data structure */
            
            if (prevlib==NULL)
                qdmi_library_list=runlib->next;
            else
                prevlib->next=runlib->next;
            
            retval=QDMI_WARN_NOBACKEND;
            
            free(newlib->libname);
            err=QInfo_free(newlib->info);
            if (QINFO_IS_FATAL(err))
                return qdmi_internal_translate_qinfo_error(err);
        }

        /* eind all symbols */
        
        newlib->QDMI_backend_init=dlsym(newlib->libhandle,"QDMI_backend_init");

        newlib->QDMI_control_pack_qasm2=dlsym(newlib->libhandle,"QDMI_control_pack_qasm2");
        newlib->QDMI_control_pack_qir=dlsym(newlib->libhandle,"QDMI_control_pack_qir");
        newlib->QDMI_control_submit=dlsym(newlib->libhandle,"QDMI_control_submit");
        newlib->QDMI_control_cancel=dlsym(newlib->libhandle,"QDMI_control_cancel");
        newlib->QDMI_control_pause=dlsym(newlib->libhandle,"QDMI_control_pause");
        newlib->QDMI_control_test=dlsym(newlib->libhandle,"QDMI_control_test");
        newlib->QDMI_control_wait=dlsym(newlib->libhandle,"QDMI_control_wait");
        newlib->QDMI_control_extract_state=dlsym(newlib->libhandle,"QDMI_control_extract_state");
        newlib->QDMI_control_readout_size=dlsym(newlib->libhandle,"QDMI_control_readout_size");
        newlib->QDMI_control_readout_hist_size=dlsym(newlib->libhandle,"QDMI_control_readout_hist_size");
        newlib->QDMI_control_readout_hist_top=dlsym(newlib->libhandle,"QDMI_control_readout_hist_top");
        newlib->QDMI_control_readout_raw_num=dlsym(newlib->libhandle,"QDMI_control_readout_raw_num");
        newlib->QDMI_control_readout_raw_sample=dlsym(newlib->libhandle,"QDMI_control_readout_raw_sample");
        newlib->QDMI_query_device_property_exists=dlsym(newlib->libhandle,"QDMI_query_device_property_exists");
        newlib->QDMI_query_device_property_type=dlsym(newlib->libhandle,"QDMI_query_device_property_type");
        newlib->QDMI_query_device_property_i=dlsym(newlib->libhandle,"QDMI_query_device_property_i");
        newlib->QDMI_query_device_property_f=dlsym(newlib->libhandle,"QDMI_query_device_property_f");
        newlib->QDMI_query_device_property_d=dlsym(newlib->libhandle,"QDMI_query_device_property_d");
        newlib->QDMI_query_gateset_num=dlsym(newlib->libhandle,"QDMI_query_gateset_num");
        newlib->QDMI_query_qubits_num=dlsym(newlib->libhandle,"QDMI_query_qubits_num");
        newlib->QDMI_query_all_gates=dlsym(newlib->libhandle,"QDMI_query_all_gates");
        newlib->QDMI_query_byname=dlsym(newlib->libhandle,"QDMI_query_byname");
        newlib->QDMI_query_gate_name=dlsym(newlib->libhandle,"QDMI_query_gate_name");
        newlib->QDMI_query_gate_size=dlsym(newlib->libhandle,"QDMI_query_gate_size");
        newlib->QDMI_query_gate_unitary=dlsym(newlib->libhandle,"QDMI_query_gate_unitary");
        newlib->QDMI_query_gate_property_exists=dlsym(newlib->libhandle,"QDMI_query_gate_property_exists");
        newlib->QDMI_query_gate_property_type=dlsym(newlib->libhandle,"QDMI_query_gate_property_type");
        newlib->QDMI_query_gate_property_i=dlsym(newlib->libhandle,"QDMI_query_gate_property_i");
        newlib->QDMI_query_gate_property_f=dlsym(newlib->libhandle,"QDMI_query_gate_property_f");
        newlib->QDMI_query_gate_property_d=dlsym(newlib->libhandle,"QDMI_query_gate_property_d");
        newlib->QDMI_query_all_qubits=dlsym(newlib->libhandle,"QDMI_query_all_qubits");
        newlib->QDMI_query_qubit_property_exists=dlsym(newlib->libhandle,"QDMI_query_qubit_property_exists");
        newlib->QDMI_query_qubit_property_type=dlsym(newlib->libhandle,"QDMI_query_qubit_property_type");
        newlib->QDMI_query_qubit_property_c=dlsym(newlib->libhandle,"QDMI_query_qubit_property_c");
        newlib->QDMI_query_qubit_property_i=dlsym(newlib->libhandle,"QDMI_query_qubit_property_i");
        newlib->QDMI_query_qubit_property_f=dlsym(newlib->libhandle,"QDMI_query_qubit_property_f");
        newlib->QDMI_query_qubit_property_d=dlsym(newlib->libhandle,"QDMI_query_qubit_property_d");
        newlib->QDMI_device_status=dlsym(newlib->libhandle,"QDMI_device_status");
        newlib->QDMI_device_quality_check=dlsym(newlib->libhandle,"QDMI_device_quality_check");
        newlib->QDMI_device_quality_limit=dlsym(newlib->libhandle,"QDMI_device_quality_limit");
        newlib->QDMI_device_quality_calibrate=dlsym(newlib->libhandle,"QDMI_device_quality_calibrate");

	//printf("\n[TODO]: (qdmi_core.c) QDMI_ERROR_BACKEND");
        if (
            //(newlib->QDMI_control_pack_qasm2==NULL) ||
            //(newlib->QDMI_control_pack_qir==NULL) ||
            (newlib->QDMI_control_submit==NULL) ||
            //(newlib->QDMI_control_cancel==NULL) ||
            //(newlib->QDMI_control_pause==NULL) ||
            //(newlib->QDMI_control_test==NULL) ||
            //(newlib->QDMI_control_wait==NULL) ||
            //(newlib->QDMI_control_extract_state==NULL) ||
            (newlib->QDMI_control_readout_size==NULL) ||
            //(newlib->QDMI_control_readout_hist_size==NULL) ||
            //(newlib->QDMI_control_readout_hist_top==NULL) ||
            (newlib->QDMI_control_readout_raw_num==NULL) ||
            //(newlib->QDMI_control_readout_raw_sample==NULL) ||
            //(newlib->QDMI_query_device_property_exists==NULL) ||
            //(newlib->QDMI_query_device_property_i==NULL) ||
            //(newlib->QDMI_query_device_property_f==NULL) ||
            //(newlib->QDMI_query_device_property_d==NULL) ||
            //(newlib->QDMI_query_gateset_num==NULL) ||
            (newlib->QDMI_query_qubits_num==NULL) ||
            (newlib->QDMI_query_all_gates==NULL) ||
            //(newlib->QDMI_query_byname==NULL) ||
            //(newlib->QDMI_query_gate_name==NULL) ||
            //(newlib->QDMI_query_gate_size==NULL) ||
            //(newlib->QDMI_query_gate_unitary==NULL) ||
            //(newlib->QDMI_query_gate_property_exists==NULL) ||
            //(newlib->QDMI_query_gate_property_i==NULL) ||
            //(newlib->QDMI_query_gate_property_f==NULL) ||
            //(newlib->QDMI_query_gate_property_d==NULL) ||
            (newlib->QDMI_query_all_qubits==NULL) ||
            (newlib->QDMI_device_status==NULL) //||
            //(newlib->QDMI_device_quality_check==NULL) ||
            //(newlib->QDMI_device_quality_limit==NULL) ||
            //(newlib->QDMI_device_quality_calibrate==NULL)
           )
        {
            /* same function didn't load / bad backend, need to abort */
            retval=QDMI_ERROR_BACKEND;
        }

        newlib=newlib->next;
    }
    
    /* Now all library are opened and symbols have been found */
    /* Initialize all backend libraries */
    
    newlib=qdmi_library_list;
    
    while ((newlib!=NULL) && (!(QDMI_IS_FATAL(retval))))
    {
        initerr=newlib->QDMI_backend_init(newlib->info);
        if (QDMI_IS_FATAL(initerr))
            retval=initerr;
        else
            if (!(QDMI_IS_WARNING(retval)))
                retval=initerr;
        
        newlib=newlib->next;
    }
    
    
    /* Finish setup, cleanup after error */
    
    if (QDMI_IS_FATAL(retval))
    {
        runlib=qdmi_library_list;
    
        while (runlib!=NULL)
        {
            free(runlib->libname);
            if (runlib->libhandle)
            dlclose(runlib->libhandle);
            QInfo_free(runlib->info);
            runlib=runlib->next;
        }
        qdmi_library_list=NULL;
        return retval;
    }

    return QDMI_SUCCESS;
}


/*.....................................*/
/* Called when first session is created */

int QDMI_internal_startup(QInfo info)
{
    int err;
    
    if (qdmi_library_list==NULL)
    {
        err=QDMI_load_libraries(info);
        if (err!=QDMI_SUCCESS)
            return err;
    }

    return QDMI_SUCCESS;
}


/*.....................................*/
/* Cleanup when last active session is freed */

int QDMI_internal_shutdown()
{
    /* So far nothing */
    
    return QDMI_SUCCESS;
}


/*----------------------------------------*/
/* Public Routines: Session Management */

/*.....................................*/
/* Create a Session */
/*  IN:  info object
    OUT: created session
*/

int QDMI_session_init(QInfo info, QDMI_Session *session)
{
    int err;
    
    *session=(QDMI_Session_impl_t*) malloc(sizeof(QDMI_Session_impl_t));
    if (*session==NULL)
        return QDMI_ERROR_OUTOFMEM;
    
    // Copy `info` to `session->info`
    err = QInfo_duplicate(info, &((*session)->info));
    if (err!=QINFO_SUCCESS)
    {
        free(*session);
        return qdmi_internal_translate_qinfo_error(err);
    }
    
    if (qdmi_session_list==NULL)
    {
        /* First session */
        
        err=QDMI_internal_startup(info);

        //printf("\n[TODO]: (qdmi_core.c) if (err !=/*==*/ QDMI_SUCCESS)");

        if (err !=/*==*/ QDMI_SUCCESS)
        {
            QInfo_free((*session)->info);
            free(*session);

            return err;
        }
    }

    (*session)->next=qdmi_session_list;
    qdmi_session_list=*session;

    return QDMI_SUCCESS;
}


/*.....................................*/
/* Destroy a Session */
/*  IN:  session to be freed
    OUT: n/a
*/

int QDMI_session_finalize(QDMI_Session session)
{
    QDMI_Session sess,prev;
    
    sess=qdmi_session_list;
    prev=NULL;
   
    while (sess!=NULL)
    {
        if (sess==session)
        {
            if (prev==NULL)
                qdmi_session_list=sess->next;
            else
                prev->next=sess->next;
            
            QInfo_free(session->info);
            free(session);
            
            if (qdmi_session_list==NULL)
                return QDMI_internal_shutdown();
            
            return QDMI_SUCCESS;
        }
        prev=sess;
        sess=sess->next;
    }
    
    return QDMI_ERROR_NOSESSION;
}


/*----------------------------------------*/
/* Public Routines for Libraries: Registration */

/*.....................................*/
/* Called by backend library to register a backend device */
/*  IN:  name of the particular backend
         pointer to internal state
    OUT: n/a
*/

int QDMI_core_register_belib(char *uri, void *regpointer)
{
    return QDMI_ERROR_NOTIMPL;
}


/*.....................................*/
/* Called by backend library to register a backend device */
/*  IN:  name of the particular backend
    OUT: n/a
*/

int QDMI_core_unregister_belib(char *uri)
{
    return QDMI_ERROR_NOTIMPL;
}


/*----------------------------------------*/
/* Public Routines for Clients: Remaining Core API */

/*.....................................*/
/* Query the standard version of the current QDMI Library */
/* IN:  Session to be queried
   OUT: return minor and major version
*/

int QDMI_core_version(QDMI_Session *session, int* major, int* minor)
{
    return QDMI_SUCCESS;
}


/*.....................................*/
/* Query the size of the device list */
/* IN:  Session to be queried
   OUT: return the length of the list of devices
*/

int QDMI_core_device_count(QDMI_Session *session, int *count)
{
    return QDMI_SUCCESS;
}


/*.....................................*/
/* Open a device based on an index */
/* IN:  Session to be queried
        index to be opened
        info object to control device open
*/

int QDMI_core_open_device(QDMI_Session *session, int idx, QInfo *info, QDMI_Device* handle)
{
    return QDMI_SUCCESS;
}


/*.....................................*/
/* Query information of an opened device */
/* IN:  Session to be queried
        index to be querie
   OUT: info object that describes the device
*/

int QDMI_core_query_device(QDMI_Session *session, int idx, QInfo *info)
{
    return QDMI_SUCCESS;
}


/*.....................................*/
/* Close a device based on an index */
/* IN:  Session to be queried
        index to be closed
*/

int QDMI_core_close_device(QDMI_Session *session, QDMI_Device handle)
{
    return QDMI_SUCCESS;
}


/*----------------------------------------*/
/* The End. */
