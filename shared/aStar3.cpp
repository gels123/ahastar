/*
 * $Id: aStar3.cpp,v 1.2 2006/09/18 06:19:31 nathanst Exp $
 *
 *  Hierarchical Open Graph File
 *
 *  Created by Nathan Sturtevant on 9/29/04.
 *  Copyright 2004 Nathan Sturtevant. All rights reserved.
 *
 * This file is part of HOG.
 *
 * HOG is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * HOG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with HOG; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "fpUtil.h"
#include "aStar3.h"
#include "heap.h"

static const int verbose = 0;

// The constructor
aStarOld::aStarOld(double _weight, bool _doPathDraw)
:searchAlgorithm()
{
	wh = _weight;
	doPathDraw = _doPathDraw;
	// Generate algorithm's name
	if (fequal(wh,1.0))
		strcpy(aStarName,"aStarOld");
	else
		sprintf(aStarName,"aStarOld(%1.1f)",wh);
}


// The same A*, but counts the number of states expanded
path *aStarOld::getPath(graphAbstraction *aMap, node *from, node *to, reservationProvider *rp)
{
	// Reset the number of states expanded
	nodesExpanded = 0;
	nodesTouched = 0;
	
	if ((from == 0) || (to == 0) || (!aMap->pathable(from, to)) || (from == to))
		return 0;
	map = aMap;
	graph *g = map->getAbstractGraph(from->getLabelL(kAbstractionLevel));
	heap *openList = new heap(30);
	std::vector<node *> closedList;
	node *n;
	
	// label start node cost 0
	n = from;
	n->setLabelF(kTemporaryLabel, wh * map->h(n, to));
	n->markEdge(0);
	
	while (1)
	{
		nodesExpanded++;
		edge_iterator ei;
		
		// move current node onto closed list
		// mark node with its location in the closed list
		closedList.push_back(n);
		n->key = closedList.size()-1;
		
		if (verbose)
			printf("Working on %d with cost %1.2f\n", n->getNum(), n->getLabelF(kTemporaryLabel));
		
		ei = n->getEdgeIter();
		
		// iterate over all the children
		for (edge *e = n->edgeIterNext(ei); e; e = n->edgeIterNext(ei))
		{
			nodesTouched++;
			unsigned int which;
			if ((which = e->getFrom()) == n->getNum()) which = e->getTo();
			
			node *nextChild = g->getNode(which);
			
			// if it's on the open list, we can still update the weight
			if (openList->isIn(nextChild))
			{
				//nodesExpanded++;
				relaxEdge(openList, g, e, n->getNum(), which, to);
			}
			else if (rp && (from->getLabelL(kAbstractionLevel)==0) && (nextChild != to) &&
							 rp->nodeOccupied(nextChild))
			{
				//printf("Can't path to %d, %d\n", (unsigned int)nextChild->getLabelL(kFirstData), (unsigned int)nextChild->getLabelL(kFirstData+1));
				closedList.push_back(nextChild);
				nextChild->key = closedList.size()-1;
				// ignore this tile if occupied.
			}
			// if it's not on the open list, then add it to the open list
			else if ((nextChild->key >= closedList.size()) ||
							 (closedList[nextChild->key] != nextChild))
			{
				nextChild->setLabelF(kTemporaryLabel, MAXINT); // initial fCost = infinity
				nextChild->setKeyLabel(kTemporaryLabel);
				nextChild->markEdge(0);
				openList->add(nextChild);
				if (verbose)
					printf("Adding neighbor/child %d\n", which);
				//nodesExpanded++;
				relaxEdge(openList, g, e, n->getNum(), which, to);
			}
		}
		
		// get the next (the best) node off the open list
		n = (node*)openList->remove();
		
		// this means we have expanded all reachable nodes and there is no path
		if (n == 0) { delete openList; return 0; }
		if (verbose) printf("Expanding %d\n", n->getNum());
		if (n == to) break; // we found the goal
	}
	delete openList;
	return extractBestPath(g, n->getNum());
}

// this is the standard definition of relaxation as in Introduction to Algorithms (cormen, leiserson and rivest)
void aStarOld::relaxEdge(heap *nodeHeap, graph *g, edge *e, int source, int nextNode, node *d)
{
	double weight;
	node *from = g->getNode(source);
	node *to = g->getNode(nextNode);
	
	/* alternate path fCost = from.gCost (derived as from.fCost - from.hCost) + to.hCost + length(from, to) */
	/*double hTo = wh*map->h(to, d);
	double hFrom = wh*map->h(from, d);
	double edgeweight = e->getWeight();
	double oldgcost = from->getLabelF(kTemporaryLabel);
	double gcost = from->getLabelF(kTemporaryLabel)-wh*map->h(from, d);
	double hcost = wh*map->h(to, d)+e->getWeight(); */

	weight = from->getLabelF(kTemporaryLabel)-wh*map->h(from, d)+wh*map->h(to, d)+e->getWeight(); 
	
	if (fless(weight, to->getLabelF(kTemporaryLabel)))
	{
		if (verbose)
			printf("Updating %d to %1.2f from %1.2f\n", nextNode, weight, to->getLabelF(kTemporaryLabel));
		to->setLabelF(kTemporaryLabel, weight);
		nodeHeap->decreaseKey(to); // move the node up in priority
		to->markEdge(e); // this is the edge used to get to this node in the min. path tree
	}
}

path *aStarOld::extractBestPath(graph *g, unsigned int current)
{
	path *p = 0;
	edge *e;
	// extract best path from graph -- each node has a single parent in the graph which is the marked edge
	// for visuallization purposes, an edge can be marked meaning it will be drawn in white
	while ((e = g->getNode(current)->getMarkedEdge()))
	{
		if (verbose) printf("%d <- ", current);
		
		p = new path(g->getNode(current), p);
		
		if (doPathDraw)
			e->setMarked(true);
		
		if (e->getFrom() == current)
			current = e->getTo();
		else
			current = e->getFrom();
	}
	p = new path(g->getNode(current), p);
	if (verbose) printf("%d\n", current);
	return p;	
}
