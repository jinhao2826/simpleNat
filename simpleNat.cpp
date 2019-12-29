#include <unordered_map>
#include <fstream>
#include <string>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
using namespace std;


/*Unordered maps are associative containers that store elements formed by the combination of a key value and a mapped value, and which allows for fast retrieval of individual elements based on their keys.
In an unordered_map, the key value is generally used to uniquely identify the element, while the mapped value is an object with the content associated to this key. Types of key and mapped value may differ.
Internally, the elements in the unordered_map are not sorted in any particular order with respect to either their key or mapped values, but organized into buckets depending on their hash values to allow for fast access to individual elements directly by their key values (with a constant average time complexity on average).
unordered_map containers are faster than map containers to access individual elements by their key, although they are generally less efficient for range iteration through a subset of their elements.
Unordered maps implement the direct access operator (operator[]) which allows for direct access of the mapped value using its keyvalue as argument.*/

class natMatchFlow{

public:
	natMatchFlow() {}
	natMatchFlow(string ip, string port): flowIp(ip), flowPort(port){}

	bool operator == (const natMatchFlow& a) const {
        	return  (a.flowIp == flowIp) && (a.flowPort == flowPort);
	}	
	string flowIp;
	string flowPort;
};

class natTransTo{

public:
	string  natIp;
	string  natPort;
	
};


/*template < class Key,
           class T,
           class Hash = hash<Key>,
           class Pred = equal_to<Key>,
           class Alloc = allocator< pair<const Key,T> >
           > class unordered_map;*/

/*natMatchFlowHash is a hash function: flowIp of low 8bits + flowPort of low 8bits*/
/*So the total number of  buckets is (2^16-1)*/
/*for example: 10.0.1.1:8080
  flowIp of low 8bits is 1 and flowPort of low 8bits is 80
  (flowIp of low 8bits)<<8 | (flowPort of low 8bits)&0x000000ff
  = 256+144 = 400
*/

class natMatchFlowHash {

public:
    size_t operator () (const natMatchFlow& t) const {
	string::size_type end = t.flowIp.size();
	string::size_type pos = t.flowIp.find_last_of(".");
	string ip_part4 = t.flowIp.substr(pos+1, end-pos+1);
	return (atoi(t.flowPort.c_str()) & 0x000000ff) | (atoi(ip_part4.c_str()) << 8);
    }
};


int main(){

	natMatchFlow matchflow;
	natTransTo natTo;
	unordered_map<natMatchFlow, natTransTo, natMatchFlowHash> natRules;
	string natRulesFile = "natfile";
	ifstream natRule_infile(natRulesFile.c_str(), ios::in);
	if (!natRule_infile ) {
		cerr << "oops! unable to open file " << natRulesFile << endl;
		exit( -1 );
	}
	
	string filt_elems(":,");
	string textline;
	cout << "==========read entries from natfile============" << endl;
	while (getline(natRule_infile, textline, '\n')) {
		cout << "natfile line read: " << textline << '\n';
		string::size_type pos = 0;
		string::size_type prev_pos = 0;
		
		unsigned int location = 0;

		while ((pos = textline.find_first_of(filt_elems, pos)) != string::npos ){
			string value = textline.substr(prev_pos, pos-prev_pos);
			cout << "location[" << location << "]:" << value << endl;
			switch (location) {
					case 0:
						matchflow.flowIp = value;
						break;
					case 1:
						matchflow.flowPort = value;
						break;
					case 2:
						natTo.natIp = value;
						break;
					case 3:
						natTo.natPort = value;
						break;
					default:
						break;
			}		
				
			prev_pos = ++pos;
			location++;
			sleep(1);
		}
		
		string port = textline.substr(prev_pos, pos-prev_pos);
		cout << "location[" << location << "]:" << port << endl;
		natTo.natPort = port;
		natRules[matchflow] = natTo;
		sleep(1);
	}
	
	
	string flowsFile = "flowfile";
	natMatchFlow perflow;
	natMatchFlow wildcard_flowIp;
	natMatchFlow wildcard_flowPort;
	ifstream flows_infile( flowsFile.c_str(), ios::in);
	if (!flows_infile) {
		cerr << "oops! unable to open file " << flowsFile << endl;
		exit(-1);
	}

	
	string outputFile = "outputfile";
	ofstream outfile(outputFile.c_str());
	if (!outfile) {
		cerr << "oops! unable to open file " << outputFile << endl;
		exit(-1);
	}
	
	string flowline;
	
	cout << "===========read entries from flowfile===========" << endl;
	while (getline(flows_infile, flowline, '\n' )) {
		cout << "flowfile line read: " << flowline << '\n';
		string::size_type pos = 0;
		string::size_type prev_pos = 0;		

		while((pos = flowline.find_first_of(':', pos)) != string::npos){
			string value = flowline.substr(prev_pos, pos-prev_pos);
			cout << "flowIp:" << value << endl;
			if(prev_pos == 0) {
				perflow.flowIp = value;				
			}else{
				perflow.flowPort = value;
			}
			
			if(prev_pos == 0) {
				wildcard_flowIp.flowIp = "*";				
			}else{
				wildcard_flowIp.flowPort = value;
			}
			
			if(prev_pos == 0) {
				wildcard_flowPort.flowIp = value;				
			}else{
				wildcard_flowPort.flowPort = "*";
			}
			
			prev_pos = ++pos;
		}
	
		string value = flowline.substr(prev_pos, pos-prev_pos);
		cout << "flowPort:" << value << endl;
		perflow.flowPort = value;
		wildcard_flowIp.flowPort = value;
		wildcard_flowPort.flowPort = "*";
	
		natTransTo mapToNat;
		
		unordered_map<natMatchFlow, natTransTo, natMatchFlowHash>::iterator it;
		/**********Search Priority************/
		/*1: search exact match, e.g., 10.0.1.1:8080
		  2: search ip is wildcard, e.g., *.8080
	 	  3: search port is wildcard, e.g., 10.0.1.1:*
		*/
			
		if((it = natRules.find(perflow)) != natRules.end()){
			mapToNat = (*it).second;
			outfile << perflow.flowIp << ":" << perflow.flowPort << "->";
			outfile << mapToNat.natIp << ":" << mapToNat.natPort << '\n';

		}else if((it = natRules.find(wildcard_flowIp)) != natRules.end()){
			mapToNat = (*it).second;	
			outfile << perflow.flowIp << ":" << perflow.flowPort << "->";
			outfile << mapToNat.natIp << ":" << mapToNat.natPort << '\n';
						
		}else if((it = natRules.find(wildcard_flowPort)) != natRules.end()) {
			mapToNat = (*it).second;	
			outfile << perflow.flowIp << ":" << perflow.flowPort << "->";
			outfile << mapToNat.natIp << ":" << mapToNat.natPort << '\n';
		}else{
			outfile <<"No nat match for " << perflow.flowIp << ":" << perflow.flowPort << endl;
		}
		
	}
	
	cout << "Finished: The results have been recorded in outputfile" << endl;
	outfile.close();
	flows_infile.close();
	natRule_infile.close();
	
	return 0;	
	
}
