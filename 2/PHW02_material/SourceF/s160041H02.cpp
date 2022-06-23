
#include "Graph_adj_array.h"

void Read_Graph_adj_array (int Vnum, vertex V[], int Enum, edge E[]) {
	// read graph information
	// V와 E의 name field는 각각 자신의 인덱스를 저장한다
	// V[].flag과 E[].flag은 반드시 false로 설정해야 한다.
	
	// ***이 함수를 작성한다
	for (int index = 0; index < Enum; index++) {
		scanf_s("%d %d %d", &E[index].vf, &E[index].vr, &E[index].cost);
		E[index].name = index;
		E[index].flag = false;
		E[index].fp = NONE; E[index].rp = NONE;
	}
	for (int index = 0; index < Vnum; index++) {
		V[index].name = index;
		V[index].flag = false;
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

int DFS_Tree_adj_array(
	int     src,   // source node index
	int     Vnum,  // number of vertices
	vertex *V,     // vertex structure array (starting index 0)
	int     Enum,  // number of edges
	edge   *E     // edge structure array (starting index 0) 
) {  
	// DFS를 사용하여 DFS tree에 속하는 에지의 flag을 true로 설정한다.
	// DFS 시작 vertex는 입력 파일에서 지정된다(src).
	// DFS tree에 속한 에지의 cost 함을 return 한다(모든 각 edge cost가 1이면 unique)
	// recursive 함수로 작성해도 무방하다.

	// ***이 함수를 작성한다***
	int k, j, cost_sum = 0;
	V[src].flag = true;
	//recursive 함수로 작성함.
	//f_hd와 fp끼리, r_hd와 rp끼리 각각 탐색하여야 하므로 두 개의 for문이 필요.
	for (k = V[src].f_hd; k != NONE; k = E[k].fp) {
		if (V[E[k].vr].flag == false) {
			cost_sum += E[k].cost;
			E[k].flag = true;
			cost_sum += DFS_Tree_adj_array(E[k].vr, Vnum, V, Enum, E);
		}
	}
	for (j = V[src].r_hd; j != NONE; j = E[j].rp) {
		if (V[E[j].vf].flag == false) {
			cost_sum += E[j].cost;
			E[j].flag = true;
			cost_sum += DFS_Tree_adj_array(E[j].vf, Vnum, V, Enum, E);
		}
	}
	return cost_sum;
}
