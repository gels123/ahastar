/*
 * $Id: sample.h,v 1.6 2006/09/18 06:23:39 nathanst Exp $
 *
 *  sample.h
 *  hog
 *
 *  Created by Nathan Sturtevant on 5/31/05.
 *  Copyright 2005 Nathan Sturtevant, University of Alberta. All rights reserved.
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

class AnnotatedClusterAbstraction;

void myDisplayHandler(unitSimulation *, tKeyboardModifier, char key);
void myPathfindingKeyHandler(unitSimulation *, tKeyboardModifier, char key);
void myRandomUnitKeyHandler(unitSimulation *, tKeyboardModifier, char key);
int myCLHandler(char *argument[], int maxNumArgs);
int myScenarioGeneratorCLHandler(char *argument[], int maxNumArgs);
int myGUICLHandler(char *argument[], int maxNumArgs);
int myQualityCLHandler(char *argument[], int maxNumArgs);
int myClustersizeCLHandler(char *argument[], int maxNumArgs);
int myExecuteScenarioCLHandler(char *argument[], int maxNumArgs);
bool myClickHandler(unitSimulation *, int x, int y, point3d loc, tButtonType, tMouseEventType);
void runNextExperiment(unitSimulation *unitSim);
void runSimulationNoGUI();
void processStats(statCollection* stat, const char* unitname);
void gogoGadgetNOGUIScenario(AnnotatedClusterAbstraction* aca);


