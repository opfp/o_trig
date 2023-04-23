# o_trig: A Trig Library 
## Owen Phillips 4/22/23 

o_trig is a simple trigonometry library which can create and use lookup tables to 
solve six trig functions: sine, arc-sine, cosine, arc-cosine, tangent and arc-tangent. 

# Using the library 

To compile: 

``` 
./make_prod.sh cli 
```
or 
```
gcc o_trig.c cli.c -lm --std=c11 -O3 -o cli 
```

To see the library in action, first generate a lookup table with the desired number
of points. For instance, 2048: 
```
./cli -g 2048 -f tables/2048.otb 
```
Then, you may query the table yourself:
``` 
./cli -f tables/2048.otb -i 
```
or run the pre-shipped traces on it 
```
./run_traces.sh 
``` 
After running the traces, you may use them to generate graphs of the results (found in plots). 
```
./make_plots.sh
``` 

cli.c provides a simple command line interface to generate and query a lookup table. 

### Flags for cli 
| FLAG | ARGUMENT | DESCRIPTION | 
|---|---|---|
| -f | [ FILEPATH ] | filepath from which to load/store lookup table. when paired with -g, the generated lookup table will be written to the specified filepath. | 
| -g | [ NUM POINTS]| o_trig will generate a lookup table with NUM POINTS values. | 
| -t | [ TABLES ] | a bitmap which specifies which tables (sine = 1, tan = 2, points = 4 ) o_trig should generate. If left blank, all will be generated. | 
| -i | N/A | (interactive) allows the user to query the loaded / generated lookup table from the command line, with instructions. | 
| -c | N/A | (csv) meant for running traces, csv mode is like interactive mode except there are no instructions, and results are returned in X, Y format. | 

## Space saving measures 

While sine and cosine have a period of 2 pi, the entire function may actually be 
solved only knowing sine(x) on 0, pi/2. This is because sine(x) on [pi/2, pi] is
merely sine(x) on [0, pi/2] reflected about x = pi/2, and sine(x) on [pi, 2*pi] 
is merely sine(x) on [0, pi] reflected about x = pi and y = 0. 

This allows us to keep a lookup table for only sine(x) x >= 0, x <= pi/2, and return 
results for all other xs by translating the input to the table and then adjusting the
output. 

Similarly, Because cos(x) = sine(x + pi/2), sine and cosine are able to share a lookup 
table, with cosine inputs also being translated, and outputs modified to provide the 
correct result. 

## Table generation ( o_trig.c : gen_lookup_tables() )

To generate lookup tables, a point-cloud representing an arc of pi/2 radians is 
generated and used to calculate the relevant values. The non-inverse functions 
receive an angle (theta) as input and return a ratio ( s1 / s2 ) of two sides 
in a right-triangle constructed with the given angle. The inverse functions 
receive a ratio and return an angle. 

It is notable that the value of an angle, in radians, corresponds to the length 
of the arc of a circle which the angle sweeps out ( arclen = theta * r ). In a 
circle with a radius of one, arclen = theta. 

So, in order to generate our lookup table, we will create a point cloud approximating 
an arc of the circle (r=1), calculate the arc length from each point to the x-axis, and 
record the arc length along with the relevant ratio ( x / hyp for sine, x / y for tangent). 

In my implementation, these steps all occur synchronously, point by point. The arc length 
begins as 0 and is incremented by the linear distance between the current and most recent 
points. 

The formula which describes a circle is quadratic, and therefore the point corresponding 
with a given x may be found simply via the quadratic formula. 

## Lookup table structure and querying 

The lookup tables are structured as a single array of floats, of size points * 2, with 
angle values at even numbered indices and their corresponding ratio values at i+1. 

All searches in the lookup tables occur as binary searches, taking place in log2(n) time. 
Binary search is able to be used because sin(x) and tan(x) have no inflection points on 
x = [0, pi/2] 

## Future plans 

### Faster searching 

The current lookup table scheme used by the library achieves log(n) search times, which, 
while not terrible, does leave some speed to be desired. 

In order to achieve constant search time, the lookup table must be configured such that 
the index of the result is deducable from the input value. This could be acheived by 
changing the table generation to ensure even spacing between x values. However, new lookup 
tables would be required to gain the same advantage in inverse-function lookups, which would
double memory overhead. 
