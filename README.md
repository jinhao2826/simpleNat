# simpleNat
This program will read a list of NAT entries and a list of flows, and output the flows after they have been processed by your NAT.  
So MUST have two files named *natfile* and *flowfile*

## How to compile and run
g++ -c simpleNat.cpp -std=c++11  
g++ -o simpleNat simpleNat.o  
./simpleNat  

## Description
Input: *natfile*    
The inputs to your program should be two files; NAT and FLOW.  
The NAT file will describe the translations for your NAT Table. The format of each line will be as so:  
<ip>:<port>,<ip>:<port>  

The first ip, port pair is original ip and port that should be translated to the second ip, port pair. The ip or the port (not both) can have a “\*” instead of an ip or port. The star means that anything should match that value. For example, the following are all valid inputs:   

10.0.1.1:8080,192.168.0.1:80  
\*:8081,192.168.0.1:80  
10.0.1.2:\*192.168.0.1:80  

The FLOW file will describe the incoming flows through your NAT. The format of each line will be as so  
<ip>:<port>  
The ip,port pair is the incoming flow that may or may not be modified by your NAT. There cannot be a “\*” in the input for the flow. For example, the following is a valid input  
192.168.0.1:8081  

Output  
The output to your program should be one file; OUTPUT.  
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

