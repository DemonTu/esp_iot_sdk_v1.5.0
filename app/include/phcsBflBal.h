/* /////////////////////////////////////////////////////////////////////////////////////////////////
//                     Copyright (c) Philips Semiconductors
//
//                       (C)PHILIPS Electronics N.V.2004
//         All rights are reserved. Reproduction in whole or in part is
//        prohibited without the written consent of the copyright owner.
//    Philips reserves the right to make changes without notice at any time.
//   Philips makes no warranty, expressed, implied or statutory, including but
//   not limited to any implied warranty of merchantability or fitness for any
//  particular purpose, or that the use will not infringe any third party patent,
//   copyright or trademark. Philips must not be liable for any loss or damage
//                            arising from its use.
///////////////////////////////////////////////////////////////////////////////////////////////// */

/*!
 * \if cond_generate_filedocu
 * \file phcsBflBal.h
 *
 * Project: Object Oriented Library Framework phcsBfl_Bal Component.
 *
 *  Source: phcsBflBal.h
 * $Author: mha $
 * $Revision: 1.3 $
 * $Date: Thu Oct 27 13:27:13 2005 $
 *
 * Comment:
 *  This is the main entry file for the C implemetation of the Bal component and shall be the same
 *  for all implementations.
 *
 * History:
 *  GK:  Generated 20. March 2003
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */

#ifndef PHCSBFLBAL_H
#define PHCSBFLBAL_H

/* Type defintions */
#include <phcsBflStatus.h>
#include <phcsBflRefDefs.h>

/*! \ingroup bal
 *  \brief Parameter structure for the phcsBfl_Bal components WriteBus functionality.
 */
typedef struct
{
    uint8_t     bus_data;  /*!< \brief [in,out] Data to write to (phcsBflBal_WriteBusParam_t) / read from
                                       (phcsBflBal_ReadBusParam_t) via the connecting bus. */
    void       *self;      /*!< \brief [in] Pointer to the C-interface (\ref phcsBflBal_t) in
                                     order to let this member function find its "object".
                                     Should be left dangling when using the C++ interface. */
} phcsBflBal_WriteBusParam_t;

/*! \name Data Structures Typedefs */
/*@{*/
/*! \ingroup bal */
/*! \brief Parameter structure for the phcsBfl_Bal ReadBus functionality. \n
 *  This structure is an alias of the phcsBflBal_WriteBusParam_t structure. */
typedef phcsBflBal_WriteBusParam_t phcsBflBal_ReadBusParam_t;
/*@}*/

/*! \ingroup bal
 *  \brief Parameter structure for the phcsBfl_Bal components Change Host baudrate functionality.
 */
typedef struct
{
    uint32_t    baudrate;  /*!< \brief [in] Data rate to be set on com port.     */
    void       *self;      /*!< \brief [in] Pointer to the C-interface (\ref phcsBflBal_t) in
                                     order to let this member function find its "object".
                                     Should be left dangling when using the C++ interface. */
} phcsBflBal_HostBaudParam_t;

/*! \ingroup bal
 *  \brief Parameter structure for the phcsBfl_Bal components Close Port functionality.
 */
typedef struct
{
    void       *self;      /*!< \brief [in] Pointer to the C-interface (\ref phcsBflBal_t) in
                                     order to let this member function find its "object".
                                     Should be left dangling when using the C++ interface. */
} phcsBflBal_ClosePortParam_t;



/*!
 * \if sec_PHFL_BFL_CPP
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflBal_Wrapper< phcsBflBcp >::WriteBus(phcsBflBal_WriteBusParam_t *writebus_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflBal_t::WriteBus()
 * \endif
 *
 * \ingroup bal
 * \par Parameters:
 * \li \ref phcsBflBal_WriteBusParam_t [in,out]: Pointer to the register I/O parameter structure.
 *
 * \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 * \retval  PH_ERR_BFL_INVALID_DEVICE_STATE   Internal state machine in different state.
 * \retval  PH_ERR_BFL_INTERFACE_ERROR        Error on input parameters.
 * \retval  PH_ERR_BFL_INSUFFICIENT_RESOURCES Not enough resources for component generation.
 * \retval  Other                             Depending on implementation.
 *
 * This function writes the data byte directly to the RS232 interface of a PC.
 * Interface specific handling as mentioned in the data sheet is done in this function.
 */
typedef phcsBfl_Status_t (*pphcsBflBal_WriteBus_t) (phcsBflBal_WriteBusParam_t*);

/*!
 * \if sec_PHFL_BFL_CPP
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflBal_Wrapper< phcsBflBcp >::ReadBus(phcsBflBal_ReadBusParam_t *readbus_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflBal_t::ReadBus
 * \endif
 *
 * \ingroup bal
 * \par Parameters:
 * \li \ref phcsBflBal_ReadBusParam_t[in,out]: Pointer to the register I/O parameter structure.
 *
 * \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 * \retval  PH_ERR_BFL_INVALID_DEVICE_STATE   Internal state machine in different state.
 * \retval  PH_ERR_BFL_INTERFACE_ERROR        Error on input parameters.
 * \retval  PH_ERR_BFL_INSUFFICIENT_RESOURCES Not enough resources for component generation.
 * \retval  Other                             Depending on implementation.
 *
 * This function reads one byte from the hardware using the RS232 interface o a PC.
 * Interface specific handling as mentioned in the data sheet is done in this function.
 */
