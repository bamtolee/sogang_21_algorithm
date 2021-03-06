#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <queue>
#include <limits.h>

#define NONE -1

typedef struct elm_edge {
	int  vf, vr;  // e = (vf, vr) where vf, vr are vertex array indices
	int  cost;    // edge cost
	bool flag;    // true if the edge is in the SP spanning tree, false otherwise 
	int  fp, rp;  // adj list ptr of forward and reverse (-1 if none)
} edge;

typedef struct elm_vertex {
	int  f_hd, r_hd;	// adj list header(-1 if none)
	int  distance;		// distance from src to this vertex
	int  heap_idx;		// minHeap index
	bool inS;			// true if SP length found(true when move from V-S to S) 
} vertex;

void heap_insert(int *heap_num, int index, vertex *V, int *minHeap) { //heap에 삽입
	++(*heap_num);
	int i = *heap_num;

	while ((i != 1) && (V[index].distance < V[minHeap[i / 2]].distance)) {
		minHeap[i] = minHeap[i / 2];
		V[minHeap[i]].heap_idx = i; //cross reference
		i /= 2;
	}
	minHeap[i] = index;
	V[index].heap_idx = i;
}

void heap_delete(int *heap_num, vertex *V, int *minHeap) { //heap의 루트 삭제
	int parent = 1, child = 2;
	int temp = minHeap[*heap_num];
	--(*heap_num);

	while (child <= *heap_num) {
		if ((child < *heap_num) && (V[minHeap[child]].distance > V[minHeap[child + 1]].distance))
			child++;
		if (V[temp].distance <= V[minHeap[child]].distance) break;
		minHeap[parent] = minHeap[child];
		V[minHeap[parent]].heap_idx = parent;
		parent = child;
		child *= 2;
	}
	minHeap[parent] = temp;
	V[minHeap[parent]].heap_idx = parent;
}

void heap_adjust(int *heap_num, int index, vertex *V, int *minHeap) {  //distance 값이 바뀐 것을 반영하여 heap 노드 위치 변경
	int temp = V[index].heap_idx;
	if (temp == 1) return;

	int subs;

	while (1) {
		if (V[minHeap[temp]].distance < V[minHeap[temp / 2]].distance) {
			subs = minHeap[temp / 2];

			minHeap[temp / 2] = minHeap[temp];
			V[minHeap[temp / 2]].heap_idx = temp / 2;
			minHeap[temp] = subs;
			V[subs].heap_idx = temp;
			temp /= 2;
		}
		else break;

		if (temp <= 1) break;
	}
}

int SPT_Dijkstra(
	int src,	// source vertex index
	// graph structure array
	// 1. the adjacency list structure is the same as PHW02
	// 2. additional fields are added for Dijkstra's algorithm(see .h file)
	int Vnum, vertex *V,	// Vertex array size and the array
	int Enum, edge *E,		// Edge array size and the array

	int *minHeap	// array for min heap (array size = Vnum+1)
		// heap index range is 1 ~ (Vnum - 1) note: src must not in the initial heap
		// just arry is passed. must construct min-heap in this function

	// OUTPUT
	// 1. V[].distance : shortest path length from src to this vertex
	// 2. shortest path spanning tree : set E[].flag = true if the edge is in the tree
	// 3. return the sum of edge costs in the shortest path spanning tree.
	//    should be small as possible (think a way to make it small)
) {
	int treeCost = 0;
	// *** 이 함수를 작성하자 ***
	// 반드시 min-heap을 사용하여 O((n+m)logn) 알고리즘을 구현해야 한다(아니면 trivial한 프로그램임)
	// heap 연산 등 필요한 함수는 자유롭게 작성하여 추가한다.
	// 그러나 global 변수, dynamic array 등은 추가로 사용하지 않는다(실제로 필요 없다)

	int temp_src = src;
	int heap_num = 0; //heap의 노드 수
	int f = 0;

	//초기화
	minHeap = new int[Vnum + 1];
	V[src].inS = true; V[src].distance = 0; V[src].heap_idx = -1;
	for (int k = V[src].f_hd; k != NONE; k = E[k].fp) V[E[k].vr].distance = E[k].cost;
	for (int j = V[src].r_hd; j != NONE; j = E[j].rp) V[E[j].vf].distance = E[j].cost;

	//heap 구축
	for (int i = 0; i < Vnum; i++) {
		if (i == src) continue;
		heap_insert(&heap_num, i, V, minHeap);
	}
	
	for (int i = 0; i < Vnum - 1; i++) {
		//V-S에서 가장 src와의 거리가 가까운 vertex 선택해서 S에 넣기
		V[minHeap[1]].inS = true;
		temp_src = minHeap[1];
		heap_delete(&heap_num, V, minHeap); //heap에서 제거해야 함
		V[temp_src].heap_idx = -1;

		//위의 과정에서 생긴 distance의 변화를 update
		for (int k = V[temp_src].f_hd; k != NONE; k = E[k].fp) {
			if ((V[E[k].vr].inS == false) && (V[E[k].vr].distance > V[temp_src].distance + E[k].cost)) {
				V[E[k].vr].distance = V[temp_src].distance + E[k].cost;
				heap_adjust(&heap_num, E[k].vr, V, minHeap); //heap을 업데이트
			}
		}
		for (int j = V[temp_src].r_hd; j != NONE; j = E[j].rp) {
			if ((V[E[j].vf].inS == false) && (V[E[j].vf].distance > V[temp_src].distance + E[j].cost)) {
				V[E[j].vf].distance = V[temp_src].distance + E[j].cost;
				heap_adjust(&heap_num, E[j].vf, V, minHeap);
			}
		}
	}

	//acutal path 찾고 shortest path spanning tree 구현
	for (int i = 0; i < Vnum; i++) {
		if (i == src) continue;
		f = 0;

		//f값을 없이 하면 cycle이 생겨 spanning tree가 아니게 될 수 있음
		for (int k = V[i].f_hd; k != NONE; k = E[k].fp) {
			if ((E[k].flag == false) && (V[E[k].vr].distance + E[k].cost == V[i].distance)) {
				E[k].flag = true;
				treeCost += E[k].cost;
				f = 1;
				break;
			}
		}

		if (f == 1) continue;

		for (int k = V[i].r_hd; k != NONE; k = E[k].rp) {
			if ((E[k].flag == false) && (V[E[k].vf].distance + E[k].cost == V[i].distance)) { 
				E[k].flag = true;
				treeCost += E[k].cost;
				break;
			}
		}
	}
	return treeCost;
}

