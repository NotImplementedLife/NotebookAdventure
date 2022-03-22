/*! \file error.hpp
	\brief error messages and execution halting routines
 */
#pragma once

#include <gba.h>

/*! \enum ErrorCode
	\brief possible error codes
 */
enum ErrorCode 
{
	ERR_UNKNOWN,          //!< Unknown error
	ERR_OAM_FULL,         //!< OAM limit exceeded
	ERR_OAM_INVALID_ID,   //!< Invalid OAM id
	ERR_OAM_ID_OVERFLOW,  //!< Too many sprites have been created
	ERR_DLG_EXCEEDED,     //!< Dialog boxes count exceeded
	ERR_DLG_INVALID_ID,   //!< Invalid dialog box id
	ERR_ARG_OUT_OF_RANGE  //!< Argument or index out of range
};

/*! \brief prints a message and an error code
 */
void fatal(const char* msg, u32 err_code = -1, const char* hint=NULL);

/*! \brief displays the error message specified by the error code
	\param code error code
 */
void fatal(ErrorCode code, const char* hint=NULL);