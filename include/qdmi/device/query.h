/*------------------------------------------------------------------------------
Part of the MQSS Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

/** @file
 * @brief Define the interface for querying properties of a device.
 */

#pragma once

#include "qdmi/common/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the sites associated with @p device.
 * @param[in] num_entries the number of entries that can be added to @p devices.
 * If @p devices is not @c NULL, @p num_entries must be greater than zero.
 * @param[out] sites returns a list of sites available on the device. The @ref
 * QDMI_Site values returned in @p sites can be used to identify a specific @ref
 * QDMI_Site. If @p sites is @c NULL, this argument is ignored. The number of
 * @ref QDMI_Site values returned is the minimum of the value specified by @p
 * num_entries and the number of sites found.
 * @param[out] num_sites returns the number of sites available. If @p num_sites
 * is @c NULL, this argument is ignored.
 * @return @ref QDMI_SUCCESS if the function is executed successfully.
 * Otherwise, it returns one of the following error codes:
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p num_entries is less than or
 * equal to zero and @p sites is not @c NULL or if both @p sites and @p
 * num_sites are @c NULL.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 */
int QDMI_query_get_sites_dev(int num_entries, QDMI_Site *sites, int *num_sites);

/**
 * @brief Get the operations available on the @p device.
 * @param[in] num_entries the number of entries that can be added to @p devices.
 * If @p devices is not @c NULL, @p num_entries must be greater than zero.
 * @param[out] operations returns a list of operations available on the device.
 * The @ref QDMI_Operation values returned in @p operations can be used to
 * identify a specific @ref QDMI_Operation. If @p operations is @c NULL, this
 * argument is ignored. The number of @ref QDMI_Operation values returned is the
 * minimum of the value specified by @p num_entries and the number of operations
 * found.
 * @param[out] num_operations returns the number of operations available. If @p
 * num_operations is @c NULL, this argument is ignored.
 * @return @ref QDMI_SUCCESS if the function is executed successfully.
 * Otherwise, it returns one of the following error codes:
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p num_entries is less than or
 * equal to zero and @p operations is not @c NULL or if both @p operations and
 * @p num_operations are @c NULL.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 */
int QDMI_query_get_operations_dev(int num_entries, QDMI_Operation *operations,
                                  int *num_operations);

/**
 * @brief Query a device property.
 * @details This function queries a property of a device. The property can be
 * one of the predefined properties in @ref QDMI_Device_Property. The property
 * can be of different types, such as defined for the individual property. The
 * function returns the value of the property in the memory pointed to by @p
 * value.
 * @param[in] prop is an enumeration constant that identifies the platform
 * information being queried. It can be one of the values specified for @ref
 * QDMI_Device_Property.
 * @param[in] size specifies the size in bytes of memory pointed to by @p value.
 * This size in bytes must be greater or equal to the size of the return type
 * specified for the @ref QDMI_Device_Property @p prop.
 * @param[out] value is a pointer to a memory location where appropriate values
 * as specified for the @ref QDMI_Device_Property @p prop, will be returned. If
 * @p value is @c NULL, it is ignored.
 * @param[out] size_ret returns the actual size in bytes of data being queried
 * by @p prop. If @p size_ret is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the function is executed successfully.
 * Otherwise, it returns one of the following error codes:
 * @return @ref QDMI_ERROR_INVALIDARGUMENT
 * if @p prop is not one of the defined values, if the size in bytes specified
 * by @p size is less than the size of the data being queried as specified for
 * the @ref QDMI_Device_Property @p prop and @p value is not a @c NULL value, or
 * if both @p value and @p size_ret are @c NULL.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the property is not supported by the
 * device.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 */
int QDMI_query_device_property_dev(QDMI_Device_Property prop, int size,
                                   void *value, int *size_ret);

