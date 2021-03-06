/*
 * $Id: graph.h,v 1.9 2006/09/18 06:20:15 nathanst Exp $
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
*/ 

// HOG File

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <list>
#include <iostream>

#define MAXINT (1<<30)
//#define MAXLABELS 15

class graph;
class node;
class edge;

typedef std::vector<edge *>::const_iterator edge_iterator;
typedef std::vector<node *>::const_iterator node_iterator;
typedef unsigned int neighbor_iterator;

typedef union { double fval; long lval; } labelValue;

/**
 * Parent class for nodes and edges allowing them to be stored in a heap or
 * manipulated with other data structures.
 */
class graph_object {
public:
  graph_object():key(0) { debuginfo = false; 	uniqueID = uniqueIDCounter++; }
  virtual ~graph_object() {}
  int getUniqueID() const { return uniqueID; }
  virtual double getKey() { return 0; }
  virtual void Print(std::ostream&) const;
  virtual graph_object *clone() const = 0;
  unsigned int key; // for use by a data structure to maintain a reverse-lookup
  // to go from an object to a table key in constant time.
  void setDebugInfo(bool debug) { debuginfo = debug; }
  bool getDebugInfo() { return debuginfo; }

protected:
	  bool debuginfo;
private:
  int uniqueID;
  static unsigned int uniqueIDCounter;

  //	double val;
};

std::ostream& operator <<(std::ostream & out, const graph_object &_Obj);

/**
 * A generic graph class.
 */

class graph : public graph_object {
public:
  graph();
  ~graph();
  graph_object *clone() const; // clones just the nodes
  graph *cloneAll() const;     // clones everything

  int addNode(node *);
  node *getNode(unsigned int num);
  void addEdge(edge *);
  edge *findDirectedEdge(unsigned int from, unsigned int to);
  edge *findEdge(unsigned int from, unsigned int to);
	
  bool relax(edge *e, int weightIndex);
  bool relaxReverseEdge(edge *e, int weightIndex);
	
  node *getRandomNode();
  edge *getRandomEdge();

  node_iterator getNodeIter() const;
  node *nodeIterNext(node_iterator&) const;
  edge_iterator getEdgeIter() const;
  edge *edgeIterNext(edge_iterator&) const;

  void removeEdge(edge *);
  // returns the node that had it's node number changed along with its previous node number, if any
  node *removeNode(node *, unsigned int&);
  void removeNode(node *n) { unsigned int x; removeNode(n, x); } // if you don't care about node #'s
  void removeNode(unsigned int nodeNum) { removeNode(getNode(nodeNum)); }
	
  inline int getNumEdges() { return _edges.size(); }
  inline int getNumNodes() { return _nodes.size(); }
  
  std::vector<node*>* getReachableNodes(node* start);
  
  bool verifyGraph() const;
  void Print(std::ostream&) const;
  void printStats();

  
  /* AHA* extensions */
//  edge* findAnnotatedEdge(node* from, node* to, int capability, int clearance, double dist);
	
private:
  std::vector<node *> _nodes;
  //unsigned int node_index;
  std::vector<edge *> _edges;
  //unsigned int edge_index;
};

enum {
	kEdgeWeight = 0,
	kEdgeWidth = 1
};

/**
 * Edge class for connections between \ref node in a \ref graph.
 */
class edge : public graph_object {
 public:
	edge(unsigned int, unsigned int, double);
	graph_object *clone() const; 

	// set/get various labels for each node
	void setLabelF(unsigned int index, double val);
	void setLabelL(unsigned int index, long val);
	inline double getLabelF(unsigned int index) const { if (index < label.size()) return label[index].fval; return MAXINT; }
	inline long getLabelL(unsigned int index) const { if (index < label.size()) return label[index].lval; return MAXINT; }

	double getWeight() { return getLabelF(kEdgeWeight); }
	void setWeight(double val) { setLabelF(kEdgeWeight, val); }
	
