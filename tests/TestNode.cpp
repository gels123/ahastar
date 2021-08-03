/*
 *  TestNode.cpp
 *  hog
 *
 *  Created by Daniel Harabor on 28/11/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "TestNode.h"
#include "constants.h"
#include <cppunit/Message.h>
#include <string>

using namespace std;
//using namespace CppUnit;

CPPUNIT_TEST_SUITE_REGISTRATION( TestNode );

void TestNode::setUp()
{
	terrains[0] = kGround;
	terrains[1] = kTrees;
	terrains[2] = (kTrees|kGround);
	
	for(int i=0;i<4;i++)
	{
		clval[i] = i;
	}
	
	n = new node("");

	g = new graph();
	from = new node("");
	to = new node("");
	g->addNode(from);
	g->addNode(to);

}

void TestNode::tearDown()
{
	delete n;
	delete g;
}

/* TerrainAnnotationsTest
	@desc:		 
		This test assumes a simple map with only two basic terrains; each node thus has a max of 3 annotations. 
*/
void TestNode::TerrainAnnotationsTest()
{
	
	// check that the terrain types are only of the defined types
	n->setTerrainType(kGround);
	CPPUNIT_ASSERT_MESSAGE("invalid terrain type",
	n->getTerrainType() == terrains[0] || n->getTerrainType() == terrains[1] || n->getTerrainType() == terrains[2]);

	for(int i=0;i<3;i++)
	{
	
		// check that clearance values are initialised to zero
		CPPUNIT_ASSERT_MESSAGE("clearance initialised to non-zero value", n->getClearance(terrains[i]) == 0);

		// check that we can set and retrieve clearance values
		n->setClearance(terrains[i], clval[i+1]);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("retrieved clearance != value set earlier", n->getClearance(terrains[i]), clval[i+1]);
		
		
	}
		
	// check that negative clearance values are not allowed and zeroed instead
	n->setClearance(terrains[0], -1);
	CPPUNIT_ASSERT_MESSAGE("clearance value < 0", n->getClearance(terrains[0])  >= 0);
	
	int i =0;
	return;
}

void TestNode::setClearanceFailsWhenNodeTerrainNotValid()
{
	n->setTerrainType(kWater);
	n->setClearance(kGround, 1);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("managed to set clearance > 0 for node with invalid terrain (kWater)", 0, n->getClearance(kGround));
}

void TestNode::setParentClusterFailsWhenClusterIdLessThanZero()
{
	int cid = -2;
	n->setParentCluster(cid);
	CPPUNIT_ASSERT_EQUAL(-1, n->getParentCluster());
}

void TestNode::setParentClusterStoresClusterIdWhenClusterIdMoreThanZero()
{
	int cid = 2;
	n->setParentCluster(cid);
	CPPUNIT_ASSERT_EQUAL(cid, n->getParentCluster());	
}

void TestNode::setParentClusterStoresClusterIdWhenClusterIdEqualsZero()
{
	int cid = 0;
	n->setParentCluster(cid);
	CPPUNIT_ASSERT_EQUAL(cid, n->getParentCluster());	
}

void TestNode::cloneShouldDeepCopyNodeAndAllAnnotationsExceptParentClusterId()
{

	/* create and annotate some nodes to use as test data */
	node* tn1 = new node("");
	
	tn1->setLabelL(kAbstractionLevel, 0);
	tn1->setParentCluster(0);
	tn1->setLabelL(kFirstData, 4);
	tn1->setLabelL(kFirstData+1, 1);	
	tn1->setTerrainType(kGround);
	tn1->setClearance(kGround, 1);
	tn1->setClearance(kTrees, 1);
	tn1->setClearance(kGround|kTrees, 2);

	node* tn1clone = dynamic_cast<node*>(tn1->clone());
	CPPUNIT_ASSERT_MESSAGE("clone returned original node! failed to copy!", tn1 != tn1clone);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("cloned node has different terrain type", tn1->getTerrainType(), tn1clone->getTerrainType());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("cloned node is incorrectly assigned to a cluster", -1, tn1clone->getParentCluster());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("cloned node has different clearance value for kTrees capability", tn1->getClearance(kTrees), tn1clone->getClearance(kTrees));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("cloned node has different clearance value for kTrees capability", tn1->getClearance(kTrees), tn1clone->getClearance(kTrees));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("cloned node has different clearance value for (kGround|kTrees) capability", tn1->getClearance((kGround|kTrees)), tn1clone->getClearance((kGround|kTrees)));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("cloned node has incorrect kAbstractionLevel label", tn1->getLabelL(kAbstractionLevel), tn1clone->getLabelL(kAbstractionLevel));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("cloned node has incorrect x-coordinate label", tn1->getLabelL(kFirstData), tn1clone->getLabelL(kFirstData));	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("cloned node has incorrect y-coordinate label", tn1->getLabelL(kFirstData+1), tn1clone->getLabelL(kFirstData+1));
	
	delete tn1;
	delete tn1clone;

}

