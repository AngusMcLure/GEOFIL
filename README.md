# GEOFIL

GEOFIL is a spatially explicit agent-based modelling framework, designed to model LF transmission dynamics in American Samoa. 

To accurately model American Samoan population dynamics, GEOFIL uses a synthetic population model, which allows for births, deaths, couple formation and separation, movement within American Samoa, immigration, and emigration. People are assigned households, workplaces, and schools; the locations of these buildings correspond to known locations of buildings in American Samoa. During the day people are at their day-time location (workplace for employed, school for students, or household for all other people) and during night people are at their night-time location (household for all people). The immediate area around these locations is where transmission may occur. 

To model LF transmission in the human population, the model is run on a daily time step and explicitly models the human-agent while implicitly modelling the mosquito-vector. At each time-step the model simulates synthetic population changes, potential transmission, and the worm life-cycle within the host (acquisition, maturation, fecundity, and mortality). As transmission can occur at either a person's day-time location or night-time location, transmission during working hours and off-work hours is modelled separately. At each time-step a person may zero, one, or multiple transmission events, where a transmission event may transmit either one third stage-larvae of single sex, or two third stage-larvae of both sexes. 

The model is currently configured to initialise in 2010 and simulate for $n$ years. There are 3 initial rounds of territory-wide MDA which occur in 2018, 2019, 2021, these are based on actual MDA rounds that occurred in American Samoa. After these initial rounds of MDA, a number of different additional interventions can be simulated.

The GEOFIL repository contains four directories. All code for the model is contained within synPop, data and $config contain a number of csv, dat, and init files which are used to build the initial synthetic population and all building locations used by GEOFIL. The parameters folder contains additional .csv files for the synthetic population & transmission, and files that control the simulation (MDAParams.csv and target_mda.csv).

MDAParams.csv contains 32 columns, each column controlling providing a value for a variable that controls different aspect of the simulation and intervention. For clarity the effect of each variable/column is listed below-

| **Variable** | **Description** |
| --- | --- |
|*Coverage*| MDA coverage of initial rounds|
|*KillProb1*| Probability of worm death from 2 Drug MDA|
|*FullSterProb1*| Probability of full worm sterilisation from 2 Drug MDA|
|*PartSterProb1*| Probability of partial worm sterilisation from 2 Drug MDA|
|*SterDuration1*| Duration of sterilisation from 2 Drug MDA|
|*PartSterMagnitude1*| Effect of partial sterilisation from 2 Drug MDA|
|*MinAge1*| Minimum of age person to receive 2 Drug MDA|
|*KillProb2*| Probability of worm death from 3 Drug MDA|
|*FullSterProb2*| Probability of full worm sterilisation from 3 Drug MDA|
|*PartSterProb2*| Probability of partial worm sterilisation from 3 Drug MDA|
|*SterDuration2*| Duration of sterilisation from 3 Drug MDA|
|*PartSterMagnitude2*| Effect of partial sterilisation from 3 Drug MDA|
|*MinAge2*| Minimum of age person to receive 3 Drug MDA|
|*RealYears*| Y for initial MDA in 2018,2019 and 2021 or N for custom initial MDA|
|*StartYear*| If N for RealYears, starting year for custom initial MDA|
|*NumRounds*| If N for RealYears, number of rounds for custom initial MDA|
|*YearsBetweenRounds*| If N for RealYears, number of years between each MDA round for custom initial MDA|
|*Additional_MDA*| Y for additional interventions, N for no additional interventions|
|*Additional_Scheme*| Select strategy for additional interventions. A- Additional rounds of territory-wide MDA, R- MDA in *n* randomly selected villages, P- MDA in *n* villages with highest prevalence, X- Testing and treatment of workplaces  (no families), F- Testing and treatment of workplaces  (with families), E- Testing and treatment of elementary aged children, H- Testing and treatment of non-elementary aged children, C- No additional intervention (used for household based strategy when configured without any other additional intervention)|
|*Additional_Start*| Starting year of additional interventions |
|*Additional_Rounds*| Number of round additional intervention will be administered|
|*Additional_Years*| Number of years between rounds of additional intervetion|
|*Additional_Villages*| Number of villages (*n*)that will be targeted by schemes R or P |
|*Additional_Coverage*|Treatment coverage for additional intervention |
|*NumSims*| Number of simulations that we be conducted for the configuration  |
|*ProbOneSex*| Probability that bite will transmit one L3 larvae|
|*ProbBothSex*| Probability that bite will transmit two L3 larvae |
|*InitType*|The prevalence the model will initialise with.  C- custom initial prevalence, A- standard unclustered initial prevalence (based on 2010 community survey), or S- clustered initial prevalence (based on 2010 community survey for prevalence and 2016 community survey for clustering)|
|*InitPrev*| If C for InitType, the desired initial prevalence|
|*SimYears*|The number of years the simulation will run for |
|*Household_MDA*| Y for household-based strategy of N for no household-based strategy. The household based strategy can run concurrently with any other additional scheme and the specifics of the household based strategy are defined in file target_mda.csv.|

If you want to run multiple different strategies, you just have to add an extra row of variable values below the first. GEOFIL will the run sequentially down the rows of input values.


The household-based strategy is run from an additional file, target_mda.csv. The file contains eight columns/variables that govern the household based strategy. It is important to note, if running with multiple teams there must be an row of input values for each team (done as teams within the same strategy can be run with different attributes).  Below is an outline of what each variable controls-

| **Variable** | **Description** |
| --- | --- |
|*Number of teams*| The number of concurrent household based teams. This must match the number of rows of input values|
|*Coverage*| Coverage for household-based strategy|
|*Household_test*| The household test aim|
|*Years*| Number of years the strategy will run for|
|*Start_Year*| The starting year for the intervention|
|*Max_Distance*| The treatment radius|
|*Min_Test*| The minimum number of households a team will aim to test in each village|
|*Days_return*| The number of days till a team can return to a village|
			  
