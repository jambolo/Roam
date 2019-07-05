#if !defined( ROAMTREE_H_INCLUDED )
#define ROAMTREE_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                      RoamTree.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Roam/RoamTree/RoamTree.h#4 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "BinaryTree.h"

#include "DataTree.h"

#include "Math/Vector3.h"
#include "Glx/Vertex.h"

class GridTriangle;


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// A ROAM tree class.

class RoamTree : public BinaryTree
{
public:

	class Node;	// Declared below

	struct SplitParameters
	{
		SplitParameters() {}
		SplitParameters( Vector3 const & cameraPosition, Vector3 const & cameraDirection, float cameraFov, float tolerance )
			: m_CameraPosition( cameraPosition ),
			m_CameraDirection( cameraDirection ),
			m_CameraFov( cameraFov ),
			m_Tolerance( tolerance )
		{
		}

		Vector3		m_CameraPosition;
		Vector3		m_CameraDirection;
		float		m_CameraFov;
		float		m_Tolerance;
	};

	/// Constructor
	RoamTree();

	/// Constructor
	RoamTree( DataTree const * pData, SplitParameters const & splitParameters );

	/// Destructor
	virtual ~RoamTree();

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

public:

	/// Assigns the neighbors for this tree
	void MeetTheNeighbors( RoamTree * m_pBottomNeighbor, RoamTree * pLeftNeighbor, RoamTree * pRightNeighbor );

	/// Tesselates the tree
	void Tesselate();

private:

	/// Splits a node
	void SplitNode( Node * pNode );

	/// Tesselates the tree rooted at this node
	void TesselateTree( Node * pNode );

	/// Returns true if the node should be split
	bool MeetsTheSplitCriteria( DataTree::Node const & data );

	SplitParameters const	m_SplitParameters;		///< Parameters for determining if a node should be split or not
	DataTree const *		m_pData;				///< All triangle data
};



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// A ROAM tree node.
class RoamTree::Node : public BinaryTree::Node
{
public:

	/// Constructor
	Node( Node * pParent, DataTree::Node const * pData, Node * pLeft = 0, Node * pRight = 0 );

	/// Destructor
	virtual ~Node();

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

	// Sets this node's neighbors
	void MeetTheNeighbors( Node * pBottomNeighbor, Node * pLeftNeighbor, Node * pRightNeighbor );

	/// Sets the bottom neighbor node.
	void SetBottomNeighbor( Node * pBottomNeighbor );

	/// Returns the bottom neighbor node.
	Node * BottomNeighbor();

	/// Returns the bottom neighbor node.
	Node const * BottomNeighbor() const;

	/// Sets the left neighbor node.
	void SetLeftNeighbor( Node * pLeftNeighbor );

	/// Returns the left neighbor node.
	Node * LeftNeighbor();

	/// Returns the left neighbor node.
	Node const * LeftNeighbor() const;

	/// Sets the right neighbor node.
	void SetRightNeighbor( Node * pRightNeighbor );

	/// Returns the right neighbor node.
	Node * RightNeighbor();

	/// Returns the right neighbor node.
	Node const * RightNeighbor() const;

	/// Returns the data for this node
	DataTree::Node const *	GetData() const;

	/// Returns the GridTriangle representation of this node.
	GridTriangle const & GetGridTriangle() const;

	/// Returns the normal
	Vector3 const & GetNormal() const;

	/// Returns the maximum variance allowed before splitting
	float GetMaxVariance() const;

	/// Returns vertex 0
	Glx::TLVertex const * GetV0() const;

	/// Returns vertex 1
	Glx::TLVertex const * GetV1() const;

	/// Returns vertex 2
	Glx::TLVertex const * GetV2() const;

	/// (Re-)Assigns all the neighbors in the neighborhood after this node is split
	void FixUpTheNeighborhoodAfterSplit();

protected:

	Node *					m_pBottomNeighbor;			///< The bottom neighbor
	Node *					m_pLeftNeighbor;			///< The left neighbor
	Node *					m_pRightNeighbor;			///< The right neighbor
	DataTree::Node const *	m_pData;					///< Data about this node
};


#include "RoamTree.inl"


#endif // !defined( ROAMTREE_H_INCLUDED )
