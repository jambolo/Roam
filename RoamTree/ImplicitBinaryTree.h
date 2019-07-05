#if !defined( IMPLICITBINARYTREE_H_INCLUDED )
#define IMPLICITBINARYTREE_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                 ImplicitBinaryTree.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Roam/RoamTree/ImplicitBinaryTree.h#3 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include <memory>



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

template < class N >
class ImplicitBinaryTree
{
public:

	class NodeRef
	{
	public:

		enum
		{
			INVALID		= 0,	// This is an invalid NodeRef value
			ROOT		= 1
		};

		NodeRef( size_t i );

		operator size_t() const;

		NodeRef	Left() const;
		NodeRef	Right() const;
		NodeRef	Parent() const;

		bool	IsLeft() const;
		bool	IsRight() const;
		bool	IsRoot() const;

		int		Depth() const;

	private:

		size_t	m_I;
	};

	explicit ImplicitBinaryTree( int depth );

	ImplicitBinaryTree( std::auto_ptr< N > paData, size_t size );

	~ImplicitBinaryTree();

	N &			operator []( NodeRef i );
	N const &	operator []( NodeRef i ) const;

	NodeRef	Root() const	{ return NodeRef::ROOT; }
	size_t	Size() const	{ return m_Size; }

private:

	enum { OFFSET = 1 };	// In order to avoid wasting space, the data
							// is shifted left by 1

	N *					m_paData;
	size_t					m_Size;
};


#endif // !defined( IMPLICITBINARYTREE_H_INCLUDED )
