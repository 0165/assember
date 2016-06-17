// ConsoleApplication1.cpp : ﹚
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

struct op
{
	string s;
	int num;
};

bool isOp(string target, int *temp);
bool isLab(string target,int *temp);
int howLong(string target);
int toNum(string target);
string cut(string target);
void mySort();
void show();

op MyOpCode[59];
op * lab;
op * lab1;
unsigned int mylong = 0,whylong=10;

int _tmain(int argc, _TCHAR* argv[])
{
	ifstream sourse,opCode,readTempCode;
	ofstream tempCode, obCode;
	char tempS[100],name[20];
	char *p;
	unsigned int location=0;
	//prepare the op code
	int i = 0,temp,tempL,start,end;
	opCode.open("opcode.txt");
	while (opCode.getline(tempS, 100)){
		p = strtok(tempS, "\t :");
		MyOpCode[i].s = p;
		p = strtok(NULL, "\t :");
		sscanf(p, "%x", &MyOpCode[i].num);
		i++;
	}
	opCode.close();

	//first run of sourse code
	sourse.open("figure.txt");
	lab = new op[whylong];		//dynamic array
	i = 0;
	while (sourse.getline(tempS, 100)){
		if(i==whylong){
			whylong+=10;
			lab1=new op[whylong];
			for(int j=0;j<mylong;j++){
				lab1[j].s=lab[j].s;
				lab1[j].num=lab[j].num;
			}
			delete[] lab;
			lab=lab1;
		}
		p = strtok(tempS, "\t ");
		if (p != NULL){
			if (isOp(p,&tempL))
				location = location + 3;
			else if (p[0] != '.' &&  strcmp(p,"END")){
				lab[i].s = p;
				lab[i++].num = location;
				mylong++;
				p = strtok(NULL, "\t ");
				if (isOp(p, &tempL) || !strcmp(p, "WORD"))
					location = location + 3;
				else if (!strcmp(p, "RESW")){
					p = strtok(NULL, "\t ");
					location = location + atoi(p) * 3;
				}
				else if (!strcmp(p, "RESB")){
					p = strtok(NULL, "\t ");
					location = location + atoi(p);
				}
				else if (!strcmp(p, "BYTE")){
					p = strtok(NULL, "\t ");
					if (p[0] == 'C')
						location = location + howLong(p)-3;
					else
						location = location + (howLong(p) - 3)/2;
				}
				else if (!strcmp(p, "START")){p = strtok(NULL, "\t ");
					sscanf(p, "%x", &location);
					start = location;
				}
				else
					location = location + 3;
			}
		}
		else{}
	}
	sourse.clear();				//reset the end-of-file, otherwise can't goback to begin
	sourse.seekg(0, ios::beg);
	mySort();
	
	//second run of sourse
	end = location - start;
	tempCode.open("tempCode.txt");
	tempCode << setw(5) << "loc" << setw(10) << "lab" << setw(10) << "op" << setw(10) << "sym" << setw(11) << "obj\n";
	location = start;bool a=true;
	while (sourse.getline(tempS, 100)){
		p = strtok(tempS, "\t ");
		if (p != NULL){
			if(location==start && a){
				sscanf(p, "%s", name);
				a=false;
			}
			if (p[0] != '.'&& strcmp(p,"END"))
				tempCode << ' ' << hex << uppercase << location;
			if (isOp(p, &tempL)){
				location = location + 3;
				tempCode << setw(20) << p;
				if (!strcmp(p, "RSUB")){
					tempCode << setw(20) << "    4C0000";
				}
				else{
					p = strtok(NULL, "\t ");
					if (p[howLong(p)-2] == ',' && p[howLong(p)-1] == 'X'){
						p[howLong(p)-2] = NULL;
						isLab(p, &temp);
						tempL = tempL + (1 << 15);
						tempCode << setw(8) << p << ",X    " << setw(6) << setfill('0') << hex << uppercase << temp + tempL << setfill(' ');
					}
					else{
						isLab(p, &temp);
						tempCode << setw(10) << p << "    " << setw(6) << setfill('0') << hex << uppercase << temp + tempL << setfill(' ');
					}
				}
				tempCode << endl;
			}
			else if (p[0] != '.'&& strcmp(p, "END")){
				tempCode << setw(10) << p;
				p = strtok(NULL, "\t ");
				tempCode << setw(10) << p;
				if (isOp(p, &tempL) ){
					location = location + 3;
					p = strtok(NULL, "\t ");
					isLab(p, &temp);
					tempCode << setw(10) << p << "    " << setw(6) << setfill('0') << hex << uppercase << temp + tempL << setfill(' ') << endl;
				}
				else if (!strcmp(p, "WORD")){
					location = location + 3;
					p = strtok(NULL, "\t ");
					tempCode << setw(10) << p;
					sscanf(p, "%d", &temp);
					sprintf(p, "%x", temp);
					tempCode<< "    " <<setw(6)<<setfill('0')<< temp << endl<<setfill(' ');
				}
				else if (!strcmp(p, "RESW")){
					p = strtok(NULL, "\t ");
					location = location + atoi(p) * 3;
					tempCode << setw(10) << p<< endl;
				}
				else if (!strcmp(p, "RESB")){
					p = strtok(NULL, "\t ");
					location = location + atoi(p);
					tempCode << setw(10) << p<< endl;
				}
				else if (!strcmp(p, "BYTE")){
					p = strtok(NULL, "\t ");
					if (p[0] == 'C'){
						location = location + howLong(p) - 3;
						tempCode << setw(10) << p << setw(10) << hex <<uppercase<< toNum(p) << endl;
					}
					else{
						location = location + (howLong(p) - 3) / 2;
						tempCode << setw(10) << p ;
						tempCode << setw(10) << cut(p) << endl;
					}
				}
				else if (!strcmp(p, "START")){
					p = strtok(NULL, "\t ");
					tempCode << setw(10) << p << endl;
				}
			}
			else if (!strcmp(p, "END")){
				p = strtok(NULL, "\t ");
				tempCode << setw(25) << "END" << setw(10) << p<<endl;
			}
		}
	}

	delete[] lab;				//release menmery
	sourse.close(); tempCode.close();
	
	//to make obCode 
	string ttt, tt, t;
	readTempCode.open("tempCode.txt");
	obCode.open("obCode.txt");
	obCode << "H" << setw(6) << name << "," << setw(6) << setfill('0') << hex << uppercase << start << "," << setw(6) << hex << uppercase << end << setfill(' ') << endl;
	readTempCode.getline(tempS,100);
	while(readTempCode.getline(tempS,100)){
		t = tempS;
		tt = t.substr(1,4);
		if (t.length() > 43)
			ttt = t.substr(39, 45);
	}
	obCode.close();
	return 0;
}

