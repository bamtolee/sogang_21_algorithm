#define _CRT_SECURE_NO_WARNINGS
using namespace std;
#include <time.h>
#include <stack>
#include "DBL.h"

//#define NO_PATH_OUT   // comment out this line for path output
void Error_Exit(const char *s);

typedef struct _Vertex {
	dblStack S;		// adj list contains edge index
	int degree;
} Vertex;

typedef struct _Edge {
	int v1, v2;
} Edge;

void graphGeneration(Vertex **V, Edge **E, int *VN, int *EN);
void adjListGenerate(Vertex *V, Edge *E, int VN, int EN);
void deallocGraph(Vertex *V, Edge *E, int VN);
int *Find_Euler(Vertex *V, Edge *E, int VN, int EN, int *flag, int *pathN);

DBList pool;	// DBL storage pool

int main() {
	int T, VN, EN;
	Vertex *V;
	Edge   *E;
	int *path;	// Euler cycle or path
	int pathN;  // path length
	int  flag;	// 0: cycle, 1: path, 2: none
	clock_t start_time, finish_time;

	scanf("%d", &T);	// read # of tests
	for (int t = 1; t <= T; t++) {	// for each test
		graphGeneration(&V, &E, &VN, &EN);

		start_time = clock(); // set the start time

		path = Find_Euler(V, E, VN, EN, &flag, &pathN); // find an Euler path or cycle

		finish_time = clock(); // set finish time
		
		double cmpt = (((double)(finish_time - start_time)) / CLK_TCK)*1000; // compute the time passed
		printf("Test= %d flag= %d VnumInCycle/Path)= %d ", t, flag, pathN);

		if (flag == 0)
			printf("Euler_cycle(exec_time= %.2f msec)\n",cmpt);
		else if (flag == 1)
			printf("Euler_path(exec_time= %.2f msec)\n", cmpt);
		else
			printf("not_Eulerian(exec_time= %.2f msec)\n", cmpt);

#ifndef NO_PATH_OUT
		if (flag != 2) {
			for (int i = 0; i < EN + 1; i++) {
				printf("%d\n", path[i]);
			}
		}
#endif
		if (flag != 2)
			delete[] path;
		deallocGraph(V, E, VN);
	}
	pool.freeDBL_pool();	// clear all the DBL elements

	return 0;
}

int *Find_Euler(Vertex *V, Edge *E, int VN, int EN, int *flag, int *pathN) {
	// input V, VN, E, EN
	// output through paramters
	//   *flag = 0 (Euler cycle), 1 (Euler path), 2 (not Eulerian)
	//   *pathN = size of path[] array
	// output by return
	//   *path = list of vertex ids found(Euler cycle or path)

	stack<int> ST;		// use STL stack as explained in the class
	int *path = NULL;

	// *** �� �κ��� �ۼ��ϼ���
	int degree_test = 0;
	int odd_v = -1;
	for (int v = 0; v < VN; v++) { //Euler cycle ����, path ����, �ƴϸ� Eulerian�� �ƴ��� Ȯ���ϴ� ����.
		if (V[v].degree % 2 != 0) {
			degree_test++; //degree�� Ȧ���� vertex ���� Ȯ��.
			odd_v = v;  //path�� ��� degree�� Ȧ���� vertex���� ��θ� �����ؾ� �ϹǷ� Ȧ�� vertex�� index ����.
		}
	}
	if (degree_test == 0) *flag = 0;  //Euler cycle
	else if (degree_test == 2) *flag = 1;  //Euler path
	else *flag = 2;  //not Eulerian

	int vv, edge_i;
	DBL* adj_e;
	*pathN = 0;
	if (*flag == 0) {  //Euler cycle
		path = new int[EN + 1];
		ST.push(0);
		while (!(ST.empty())) {
			vv = ST.top();
			if (V[vv].degree == 0) {
			//���� �湮�� vertex�� degree�� 0�̸� ���� ��� path�� ����
				path[*pathN] = vv;
				*pathN = *pathN + 1;
				ST.pop();
			}
			else {
	    //degree�� 0�� �ƴϸ� �湮�� vertex �����ϰ� �湮�� ����, ������ edge�� �� adjacent list���� �����ϰ� �ش� vertex���� degree -1.
				adj_e = V[vv].S.top();
				edge_i = adj_e->d;
				if (E[edge_i].v1 == vv) { //���� vertex�� E �迭�� v1���� ������ ������ v2�� ������ ������ if������ Ȯ��.
					V[E[edge_i].v2].S.remove(adj_e->twin);
					V[E[edge_i].v1].S.remove(adj_e);
					ST.push(E[edge_i].v2);
				}
				else if (E[edge_i].v2 == vv) {
					V[E[edge_i].v1].S.remove(adj_e->twin);
					V[E[edge_i].v2].S.remove(adj_e);
					ST.push(E[edge_i].v1);
				}
				V[E[edge_i].v2].degree--;
				V[E[edge_i].v1].degree--;
				pool.freeDBL(adj_e->twin); //twin pointer�� ���ÿ� ��ȯ�ؾ� ��.
				pool.freeDBL(adj_e);
			}
		}
	}
	else if (*flag == 1) {  //Euler path
		path = new int[EN + 1];
		ST.push(odd_v);
		while (!(ST.empty())) {
			vv = ST.top();
			if (V[vv].degree == 0) {
				path[*pathN] = vv;
				*pathN = *pathN + 1;
				ST.pop();
			}
			else {
				adj_e = V[vv].S.top();
				edge_i = adj_e->d;
				if (E[edge_i].v1 == vv) {
					V[E[edge_i].v2].S.remove(adj_e->twin);
					V[E[edge_i].v1].S.remove(adj_e);
					ST.push(E[edge_i].v2);
				}
				else if (E[edge_i].v2 == vv) {
					V[E[edge_i].v1].S.remove(adj_e->twin);
					V[E[edge_i].v2].S.remove(adj_e);
					ST.push(E[edge_i].v1);
				}
				V[E[edge_i].v2].degree--;
				V[E[edge_i].v1].degree--;
				pool.freeDBL(adj_e->twin);
				pool.freeDBL(adj_e);
			}
		}
	}

	return path;
}

