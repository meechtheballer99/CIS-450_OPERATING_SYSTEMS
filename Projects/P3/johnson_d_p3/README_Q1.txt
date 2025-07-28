*For the producer and consumer problem, I simply used the same ideas as the single buffer, 
except I made a integer array to store the data of each buffer - where the number of
buffers (array size) is specified by the user via a command line parameter.

*There are two pointers, one that points to the front of the buffer array, and one that points to the tail.
The producer is always leading, while the consumer is always trailing behind the producer,
in order to read everything a producer outputs into each subsequent buffer.

*I used % operator in order to wrap around the buffer array, that way producers and consumers 
can continue to loop through the buffer.