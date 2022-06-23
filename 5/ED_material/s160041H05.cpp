#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Edit_Distance(
	// inputs
	char *SS,		// Source string array
	char *TS,		// Edited string array
	int ins_cost, int del_cost, int sub_cost,
	// insertion, deletion, substitution cost
    // outputs
    int **Table,    // Cost Table Calculated (2-d array (|SS|+1) x (|TS|+1) will be set)
    char **SR,		// Source string with insertion to T marks '*'
    char **OP,		// Operation sequence('i', 'd', 's', or '.' (nop))
    char **TR,      // Edited string with deletion form S mark '*'
				    // NOTE: ST, OP, TR must be allocated inside the function as needed(minimum as possible)
	int  *Mem_Allocated
) {
	int ins, del, nop, sub;								//for cost table calculation
	int s_index = strlen(SS), t_index = strlen(TS);		//for top-down
	int op_num = 0;										//for SR, OP, TR memory allocation

	//bottom-up
	for (int i = 0; i <= (int)strlen(SS); i++) Table[i][0] = i * del_cost; //table initialization
	for (int j = 0; j <= (int)strlen(TS); j++) Table[0][j] = j * ins_cost;
	//cost table calculation
	for (int i = 1; i <= (int)strlen(SS); i++) {
		for (int j = 1; j <= (int)strlen(TS); j++) {
			ins = Table[i][j - 1] + ins_cost;
			del = Table[i - 1][j] + del_cost;
			if (SS[i-1] == TS[j-1]) {
				nop = Table[i - 1][j - 1] + 0;
				sub = -1;
			}
			else {
				sub = Table[i - 1][j - 1] + sub_cost;
				nop = -1;
			}

			if (nop == -1) {
				if (ins <= del) Table[i][j] = ins > sub ? sub : ins;
				else Table[i][j] = del > sub ? sub : del;
			}
			else if (sub == -1) {
				if (ins <= del) Table[i][j] = ins > nop ? nop : ins;
				else Table[i][j] = del > nop ? nop : del;
			}
		}
	}
	// for memory allocation, calculate op_num
	while (1) {
		if (s_index > 0 && t_index > 0) {
			if (Table[s_index][t_index] == Table[s_index][t_index - 1] + ins_cost) {
				if (Table[s_index][t_index] == Table[s_index - 1][t_index] + del_cost) {
					if (ins_cost <= del_cost) t_index--;
					else s_index--;
				}
				else t_index--;
			}
			else if (Table[s_index][t_index] == Table[s_index - 1][t_index] + del_cost) s_index--;
			else {
				s_index--; t_index--;
			}
		}
		else if (s_index > 0 && t_index == 0) s_index--;
		else if (s_index == 0 && t_index > 0) t_index--;
		op_num++;

		if (s_index == 0 && t_index == 0) break;
	}
	//memory allocation
	*SR = new char[op_num+1]; *OP = new char[op_num+1]; *TR = new char[op_num+1];
	*Mem_Allocated = (op_num+1) * 3;
	(*SR)[op_num] = '\0'; (*OP)[op_num] = '\0'; (*TR)[op_num] = '\0'; //for string
	s_index = strlen(SS); t_index = strlen(TS); op_num--;
	//top-down
	while (1) {
		if (s_index > 0 && t_index > 0) {
			if (Table[s_index][t_index] == Table[s_index][t_index - 1] + ins_cost) {
				if (Table[s_index][t_index] == Table[s_index - 1][t_index] + del_cost) {
					if (ins_cost <= del_cost) {
						t_index--;
						(*OP)[op_num] = 'i';
						(*SR)[op_num] = '*';
						(*TR)[op_num] = TS[t_index];
					}
					else {
						s_index--;
						(*OP)[op_num] = 'd';
						(*SR)[op_num] = SS[s_index];
						(*TR)[op_num] = '*';
					}
				}
				else {
					t_index--;
					(*OP)[op_num] = 'i';
					(*SR)[op_num] = '*';
					(*TR)[op_num] = TS[t_index];
				}
			}
			else if (Table[s_index][t_index] == Table[s_index - 1][t_index] + del_cost) {
				s_index--;
				(*OP)[op_num] = 'd';
				(*SR)[op_num] = SS[s_index];
				(*TR)[op_num] = '*';
			}
			else {
				if (Table[s_index][t_index] == Table[s_index - 1][t_index - 1] + sub_cost) {
					s_index--; t_index--;
					(*OP)[op_num] = 's';
					(*SR)[op_num] = SS[s_index];
					(*TR)[op_num] = TS[t_index];
				}
				else {
					s_index--; t_index--;
					(*OP)[op_num] = '.';
					(*SR)[op_num] = SS[s_index];
					(*TR)[op_num] = TS[t_index];
				}
			}
		}
		else if (s_index > 0 && t_index == 0) {
			s_index--;
			(*OP)[op_num] = 'd';
			(*SR)[op_num] = SS[s_index];
			(*TR)[op_num] = '*';
		}
		else if (s_index == 0 && t_index > 0) {
			t_index--;
			(*OP)[op_num] = 'i';
			(*SR)[op_num] = '*';
			(*TR)[op_num] = TS[t_index];
		}
		op_num--;
		if (op_num == -1) break; //find a solution
	}
}