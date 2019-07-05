#if !defined( GRIDTRIANGLE_H_INCLUDED )
#define GRIDTRIANGLE_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                    GridTriangle.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Roam/RoamTree/GridTriangle.h#3 $

	$NoKeywords: $

 ********************************************************************************************************************/


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class GridTriangle
{
public:
	// The orientation specifies which way a triangle faces ("up" signifies
	// increasing y and "right" signifies increasing x)

	enum Orientation
	{
		O_UPRIGHT,
		O_RIGHT,
		O_DOWNRIGHT,
		O_DOWN,
		O_DOWNLEFT,
		O_LEFT,
		O_UPLEFT,
		O_UP
	};

	GridTriangle() {};
	GridTriangle( int x, int y, Orientation orientation, int size );

	// Get the top vertex of a child node
	void ChildTop( int *pChildX, int * pChildY ) const;

	// Get the size of a split triangle's child
	int ChildSize() const;

	// Get the orientation of a split triangle's children
	Orientation RightChildOrientation() const;
	Orientation LeftChildOrientation() const;


	// Return a GridTriangle representing the left child
	GridTriangle LeftChild() const;

	// Return a GridTriangle representing the right child
	GridTriangle RightChild() const;

	int			m_X, m_Y;
	Orientation	m_Orientation;
	int			m_Size;
};


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

inline GridTriangle::GridTriangle( int x, int y, Orientation orientation, int size )
	: m_X( x ), m_Y( y ), m_Orientation( orientation ), m_Size( size )
{
}


#endif // !defined( GRIDTRIANGLE_H_INCLUDED )
