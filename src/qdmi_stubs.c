/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include "private/qdmi_internal.h"


/*----------------------------------------*/
/* Stub routines for control API */

int QDMI_control_pack_qasm2(QDMI_Device dev, char *qasmstr, QDMI_Fragment *frag)
{
    return dev->library.QDMI_control_pack_qasm2(dev, qasmstr, frag);
}

int QDMI_control_pack_qir(QDMI_Device dev, void *qirmod, QDMI_Fragment *frag)
{
    return dev->library.QDMI_control_pack_qir(dev, qirmod, frag);
}

int QDMI_control_submit(QDMI_Device dev, QDMI_Fragment *frag, int numshots, QInfo info, QDMI_Job *job)
{
    return dev->library.QDMI_control_submit(dev, frag, numshots, info, job);
}

int QDMI_control_cancel(QDMI_Device dev, QDMI_Job *job, QInfo info)
{
    return dev->library.QDMI_control_cancel(dev, job, info);
}

int QDMI_control_pause(QDMI_Device dev, QDMI_Job *job, QInfo info)
{
    return dev->library.QDMI_control_pause(dev, job, info);
}

int QDMI_control_test(QDMI_Device dev, QDMI_Job *job, int *flag, QDMI_Status *status)
{
    return dev->library.QDMI_control_test(dev, job, flag, status);
}

int QDMI_control_wait(QDMI_Device dev, QDMI_Job *job, QDMI_Status *status)
{
    return dev->library.QDMI_control_wait(dev, job, status);
}

int QDMI_control_extract_state(QDMI_Device dev, QDMI_Status status, int *state)
{
    return dev->library.QDMI_control_extract_state(dev, status, state);
}

int QDMI_control_readout_size(QDMI_Device dev, QDMI_Status *status, int *numbits)
{
    return dev->library.QDMI_control_readout_size(dev, status, numbits);
}

int QDMI_control_readout_hist_size(QDMI_Device dev, QDMI_Status *status, int *size)
{
    return dev->library.QDMI_control_readout_hist_size(dev, status, size);
}

int QDMI_control_readout_hist_top(QDMI_Device dev, QDMI_Status *status, int numhist, QInfo info, long *hist)
{
    return dev->library.QDMI_control_readout_hist_top(dev, status, numhist, info, hist);
}

int QDMI_control_readout_raw_num(QDMI_Device dev, QDMI_Status *status, int task_id, int *num)
{
    return dev->library.QDMI_control_readout_raw_num(dev, status, task_id, num);
}

int QDMI_control_readout_raw_sample(QDMI_Device dev, QDMI_Status *status, int numraw, QInfo info, long *hist)
{
    return dev->library.QDMI_control_readout_raw_sample(dev, status, numraw, info, hist);
}


/*----------------------------------------*/
/* Stub routines for query API */

int QDMI_query_device_property_exists(QDMI_Device dev, QDMI_Device_property prop, int* scope)
{
    return dev->library.QDMI_query_device_property_exists(dev, prop, scope);
}

int QDMI_query_device_property_type(QDMI_Device dev, QDMI_Device_property prop)
{
    return dev->library.QDMI_query_device_property_type(dev, prop);
}

int QDMI_query_device_property_i(QDMI_Device dev, QDMI_Device_property prop, int *value)
{
    return dev->library.QDMI_query_device_property_i(dev, prop, value);
}

int QDMI_query_device_property_f(QDMI_Device dev, QDMI_Device_property prop, float *value)
{
    return dev->library.QDMI_query_device_property_f(dev, prop, value);
}

int QDMI_query_device_property_d(QDMI_Device dev, QDMI_Device_property prop, double *value)
{
    return dev->library.QDMI_query_device_property_d(dev, prop, value);
}

int QDMI_query_gateset_num(QDMI_Device dev, int *num_gates)
{
    return dev->library.QDMI_query_gateset_num(dev, num_gates);
}

int QDMI_query_qubits_num(QDMI_Device dev, int *num_qubits)
{
    return dev->library.QDMI_query_qubits_num(dev, num_qubits);
}

int QDMI_query_all_gates(QDMI_Device dev, QDMI_Gate *gates)
{
    return dev->library.QDMI_query_all_gates(dev, gates);
}

