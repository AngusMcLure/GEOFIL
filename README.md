# GEOFIL

GEOFIL is a spatially explicit agent-based modelling framework, designed to model lymphatic filariasis (LF) transmission  in American Samoa. GEOFIL can model a variery of disease surveillance options and interventions for the elimintaiton of LF using mass drug administration (MDA) and alternative targetted treatement strategies. GEOFIL was developed at the National Centre for Epidemiology and Population Health at the Australian National University. Early versions of the code were developed by Zhijing (Sting) Xu, with further development by Angus McLure and Callum Shaw. More details on the software and model outputs can be found in the following peer-reviewed journal articles:

* Shaw C, McLure A, Graves PM, Lau CL, Glass K. *Lymphatic filariasis endgame strategies: Using GEOFIL to model mass drug administration and targeted surveillance and treatment strategies in American Samoa*. PLOS Neglected Tropical Diseases 2023 17(5): e0011347. https://doi.org/10.1371/journal.pntd.0011347

* McLure A, Graves PM, Lau C, Shaw C, Glass K. *Modelling lymphatic filariasis elimination in American Samoa: GEOFIL predicts need for new targets and six rounds of mass drug administration*. Epidemics. 2022 Sep 1;40:100591. https://doi.org/10.1016/j.epidem.2022.100591

* Xu Z, Graves PM, Lau CL, Clements A, Geard N, Glass K. *GEOFIL: A spatially-explicit agent-based modelling framework for predicting the long-term transmission dynamics of lymphatic filariasis in American Samoa*. Epidemics. 2019 Jun 1;27:19-27. https://doi.org/10.1016/j.epidem.2018.12.003


To accurately model American Samoan population dynamics, GEOFIL uses a synthetic population model, which allows for births, deaths, couple formation and separation, movement within American Samoa, immigration, and emigration. People are assigned households, workplaces, and schools; the locations of these buildings correspond to known locations of buildings in American Samoa. During the day people are at their day-time location (workplace for employed, school for students, or household for all other people) and during night people are at their night-time location (household for all people). The immediate area around these locations is where transmission may occur. 

To model LF transmission in the human population, the model is run on a daily time step and explicitly models the human-agent while implicitly modelling the mosquito-vector. At each time-step the model simulates synthetic population changes, potential transmission, and the worm life-cycle within the host (acquisition, maturation, fecundity, and mortality). As transmission can occur at either a person's day-time location or night-time location, transmission during working hours and off-work hours is modelled separately. At each time-step a person may zero, one, or multiple transmission events, where a transmission event may transmit either one third stage-larvae of single sex, or two third stage-larvae of both sexes. GEOFIL can model treatments with flexible range of effects on worms and can be changing the efficiacy of the treatment can be used to emulate treatment with ivermectin (I), diethylcarbamazine (D), albendazole (A), or their combinations (e.g. DA or IDA). GEOFIL can different treatments to two different age groups (e.g. DA to <5 yearolds and IDA to others)

The model is currently configured to initialise in 2010 and runs for $n$ years. There are three initial rounds of territory-wide MDA which occur in 2018, 2019, 2021, these are based on actual MDA rounds that occurred in American Samoa. After these initial rounds of MDA, a number of different additional interventions can be simulated.

The GEOFIL repository contains four directories. All code is contained within synPop. Data and $config contain a number of csv, dat, and init files which are used to build the initial synthetic population and contain all building locations used by GEOFIL. The parameters folder contains additional .csv files for the synthetic population & transmission, and files that control the simulation (MDAParams.csv and target_mda.csv).

MDAParams.csv contains 32 columns, each column providing a value for a variable that controls different aspect of the simulation and intervention. For clarity the effect of each variable/column is listed below-

| **Variable** | **Description** |
| --- | --- |
|*Coverage*| MDA coverage of initial rounds|
|*KillProb1*| Probability of worm death from treatment option 1|
|*FullSterProb1*| Probability of full worm sterilisation from treatment option 1|
|*PartSterProb1*| Probability of partial worm sterilisation from treatment option 1|
|*SterDuration1*| Duration of sterilisation from treatment option 1|
|*PartSterMagnitude1*| Effect of partial sterilisation from treatment option 1|
|*MinAge1*| Minimum of age person to receive treatment option 1|
|*KillProb2*| Probability of worm death from treatment option 2|
|*FullSterProb2*| Probability of full worm sterilisation from treatment option 2|
|*PartSterProb2*| Probability of partial worm sterilisation from treatment option 2|
|*SterDuration2*| Duration of sterilisation from treatment option 2|
|*PartSterMagnitude2*| Effect of partial sterilisation from treatment option 2|
|*MinAge2*| Minimum of age person to receive treatment option 2|
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

If you want to simulate more than one strategy, you just have to add an extra row of variables values below the first row. GEOFIL will the run sequentially down the rows of input values.

The parameters for the household-based strategy are set in a seperate file, target_mda.csv. The file contains eight columns/variables that govern the household based strategy. If running with multiple teams there must be an row of input values for each team (teams within the same strategy can be run with different attributes).  Below is an outline of what each variable controls-

| **Variable** | **Description** |
| --- | --- |
|*Number of teams*| The number of concurrent household based teams. This must match the number of rows of input values|
|*Coverage*| Coverage for household-based strategy|
|*Household_test*| The household test aim|
|*Years*| Number of years the strategy will run for|
|*Start_Year*| The starting year for the intervention|
|*Treatment_Radius*| The treatment radius|
|*Min_Test*| The minimum number of households a team will aim to test in each village|
|*Days_return*| The number of days till a team can return to a village|
			  
Parameters from MDAParams.csv and target_mda.csv are read at run time. The executable from the compiled code runs from the console with a single argument specifying the directory of the output file (formatted as a csv). Outputs will be appended to the specified file.