bool isOp(string target, int *temp){
	int low = 0, high = 58,mid;
    while (low <= high){
        mid = (low + high) / 2;
		if (target == MyOpCode[mid].s){
            *temp=MyOpCode[mid].num << 16;
            return true;
        }
        else if (MyOpCode[mid].s > target)
            high = mid - 1;
        else if (MyOpCode[mid].s < target)
            low = mid + 1;
    }
	return false;
}

bool isLab(string target, int *temp){
	int low = 0, high = mylong-1, mid;
	while (low <= high){
		mid = (low + high) / 2;
		if (target == lab[mid].s){
			*temp = lab[mid].num;
			return true;
		}
		else if (lab[mid].s > target)
			high = mid - 1;
		else if (lab[mid].s < target)
			low = mid + 1;
	}
	return false;
}

int howLong(string target){
	return target.size();
}

int toNum(string target){
	int temp=0;
	for (int i = 2; target[i]!='\''; i++){
		temp = temp << 8;
		temp=temp+ target[i];
	}
	return temp;
}

string cut(string target){
	string temp;
	for (int i = 2; target[i] != '\''; i++)
		temp = temp + target[i];
	return temp;
}

void mySort(){
	op temp;
	for (int i = 0; i < mylong-1; i++)
		for (int j = i+1 ; j < mylong; j++)
			if (lab[i].s > lab[j].s){
				temp.s = lab[i].s;
				temp.num = lab[i].num;
				lab[i].s = lab[j].s;
				lab[i].num= lab[j].num;
				lab[j].s = temp.s;
				lab[j].num = temp.num;
			}
}