/**
 * @brief Query a site property.
 * @details This function queries a property of a site. The property can be
 * one of the predefined properties in @ref QDMI_Site_Property. The property
 * can be of different types, such as defined for the individual property. The
 * function returns the value of the property in the memory pointed to by @p
 * value.
 * @param[in] site is the index of the site for which the property is queried.
 * @param[in] prop is an enumeration constant that identifies the platform
 * information being queried. It can be one of the values specified for @ref
 * QDMI_Site_Property.
 * @param[in] size specifies the size in bytes of memory pointed to by @p value.
 * This size in bytes must be greater or equal to the size of the return type
 * specified for the @ref QDMI_Site_Property @p prop.
 * @param[out] value is a pointer to a memory location where appropriate values
 * as specified for the @ref QDMI_Site_Property @p prop, will be returned. If
 * @p value is @c NULL, it is ignored.
 * @param[out] size_ret returns the actual size in bytes of data being queried
 * by @p prop. If @p size_ret is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the function is executed successfully.
 * Otherwise, it returns one of the following error codes:
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p site is an invalid site,
 * if @p prop is not one of the defined values, if the size in bytes specified
 * by @p size is less than the size of the data being queried as specified for
 * the @ref QDMI_Site_Property @p prop and @p value is not a @c NULL value, or
 * if both @p value and @p size_ret are @c NULL.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the property is not supported by the
 * device.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 */
int QDMI_query_site_property_dev(QDMI_Site site, QDMI_Site_Property prop,
                                 int size, void *value, int *size_ret);

/**
 * @brief Query an operation property.
 * @details This function queries a property of an operation. The property can
 * be one of the predefined properties in @ref QDMI_Operation_Property. The
 * property can be of different types, such as defined for the individual
 * property. The function returns the value of the property in the memory
 * pointed to by @p value.
 *
 * @par
 * The device may return @ref QDMI_ERROR_NOTSUPPORTED also if the queried
 * property cannot be provided for the given sites. E.g., some properties may
 * only be available as an average value for all sites. Those may, nevertheless,
 * be queried through this function by providing no sites at all, i.e., passing
 * @c NULL to @p sites.
 * @param[in] operation is the name of the operation for which the property is
 * queried.
 * @param[in] num_sites is the number of sites for which the property is
 * queried.
 * @param[in] sites is a list of indices of the sites for which the property is
 * queried or can be @c NULL. If @p sites is @c NULL, the property is queried
 * for all sites, e.g., as an average value over all sites.
 * @param[in] prop is an enumeration constant that identifies the platform
 * information being queried. It can be one of the values specified for @ref
 * QDMI_Site_Property.
 * @param[in] size specifies the size in bytes of memory pointed to by @p value.
 * This size in bytes must be greater or equal to the size of the return type
 * specified for the @ref QDMI_Site_Property @p prop.
 * @param[out] value is a pointer to a memory location where appropriate values
 * as specified for the @ref QDMI_Site_Property @p prop, will be returned. If
 * @p value is @c NULL, it is ignored.
 * @param[out] size_ret returns the actual size in bytes of data being queried
 * by @p prop. If @p size_ret is @c NULL, it is ignored.
 * @return @ref QDMI_SUCCESS if the function is executed successfully.
 * Otherwise, it returns one of the following error codes:
 * @return @ref QDMI_ERROR_INVALIDARGUMENT if @p operation is an invalid
 * operation, if @p num_sites is less than or equal to zero and @p sites is not
 * @c NULL, if
 * @p prop is not one of the defined values, if the size in bytes specified by
 * @p size is less than the size of the data being queried as specified for the
 * @ref QDMI_Site_Property @p prop and @p value is not a @c NULL value, or if
 * both @p value and @p size_ret are @c NULL.
 * @return @ref QDMI_ERROR_NOTSUPPORTED if the property is not supported by the
 * device or for the given list of sites.
 * @return @ref QDMI_ERROR_FATAL if an unexpected error occurred.
 */
int QDMI_query_operation_property_dev(QDMI_Operation operation, int num_sites,
                                      const QDMI_Site *sites,
                                      QDMI_Operation_Property prop, int size,
                                      void *value, int *size_ret);

#ifdef __cplusplus
} // extern "C"
#endif
