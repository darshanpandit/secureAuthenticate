#secureAuthenticate

Github Link: https://github.com/darshanpandit/secureAuthenticate

Demo Can be found at 

> Pardon me for this was in a hurry.

Lots of redundancies but pretty much what I could do in around less than 48 hours to work with.
Lacks on comments and a terrible redundancies.

Following is a protoype for a secure authentication scheme.
* We first intiate a 'm' random strings each of length 'l' and call this set as 'R'.
* In the next step we initiate a set of 't' permutations, and save them in set 'I'.
* The idea is instead of saving a hash directly, we store computations over parts of a bigger file. In this case using hash  value from the identification tuple, we then select 32 bits selected using the I[t] and select the R[n] using it.
* R[n] as xored for each I[t] and the resulting string is stored as the identifier, instead of hash.

###Building the tool.
The utility contains only 1 file. main.cpp
The following flags must be set to build each of the files, if a build is desired:
* fpermissive
* lss
* lcrypto

The program is self-interactive thereon.
>Do find the attached analysis report and graph plot.