	double getWidth() { return getLabelF(kEdgeWidth); }
	void setWidth(double val) { setLabelF(kEdgeWidth, val); }
	
	unsigned int getFrom() { return from; }
	unsigned int getTo() { return to; }
	
	void setMarked(bool marked) { mark = marked; }
	bool getMarked() { return mark; }

	int getEdgeNum() const { return edgeNum; } 

	void Print(std::ostream&) const;
	
	void setClearance(int, int);
	int getClearance(int);
	int getCapability() { return capability; }
	
 private:
	friend class graph;
	bool mark;
	unsigned int from, to;
//	double weight;
//	double width;
	unsigned int edgeNum;//, label[MAXLABELS];
	std::vector<labelValue> label;
	int capability;
	int clearance;
};

/**
 * Nodes to be stored within a \ref graph.
 */

class node : public graph_object {
public:
  node(const char *);
  graph_object *clone() const;

  const char *getName() const { return name; }
  unsigned int getNum() const { return nodeNum; }
  void addEdge(edge *);
  void removeEdge(edge *);

  // iterator over incoming edges
  edge *edgeIterNextIncoming(edge_iterator&) const;
  edge_iterator getIncomingEdgeIter() const;
  // iterator over outcoming edges
  edge *edgeIterNextOutgoing(edge_iterator&) const;
  edge_iterator getOutgoingEdgeIter() const;
  // iterate over all edges
  edge *edgeIterNext(edge_iterator&) const;
  edge_iterator getEdgeIter() const;

	edge *getEdge(unsigned int which);
	
  // iterate over all neighbors
  // don't return node* because we don't have access to them internally
  neighbor_iterator getNeighborIter() const;
  int nodeNeighborNext(neighbor_iterator&) const;

  edge *getRandomIncomingEdge();
  edge *getRandomOutgoingEdge();
	edge *getRandomEdge();
	
  int getNumOutgoingEdges();
  int getNumIncomingEdges();
  int getNumEdges() { return getNumOutgoingEdges()+getNumIncomingEdges(); }
	
  // chooses which label will be used as the key for
  // priority queue
  void setKeyLabel(int which) { keyLabel = which; }
  double getKey() { return label[keyLabel].fval; }

  // set/get various labels for each node
  void setLabelF(unsigned int index, double val);
  void setLabelL(unsigned int index, long val);
  
  inline double getLabelF(unsigned int index) {
    if (index < label.size()) return label[index].fval; return MAXINT;
  }

  inline long getLabelL(unsigned int index) {
    if (index < label.size()) return label[index].lval; return MAXINT;
  }
  
  // set/get marked edge for each node (limit 1)
  void markEdge(edge *e) { markedEdge = e; }
  edge *getMarkedEdge() { return markedEdge; }
	
  double getWidth() { return width; }
  void setWidth(double val) { width = val; }

  void Print(std::ostream&) const;

  /* AHA* extensions */
  void setClearance(int terraintype, int value);
  int getClearance(int terrain);
  void setTerrainType(int terrain);
  int getTerrainType();
  void setParentCluster(int clusterid);
  int getParentCluster(); 
  edge* findAnnotatedEdge(node* to, int capability, int clearance, double dist);
  
private:
  friend class graph;
  unsigned int nodeNum;//, label[MAXLABELS];
  std::vector<labelValue> label;
  edge *markedEdge;
  std::vector<edge *> _edgesOutgoing;
  std::vector<edge *> _edgesIncoming;
  std::vector<edge *> _allEdges;
  char name[30];
  int keyLabel;
  double width;
  
  int clearance[3];
  int terraintype;
  int clusterid;
  
};

std::ostream& operator <<(std::ostream & out, const graph &_Graph);
std::ostream& operator <<(std::ostream & out, const node &_Node);
std::ostream& operator <<(std::ostream & out, const edge &_Edge);

#endif
