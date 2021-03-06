#include <stdio.h>
#include <math.h>
#include <float.h>

/* 추가 함수 */
void mergesort(double *X, unsigned *Xid, unsigned *TMP, int low, int high); // merge sort를 수행
void merge(double *X, unsigned *Xid, unsigned *TMP, int low, int mid, int high); //merge sort의 combine 과정 수행
double find_distance(double x1, double x2, double y1, double y2); //Euclidean distance를 구하는 함수
void Yid_bsort(unsigned L, unsigned R, double *Y, unsigned *Xid, unsigned *Yid);
	//Xid를 L에서 R만큼 Yid 배열에 복사하고 Yid 배열을 비내림차순으로 bubble sort를 이용하여 정렬
double combine(unsigned *Yid, unsigned L, unsigned R, double dlr, double xmid, unsigned *pt1, unsigned *pt2, double *X, double *Y, unsigned *TMP);
	// closestPairDc에서 combine 과정 수행

void sortXid(double *X, unsigned *Xid, unsigned *TMP, unsigned N) {
	mergesort(X, Xid, TMP, 0, N - 1);
}

double closestPairDC(
	unsigned L, unsigned R,	  // current leftmost and rightmost indices
	unsigned *pt1, unsigned *pt2, // closest pair points indices
	double *X, double *Y,         // (x,y) positions array(input)
	unsigned *Xid,  // point index array(sorted by x coordinates)
	unsigned *Yid,  // point index array(gradually sorted by y coordinates)
	unsigned *TMP,  // temporal index array
	unsigned THR	// threshold value
) {
	double min = DBL_MAX; //최소값을 구하기 위해 초기값은 double 자료형의 가장 높은 값으로 설정
	double temp_dist = 0; //최소값 구하기 위한 중간 과정에서의 거리 값
	
	unsigned M = 0; //divide를 위한 값
	double dl, dr, dlr, dmin; //dl, dr은 각 conquer한 결과, dlr은 각 conquer한 결과의 최소값, dmin은 combine한 결과
	double xmid; //combine하기 위한 범위 설정을 위해 필요한 중간값
	unsigned temp_pt1, temp_pt2, temp_pt3, temp_pt4;
	unsigned f_pt1, f_pt2; //이상 6개의 변수는 마지막 pt1과 pt2를 구하기 위한 중간값

	if (R - L + 1 <= THR) { //THR보다 점의 개수가 적은 경우
		if (THR == 1) {  //THR이 1로 설정된 경우, combine에서 거리를 구해야 함
			*pt1 = Xid[L];	*pt2 = Xid[R];
			Yid_bsort(L, R, Y, Xid, Yid);
			return min;
		}
		else {
			for (int i = (int)L; i <= (int)R - 1; i++) { //L과 R이 unsigned 자료형일 경우 오류가 날 수 있음
				for (int j = i + 1; j <= (int)R; j++) {
					temp_dist = find_distance(X[Xid[i]], X[Xid[j]], Y[Xid[i]], Y[Xid[j]]);
					if (min > temp_dist) {
						min = temp_dist;
						*pt1 = Xid[i]; *pt2 = Xid[j];
					}
				}
			}
			Yid_bsort(L, R, Y, Xid, Yid);
			return min;
		}
	}
	else { //THR보다 점의 개수가 많은 경우
		M = (L + R) / 2;
		//divide & conquer
		dl = closestPairDC(L, M, &temp_pt1, &temp_pt2, X, Y, Xid, Yid, TMP, THR);
		dr = closestPairDC(M + 1, R, &temp_pt3, &temp_pt4, X, Y, Xid, Yid, TMP, THR);
		merge(Y, Yid, TMP, L, M, R); //M으로 분리된 Yid배열 merge하여 재정렬 필요
		// dlr값 구하고 그에 맞는 index 구하기
		if (dl > dr) {
			dlr = dr;
			f_pt1 = temp_pt3;	f_pt2 = temp_pt4;
		}
		else {
			dlr = dl;
			f_pt1 = temp_pt1;	f_pt2 = temp_pt2;
		}

		xmid = (X[Xid[M]] + X[Xid[M + 1]]) / 2;
		dmin = combine(Yid, L, R, dlr, xmid, &f_pt1, &f_pt2, X, Y, TMP); //combine하여 최종적인 결과 구함.
		*pt1 = f_pt1;	*pt2 = f_pt2;
		return dmin;
	}
}

void mergesort(double *X, unsigned *Xid, unsigned *TMP, int low, int high) {
	int mid = 0;
	if (low < high) {
		mid = (low + high) / 2;
		mergesort(X, Xid, TMP, low, mid);
		mergesort(X, Xid, TMP, mid + 1, high);
		merge(X, Xid, TMP, low, mid, high);
	}
}

void merge(double *X, unsigned *Xid, unsigned *TMP, int low, int mid, int high) {
	int i = low, j = mid + 1, k = low;
	while (i <= mid && j <= high) {
		if (X[Xid[i]] < X[Xid[j]]) {
			TMP[k] = Xid[i]; i++;
		}
		else {
			TMP[k] = Xid[j]; j++;
		}
		k++;
	}

	if (i > mid) {
		while (j <= high) {
			TMP[k] = Xid[j];
			j++;
			k++;
		}
	}
	else {
		while (i <= mid) {
			TMP[k] = Xid[i];
			i++;
			k++;
		}
	}

	k = low;
	while (k <= high) {
		Xid[k] = TMP[k];
		k++;
	}
}

double find_distance(double x1, double x2, double y1, double y2) {
	double a = x1 - x2;
	double b = y1 - y2;
	double ret = sqrt(a * a + b * b);
	return ret;
}

void Yid_bsort(unsigned L, unsigned R, double *Y, unsigned *Xid, unsigned *Yid) {
	unsigned temp, k = 1;
	for (int i = (int)L; i <= (int)R; i++) Yid[i] = Xid[i]; //Yid에 배열 복사

	//bubble sort
	for (int i = (int)L; i <= (int)R - 1; i++) { //L, R, k가 unsigned 자료형인 경우 오류 날 수 있음
		for (int j = (int)L; j <= (int)R - (int)k; j++) {
			if (Y[Yid[j]] > Y[Yid[j + 1]]) {
				temp = Yid[j + 1];
				Yid[j + 1] = Yid[j];
				Yid[j] = temp;
			}
		}
		k++;
	}
}

double combine(unsigned *Yid, unsigned L, unsigned R, double dlr, double xmid, unsigned *pt1, unsigned *pt2, double *X, double *Y, unsigned *TMP) {
	int k = 0;
	double dtemp;
	//중간 영역에 있는 점들 TMP에 순서 유지하여 복사
	for (int i = (int)L; i <= (int)R; i++) {
		if ((X[Yid[i]] >= xmid - dlr) && (X[Yid[i]] <= xmid + dlr)) {
			TMP[k] = Yid[i];
			k++;
		}
	}
	
	if (k == 0) return dlr; //점 없으면 그대로 dlr 리턴
	else {
		//중간영역 내에 더 거리가 짧은 쌍이 있는지 찾기
		for (int i = 0; i <= k - 2; i++) {
			for (int j = i + 1; j <= i + 15; j++) { //15까지만 해도 충분
				if (j > k - 1) break;
				dtemp = find_distance(X[TMP[i]], X[TMP[j]], Y[TMP[i]], Y[TMP[j]]);
				if (dtemp < dlr) {
					dlr = dtemp;
					*pt1 = TMP[i];	*pt2 = TMP[j];
				}
			}
		}
		return dlr;
	}
}