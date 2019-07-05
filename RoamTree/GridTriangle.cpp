/** @file *//********************************************************************************************************

                                                   GridTriangle.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Roam/RoamTree/GridTriangle.cpp#3 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "GridTriangle.h"

#include <cassert>

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void GridTriangle::ChildTop( int *pChildX, int * pChildY ) const
{
	// Fill in the vertex pointers

	int const	size2	= m_Size / 2;

	switch( m_Orientation )
	{
	case O_UP:
		*pChildX = m_X + 0;
		*pChildY = m_Y + size2;
		break;

	case O_UPRIGHT:
		*pChildX = m_X + size2;
		*pChildY = m_Y + size2;
		break;

	case O_RIGHT:
		*pChildX = m_X + size2;
		*pChildY = m_Y + 0;
		break;

	case O_DOWNRIGHT:
		*pChildX = m_X + size2;
		*pChildY = m_Y - size2;
		break;

	case O_DOWN:
		*pChildX = m_X + 0;
		*pChildY = m_Y - size2;
		break;

	case O_DOWNLEFT:
		*pChildX = m_X - size2;
		*pChildY = m_Y - size2;
		break;

	case O_LEFT:
		*pChildX = m_X - size2;
		*pChildY = m_Y + 0;
		break;

	case O_UPLEFT:
		*pChildX = m_X - size2;
		*pChildY = m_Y + size2;
		break;
	}
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

int GridTriangle::ChildSize() const
{
	// Assumes that diagonally facing triangles are even
	assert( O_UPRIGHT == 0 );

	return ( m_Orientation & 1 ) ? m_Size / 2 : m_Size;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

GridTriangle::Orientation GridTriangle::LeftChildOrientation() const
{
	// Assume that order is clockwise starting at O_UPRIGHT
	assert( GridTriangle::O_UPRIGHT == 0 );

	static Orientation const	leftChildOrientationMap[] =
	{
		O_DOWN,
		O_DOWNLEFT,
		O_LEFT,
		O_UPLEFT,
		O_UP,
		O_UPRIGHT,
		O_RIGHT,
		O_DOWNRIGHT,
	};

	return leftChildOrientationMap[ m_Orientation ];
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

GridTriangle::Orientation GridTriangle::RightChildOrientation() const
{
	// Assume that order is clockwise starting at O_UPRIGHT
	assert( O_UPRIGHT == 0 );

	static Orientation const	rightChildOrientationMap[] =
	{
		O_LEFT,
		O_UPLEFT,
		O_UP,
		O_UPRIGHT,
		O_RIGHT,
		O_DOWNRIGHT,
		O_DOWN,
		O_DOWNLEFT,
	};

	return rightChildOrientationMap[ m_Orientation ];
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

GridTriangle GridTriangle::LeftChild() const
{
	int topX, topY;
	ChildTop( &topX, &topY );

	return GridTriangle( topX, topY, LeftChildOrientation(), ChildSize() );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

GridTriangle GridTriangle::RightChild() const
{
	int topX, topY;
	ChildTop( &topX, &topY );

	return GridTriangle( topX, topY, RightChildOrientation(), ChildSize() );
}
