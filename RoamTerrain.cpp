/** @file *//********************************************************************************************************

                                                   RoamTerrain.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Roam/RoamTerrain.cpp#3 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "RoamTerrain.h"

#include "RoamTree/RoamTree.h"
#include "RoamTree/GridTriangle.h"

#include <memory>


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

RoamTerrain::RoamTerrain( DataTree const * pLowerLeftData,
						  DataTree const * pUpperRightData,
						  RoamTree::SplitParameters const & splitParameters )
{
	// Create the Roam trees

	m_pLowerLeft = new RoamTree( pLowerLeftData, splitParameters );
	if ( !m_pLowerLeft ) throw std::bad_alloc();

	m_pUpperRight = new RoamTree( pUpperRightData, splitParameters );
	if ( !m_pUpperRight ) throw std::bad_alloc();

	// Assign neighbors

	m_pLowerLeft->MeetTheNeighbors( m_pUpperRight, 0, 0 );
	m_pUpperRight->MeetTheNeighbors( m_pLowerLeft, 0, 0 );

	// Tesselate

	m_pLowerLeft->Tesselate();
	m_pUpperRight->Tesselate();
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

RoamTerrain::~RoamTerrain()
{
	delete m_pLowerLeft;
	delete m_pUpperRight;
}
