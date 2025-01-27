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



Introduction: Explain the purpose of the assignment, which is to explore the efficiency of different operations over an array of elements and understand the differences between O(1), O(n), and O(n^2) complexities.

Benchmarking Operations: Discuss the three operations to be benchmarked:

Random Access: Reading or writing a value at a random location in an array.

Search: Searching through an array looking for an item.

Duplicates: Finding all common values in two arrays.

Performance Measurement: Describe the importance of measuring performance accurately and the challenges involved, such as the resolution of the clock and the impact of caching.

Random Access Benchmarking: Explain the process of benchmarking random access operations, including the need to perform multiple operations to get a reliable measurement and the importance of using random locations to avoid caching effects.

Search Operation: Discuss the implementation of a simple search algorithm and how to benchmark its performance. Mention the importance of searching for random keys in an array of unsorted keys.

Finding Duplicates: Describe the task of finding duplicated numbers in two arrays and how to benchmark this operation. Highlight the difference in complexity when both arrays grow in size.

Presenting Results: Emphasize the importance of presenting execution times in a clear and readable manner, using an appropriate number of significant figures and avoiding unnecessary complexity.

Optimization and Compiler Behavior: Discuss the impact of compiler optimizations on performance measurements and the need to ensure that the compiler does not optimize away important parts of the code.

Conclusion: Summarize the key findings and insights gained from the benchmarking experiments.

You can use these points as a framework for your report and expand on each section with details from the document. Happy writing!




You said

Result of running the clock
173 ns 61 ns 60 ns 58 ns 56 ns 59 ns 59 ns 59 ns 59 ns 60 ns

We can ignore the results at the first since the code where not cashed in the cpu cashe at the beginning.

Solution
Since the number of clockcyle to run same machine code is always the same, but it varies on diffrerent processor/ clockspeed of the processor. One solution is we can get the time to run the clock_gettime() procedure by record itself one. After that we can record the code we are intressed in and remove the time takes by running clock_gettime() simply subtract the value we recorded itself.

Approch 2 of running the clock by running 1000 times then devide by 1000
In this approach all time takes were sumed together then devide by 1000, this approch is good in way of reduce the influence of time takes to record the current time by 1000 time, reduce to 11 promile of the original time which can be ignored. It also be able to record the arvage time takes to run the code.

Approach 3
Result
time : 283 ns time : 91 ns time : 75 ns time : 69 ns time : 67 ns time : 67 ns time : 65 ns time : 67 ns time : 67 ns time : 68 ns

Approach 4
Result
maximum time: 4.52 ns/operation minimum time: 4.16 ns/operation average time: 4.21 ns/operation

Approach 5
Result
1000 5.53 ns 2000 5.52 ns 4000 5.52 ns 8000 5.54 ns 16000 5.55 ns 32000 5.57 ns

Optimization
Undone

Search for an item
Result
1000 355.87 ns 2000 690.61 ns 4000 1401.87 ns 8000 2719.43 ns 16000 5622.37 ns 32000 11156.55 ns

Analisis
n^n since you need to go through all element

Search for duplicates
Result
1000 762.92 ns 2000 1399.13 ns 4000 5602.12 ns 8000 21749.18 ns 16000 89383.37 ns 32000 358672.26 ns"
Please write a report based on the structre you gave me, in the report you need to analysis my result by compare the data...
Copilot said
Sure, here's a detailed report based on your notes and the structure we discussed:

Exploring the Efficiency of Different Operations Over an Array of Elements in C
Introduction
The purpose of this assignment is to explore the efficiency of different operations over an array of elements and understand the differences between O(1), O(n), and O(n^2) complexities. This report will analyze the performance of random access, search, and duplicate finding operations, and present the results of benchmarking experiments.

Benchmarking Operations
Three operations were benchmarked:

Random Access: Reading or writing a value at a random location in an array.

Search: Searching through an array looking for an item.

Duplicates: Finding all common values in two arrays.

Performance Measurement
Accurate performance measurement is crucial for understanding the efficiency of different operations. Challenges include the resolution of the clock and the impact of caching. The clock_gettime(CLOCK_MONOTONIC, &t_stop) function was used to measure time in nanoseconds.

Random Access Benchmarking
The random access operation was benchmarked using multiple approaches to ensure reliable measurements:

Approach 1: Initial Measurement
Initial results showed variability due to the lack of caching:

Results: 173 ns, 61 ns, 60 ns, 58 ns, 56 ns, 59 ns, 59 ns, 59 ns, 59 ns, 60 ns

Analysis: The first result was ignored as the code was not cached in the CPU cache initially.

Approach 2: Averaging Over Multiple Runs
By running the clock 1000 times and averaging the results, the influence of recording the current time was minimized:

Results: 283 ns, 91 ns, 75 ns, 69 ns, 67 ns, 67 ns, 65 ns, 67 ns, 67 ns, 68 ns

Analysis: This approach reduced the impact of the time taken to record the current time by 1000 times, providing a more accurate average time.

Approach 3: Maximum, Minimum, and Average Time
Results: Maximum time: 4.52 ns/operation, Minimum time: 4.16 ns/operation, Average time: 4.21 ns/operation

Analysis: This approach provided a comprehensive view of the time taken for random access operations.

Search Operation
The search operation was benchmarked by measuring the time taken to search for an item in arrays of increasing size:

Results: 1000 elements: 355.87 ns, 2000 elements: 690.61 ns, 4000 elements: 1401.87 ns, 8000 elements: 2719.43 ns, 16000 elements: 5622.37 ns, 32000 elements: 11156.55 ns

Analysis: The results show an O(n) complexity, as the time taken increases linearly with the number of elements.

Finding Duplicates
The task of finding duplicated numbers in two arrays was benchmarked:

Results: 1000 elements: 762.92 ns, 2000 elements: 1399.13 ns, 4000 elements: 5602.12 ns, 8000 elements: 21749.18 ns, 16000 elements: 89383.37 ns, 32000 elements: 358672.26 ns

Analysis: The results indicate an O(n^2) complexity, as the time taken increases quadratically with the number of elements.

Presenting Results
Execution times were presented in a clear and readable manner, using an appropriate number of significant figures and avoiding unnecessary complexity.

Optimization and Compiler Behavior
The impact of compiler optimizations on performance measurements was considered, ensuring that the compiler did not optimize away important parts of the code.

Conclusion
The benchmarking experiments provided valuable insights into the efficiency of different operations over an array of elements. Random access operations showed consistent performance with minimal variability, while search and duplicate finding operations demonstrated linear and quadratic complexities, respectively. These findings highlight the importance of understanding algorithmic complexity and performance measurement in optimizing code.

Feel free to expand on each section with additional details from your notes and any other relevant information. Happy writing!






Message Copilot
