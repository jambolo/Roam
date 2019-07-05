#if !defined( DATATREE_H_INCLUDED )
#define DATATREE_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                      DataTree.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Roam/RoamTree/DataTree.h#2 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "BinaryTree.h"
#include "GridTriangle.h"
#include <Glx/Glx.h>
#include <memory.h>



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class DataTree : public BinaryTree
{
public:

	class Node;	// Declared below

	/// Constructor
	DataTree( GridTriangle const & gt, Glx::TLVertex * paVertexGrid, int size );

	/// @name Overrides BinaryTree
	//@{
	Node * Root();
	Node const * Root() const;
	Node * FirstLeaf();
	Node const * FirstLeaf() const;
	Node * NextLeaf( Node * pNode );
	Node const * NextLeaf( Node const * pNode ) const;
	void Grow( Node * pNode );
	void Insert( Node * pNode, std::auto_ptr< Node > pLeft, std::auto_ptr< Node > pRight );
	//	void DeleteTree( Node * pNode );
	//	void SetRoot( Node * pRoot );
	//	void Insert( Node * pNode, Node * pLeft, Node * pRight );
	//@}


	Node *				m_pRoot;
	Glx::TLVertex *		m_paVertexGrid;
	int					m_Size;
};


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class DataTree::Node : public BinaryTree::Node
{
public:
	Node() {}
	virtual ~Node() {}

	/// @name Overrides BinaryTree::Node
	//@{
	//	void SetParent( Node * pParent );
	Node * Parent();
	Node const * Parent() const;
	//	void SetLeftChild( Node * pLeft );
	Node * LeftChild();
	Node const * LeftChild() const;
	//	void SetRightChild( Node * pRight );
	Node * RightChild();
	Node const * RightChild() const;
	//	bool IsALeaf() const;
	//@}

	Glx::TLVertex	*	m_pV0;
	Glx::TLVertex	*	m_pV1;
	Glx::TLVertex	*	m_pV2;
	Vector3				m_Normal;
	float				m_MaxVariance;
	GridTriangle		m_Gt;
};


#endif // !defined( DATATREE_H_INCLUDED )
