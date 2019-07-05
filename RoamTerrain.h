#if !defined( ROAMTERRAIN_H_INCLUDED )
#define ROAMTERRAIN_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                    RoamTerrain.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Roam/RoamTerrain.h#3 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "Glx/Glx.h"

#include "RoamTree/RoamTree.h"

class DataTree;

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class RoamTerrain
{
public:

	RoamTerrain( DataTree const * pLowerLeftData,
				 DataTree const * pUpperRightData,
				 RoamTree::SplitParameters const & splitParameters );
	virtual ~RoamTerrain();

	RoamTree *	m_pLowerLeft;	// RoamTree in the lower left corner
	RoamTree *	m_pUpperRight;	// RoamTree in the upper right corner
};


#endif // !defined( ROAMTERRAIN_H_INCLUDED )
