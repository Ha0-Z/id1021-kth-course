# Array and performance in c

## Introduction

## Method

## The clock- Random access
The code of clock which the array-c.pdf have provided have a few interellged procedure which determine the time in ns where the procedure take to run.
Since it take some clockcyckle to record the current time with procedure **clock_gettime(CLOCK_MONOTONIC, &t_stop)**, there fore it will take some time before it ends.

#### Result of running the clock
173 ns
61 ns
60 ns
58 ns
56 ns
59 ns
59 ns
59 ns
59 ns
60 ns

We can ignore the results at the first since the code where not cashed in the cpu cashe at the beginning.

#### Solution
Since the number of clockcyle to run same machine code is always the same, but it varies on diffrerent processor/ clockspeed of the processor. One solution is we can get the time to run the **clock_gettime()** procedure by record itself one. After that we can record the code we are intressed in and remove the time takes by running **clock_gettime()** simply subtract the value we recorded itself.

### Approch 2 of running the clock by running 1000 times then devide by 1000
In this approach all time takes were sumed together then devide by 1000, this approch is good in way of reduce the influence of time takes to record the current time by 1000 time, reduce to 11 promile of the original time which can be ignored. It also be able to record the arvage time takes to run the code.


### Approach 3
#### Result
time : 283 ns
time : 91 ns
time : 75 ns
time : 69 ns
time : 67 ns
time : 67 ns
time : 65 ns
time : 67 ns
time : 67 ns
time : 68 ns

### Approach 4
#### Result
maximum time: 4.52 ns/operation 
minimum time: 4.16 ns/operation 
average time: 4.21 ns/operation 

### Approach 5
#### Result
1000 5.53 ns
2000 5.52 ns
4000 5.52 ns
8000 5.54 ns
16000 5.55 ns
32000 5.57 ns


### Optimization
Undone


## Search for an item
#### Result
1000 355.87 ns
2000 690.61 ns
4000 1401.87 ns
8000 2719.43 ns
16000 5622.37 ns
32000 11156.55 ns
#### Analisis
n^n since you need to go through all element


## Search for duplicates
#### Result
1000 762.92 ns
2000 1399.13 ns
4000 5602.12 ns
8000 21749.18 ns
16000 89383.37 ns
32000 358672.26 ns
#### 



## Discussion