/*
 *  CapabilityUnit.cpp
 *  hog
 *
 *  Created by Daniel Harabor on 19/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "CapabilityUnit.h"

void CapabilityUnit::openGLDraw(mapProvider *mp, simulationInfo *si)
{
	searchUnit::openGLDraw(mp, si);
	
	GLdouble xx, yy, zz, rad;
	Map *map = mp->getMap();


	int posx = x, posy = y;
	int size = ((AbstractAnnotatedAStar*)this->getAlgorithm())->getClearance();
	
	if (onTarget)
	{
		double perc = (1.0-sqrt(sqrt(abs(sin(targetTime+0.25*si->getSimulationTime())))));
		glColor3f(r*perc, g*perc, b*perc);
	}
	else	
		glColor3f(r, g, b);	
	
	if(size == 2)
	{	
		map->getOpenGLCoord(posx, posy, xx, yy, zz, rad);
		GLdouble xx1, yy1;
		xx1 = xx;
		yy1 = yy;
			
		map->getOpenGLCoord(posx+1, posy, xx, yy, zz, rad);
		xx1 += (xx-xx1)*0.5;
		drawSphere(xx, yy, zz, 1);

		map->getOpenGLCoord(posx, posy+1, xx, yy, zz, rad);
		yy1 += (yy-yy1)*0.5;
		drawSphere(xx, yy, zz, rad);

		map->getOpenGLCoord(posx+1, posy+1, xx, yy, zz, rad);
		drawSphere(xx, yy, zz, rad);
		drawSphere(xx1, yy1, zz, rad);
	}

}
