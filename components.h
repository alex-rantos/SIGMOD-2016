/*
 * components.h
 *
 *  Created on: Dec 1, 2016
 *      Author: alex
 */

#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include "buffer.h"
#include "template_list.h"
#include "template_stack.h"
#include "template_queue.h"
// MACRO using typeof/template.
#define MIN(a,b) \
    ({ typeof (a) _a = (a); \
	typeof (b) _b = (b); \
    _a < _b ? _a : _b; })

#define MAX(a,b) \
    ({ typeof (a) _a = (a); \
	typeof (b) _b = (b); \
    _a > _b ? _a : _b; })

#define ROWS 50
#define THRESHOLD 0.5
enum GRAIL_ANSWER {NO, MAYBE, YES};

struct Label{
	int min_rank;
	int rank;
    int visited;
    int flag;
	Label() :min_rank(-1),rank(-1),visited(0),flag(0){}
};

struct Component {
    Label label[GRAILTIMES];  //Gia ton Grail
    //int* connectedComponents;
	int componentID; //current component id
	int nodesSum; //number of nodes in component
    int lastArrayEdge;
	List<int>* includedNodesID; //ids of included nodes
	Component() : componentID(-1), nodesSum(0),lastArrayEdge(0) {
		//connectedComponents = NULL;
		includedNodesID=new List<int>;
	}
};

struct ComponentCursor {
	int cursor;
};

struct TarzanInfoStruct {
	int recursive_level; // numbers the iteration this node was discovered.Mporw na xrisimopoihsw kai src_level kai na to thetw pali -1 sto telos
	int index; // numbers the nodes consecutively in the order in which they are discovered
	int lowlink; // smallest index of any node known to be reachable this->Node,this included
	int parentNode; // node_id which this->node has been called from.
	bool visited;
	TarzanInfoStruct() : recursive_level(0), index(-1),
						 lowlink(-1), parentNode(-1),visited(false) {}
};

class SCC {
	Component** components;// Components index - a vector which storesthe components information
	int componentsCount;
    int size;
    int* PushChecker;
	//int componentIDs[NUM]; //inverted index den to xreiazomaste exoume metavliti sto index
	int level;
    List<int>** edges; //oi akmes tou hypergraph
    int** edgesArray;
public:
	SCC(int);
	~SCC();
    List<int>** GetStrongEdges(){ return edges;};
    Component** GetComponent(){ return components;};
    int GetCompCount(){ return componentsCount;};
    int* GetPushChecker(){ return PushChecker;};
    void AddComponentToArray(Component*);
    void EstimateSCC(Buffer* ,Index* ,int);
	int FindNodeSCC_ID(int,Index*);
	bool NextSCC_ID(ComponentCursor* );
	int EstimateShortestPathSCC(Buffer*,Index*,int ,int,int,int);
    int SrcBeforeDest(Index*,Buffer*,int,int,int,int,int ,int,int,Queue<int>*,Queue<int>* );
    int DestBeforeSrc(Index*,Buffer*,int,int,int,int,int ,int,int,Queue<int>*,Queue<int>* );
    bool DestroySCC();
    void Tarjan(int ,Stack<int>* ,Index* ,Buffer* ,int, TarzanInfoStruct[], int*);
	void Print();
    int Subset(Label a,Label b);
    void BuildHypergraph(Index*,Buffer*);
    void BuildGrailIndex(int);
    void ResetEdges();
    int GetUnvisitedEdge(int);
    int GetNextEdge(int);
    void ShuffleEdges();
    int GetNextArrayEdge(int);
    void GrailProgress(int,int*,int);
    GRAIL_ANSWER IsReachableGrail(Index*,int,int);
};



struct UpdateIndex {
	List<int>*index;
	int emptyindex;
	int differentcc; //poses enwseis tha yparksoun

	UpdateIndex(int);
	void Insert_Components(int ,int);
	void Find_Connections(int ,List<int>*);
	int Find_Non_Empty_Cell();
	int Search_Connection(int, int);
	void DestroyIndex();

};
struct UpdateIndex {
	
	List<int>*index;
	int emptyindex;
	int differentcc; //poses enwseis tha yparksoun

	UpdateIndex(int);
	void Insert_Components(int ,int);
	int Find_Connections(List<int>*);
	int Find_Non_Empty_Cell();
	int Search_Connection(int, int);
	void Clear_Table();
	void DestroyIndex();
	
};

class CC {	
	int* ccindex; //CCIndex
	
public:
	CC(int);
	int metricVal;
	UpdateIndex* updateIndex;
	int num_of_comp;
	CC* EstimateCC(Buffer* );
	bool InsertNewEdge(int ,int );
	void Set_Comp(int, int);
	int Get_Comp(int);
	int*Get_CCindex(){return ccindex;}
	int FindNodeCC_ID(uint32_t nodeId);
	void RebuildIndex(List<int>*,int, int );
	void DestroyCC();
	void print_cc();
	
	void Print_Index();
};

#endif
