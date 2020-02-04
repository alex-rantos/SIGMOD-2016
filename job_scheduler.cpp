#include "job_scheduler.h"

using namespace std;


pthread_mutex_t mtx;
pthread_cond_t empty_queue;
pthread_cond_t print;

void StaticJobInit(
		Job *job,
		void* (*adressToFunction)(void *) ,
		int source,int dest,int commandCounter,
		Index *index,Buffer *buffer,void *compPointer,JobScheduler *_js)
{
	job->adressToFunction = adressToFunction; // storing function call

	job->src = source;
	job->dest = dest;
	job->printPos = commandCounter;
	job->componentsPointer = compPointer;
	job->index = index;
	job->buffer = buffer;
	job->js = _js;
}
void DynamicJobInit(
		Job *job,
		void* (*adressToFunction)(void *) ,
		int source,int dest,int commandCounter,
		Index *index,Buffer *buffer,void *compPointer,JobScheduler *_js,
		int query_version,int* _metric)
{
	job->adressToFunction = adressToFunction; // storing function call

	job->src = source;
	job->dest = dest;
	job->printPos = commandCounter;
	job->componentsPointer = compPointer;
	job->index = index;
	job->buffer = buffer;
	job->js = _js;

	// Dynamic Query extra variables
	job->query_version = query_version;
	job->metric = _metric;
}

void* DynamicQuery(void *job) {
	Job *j = ((Job*) job);
	Index *index = j->index;
	Buffer *buffer = j->buffer;
	int *printArray = j->js->GetArray();
	//CC* cindex = static_cast<CC*>(j->componentsPointer);
	int source = j->src;
	int dest = j->dest;
	int repeat = j->repeat;
	int threadNum = j->id;
	int version = j->query_version;

	printArray[j->printPos] = //buffer->Query(source, dest, index,-1,repeat,threadNum);
			buffer->DynamicQuery(source,dest,index,repeat,threadNum,version);
	/*if (cindex->Get_Comp(source) == cindex->Get_Comp(dest))
		printArray[j->printPos] = //buffer->Query(source,dest,index,-1,repeat,threadNum);
				buffer->DynamicQuery(source,dest,index,repeat,threadNum,version);
		//cout << "ektelesi query" << endl;
	else {

		(*(j->metric))++;
		int result = cindex->updateIndex->
				Search_Connection(cindex->Get_Comp(source),cindex->Get_Comp(dest));
		if (result == 1)
			printArray[j->printPos] = //buffer->Query(source,dest,index,-1,repeat,threadNum);
					buffer->DynamicQuery(source,dest,index,repeat,threadNum,version);
			//cout << "the 2 componets are joined" << endl;
		else {
			printArray[j->printPos] = -1;
		}
	}*/
	delete j;
	return NULL;
}


void* StaticQuery(void *job) {
	Job *j = ((Job*) job);
	Index *index = j->index;
	Buffer *buffer = j->buffer;
	int *printArray = j->js->GetArray();
	SCC* strongCC = static_cast<SCC*>(j->componentsPointer);
	int source = j->src;
	int dest = j->dest;
	int repeat = j->repeat;
	int threadNum=j->id;
	IndexNode* p = index->GetIndexNode();

	int k = strongCC->IsReachableGrail(index, source, dest);
	if (k == 0) {
		printArray[j->printPos] = -1;
	}
	else if (k == 1) {
		//result << "MAYBE";
		printArray[j->printPos] =
				strongCC->EstimateShortestPathSCC(buffer,index,source,dest,repeat,threadNum);

	} else if (k == 2) {
		//result << "YES";
		printArray[j->printPos] = buffer->Query(source, dest, index,
						p[source].componentID,repeat,threadNum);
	}
	delete j;
	return NULL;
}

