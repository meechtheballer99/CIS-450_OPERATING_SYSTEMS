***The output for question 2 - one lane bridge question - is stored in bridge_output.txt

*for this algorithm, I have made the default number of cars == 50.
*you can add one parameter if you want in order to specify the number of cars.

*for this program, each car arrives at the bridge and attempts to cross; if its direction (north or south)
is in the same direction as current traffic and number of cars on bridge is less than 3,
then a given car can cross; if no cars are on the bridge, a car may enter.

*I used the modulo operator % and rand() function to generate random directions for each car thread.
