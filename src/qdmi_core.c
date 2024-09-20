/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

#include "private/qdmi_internal.h"

#include <ctype.h>
#include <dlfcn.h>
#include <qinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*----------------------------------------*/
/* Internal Routines: Startup and Shutdown */

/*.....................................*/
/* read configuration and load libraries */

int QDMI_load_libraries(QDMI_Session *session, QInfo sesioninfo) {
  char *configfilename = NULL;
  FILE *configfile;
  char *line;
  size_t length;
  int readlen;
  // char *separator,*key,*param;
  QDMI_Library newlib, runlib, prevlib, qdmi_library_list;
  double dval;
  long lval;
  int retval, initerr, err;
  qdmi_library_list = (*session)->qdmi_library_list;

  /* Determine location of configfile */

  configfilename = getenv(QDMI_CONFIG_FILE);
  if (configfilename == NULL)
    configfilename = strdup(QDMI_CONFIG_FILE_DEFAULT);

  /* Read configuration file */

  configfile = fopen(configfilename, "r");
  if (configfile == NULL) {
    return QDMI_ERROR_NOCFGFILE;
  }

  do {
    line = NULL;
    readlen = getline(&line, &length, configfile);
    if (readlen < 0) {
      if (line != NULL)
        free(line);

      if (feof(configfile))
        break;
      else
        return QDMI_ERROR_CFGFILE;
    }

    /* no error, no feof, i.e., line is valid */

    if (line[0] != '#') {
      /* not a comment */

      const char *separator = strchr(line, '=');

      if (separator == NULL) {
        int err;

        /* must be a starter for a new library */

        /* create library object */

        newlib = (QDMI_Library)calloc(sizeof(QDMI_Library_impl_t), 1);
        if (newlib == NULL) {
          if (line != NULL)
            free(line);
          return QDMI_ERROR_OUTOFMEM;
        }

        /* setup basic objects in library object */

        err = QInfo_create(&(newlib->info));
        if (err != QINFO_SUCCESS) {
          if (line != NULL)
            free(line);
          return qdmi_internal_translate_qinfo_error(err);
        }

        char *newline = strrchr(line, '\n');
        if (newline != NULL) {
          char *nullterminatedline = malloc(newline - line + 1);
          strncpy(nullterminatedline, line, newline - line);
          nullterminatedline[newline - line /*- 1*/] = '\0';
          newlib->libname = nullterminatedline;
        } else
          newlib->libname = line;

        newlib->libhandle = NULL;

        /* add new library to list */

        newlib->next = qdmi_library_list;
        qdmi_library_list = newlib;
      } else {
        int err;

        /* must be a parameter for an already started library */

        if (qdmi_library_list == NULL) {
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

        if (param != NULL) {
          const char *valueString = separator + 1;

          /* if parameter starts end ends with ", treat it as a string */
          int isQuotedString = 0;
          if ((valueString[0] == '"' && valueString[strlen(valueString) - 1] == '"') ||
              (valueString[0] == '"' && valueString[strlen(valueString) - 2] == '"' && valueString[strlen(valueString) - 1] == '\n'))
          {
              isQuotedString = 1;
          }

          if (!isQuotedString && strchr(valueString, '.') != NULL) {
            QInfo_index index;
            err = QInfo_add(newlib->info, param, QINFO_TYPE_DOUBLE, &index);
            if (err != QINFO_SUCCESS) {
              if (line != NULL) {
                free(line);
              }
              return qdmi_internal_translate_qinfo_error(err);
            }
            err = QInfo_set_d(newlib->info, index, strtod(valueString, NULL));
            if (err != QINFO_SUCCESS) {
              if (line != NULL) {
                free(line);
              }
              return qdmi_internal_translate_qinfo_error(err);
            }
          } else if (!isQuotedString && isdigit(*valueString)) {
            char *endptr;
            int64_t val = strtoll(valueString, &endptr, 10);
            if (*endptr == '\0') {
              QInfo_index index;
              err = QInfo_add(newlib->info, param, QINFO_TYPE_INT64, &index);
              if (err != QINFO_SUCCESS) {
                if (line != NULL) {
                  free(line);
                }
                return qdmi_internal_translate_qinfo_error(err);
              }
              err = QInfo_set_i64(newlib->info, index, val);
              if (err != QINFO_SUCCESS) {
                if (line != NULL) {
                  free(line);
                }
                return qdmi_internal_translate_qinfo_error(err);
              }
            }
          } else {
            /* now we have a string */
            QInfo_index index;
            err = QInfo_add(newlib->info, param, QINFO_TYPE_STRING, &index);
            if (err != QINFO_SUCCESS) {
              if (line != NULL) {
                free(line);
              }
              return qdmi_internal_translate_qinfo_error(err);
            }
            /* remove trailing newline */
            char *localValueString = strndup(valueString, strlen(valueString));
            if (localValueString[strlen(localValueString) - 1] == '\n') {
                localValueString[strlen(localValueString) - 1] = '\0';
            }
            /* remove quotes */
            char *strippedLocalValueString = localValueString;
            if (isQuotedString) {
                strippedLocalValueString[strlen(strippedLocalValueString) - 1] = '\0';
                strippedLocalValueString = strippedLocalValueString + 1;
            }
            err = QInfo_set_c(newlib->info, index, strippedLocalValueString);
            if (err != QINFO_SUCCESS) {
              if (line != NULL) {
                free(line);
              }
              return qdmi_internal_translate_qinfo_error(err);
            }
            free(localValueString);
          }
        }

        free(param);
        free(line);
      }
    }
  } while (1);

  /* the entire file is read and parsed into library list */
  /* now load all backend libraries and grab fct pointers */

  newlib = qdmi_library_list;
  retval = QDMI_SUCCESS;

  while (newlib != NULL) {
    /* open library */

    newlib->libhandle = dlopen(newlib->libname, RTLD_LAZY);
    if (newlib->libhandle == NULL) {
      /* opening failed, remove that */

      runlib = qdmi_library_list;
      prevlib = NULL;

      while (runlib != newlib) {
        prevlib = runlib;
        runlib = runlib->next;
      }

      if (runlib == NULL) {
        /* can't find the library anymore, this should never happen */
        return QDMI_ERROR_FATAL;
      }

      /* remove library from data structure */

      if (prevlib == NULL)
        qdmi_library_list = runlib->next;
      else
        prevlib->next = runlib->next;

      retval = QDMI_WARN_NOBACKEND;

      free(newlib->libname);
      err = QInfo_free(newlib->info);
      if (QInfo_is_Error(err))
        return qdmi_internal_translate_qinfo_error(err);
    }

    /* find all symbols */

    newlib->QDMI_backend_init =
        (QDMI_backend_init_t)dlsym(newlib->libhandle, "QDMI_backend_init");
    newlib->QDMI_control_pack_qasm2 = (QDMI_control_pack_qasm2_t)dlsym(
        newlib->libhandle, "QDMI_control_pack_qasm2");
    newlib->QDMI_control_pack_qir = (QDMI_control_pack_qir_t)dlsym(
        newlib->libhandle, "QDMI_control_pack_qir");
    newlib->QDMI_control_submit =
        (QDMI_control_submit_t)dlsym(newlib->libhandle, "QDMI_control_submit");
    newlib->QDMI_control_cancel =
        (QDMI_control_cancel_t)dlsym(newlib->libhandle, "QDMI_control_cancel");
    newlib->QDMI_control_pause =
        (QDMI_control_pause_t)dlsym(newlib->libhandle, "QDMI_control_pause");
    newlib->QDMI_control_test =
        (QDMI_control_test_t)dlsym(newlib->libhandle, "QDMI_control_test");
    newlib->QDMI_control_wait =
        (QDMI_control_wait_t)dlsym(newlib->libhandle, "QDMI_control_wait");
    newlib->QDMI_control_extract_state = (QDMI_control_extract_state_t)dlsym(
        newlib->libhandle, "QDMI_control_extract_state");
    newlib->QDMI_control_readout_size = (QDMI_control_readout_size_t)dlsym(
        newlib->libhandle, "QDMI_control_readout_size");
    newlib->QDMI_control_readout_hist_size =
        (QDMI_control_readout_hist_size_t)dlsym(
            newlib->libhandle, "QDMI_control_readout_hist_size");
    newlib->QDMI_control_readout_hist_top =
        (QDMI_control_readout_hist_top_t)dlsym(newlib->libhandle,
                                               "QDMI_control_readout_hist_top");
    newlib->QDMI_control_readout_raw_num =
        (QDMI_control_readout_raw_num_t)dlsym(newlib->libhandle,
                                              "QDMI_control_readout_raw_num");
    newlib->QDMI_control_readout_raw_sample =
        (QDMI_control_readout_raw_sample_t)dlsym(
            newlib->libhandle, "QDMI_control_readout_raw_sample");
    newlib->QDMI_query_device_property_exists =
        (QDMI_query_device_property_exists_t)dlsym(
            newlib->libhandle, "QDMI_query_device_property_exists");
    newlib->QDMI_query_device_property_type =
        (QDMI_query_device_property_type_t)dlsym(
            newlib->libhandle, "QDMI_query_device_property_type");
    newlib->QDMI_query_device_property_i =
        (QDMI_query_device_property_i_t)dlsym(newlib->libhandle,
                                              "QDMI_query_device_property_i");
    newlib->QDMI_query_device_property_f =
        (QDMI_query_device_property_f_t)dlsym(newlib->libhandle,
                                              "QDMI_query_device_property_f");
    newlib->QDMI_query_device_property_d =
        (QDMI_query_device_property_d_t)dlsym(newlib->libhandle,
                                              "QDMI_query_device_property_d");
    newlib->QDMI_query_device_property_c =
        (QDMI_query_device_property_c_t)dlsym(newlib->libhandle,
                                              "QDMI_query_device_property_c");
    newlib->QDMI_query_gateset_num = (QDMI_query_gateset_num_t)dlsym(
        newlib->libhandle, "QDMI_query_gateset_num");
    newlib->QDMI_query_qubits_num = (QDMI_query_qubits_num_t)dlsym(
        newlib->libhandle, "QDMI_query_qubits_num");
    newlib->QDMI_query_all_gates = (QDMI_query_all_gates_t)dlsym(
        newlib->libhandle, "QDMI_query_all_gates");
    newlib->QDMI_query_byname =
        (QDMI_query_byname_t)dlsym(newlib->libhandle, "QDMI_query_byname");
    newlib->QDMI_query_gate_name = (QDMI_query_gate_name_t)dlsym(
        newlib->libhandle, "QDMI_query_gate_name");
    newlib->QDMI_query_gate_size = (QDMI_query_gate_size_t)dlsym(
        newlib->libhandle, "QDMI_query_gate_size");
    newlib->QDMI_query_gate_unitary = (QDMI_query_gate_unitary_t)dlsym(
        newlib->libhandle, "QDMI_query_gate_unitary");
    newlib->QDMI_query_gate_property_exists =
        (QDMI_query_gate_property_exists_t)dlsym(
            newlib->libhandle, "QDMI_query_gate_property_exists");
    newlib->QDMI_query_gate_property_type =
        (QDMI_query_gate_property_type_t)dlsym(newlib->libhandle,
                                               "QDMI_query_gate_property_type");
    newlib->QDMI_query_gate_property_i = (QDMI_query_gate_property_i_t)dlsym(
        newlib->libhandle, "QDMI_query_gate_property_i");
    newlib->QDMI_query_gate_property_f = (QDMI_query_gate_property_f_t)dlsym(
        newlib->libhandle, "QDMI_query_gate_property_f");
    newlib->QDMI_query_gate_property_d = (QDMI_query_gate_property_d_t)dlsym(
        newlib->libhandle, "QDMI_query_gate_property_d");
    newlib->QDMI_query_all_qubits = (QDMI_query_all_qubits_t)dlsym(
        newlib->libhandle, "QDMI_query_all_qubits");
    newlib->QDMI_query_qubit_property_exists =
        (QDMI_query_qubit_property_exists_t)dlsym(
            newlib->libhandle, "QDMI_query_qubit_property_exists");
    newlib->QDMI_query_qubit_property_type =
        (QDMI_query_qubit_property_type_t)dlsym(
            newlib->libhandle, "QDMI_query_qubit_property_type");
    newlib->QDMI_query_qubit_property_c = (QDMI_query_qubit_property_c_t)dlsym(
        newlib->libhandle, "QDMI_query_qubit_property_c");
    newlib->QDMI_query_qubit_property_i = (QDMI_query_qubit_property_i_t)dlsym(
        newlib->libhandle, "QDMI_query_qubit_property_i");
    newlib->QDMI_query_qubit_property_f = (QDMI_query_qubit_property_f_t)dlsym(
        newlib->libhandle, "QDMI_query_qubit_property_f");
    newlib->QDMI_query_qubit_property_d = (QDMI_query_qubit_property_d_t)dlsym(
        newlib->libhandle, "QDMI_query_qubit_property_d");
    newlib->QDMI_device_status =
        (QDMI_device_status_t)dlsym(newlib->libhandle, "QDMI_device_status");
    newlib->QDMI_device_quality_check = (QDMI_device_quality_check_t)dlsym(
        newlib->libhandle, "QDMI_device_quality_check");
    newlib->QDMI_device_quality_limit = (QDMI_device_quality_limit_t)dlsym(
        newlib->libhandle, "QDMI_device_quality_limit");
    newlib->QDMI_device_quality_calibrate =
        (QDMI_device_quality_calibrate_t)dlsym(newlib->libhandle,
                                               "QDMI_device_quality_calibrate");

    if ((newlib->QDMI_control_pack_qasm2 == NULL) ||
        (newlib->QDMI_control_pack_qir == NULL) ||
        (newlib->QDMI_control_submit == NULL) ||
        (newlib->QDMI_control_cancel == NULL) ||
        (newlib->QDMI_control_pause == NULL) ||
        (newlib->QDMI_control_test == NULL) ||
        (newlib->QDMI_control_wait == NULL) ||
        (newlib->QDMI_control_extract_state == NULL) ||
        (newlib->QDMI_control_readout_size == NULL) ||
        (newlib->QDMI_control_readout_hist_size == NULL) ||
        (newlib->QDMI_control_readout_hist_top == NULL) ||
        (newlib->QDMI_control_readout_raw_num == NULL) ||
        (newlib->QDMI_control_readout_raw_sample == NULL) ||
        (newlib->QDMI_query_device_property_exists == NULL) ||
        (newlib->QDMI_query_device_property_type == NULL) ||
        (newlib->QDMI_query_device_property_i == NULL) ||
        (newlib->QDMI_query_device_property_f == NULL) ||
        (newlib->QDMI_query_device_property_d == NULL) ||
        (newlib->QDMI_query_device_property_c == NULL) ||
        (newlib->QDMI_query_gateset_num == NULL) ||
        (newlib->QDMI_query_qubits_num == NULL) ||
        (newlib->QDMI_query_all_gates == NULL) ||
        (newlib->QDMI_query_byname == NULL) ||
        (newlib->QDMI_query_gate_name == NULL) ||
        (newlib->QDMI_query_gate_size == NULL) ||
        (newlib->QDMI_query_gate_unitary == NULL) ||
        (newlib->QDMI_query_gate_property_exists == NULL) ||
        (newlib->QDMI_query_gate_property_type == NULL) ||
        (newlib->QDMI_query_gate_property_i == NULL) ||
        (newlib->QDMI_query_gate_property_f == NULL) ||
        (newlib->QDMI_query_gate_property_d == NULL) ||
        (newlib->QDMI_query_all_qubits == NULL) ||
        (newlib->QDMI_query_qubit_property_exists == NULL) ||
        (newlib->QDMI_query_qubit_property_type == NULL) ||
        (newlib->QDMI_query_qubit_property_i == NULL) ||
        (newlib->QDMI_query_qubit_property_f == NULL) ||
        (newlib->QDMI_query_qubit_property_d == NULL) ||
        (newlib->QDMI_query_qubit_property_c == NULL) ||
        (newlib->QDMI_device_status == NULL) ||
        (newlib->QDMI_device_quality_check == NULL) ||
        (newlib->QDMI_device_quality_limit == NULL) ||
        (newlib->QDMI_device_quality_calibrate == NULL)) {
      /* some function didn't load / bad backend, need to abort */
      retval = QDMI_ERROR_BACKEND;
    }

    newlib = newlib->next;
  }

  /* Now all library are opened and symbols have been found */
  /* Initialize all backend libraries */

  newlib = qdmi_library_list;

  while ((newlib != NULL) && (!(QDMI_IS_FATAL(retval)))) {
    initerr = newlib->QDMI_backend_init(newlib->info);
    if (QDMI_IS_FATAL(initerr))
      retval = initerr;
    else if (!(QDMI_IS_WARNING(retval)))
      retval = initerr;

    newlib = newlib->next;
  }

  /* Finish setup, cleanup after error */

  if (QDMI_IS_FATAL(retval)) {
    runlib = qdmi_library_list;

    while (runlib != NULL) {
      free(runlib->libname);
      if (runlib->libhandle)
        dlclose(runlib->libhandle);
      QInfo_free(runlib->info);
      runlib = runlib->next;
    }
    qdmi_library_list = NULL;
    return retval;
  }
  (*session)->qdmi_library_list = qdmi_library_list;

  return QDMI_SUCCESS;
}

/*.....................................*/
/* Called when first session is created */

int QDMI_internal_startup(QDMI_Session *session, QInfo info) {
  return QDMI_load_libraries(session, info);
}

/*.....................................*/
/* Cleanup when last active session is freed */

int QDMI_internal_shutdown() {
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

int QDMI_session_init(QInfo info, QDMI_Session *session) {
  int err;

  *session = (QDMI_Session_impl_t *)malloc(sizeof(QDMI_Session_impl_t));
  if (*session == NULL)
    return QDMI_ERROR_OUTOFMEM;
  
  (*session)->qdmi_library_list = NULL;

  // Copy `info` to `session->info`
  err = QInfo_duplicate(info, &((*session)->info));
  if (err != QINFO_SUCCESS) {
    free(*session);
    return qdmi_internal_translate_qinfo_error(err);
  }

  if (qdmi_session_list == NULL) {
    /* First session */

    err = QDMI_internal_startup(session, info);

    // printf("\n[TODO]: (qdmi_core.c) if (err !=/*==*/ QDMI_SUCCESS)");

    if (err != /*==*/QDMI_SUCCESS) {
      QInfo_free((*session)->info);
      free(*session);

      return err;
    }
  }

  (*session)->next = qdmi_session_list;
  qdmi_session_list = *session;

  return QDMI_SUCCESS;
}

/*.....................................*/
/* Destroy a Session */
/*  IN:  session to be freed
    OUT: n/a
*/

int QDMI_session_finalize(QDMI_Session session) {
  QDMI_Session sess, prev;

  sess = qdmi_session_list;
  prev = NULL;

  while (sess != NULL) {
    if (sess == session) {
      if (prev == NULL)
        qdmi_session_list = sess->next;
      else
        prev->next = sess->next;

      QInfo_free(session->info);
      free(session);

      if (qdmi_session_list == NULL)
        return QDMI_internal_shutdown();

      return QDMI_SUCCESS;
    }
    prev = sess;
    sess = sess->next;
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

int QDMI_core_register_belib(char *uri, void *regpointer) {
  return QDMI_ERROR_NOTIMPL;
}

/*.....................................*/
/* Called by backend library to register a backend device */
/*  IN:  name of the particular backend
    OUT: n/a
*/

int QDMI_core_unregister_belib(char *uri) { return QDMI_ERROR_NOTIMPL; }

/*----------------------------------------*/
/* Public Routines for Clients: Remaining Core API */

/*.....................................*/
/* Query the standard version of the current QDMI Library */
/* IN:  Session to be queried
   OUT: return minor and major version
*/

int QDMI_core_version(QDMI_Session *session, int *major, int *minor) {
  return QDMI_SUCCESS;
}

/*.....................................*/
/* Query the size of the device list */
/* IN:  Session to be queried
   OUT: return the length of the list of devices
*/

int QDMI_core_device_count(QDMI_Session *session, int *count) {
  QDMI_Library lib = (*session)->qdmi_library_list;
  *count = 0;

  while (lib != NULL) {
    (*count)++;
    lib = lib->next;
  }

  return QDMI_SUCCESS;
}

/*.....................................*/
/* Open a device based on an index */
/* IN:  Session to be queried
        index to be opened
        info object to control device open
*/

int QDMI_core_open_device(QDMI_Session *session, int idx, QInfo *info,
                          QDMI_Device *handle) {
  int count;
  int err = QDMI_core_device_count(session, &count);
  if QDMI_IS_ERROR (err)
    return err;

  if (count < idx)
    return QDMI_ERROR_FATAL;

  (*handle) =
      (struct QDMI_Device_impl_d *)malloc(sizeof(struct QDMI_Device_impl_d));
  if (*handle == NULL)
    return QDMI_ERROR_OUTOFMEM;

  QDMI_Library lib = (*session)->qdmi_library_list;
  if (lib == NULL)
    return QDMI_ERROR_FATAL;

  for (int index = 0; index < idx; index++)
    lib = lib->next;

  (*handle)->library = *lib;

  return QDMI_SUCCESS;
}

/*.....................................*/
/* Query information of an opened device */
/* IN:  Session to be queried
        index to be querie
   OUT: info object that describes the device
*/

int QDMI_core_query_device(QDMI_Session *session, int idx, QInfo *info) {
  return QDMI_SUCCESS;
}

/*.....................................*/
/* Close a device based on an index */
/* IN:  Session to be queried
        index to be closed
*/

int QDMI_core_close_device(QDMI_Session *session, QDMI_Device handle) {
  return QDMI_SUCCESS;
}

/*----------------------------------------*/
/* The End. */