void deallocGraph(Vertex *V, Edge *E, int VN) {
	DBL *p;

	// *** ���⿡ adj list�� �����ϴ� routine�� �ۼ��ϼ���
	for (int v = 0; v < VN; v++) {
		while (!(V[v].S.empty())) {
			p = V[v].S.pop();
			pool.freeDBL(p);
		}
	}
	delete[] V;
	delete[] E;
}

void graphGeneration(Vertex **V, Edge **E, int *VN, int *EN) {
	scanf("%d %d", VN, EN);	// read # of vertices and edges
	//allocVandEarray(V, E, *VN, *EN);	// vertex and edge array allocation

	*V = new Vertex[*VN];
	*E = new Edge[*EN];
	if (*V == NULL || *E == NULL) {
		Error_Exit("Memory Allocation Error!");
	}

	for (int v = 0; v < *VN; v++) {
		(*V)[v].degree = 0;
	}
	for (int e = 0; e < *EN; e++) {
		scanf("%d %d", &((*E)[e].v1), &((*E)[e].v2));	// read edge information
		++((*V)[(*E)[e].v1].degree);
		++((*V)[(*E)[e].v2].degree);
	}
	adjListGenerate(*V, *E, *VN, *EN);	// create adj lust in G=(V,E)
}

void adjListGenerate(Vertex *V, Edge *E, int VN, int EN) {
	// Construct adjacent list in V array
	int v1, v2;
	DBL *adjE1, *adjE2;

	// *** �� �κ��� �ۼ��ϼ���
	pool.DBL_cnt = 0;
	pool.UsedMemoryForDBLs = 0;
	for (int e = 0; e < EN; e++) {
		v1 = E[e].v1; v2 = E[e].v2;
		adjE1 = pool.allocDBL();
		adjE2 = pool.allocDBL();
		adjE1->d = e; adjE2->d = e;
		adjE1->twin = adjE2; adjE2->twin = adjE1;
		V[v1].S.push(adjE1); V[v2].S.push(adjE2);
	}
}

void Error_Exit(const char *s) {
	printf("%s", s);
	exit(-1);
}

DBL *DBList::allocDBL(void) {		// allocate a DBL element
	DBL *p;

	// *** �� �κ��� �ۼ��ϼ���
	if (DBL_pool == NULL) {
		p = new DBL;
		if (p == NULL) {
			Error_Exit("Memory alloc error(Alloc_DBL)");
		}
		UsedMemoryForDBLs += sizeof(DBL);
		p->d = NONE;
	}
	else {
		p = DBL_pool;
		DBL_pool = p->rp;
	}
	p->rp = p->lp = p->twin = NULL;
	++DBL_cnt;

	return(p);
}

void DBList::freeDBL(DBL *p) {
	// move p to pool

	if (p->d == NONE) {
		Error_Exit("This element is already freed(Free_DBL).");
	}
	
	// *** �� �κ��� �ۼ��ϼ���
	p->d = NONE;
	p->rp = DBL_pool;
	DBL_pool = p;
	--DBL_cnt;		// reduce # of active DBL elements by one 
}

void DBList::freeDBL_pool(void) {
	DBL *p = DBL_pool;

	while (p != NULL) {
		DBL_pool = p->rp;	// get next pointer
		delete p;
		p = DBL_pool;
		UsedMemoryForDBLs -= sizeof(DBL);
	}
	if (DBL_cnt != 0) {
		Error_Exit("Non-zero DBL_cnt after cleanup.");
	}
}

void dblStack::push(DBL *p) {

	// *** �� �κ��� �ۼ��ϼ���
	if (ST != NULL) ST->lp = p;
	p->rp = ST;
	p->lp = NULL;
	ST = p;
}

DBL *dblStack::pop(void) {	// remove and return p in front of Q
	DBL *p;

	// *** �� �κ��� �ۼ��ϼ���
	p = ST;
	if (ST->rp == NULL) ST = NULL;
	else {
		ST = ST->rp;
		ST->lp = NULL;
	}
	return p;
}

void dblStack::remove(DBL *p) {	// delete p from ST

	// *** �� �κ��� �ۼ��ϼ���
	if (ST == p) {
		ST = p->rp;
		if (p->rp != NULL) (p->rp)->lp = NULL;
	}
	else {
		(p->lp)->rp = p->rp;
		if (p->rp != NULL) (p->rp)->lp = p->lp;
	}
}

DBL *dblStack::top(void) {
	return ST;
}

bool dblStack::empty(void) {
	if (ST == NULL)
		return true;
	else
		return false;
}
