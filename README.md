# GEOFIL

GEOFIL is a spatially explicit agent-based modelling framework, designed to model LF transmission dynamics in American Samoa. 

To accurately model American Samoan population dynamics, GEOFIL uses a synthetic population model, which allows for births, deaths, couple formation and separation, movement within American Samoa, immigration, and emigration. People are assigned households, workplaces, and schools; the locations of these buildings correspond to known locations of buildings in American Samoa. During the day people are at their day-time location (workplace for employed, school for students, or household for all other people) and during night people are at their night-time location (household for all people). The immediate area around these locations is where transmission may occur. 

To model LF transmission in the human population, the model is run on a daily time step and explicitly models the human-agent while implicitly modelling the mosquito-vector. At each time-step the model simulates synthetic population changes, potential transmission, and the worm life-cycle within the host (acquisition, maturation, fecundity, and mortality). As transmission can occur at either a person's day-time location or night-time location, transmission during working hours and off-work hours is modelled separately. At each time-step a person may zero, one, or multiple transmission events, where a transmission event may transmit either one third stage-larvae of single sex, or two third stage-larvae of both sexes. 

The GEOFIL repository contains four directories. All code for the model is contained within synPop, data and $config contain a number of csv, dat, and init files which are used to build the intial synthic population and all building locations used by GEOFIL.