void TestNode::cloneShouldNotDeepCopyEdges()
{
	node* n1 = new node("n1");
	node* n2 = new node("n2");
	graph* g = new graph();
	g->addNode(n1);
	g->addNode(n2);
	edge* e = new edge(n1->getNum(), n2->getNum(), 1);
	g->addEdge(e);
		
	node* clone = dynamic_cast<node*>(n1->clone());
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("clone copied some edges when it shouldn't have", 0, clone->getNumEdges());
	
	delete g;
}


void TestNode::addEdgeToNode(int caps, int clearance, double weight)
{
	e = new edge(from->getNum(), to->getNum(), weight);
	e->setClearance(caps,clearance);
	g->addEdge(e);
}



void TestNode::findAnnotatedEdgeShouldReturnAnExistingEdgeIfOneExistsWhichIsIdenticalToWhatParametersAskFor()
{
	double weight = 1.0;
	int targetClearance = 2;
	int targetCapability = (kTrees|kGround);
	
	addEdgeToNode(kGround, targetClearance, weight);
		
	edge* expectedEdge = from->findAnnotatedEdge(to, kGround, targetClearance, weight);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("failed to find identical existing annotated edge", e, expectedEdge);

	expectedEdge = from->findAnnotatedEdge(to, targetCapability, targetClearance, weight);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("failed to find existing annotated edge using capability superset", e, expectedEdge);
}

void TestNode::findAnnotatedEdgeShouldReturnAnExistingEdgeIfOneExistsWhichIsShorterButOtherwiseIdenticalToWhatParametersAskFor()
{
	double weight = 3.0;
	int targetClearance = 2;
	int targetCapability = (kTrees|kGround);
	
	addEdgeToNode(kGround, targetClearance, weight-1);
		
	edge* expectedEdge = from->findAnnotatedEdge(to, kGround, targetClearance, weight);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("failed to find identical existing annotated edge", e, expectedEdge);

	expectedEdge = from->findAnnotatedEdge(to, targetCapability, targetClearance, weight);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("failed to find existing annotated edge using capability superset", e, expectedEdge);
}

void TestNode::findAnnotatedEdgeShouldReturnAnExistingEdgeIfOneExistsWhichIsWiderAndShorterThanWhatParametersAskFor()
{
	double weight = 3.0;
	int targetClearance = 2;
	int targetCapability = (kTrees|kGround);
	
	addEdgeToNode(kGround, targetClearance+1, weight-1);
		
	edge* expectedEdge = from->findAnnotatedEdge(to, kGround, targetClearance, weight);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("failed to find identical existing annotated edge", e, expectedEdge);

	expectedEdge = from->findAnnotatedEdge(to, targetCapability, targetClearance, weight);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("failed to find existing annotated edge using capability superset", e, expectedEdge);	
}

void TestNode::findAnnotatedEdgeShouldReturnAnExistingEdgeIfOneExistsWhichIsWiderButOtherwiseIdenticalToWhatParametersAskFor()
{
	double weight = 3.0;
	int targetClearance = 2;
	int targetCapability = (kTrees|kGround);
	
	addEdgeToNode(kGround, targetClearance+1, weight);
		
	edge* expectedEdge = from->findAnnotatedEdge(to, kGround, targetClearance, weight);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("failed to find identical existing annotated edge", e, expectedEdge);

	expectedEdge = from->findAnnotatedEdge(to, targetCapability, targetClearance, weight);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("failed to find existing annotated edge using capability superset", e, expectedEdge);	
}

void TestNode::findAnnotatedEdgeShouldReturnZeroWhenNoEdgeExistsBetweenParameterEndpoints()
{
	double weight = 3.0;
	int targetClearance = 2;
	int targetCapability = (kTrees|kGround);

	edge *expectedEdge = from->findAnnotatedEdge(to, targetCapability, targetClearance, weight);
	CPPUNIT_ASSERT_MESSAGE("failed to return zero when no edge exists connecting target endpoints", expectedEdge == 0);	
}

