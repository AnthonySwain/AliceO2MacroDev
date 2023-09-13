# Macros for ALICE Optimisation framework 

# Voxel Maps / HashMaps
Found in the HashMaps folder

## Pre-requistes
https://gitlab.cern.ch/VecGeom/VecGeom/-/blob/89a05d148cc708d4efc2e7b0eb6e2118d2610057/VecGeom/base/FlatVoxelHashMap.h#L207

<br>

# Step and Hit Analysis 
Found in the StepandHit_Analysis folder.
## Pre-requistes 
The O2pdgspecific build - https://github.com/AnthonySwain/AliceO2/tree/ParticleSpecificMaps
SaveSteps enabled in said build (see Steer readme).

## Use
Configure globals.C / globals.h to confirm the histogram dimensions and which particles to look into (unfortunately this is not synced to the O2 build above which will also have a list of which PDGs to only save - this will also need modifying.)

<br>
Copy & paste all the files found in the StepandHit_Analysis folder into the output folder of the simulation and run 'root AllMacros.C'. 


