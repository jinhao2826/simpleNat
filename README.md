# simpleNat

## Introduction
This program will read a list of NAT entries from **natfile** and a list of flows from **flowfile**, and output the flows after they have been processed by the NAT program.  
So MUST have two files named **natfile** and **flowfile**

## How to run
g++ -c simpleNat.cpp -std=c++11  
g++ -o simpleNat simpleNat.o  
./simpleNat  
**NOTE: MUST add -std=c++11 when you are compiling this code because unordered_map is used**

## Description
The inputs to this program should be two files, NAT file and FLOW file.  
Input1: named **natfile**    

The NAT file will describe the translations for the NAT Table. The format of each line will be as so:  
\<ip\>:\<port\>,\<ip\>:\<port\>  

The first ip, port pair is original ip and port that should be translated to the second ip, port pair. The ip or the port (not both) can have a “\*” instead of an ip or port. The star means that anything should match that value. For example, the following are all valid inputs:   

10.0.1.1:8080,192.168.0.1:80  
\*:8081,192.168.0.1:80  
10.0.1.2:\*192.168.0.1:80  

Input2: named **flowfile**  
The FLOW file will describe the incoming flows through the NAT. The format of each line will be as so:    
\<ip\>:\<port\>  

The ip,port pair is the incoming flow that may or may not be modified by the NAT program. There cannot be a “\*” in the input for the flow. For example, the following is a valid input:    
192.168.0.1:8081  

Output: named **outputfile**  
The output of this program should be one file.  
The ouput file should have one entry corresponding to each line in the FLOW file. If there is a match, that line should output the original ip, port pair and the translated ip, port pair. If there is no match, the program should output that there is no NAT entry for that ip, port pair.  
You may assume that the input file contains only valid, well-formed entries. A sample input NAT and FLOW file as well as the corresponding sample output for those two inputs are below.

Sample Inputs  
NAT File  
10.0.1.1:8080,192.168.0.1:80  
10.0.1.1:8084,192.168.0.2:8080  
10.0.1.1:8086,192.168.0.4:80  
10.0.1.1:8082,192.168.0.3:80  
10.0.1.2:8080,1.1.1.1:1  
\*:21,1.2.3.4:12  
10.11.12.13:\*,40.100.20.7:3389  

FLOW File  
10.0.1.1:8080  
5.6.7.8:55555  
10.0.1.1:8086  
9.8.7.6:21  
10.1.1.2:8080  
34.65.12.9:22  
10.0.1.2:8080  

Sample Output  
10.0.1.1:8080 -> 192.168.0.1:80  
No nat match for 5.6.7.8:55555  
10.0.1.1:8086 -> 192.168.0.4:80  
9.8.7.6:21 -> 1.2.3.4:12  
No nat match for 10.1.1.2:8080  
No nat match for 34.65.12.9:22  
10.0.1.2:8080 -> 1.1.1.1:1  

## Design
### Why is unordered_maps NOT map?  
Unordered maps are associative containers that store elements formed by the combination of a key value and a mapped value, and which allows for fast retrieval of individual elements based on their keys.  
In an unordered_map, the key value is generally used to uniquely identify the element, while the mapped value is an object with the content associated to this key. Types of key and mapped value may differ.  
Internally, the elements in the unordered_map are not sorted in any particular order with respect to either their key or mapped values, but organized into buckets depending on their hash values to allow for fast access to individual elements directly by their key values (with a constant average time complexity on average).  
unordered_map containers are faster than map containers to access individual elements by their key, although they are generally less efficient for range iteration through a subset of their elements.  
**Unordered map is O(1) while map is O(logn).**     
In this NAT program, unordered_map contains three objects, **natMatchFlow**, **natTransTo** and **natMatchFlowHash**. Object natMatchFlow is used to present the first ip, port pair in NAT file.  natTransTo is used to present the second ip, port pair in NAT file which is translated to. natMatchFlowHash is a hash function which presents **the map between key(natMatchFlow) and bucket(natTransTo)**.  
```C
unordered_map<natMatchFlow, natTransTo, natMatchFlowHash> natRules
```


```c
template < class Key,  
           class T,  
           class Hash = hash<Key>,  
           class Pred = equal_to<Key>,  
           class Alloc = allocator< pair<const Key,T> >  
           > class unordered_map;  
```
The hash function is a unary function that takes an object of type key_type as argument and returns a unique value of type size_t based on it.  
natMatchFlowHash is a hash function: flowIp of low 8bits + flowPort of low 8bits  
So the total number of  buckets is (2^16-1)   
for example: 10.0.1.1:8080  
flowIp of low 8bits is 1 and flowPort of low 8bits is 80  
(flowIp of low 8bits)<<8 | ((flowPort of low 8bits)&0x000000ff)  
= 256+144 = 400  
**NOTE:  
Define a good hash function is a worthy thinking problem to avoid hash collision. In here, the key I only use is flowIp of low 8bits + flowPort of low 8bits because I think the number of  buckets is (2^16-1) is enough for this simple NAT Program**

### How to handle the wildcard?
I defined a search sequence. At first, search exact match, e.g., 10.0.1.1:8080, if not found, search ip is wildcard, e.g., \*.8080, if still not found, search port is wildcard, e.g., 10.0.1.1:\*. If still failed to find, output no nat match for.




