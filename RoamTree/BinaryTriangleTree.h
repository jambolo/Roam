#if !defined( BINARYTRIANGLETREE_H_INCLUDED )
#define BINARYTRIANGLETREE_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                 BinaryTriangleTree.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Roam/RoamTree/BinaryTriangleTree.h#4 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "BinaryTree.h"
#include "Glx/Glx.h"
#include <memory>

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class BinaryTriangleTree : public BinaryTree
{
public:

	class Node;	// Declared below

	BinaryTriangleTree();
	virtual ~BinaryTriangleTree();

	/// @name Overrides BinaryTree
	//@{
	Node * Root();
	Node const * Root() const;
	Node * FirstLeaf();
	Node const * FirstLeaf() const;
	Node * NextLeaf( Node * pNode );
	Node const * NextLeaf( Node const * pNode ) const;
	virtual void Grow( Node * pNode );
	void Insert( Node * pNode, std::auto_ptr< Node > pLeft, std::auto_ptr< Node > pRight );
	//	void DeleteTree( Node * pNode );
	//	void SetRoot( Node * pRoot );
	//	void Insert( Node * pNode, Node * pLeft, Node * pRight );
	//@}
};




/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class BinaryTriangleTree::Node : public BinaryTree::Node
{
public:

	explicit Node( Node *					pParent	= 0,
					Glx::TLVertex const *	pV0		= 0,
					Glx::TLVertex const *	pV1		= 0,
					Glx::TLVertex const *	pV2		= 0,
					Node *					pLeft	= 0,
					Node *					pRight	= 0 );
	virtual ~Node();

	// *** Functions defined in the base class

	//	// Get/set the parent
	//	void SetParent( Node * pParent );
	Node * Parent()							{ return static_cast< Node * >( m_pParent ); }
	Node const * Parent() const				{ return static_cast< Node const * >( m_pParent ); }
	//	
	//	// Get/set the left child
	//	void SetLeftChild( Node * pLeft );
	Node * LeftChild()						{ return static_cast< Node * >( m_pLeftChild ); }
	Node const * LeftChild() const			{ return static_cast< Node const * >( m_pLeftChild ); }
	//										
	//	// Get/set the right child
	//	void SetRightChild( Node * pRight );
	Node * RightChild()						{ return static_cast< Node * >( m_pRightChild ); }
	Node const * RightChild() const			{ return static_cast< Node const * >( m_pRightChild ); }
	//
	//	// Return true if the node has no children
	//	bool IsALeaf() const;

	// *** End of functions defined in the base class

	// Get/set vertices
	void SetVertices( Glx::TLVertex const *	pV0, Glx::TLVertex const * pV1, Glx::TLVertex const * pV2 )
	{
		m_pV0 = pV0;
		m_pV1 = pV1;
		m_pV2 = pV2;
	}

	Glx::TLVertex const * GetV0() const					{ return m_pV0; }
	Glx::TLVertex const * GetV1() const					{ return m_pV1; }
	Glx::TLVertex const * GetV2() const					{ return m_pV2; }

protected:

	// Vertices are CCW, starting with the "top"
	Glx::TLVertex const *	m_pV0;
	Glx::TLVertex const *	m_pV1;
	Glx::TLVertex const *	m_pV2;
};



#endif // !defined( BINARYTRIANGLETREE_H_INCLUDED )
