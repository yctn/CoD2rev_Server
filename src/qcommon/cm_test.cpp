#include "qcommon.h"
#include "cm_local.h"

#ifdef TESTING_LIBRARY
#define cm (*((clipMap_t*)( 0x08185BE0 )))
#define cme (*((clipMapExtra_t*)( 0x08185CF4 )))
#else
extern clipMap_t cm;
extern clipMapExtra_t cme;
#endif

/*
======================================================================
LEAF LISTING
======================================================================
*/

void CM_StoreLeafs( leafList_t *ll, int nodenum )
{
	int leafNum;

	leafNum = -1 - nodenum;

	// store the lastLeaf even if the list is overflowed
	if ( cm.leafs[ leafNum ].cluster != -1 )
	{
		ll->lastLeaf = leafNum;
	}

	if ( ll->count >= ll->maxcount )
	{
		ll->overflowed = qtrue;
		return;
	}

	ll->list[ ll->count++ ] = leafNum;
}

/*
=============
CM_BoxLeafnums
Fills in a list of all the leafs touched
=============
*/
void CM_BoxLeafnums_r( leafList_t *ll, int nodenum )
{
	cplane_t    *plane;
	cNode_t     *node;
	int s;

	while ( nodenum >= 0 )
	{
		node = &cm.nodes[nodenum];
		plane = node->plane;
		s = BoxOnPlaneSide( ll->bounds[0], ll->bounds[1], plane );
		if ( s == 1 )
		{
			nodenum = node->children[0];
		}
		else
		{
			if ( s != 2 )
			{
				CM_BoxLeafnums_r(ll, node->children[0]);
			}
			nodenum = node->children[1];
		}
	}

	CM_StoreLeafs( ll, nodenum );
}

/*
==================
CM_BoxLeafnums
==================
*/
int CM_BoxLeafnums( const vec3_t mins, const vec3_t maxs, uint16_t *list, int listsize, int *lastLeaf )
{
	leafList_t ll;

	VectorCopy( mins, ll.bounds[0] );
	VectorCopy( maxs, ll.bounds[1] );
	ll.count = 0;
	ll.maxcount = listsize;
	ll.list = list;
	ll.lastLeaf = 0;
	ll.overflowed = qfalse;

	CM_BoxLeafnums_r( &ll, 0 );

	*lastLeaf = ll.lastLeaf;

	return ll.count;
}