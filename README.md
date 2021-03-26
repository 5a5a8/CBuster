# CBuster

A simple dirbuster clone written in C.

This software works if you use it properly but it is not here to hold your hand.
It will not catch every error and is actually simple to use.

If you fire up too many threads and bring down your target or get your IP
blocked that's your problem.

## Installation and Usage
Install libcurl if not already installed, then clone this repository. Then:
```
cd CBuster
make
cd bin
./cbuster -h
```

Example usage - search for php files on example.com using 32 threads,
a 1 second delay between requests for each thread, and write the result
to example.csv:

```
./cbuster -t 32 -o example.csv -s 1000 https://example.com/{}.php
```

The output will be written to a csv file with the format
```[url];[status_code].```
This can then be processed using other tools. For example, to extract all
lines with a 200 response:

```
cat example.csv | grep \;200 > 200.csv
```
