/*! \file	process_metadata.h
 *	\brief	MXF wrapping
 *
 *	\version $Id$
 *
 */
/*
 *	Copyright (c) 2011, Metaglue Corporation. All Rights Reserved.
 */

#ifndef _process_metadata_h_
#define _process_metadata_h_

#include "mxflib/mxflib.h"
using namespace mxflib;

#include "libprocesswrap/process.h"


	void ProcessMetadata(		int									OutFileNum,
								ProcessOptions						*pOpt,
								EssenceSourcePair					*Source,
								EssenceParser::WrappingConfigList	WrapCfgList,
								Rational							EditRate,
								BodyWriterPtr						Writer,
								MetadataPtr							MData,
								UMIDPtr								MPUMID,
								UMIDPtr								*FPUMID,
								UMIDPtr								*SPUMID,
								EssenceStreamInfo					*EssStrInf,
								PackagePtr							&FilePackage,  //OUT variable
								TimecodeComponentPtr				&MPTimecodeComponent //OUT variable
							);

#endif // _process_metadata_h_