int QDMI_query_byname(QDMI_Device dev, char *name, QDMI_Gate *gate)
{
    return dev->library.QDMI_query_byname(dev, name, gate);
}

int QDMI_query_gate_name(QDMI_Device dev, QDMI_Gate gate, char* name, int* len)
{
    return dev->library.QDMI_query_gate_name(dev, gate, name, len);
}

int QDMI_query_gate_size(QDMI_Device dev, QDMI_Gate gate, int* gatesize)
{
    return dev->library.QDMI_query_gate_size(dev, gate, gatesize);
}

int QDMI_query_gate_unitary(QDMI_Device dev, QDMI_Gate gate, QDMI_Unitary *unitary)
{
    return dev->library.QDMI_query_gate_unitary(dev, gate, unitary);
}

int QDMI_query_gate_property_exists(QDMI_Device dev, QDMI_Gate gate, QDMI_Gate_property prop, int* scope)
{
    return dev->library.QDMI_query_gate_property_exists(dev, gate, prop, scope);
}

int QDMI_query_gate_property_type(QDMI_Device dev, QDMI_Gate gate, QDMI_Gate_property prop)
{
    return dev->library.QDMI_query_gate_property_type(dev, gate, prop);
}

int QDMI_query_gate_property_i(QDMI_Device dev, QDMI_Gate_property prop, QDMI_Gate gate, int *coor, int* value)
{
    return dev->library.QDMI_query_gate_property_i(dev, prop, gate, coor, value);
}

int QDMI_query_gate_property_f(QDMI_Device dev, QDMI_Gate_property prop, QDMI_Gate gate, int *coor, float* value)
{
    return dev->library.QDMI_query_gate_property_f(dev, prop, gate, coor, value);
}

int QDMI_query_gate_property_d(QDMI_Device dev, QDMI_Gate_property prop, QDMI_Gate gate, int *coor, double* value)
{
    return dev->library.QDMI_query_gate_property_d(dev, prop, gate, coor, value);
}

int QDMI_query_all_qubits(QDMI_Device dev, QDMI_Qubit *qubits)
{
    return dev->library.QDMI_query_all_qubits(dev, qubits);
}

int QDMI_query_qubit_property_exists(QDMI_Device dev, QDMI_Qubit qubit, QDMI_Qubit_property prop, int* scope)
{
    return dev->library.QDMI_query_qubit_property_exists(dev, qubit, prop, scope);
}

int QDMI_query_qubit_property_type(QDMI_Device dev, QDMI_Qubit qubit, QDMI_Qubit_property prop)
{
    return dev->library.QDMI_query_qubit_property_type(dev, qubit, prop);
}

int QDMI_query_qubit_property_c(QDMI_Device dev, QDMI_Qubit qubit, QDMI_Qubit_property prop, char *value)
{
    return dev->library.QDMI_query_qubit_property_c(dev, qubit, prop, value);
}
int QDMI_query_qubit_property_i(QDMI_Device dev, QDMI_Qubit qubit, QDMI_Qubit_property prop, int *value)
{
    return dev->library.QDMI_query_qubit_property_i(dev, qubit, prop, value);
}

int QDMI_query_qubit_property_f(QDMI_Device dev, QDMI_Qubit qubit, QDMI_Qubit_property prop, float *value)
{
    return dev->library.QDMI_query_qubit_property_f(dev, qubit, prop, value);
}

int QDMI_query_qubit_property_d(QDMI_Device dev, QDMI_Qubit qubit, QDMI_Qubit_property prop, double *value)
{
    return dev->library.QDMI_query_qubit_property_d(dev, qubit, prop, value);
}


/*----------------------------------------*/
/* Stub routines for device API */

int QDMI_device_status(QDMI_Device dev, QInfo info, int *status)
{
    return dev->library.QDMI_device_status(dev, info, status);
}

int QDMI_device_quality_check(QDMI_Device dev, double *result)
{
    return dev->library.QDMI_device_quality_check(dev, result);
}

int QDMI_device_quality_limit(QDMI_Device dev, double *result)
{
    return dev->library.QDMI_device_quality_limit(dev, result);
}

int QDMI_device_quality_calibrate(QDMI_Device dev)
{
    return dev->library.QDMI_device_quality_calibrate(dev);
}


/*----------------------------------------*/
/* The End. */
