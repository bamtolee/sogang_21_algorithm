#include "Graph_adj_list.h"
#include <queue>

extern void errorExit(const char *s);

SLList2 pool;
unsigned long UsedMemoryForArray = 0;

void Read_Graph_adj_list(int Vnum, vertex *V, int Enum, edge *E) {
	// ���Ͽ��� �׷��� ������ �о� V�� E �迭�� �����Ѵ�.
	// V[ ].name�� E[ ].name�� ������ �ʿ䰡 ������ ���ǻ� �� �迭������
	//   index�� �����Ѵ�.
	// V[ ].S�� adjacent list header�� ���� ����ε� ���ȴ�.
	// ���� edge (i,j)�� ������ V[i].S�� edge index k�� push�ϰ�
	//  ���������� V[j].S�� k�� push�Ѵ�.
	// V[].flag�� E[].flag�� �ݵ�� false�� �����ؾ� �Ѵ�.

	// ***�� �Լ��� �ۼ��Ѵ�
	pool.SLL_cnt = 0;
	pool.UsedMemoryForSLLs = 0;

	for (int index = 0; index < Enum; index++) { //���Ͽ��� �׷��� ������ �о� E �迭 ����.
		scanf_s("%d %d %d", &E[index].vf, &E[index].vr, &E[index].cost);
		E[index].name = index;
		E[index].flag = false;
		SLL *p = pool.allocSLL(); p->i = index;
		SLL *q = pool.allocSLL(); q->i = index;  //storage pool���� �̻�� SLL ��������.
		V[E[index].vf].S.push(p);
		V[E[index].vr].S.push(q);		//���� index�� Edge�� incident�ϴ� �� vertex�� adjacent list�� �ش� edge�� index�� push.
	}
	for(int index = 0; index < Vnum; index++){
		V[index].name = index;
		V[index].flag = false;
	}
}

void Free_Graph_adj_list(int Vnum, vertex *V) {
	// V �迭�� adjacency list�� ���Ҹ� pop()�� ����Ͽ�
	// ���  pool�� �ǵ��� �ش�
	int k;

	// ***�� �Լ��� �ۼ��Ѵ�
	for (k = 0; k < Vnum; k++) {	//V �迭�� �� vertex���� adjacent list�� ���� ����� ������ SLL�� ��� pool�� �ǵ��� �ش�.
		while (!(V[k].S.empty())) {
			SLL *p = V[k].S.pop();
			pool.freeSLL(p);
		}
	}
}

int BFS_Tree_adj_list(
	int     src,   // source node index
	int     Vnum,  // number of vertices
	vertex *V,     // vertex structure array (starting index 0)
	int     Enum,  // number of edges
	edge   *E     // edge structure array (starting index 0)
) {
	// BFS�� ����Ͽ� BFS tree�� ���ϴ� ������ flag�� true�� �����Ѵ�.
	// BFS ���� vertex�� �Է� ���Ͽ��� �����ȴ�(src).
	// V[]�� adjacent list Ž���� .top() ��� �Լ��� ����Ͽ�
	//  adj list ����� ��� �̸� ���󰡸� ���� vertex�� Ž���Ѵ�.
	// BFS tree�� ���� ������ cost ���� return �Ѵ�
	//   (not unique but ��� �� edge cost�� 1�̸� unique)
	// -- �Լ��� ��� parameter�� ���� �ʿ�� ����.
	// -- BFS�� ���� queue�� �ʿ��ϸ� STL�� queue�� ����ص� ����

	// ***�� �Լ��� �ۼ��Ѵ�
	std::queue<int> q;	//BFS�� ���� queue.
	std::queue<int> cost_q;  //return ���� cost ���� ���ϱ� ���� queue.
	int cost_sum = 0;
	V[src].flag = true;
	do {  //BFS
		for (SLL *p = V[src].S.top(); p != NULL; p = p->p) {
			if (src == E[p->i].vf) {
		//E �迭�� ������ ��, ���� �湮 ���� vertex�� vf�� ���Ͽ� ����� ���� ���� �ְ�, vr�� ���Ͽ� ��������� ���� �ִ�. ���� if�� ���.
				if (!V[E[p->i].vr].flag) {
					V[E[p->i].vr].flag = true;
					q.push(E[p->i].vr);
					cost_q.push(E[p->i].cost);
					E[p->i].flag = true;
				}
			}
			else if (src == E[p->i].vr) {
				if (!V[E[p->i].vf].flag) {
					V[E[p->i].vf].flag = true;
					q.push(E[p->i].vf);
					cost_q.push(E[p->i].cost);
					E[p->i].flag = true;
				}
			}
		}
		if (q.empty()) break;
		src = q.front();
		cost_sum += cost_q.front();
		q.pop();
		cost_q.pop();
	} while (1);
	return cost_sum;
}

// SLL management functions
// �Ʒ� �Լ����� 02_04_CSE3081_CPrg&DataStructure_�߰��ڷ�.pdf��
//   �����Ͽ� �� �Լ����� ä�� ����.
// ���ø� �ʿ��ϹǷ� ť�� ��� �Լ��� �Ʒ� �Լ� ����Ʈ�� ����

// stack member fuctions
void sllStack2::push(SLL *p) {
	// insert p to the top of stack ST
	p->p = ST;
	ST = p;
}

SLL *sllStack2::pop(void) {
	// remove and return p at the top of ST

	// ***�� �Լ��� �ۼ��Ѵ�
	SLL *p = ST;
	if (ST->p == NULL)
		ST = NULL;
	else
		ST = ST->p;
	return p;
}

SLL *sllStack2::top(void) {
	// return p at the top of ST
	return ST;
}

bool sllStack2::empty(void) {
	// return true if empty
	if (ST == NULL)
		return true;
	else
		return false;
}

// SLList2 member functions
SLL *SLList2::allocSLL(void) {		// allocate an SLL element
	SLL *p;

	// ***�� �Լ��� �ۼ��Ѵ�
	if (SLL_pool == NULL) {
		p = (SLL *)malloc(sizeof(SLL));
		if (p == NULL)
			errorExit("Memory allocation error(allocSLL)");
		UsedMemoryForSLLs += sizeof(SLL);
		p->i = NONE;
	}
	else {
		p = SLL_pool;
		SLL_pool = p->p;
	}
	p->p = NULL;
	++SLL_cnt;
	return(p);
}

void SLList2::freeSLL(SLL *p) {		// free *p (move it to SLL_pool
	if (p->i == NONE) {
		errorExit("This is already freed(freeSLL).");
	}

	// ***�� �Լ��� �ۼ��Ѵ�
	p->i = NONE;
	p->p = SLL_pool;
	SLL_pool = p;
	--SLL_cnt;
}

void SLList2::freeSLL_pool(void) {	// clear SLL_pool
	SLL *p = SLL_pool;

	// ***�� �Լ��� �ۼ��Ѵ�
	while (p != NULL) {
		SLL_pool = p->p;
		free(p);
		UsedMemoryForSLLs -= sizeof(SLL);
		p = SLL_pool;
	}
	if (SLL_cnt != 0) {
		errorExit("Non-zero SLL_cnt after cleanup.");
	}
}