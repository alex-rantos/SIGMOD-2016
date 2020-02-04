#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <chrono>

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "buffer.h" // Part 1
#include "components.h" // Part 2
#include "job_scheduler.h" // Part 3

using namespace std;

inline int max(int a, int b) {
	return((a>b) ? (a) : (b));
}

void CreateGraph(Buffer *buffer,Index *index,ifstream &myFile,string specifier) {
	int source, dest;
	string line;
	if (specifier == "STATIC") {
		while (getline(myFile, line)) {
			istringstream iss(line);
			if (!(iss >> source >> dest))
				break;
			index->Insert(source, dest, buffer);
			buffer->InsertBuffer(source, dest, index);
		}
	} else if (specifier == "DYNAMIC") {
		while (getline(myFile, line)) {
			istringstream iss(line);
			if (!(iss >> source >> dest))
				break;
			index->Insert(source, dest, buffer);
			buffer->InsertBuffer(source, dest, index,0);
		}
	} else {
		cerr << "Wrong specifier @ workload file(neither 'STATIC' nor 'DYNAMIC'."
				"Check spelling.Program exiting." << endl;
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv) {
	cout << "Program is running " << endl;
	if (argc != 3) {
		cerr << "Need TWO(not " << argc << ") input files(Graph & Workload,respectively) as arguments" << endl;
		exit(EXIT_FAILURE);
	}

	auto start_time = std::chrono::high_resolution_clock::now();

	/*
	 * Read workload file 1st line to see the kind of graph,
	 * so to avoid initializing edgeProperty.
	 */
	ifstream workload;
	char *workFile = argv[2];
	workload.open(workFile);
	string specifier;
	if (workload.is_open()) {
		getline(workload,specifier); // DYNAMIC || STATIC
		cout << "Graph is " << specifier << endl;
	} else {
		cerr << "Workload-file couldn't be opened!Aborting graph creation & exitting" << endl;
		exit(EXIT_FAILURE);
	}

	/**************		Read from Graph file 	 **************/
	char *graphFile = argv[1];
	ifstream myFile;
	myFile.open(graphFile);
	string line;
	int maxVal = 0;
	/*
	* Find :
	* max value of file so we don't need to reallocate Index array.
	* the average of neighbors for the most optimal allocation.
	*/
	if (myFile.is_open()) {
		int source, dest;
		while (getline(myFile, line)) {
			istringstream iss(line);
			if (!(iss >> source >> dest))
				break;
			maxVal = max(maxVal, max(source, dest));
			/*if (source != srcompare) {
				uniqueNodes++;
				srcompare = source;
			}
			linesCounter++;*/
		}
		//optimalCellValue = ceil(linesCounter / (float)uniqueNodes);
	}
	else
		cerr << "Unable to open Graph file(1)" << endl;

	Buffer *buffer = new Buffer(maxVal + 1);
	Index *index = new Index(maxVal + 1);

	/** Resetting file for second read**/
	myFile.clear();
	myFile.seekg(0, myFile.beg);

	if (myFile.is_open()) {
		CreateGraph(buffer,index,myFile,specifier);
	}
	else
		cerr << "Unable to open Graph file(2)" << endl;
	myFile.close();

	cout << "Index & Graph were created." << endl;

	// Creating a threadpool equal to the system cores
	JobScheduler *js = new JobScheduler(THREADPOOL);
	index->InitializeVisited(THREADPOOL);
	auto current_time = std::chrono::high_resolution_clock::now();

	std::cout << "Function was running for " <<
	std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() << "\"" << std::endl;

	//buffer->PrintBuffer(index); // insert_unitest

	/**************		Read from Workload file	 **************/

	int source, dest,
	   commandCounter = -1,version = 0;
	char command;
	if (specifier == "STATIC") {
		cout << "Graph is labeled as STATIC.\nPerfoming Tarjan algorithm." << endl;

		int estimatedComponentsAmount = maxVal / 3 ;
		if (estimatedComponentsAmount == 0)
			estimatedComponentsAmount = 50;
		SCC* strongCC = new SCC(estimatedComponentsAmount);
		strongCC->EstimateSCC(buffer,index,maxVal);

		cout << "Building Hyper Graph & Grail Indexes." << endl;
		strongCC->BuildHypergraph(index, buffer);
		for (int i = 0; i < GRAILTIMES; i++) {
			strongCC->BuildGrailIndex(i);
		}

		auto current_time = std::chrono::high_resolution_clock::now();

		std::cout << "Function was running for " <<
		std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() << "\"" << std::endl;
		//result << strongCC.GetCompCount();
		if (workload.is_open()) {
			while (getline(workload, line)) {
				istringstream iss(line);
				iss >> command;
				if (command == 'Q') {
					Job *job = new Job();
					commandCounter++;
					iss >> source >> dest;
					StaticJobInit(job,StaticQuery,source,dest,commandCounter,
							index,buffer,(void *)strongCC,js);
					js->SubmitJob(job);
				} else if (command == 'F') {
					commandCounter = -1;
					js->ExecuteJobs(); // Execute whole queue of jobs and wait till finished.
					continue;
				} else if (command == 'A') {
					//cout << "Found additions on static graph.Expect wrong results." << endl;
					continue;
				}
			}
		delete strongCC;
		}
	} else if (specifier == "DYNAMIC") {
		CC*cindex=new CC(index->GetSize());
	//	CC *cindex = new CC;
		
		int *metric = new int();
		
		cout << "Graph is labeled as DYNAMIC.\n"
				"Perfoming BFS for weakly connected componenets." << endl;
		cindex = buffer->estimateConnectedComponents(index);
		cindex->updateIndex = new UpdateIndex(cindex->num_of_comp);
		cindex->print_cc();
		//int num_of_queries=0;  //arithmps erwtimatwn
		int index_queries=0; // queries pou xrisimopoioun updateindex
 		float metricVal=0.0;
		List<int>* Connected=new List<int>[cindex->updateIndex->differentcc]; //pinakas pou tha organwnei tis cc pou enwthikan
		if (workload.is_open()) {
			char lastCommand = 'N';
			while (getline(workload, line)) {
				istringstream iss(line);
				iss >> command;
				if (command == 'Q') {
					Job *job = new Job();
					commandCounter++;
					iss >> source >> dest;
					DynamicJobInit(job,DynamicQuery,source,dest,commandCounter,
							index,buffer,(void *)NULL,js,
							version,metric);
							
					if(cindex->Get_Comp(source)==cindex->Get_Comp(dest)) cout<<"ektelesi query"<<endl; //oi kovmoi vriskontai sto idio component
				else{
					
					index_queries++;
				
					result=cindex->updateIndex->Search_Connection(cindex->Get_Comp(source),cindex->Get_Comp(dest));	 //elegxos gia to an oi components exoun enwthei
				
					if(result==1){
						 cout<<"the 2 componets are joined"<<endl;
							//getchar();
					}
					else if(result==0){ //ksexwrista components: den yparxei monopati metaksy twn komvwn
						cout<<"no path between the nodes"<<endl;
					}
					metricVal=(float)index_queries/(float)commandCounter;
					
					if((metricVal>=THRESHOLD)&& (cindex->updateIndex->emptyindex!=0)){ //rebuild otan kseperastei to threshold
					
						//euresi twn enwmenwn pleon components  kai anadomisi tou ccindex me ta nea stoixeia
					 
						int count_comp=cindex->updateIndex->Find_Connections(Connected);	
		
						cindex->RebuildIndex(Connected,index->GetSize(),count_comp);
					
						metricVal=0;
					
						index_queries=0;
						
						num_of_queries=0;
						
						delete []Connected;
				
						Connected=NULL;
						
						Connected= new List<int>[cindex->updateIndex->differentcc];
					//	cindex->updateIndex->Clear_Table();
					}
				} 		
					js->SubmitJob(job);
					//buffer->Query(source,dest,index,'p',1,0);
				}
				else if (command == 'A') {
					if (lastCommand == 'Q')
						version++;
					iss >> source >> dest;
					 //Checking if reallocation is needed for Index.
					 index->CheckCap(source,dest);
					 index->Insert(source, dest, buffer);
	
					buffer->AddNeighbor(source, dest, index, version);
					if (cindex->Get_Comp(source) != cindex->Get_Comp(dest))	cindex->updateIndex->Insert_Components(source, dest);
				}
				else if (command == 'F') {
					commandCounter = -1;
					js->ExecuteJobs();
				}
				lastCommand = command;
			}
		}
		else
			cout << "Wrong specifier for WORKLOAD file." << endl;

		//delete metric;
		cindex->DestroyCC();
		delete cindex;
	}
	else
		cerr << "Unable to open Workload file" << endl;

	workload.close();

	delete js;
	delete buffer;
	delete index;

	cout << "Program terminated!" << endl
			<< "Results can be found @ " << OUTPUT_FILE << endl;
	return 0;
}