void* JobScheduler::ExecuteThread(void *job) {
	Job *j = ((Job*) job);
	int threadID = j->id;
	Queue<Job*> *queue = j->js->GetQueue();
	JobScheduler *js = j->js;
	int repeat = 0;
	int *running = js->GetRunningThreads();
	(*running)++;// +1 to avoid signalling cond_print
	/*pthread_mutex_lock(&mtx);
	cout << "\tThread #" << threadID << " was created!" << endl;
	pthread_mutex_unlock(&mtx);*/
	delete j; // delete initializer task
	while(1) {
		/* Mutex lock outside of if-condition
		 * to avoid 'dequeuing' NULL jobs. */
		pthread_mutex_lock(&mtx);
		if (!queue->isEmpty()) {
			j = queue->Dequeue();
		}
		else {
			//pthread_mutex_lock(&mtx);
			if (js->IsFinished()) {
				pthread_mutex_unlock(&mtx);
				break;
			}
			else {
				(*running)--;
				if ((*running) == 0) {
					//pthread_mutex_lock(&pmtx);
					//cout << "\tThread #" << threadID << " signalling for print!" << endl;
					pthread_cond_signal(&print);
					//pthread_mutex_unlock(&pmtx);
				}
				//cout << "\tThread #" << threadID << " is waiting|EMPTY QUEUE! " << *running << endl;
				pthread_cond_wait(&empty_queue,&mtx);
			}
			//cout << "\tThread #" << id << " exiting!" << endl;
			pthread_mutex_unlock(&mtx);
			continue;
		}
		//cout << "\tThread #" << threadID << " perfomming address!" << endl;
		pthread_mutex_unlock(&mtx);
		j->id = threadID;
		j->repeat = repeat;
		j->adressToFunction((void*) j);
		repeat++;
	}
	return NULL;
}

JobScheduler::JobScheduler(int threadpool) {
	executionThreads = threadpool;
	queue = new Queue<Job*>();
	printArray = NULL;
	queue_size = 0;
	runningThreads = new int(threadpool);
	finished = false;
	result.open(OUTPUT_FILE); //output file for Queries
	tids = new pthread_t[executionThreads];

	if (pthread_mutex_init(&mtx, NULL) != 0)
		Psystem_error("mutex init ");
	if (pthread_cond_init(&empty_queue, NULL) != 0)
		Psystem_error("cond init ");
	if (pthread_cond_init(&print, NULL) != 0)
		Psystem_error("cond init ");

	for( int i = 0; i < executionThreads; i++) {
		Job *job = new Job();
		job->js = this;
		job->id = i;
		if (pthread_create(&tids[i],NULL,this->ExecuteThread,job) != 0)
			Psystem_error("pthread_create ");
	}
}

JobScheduler::~JobScheduler() {

	this->DestroyAll(); // Destroy all waiting threads.

	delete queue;
	delete[] tids;
	delete runningThreads;
	delete[] printArray;
	result.close();
	if ((pthread_mutex_destroy(&mtx)) != 0)
		Psystem_error("mutex destroy mtx");
	if ((pthread_cond_destroy(&empty_queue)) != 0)
		Psystem_error("cond destroy cond");
	if ((pthread_cond_destroy(&print)) != 0)
		Psystem_error("cond destroy print");

}

void JobScheduler::SubmitJob(Job *j) {
	if (pthread_mutex_lock(&mtx) != 0)
		Psystem_error("mtx lock @ JobScheduler::SubmitJob ");

	queue->Enqueue(j);

	if (pthread_mutex_unlock(&mtx) != 0)
		Psystem_error("mtx unlock @ JobScheduler::SubmitJob ");
}
void JobScheduler::ExecuteJobs() {
	(*runningThreads) = executionThreads;
	queue_size = this->queue->GetSize();
	if (printArray == NULL)
		this->printArray = new int[queue_size];
	else {
		delete[] printArray;
		this->printArray = new int[queue_size];
	}

	// Assign tasks to empty Thread pool
	pthread_mutex_lock(&mtx);
	pthread_cond_broadcast(&empty_queue);
	pthread_mutex_unlock(&mtx);

	this->WaitAll();
}


void JobScheduler::WaitAll() {
	pthread_mutex_lock(&mtx);
	pthread_cond_wait(&print,&mtx);
	pthread_mutex_unlock(&mtx);

	this->PrintResults();
}

void JobScheduler::DestroyAll() {

	finished = true;
	cout << "Signalling threads for destruction." << endl;
	pthread_mutex_lock(&mtx);
	pthread_cond_broadcast(&empty_queue);
	pthread_mutex_unlock(&mtx);

	for( int i = 0; i < executionThreads; i++) {
		void *threadReturn;
        if (pthread_join(tids[i], &threadReturn) < 0) {
        	perror("pthread_join ");
        	exit(EXIT_FAILURE);
        }
        cout << "Thread #" << i << " ended." << endl;
	}
}

void JobScheduler::PrintResults() {
	for (int i = 0 ; i < this->queue_size ; i++) {
		result << this->printArray[i] << "\n";
	}
}