// NB: if capability values are identical, this method returning zero is a warning sign something is probably wrong with cluster annotation 
// ie. we're querying about an edge connecting two endpoints which represents a shorter path with larger clearance. why didn't we find it before?
void TestNode::findAnnotatedEdgeShouldReturnZeroWhenExistingEdgeIsShorterAndMoreNarrowThanWhatParametersAskFor()
{
	double weight = 3.0;
	int targetClearance = 2;
	int targetCapability = (kTrees|kGround);
	
	addEdgeToNode(kGround, targetClearance-1, weight-1);
		
	edge* expectedEdge = from->findAnnotatedEdge(to, kGround, targetClearance, weight);
	CPPUNIT_ASSERT_MESSAGE("failed to return zero", expectedEdge == 0);

	expectedEdge = from->findAnnotatedEdge(to, targetCapability, targetClearance, weight);
	CPPUNIT_ASSERT_MESSAGE("failed to return zero using capability superset", expectedEdge == 0);	
}

void TestNode::findAnnotatedEdgeShouldReturnZeroWhenExistingEdgeIsOfTheSameLengthButMoreNarrowThanWhatParametersAskFor()
{
	double weight = 3.0;
	int targetClearance = 2;
	int targetCapability = (kTrees|kGround);
	
	addEdgeToNode(kGround, targetClearance-1, weight);
		
	edge* expectedEdge = from->findAnnotatedEdge(to, kGround, targetClearance, weight);
	CPPUNIT_ASSERT_MESSAGE("failed to return zero", expectedEdge == 0);

	expectedEdge = from->findAnnotatedEdge(to, targetCapability, targetClearance, weight);
	CPPUNIT_ASSERT_MESSAGE("failed to return zero using capability superset", expectedEdge == 0);	
}

void TestNode::findAnnotatedEdgeShouldReturnZeroWhenExistingEdgeIsLongerAndMoreNarrowThanWhatParametersAskedFor()
{
	double weight = 3.0;
	int targetClearance = 2;
	int targetCapability = (kTrees|kGround);
	
	addEdgeToNode(kGround, targetClearance-1, weight+1);
		
	edge* expectedEdge = from->findAnnotatedEdge(to, kGround, targetClearance, weight);
	CPPUNIT_ASSERT_MESSAGE("failed to return zero", expectedEdge == 0);

	expectedEdge = from->findAnnotatedEdge(to, targetCapability, targetClearance, weight);
	CPPUNIT_ASSERT_MESSAGE("failed to return zero using capability superset", expectedEdge == 0);	
}

// NB: another warning method. we're querying about an edge which is more optimal than the current connection between the two endpoints
void TestNode::findAnnotatedEdgeShouldReturnZeroWhenExistingEdgeIsLongerButOtherwiseIdenticalToWhatParametersAskFor()
{
	double weight = 3.0;
	int targetClearance = 2;
	int targetCapability = (kTrees|kGround);
	
	addEdgeToNode(kGround, targetClearance, weight+1);
		
	edge* expectedEdge = from->findAnnotatedEdge(to, kGround, targetClearance, weight);
	CPPUNIT_ASSERT_MESSAGE("failed to return zero", expectedEdge == 0);

	expectedEdge = from->findAnnotatedEdge(to, targetCapability, targetClearance, weight);
	CPPUNIT_ASSERT_MESSAGE("failed to return zero using capability superset", expectedEdge == 0);
}

void TestNode::findAnnotatedEdgeShouldReturnZeroWhenExistingEdgeIsLongerAndWiderThanWhatParametersAskedFor()
{
	double weight = 3.0;
	int targetClearance = 2;
	int targetCapability = (kTrees|kGround);
	
	addEdgeToNode(kGround, targetClearance+1, weight+1);
		
	edge* expectedEdge = from->findAnnotatedEdge(to, kGround, targetClearance, weight);
	CPPUNIT_ASSERT_MESSAGE("failed to return zero", expectedEdge == 0);

	expectedEdge = from->findAnnotatedEdge(to, targetCapability, targetClearance, weight);
	CPPUNIT_ASSERT_MESSAGE("failed to return zero using capability superset", expectedEdge == 0);	
}

void TestNode::findAnnotatedEdgeShouldReturnZeroWhenEndpointParametersAreNull()
{
	double weight = 3.0;
	int targetClearance = 2;
	int targetCapability = (kTrees|kGround);
			
	edge* expectedEdge = from->findAnnotatedEdge(NULL, targetCapability, targetClearance, weight);
	CPPUNIT_ASSERT_MESSAGE("failed to return zero when to is NULL", expectedEdge == 0);	
}

void TestNode::findAnnotatedEdgeShouldReturnTheShortestEdgeIfSeveralCandidateEdgesExistThatMatchWhatParametersAskedFor()
{
	addEdgeToNode(kGround, 2, 5);
	addEdgeToNode(kGround, 1, 4.5);


	edge* e = from->findAnnotatedEdge(to, kGround, 1, 50);
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE("failed to return shortest edge among those existing between endpoints", 4.5, e->getWeight());
}