void Read_Graph(int Vnum, vertex *V, int Enum, edge *E) {
	// Graph 자료구조를 만드는 함수
	// *** 이 함수를 추가하자 ***
	// PHW02의 Read_Graph_adj_array()를 이 과제의 자료구조에 맞춰 살짝 수정하여 사용한다
	// 즉, Read_Graph_adj_array()에서 불필요한 내용을 제거한 후 사용.
	for (int index = 0; index < Enum; index++) {
		scanf_s("%d %d %d", &E[index].vf, &E[index].vr, &E[index].cost);
		E[index].flag = false;
		E[index].fp = NONE; E[index].rp = NONE;
	}
	for (int index = 0; index < Vnum; index++) {
		V[index].inS = false;
		V[index].distance = INT_MAX; //최대값으로 초기화
		V[index].f_hd = NONE; V[index].r_hd = NONE;
	}
	for (int index = 0; index < Enum; index++) {
		//파일로부터 읽어들인 E 배열의 정보를 토대로, E 배열의 fp와 V 배열의 f_hd, 그리고 E 배열의 rp와 V 배열의 r_hd 각각 구성.
		if (V[E[index].vf].f_hd == NONE) V[E[index].vf].f_hd = index;
		else {
			E[index].fp = V[E[index].vf].f_hd;
			V[E[index].vf].f_hd = index;
		}
		if (V[E[index].vr].r_hd == NONE) V[E[index].vr].r_hd = index;
		else {
			E[index].rp = V[E[index].vr].r_hd;
			V[E[index].vr].r_hd = index;
		}
	}
}

// the following functions are for testing if the submitted program is correct.
int  Tree_Check(int Vnum, vertex *V, int Enum, edge *E, int *visited);
bool SPT_test(int src, int Vnum, vertex *V, int Enum, edge *E, int *minHeap);
void Error_Exit(const char *s);

int main ( void ) {
	int		src;
	vertex *V;		int Vnum;
	edge   *E;		int Enum;
	int    *minHeap;
	int    Tree_cost;
	int    Tnum; 		// # of test cases
	clock_t start, finish;
	double cmpt;

	scanf_s("%d", &Tnum);		// read # of tests

	for (int t = 0; t < Tnum; t++ ) {
		scanf_s("%d %d %d", &Vnum, &Enum, &src);
		V = new vertex [Vnum];
		E = new edge [Enum];
		minHeap = new int[Vnum + 1];	// heap array allocation
		if ( V == NULL || E == NULL || minHeap == NULL ) {
			Error_Exit("Memory Allocation Error");
		}
		Read_Graph(Vnum, V, Enum, E);

		/**/start = clock();	// start timer

		Tree_cost = SPT_Dijkstra(src, Vnum, V, Enum, E, minHeap);	// code by students

		/**/finish = clock();	// stop timer
		cmpt = ((double)(finish - start)) / (double)CLK_TCK;

		// 아래 Tree_Check와 SPT_test 함수는 첨부한 SPT_test.obj 파일에 있다.
		// 이 테스트에서 오류로 인하여 프로그램이 정지하면 뭔가 잘못된 것이다(제출해도 0점)
        if (Tree_Check(Vnum, V, Enum, E, minHeap) == 0) {
			Error_Exit("   ERROR The result is not a spanning tree");
		}
		if (SPT_test(src, Vnum, V, Enum, E, minHeap) == false) {
			Error_Exit("** Something wrong in applying Dijkstra's");
		}
		if ( t != 0 ) 
			printf("\n");

		printf("**T%d (Dijkstra) (V = %d, E = %d, time = %.3f sec) Tree Cost = %d\n", 
			t+1, Vnum, Enum, cmpt, Tree_cost);

		delete [] minHeap; delete [] V; delete [] E;
	}
	return 0;
}

void Error_Exit (const char *s ) {
  printf("%s\n", s);
  exit(-1);
}