typedef phcsBfl_Status_t (*pphcsBflBal_ReadBus_t)  (phcsBflBal_ReadBusParam_t*);

/*!
 * \if sec_PHFL_BFL_CPP
 * \fn void phcs_BFL::phcsBflBal_Wrapper< phcsBflBcp >::ChangeHostBaud(phcsBflBal_HostBaudParam_t *hostbaud_param);
 * \else
 * \fn void phcsBflBal_t::ChangeHostBaud()
 * \endif
 *
 * \ingroup bal
 * \par Parameters:
 * \li \ref phcsBflBal_HostBaudParam_t [in]: Pointer to the I/O parameter structure.
 *
 * This function adjusts the host data rate of the com port to the value of phcsBflBal_HostBaudParam_t.
 */
typedef void (*pphcsBflBal_HostBaud_t)  (phcsBflBal_HostBaudParam_t*);

/*!
 * \if sec_PHFL_BFL_CPP
 * \fn void phcs_BFL::phcsBflBal_Wrapper< phcsBflBcp >::ClosePort(phcsBflBal_ClosePortParam_t *closeport_param);
 * \else
 * \fn void phcsBflBal_t::ClosePort
 * \endif
 *
 * \ingroup bal
 * \par Parameters:
 * \li \ref phcsBflBal_ClosePortParam_t [in]: Pointer to the I/O parameter structure.
 *
 * This function closes the com port used in the bal if opened or activated before.
 */
typedef void (*pphcsBflBal_ClosePort_t)  (phcsBflBal_ClosePortParam_t*);
/*@}*/


/*! \ifnot sec_PHFL_BFL_CPP
 *
 *  \ingroup bal
 *  \brief  C-interface structure of the phcsBfl_Bal component.
 *
 *  The phcsBfl_Bal component takes away the need to be aware of different bus-interface specifics
 *  from upper layers. Usually the phcsBfl_Bal component is the lowest layer of the stack. In most
 *  cases, the bal can be omitted. Sometimes however it can be useful, especially when using the
 *  RS 232 serial link on PCs with different operating systems.
 *
 * \endif
 */
typedef struct
{
    /* Methods: */
    pphcsBflBal_WriteBus_t      WriteBus;       /* WriteBus member function. */
    pphcsBflBal_ReadBus_t       ReadBus;        /* ReadBus member function. */
    pphcsBflBal_HostBaud_t      ChangeHostBaud; /* Baudrate change member function. */
    pphcsBflBal_ClosePort_t     ClosePort;      /* ClosePort member function. */

    void *mp_Members;   /* Internal variables of the C-interface. Usually a structure is behind
                           this pointer. The type of the structure depends on the implementation
                           requirements. */
    #ifdef PHFL_BFL_CPP
        void       *mp_CallingObject;   /* Used by the "Glue-Class" to reference the wrapping
                                           C++ object, calling into the C-interface. */
    #endif

   /* phcsBfl_Bal has no lower edge */
} phcsBflBal_t;






/* //////////////////////////////////////////////////////////////////////////////////////////////
// Bal Initialise for Windows:
*/
/*! \ingroup bal
 *  \brief Control variable structure for phcs_BFLBal initialization for RS232 on Windows. This
 *         variable is used for paramter storage and shall not be re-used for anything else!
 */
typedef struct
{
    void          *com_handle;      /*!< \brief [in,out] Handle to currently opened port. If zero
                                                com_port and data_rate parameters are used for
                                                configuration and no new port is opened. */
    int8_t        *com_port;        /*!< \brief [in] Character string of com port name. Data rate
                                                is set to 9600. */
} phcsBflBal_Hw1SerWinInitParams_t;

/*!
 * \ingroup bal
 * \param[in,out] *cif                       C object interface structure.
 * \param[in,out] *communication_parameters  Pointer to the communication parameter structure.
 *
 * This function should be called first to initialise the phcsBfl_Bal component and set all
 * required parameters.
 *
 */
void phcsBflBal_Hw1SerWinInit(phcsBflBal_t *cif, void *communication_parameters);



/* //////////////////////////////////////////////////////////////////////////////////////////////
// Bal Initialise for Linux:
*/
/*! \ingroup bal
 *  \brief Control variable structure for phcs_BFLBal initialization for RS232 on Linux. For
 *         the current implementation we only require a handle to the PCs pre-opened RS232 link.
 */
typedef struct
{
    void          *com_handle;      /*!< \brief [in,out] Handle to currently opened port. If zero
                                                com_port and data_rate parameters are used for
                                                configuration and no new port is opened. */
    int8_t        *com_port;        /*!< \brief [in] Character string of com port name. Data rate
                                                is set to 9600. */
} phcsBflBal_Hw1SerLinInitParams_t;







#endif /* PHCSBFLBAL_H */
