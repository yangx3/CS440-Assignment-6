/*
CS440 Assignment-3
Query Processing
by Xiaoyi Yang
*/
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

struct Dept {
	string did;
	string dname;
	string budget;
	int managerid;
};

struct Emp {
	int eid;
	string ename;
	string age;
	string salary;
};

struct SortTemp {
	int itemid;
	int sortid;
};

struct mergeRS {
	int managerid;
	string dname, ename;
	string budget, age;
	string did, salary;
};

//convert number to string
template <typename T>
string NumtoStr(T Number){
	ostringstream ss;
	ss << Number;
	return ss.str();
}

//Delete "" within string
string CharDel(string a){
	string temp = "";
	for(int i = 0; i < a.length(); i++){
		if(a[i] != '"')
			temp += a[i];
	}
	return temp;
}

void Merge(SortTemp *a, int low, int high, int mid);
void MergeSort(SortTemp *a, int low, int high);
Dept *Read_Dept(string filename);
Emp *Read_Emp(string filename);
int size_dept, size_emp;
void Write_Dept(Dept *dept);
void Write_Emp(Emp *emp);
Dept *Sort_Dept(Dept *dept);
Emp *Sort_Emp(Emp *emp);
Dept *Block_Dept_Read(string filename);
Emp *Block_Emp_Read(string filename);

//Sort relations, and output temp files
void pre_filing(){
	Dept *dept = Read_Dept("department.csv");
	Emp *emp = Read_Emp("employee.csv");
	dept = Sort_Dept(dept);
	emp = Sort_Emp(emp);
	Write_Dept(dept);
	Write_Emp(emp);
}

//write data to join.csv
void Write_Join(mergeRS *join){
	ofstream newfile;
	newfile.open("join.csv");
	int t = 0;
	while(true){
		if(join[t].ename == "")
			break;
		t++;
	}
	for(int i = 0; i < t; i++){
		string temp = NumtoStr(join[i].managerid) + "," + join[i].ename + "," + join[i].age + "," + join[i].dname + "," + join[i].did + "," + join[i].budget + "," + join[i].salary + "\n";
		cout << NumtoStr(i+1)+ ". " + temp << endl;
		newfile << temp;
	}
	newfile.close();
}

int main(){
	pre_filing();
	Dept *R = Read_Dept("Dept.csv");
	Emp *S = Read_Emp("Emp.csv");
	mergeRS *join = new mergeRS[200];
	int a=0, b=0;
	int t = 0;
	bool complete = false;
	while(!complete){
		int i=a, j=b;
		for(int k = 0; k < 22; k++){
			if(i>= size_dept || j>=size_emp)
				break;
			if(R[i].managerid == S[j].eid){
				join[t].managerid = R[i].managerid;
				join[t].dname = R[i].dname;
				join[t].ename = S[j].ename;
				join[t].budget = R[i].budget;
				join[t].age = S[j].age;
				join[t].salary = S[j].salary;
				join[t].did = R[i].did;
				t++;
				i++;
				//cout << "" + NumtoStr(t) +". "+ NumtoStr(join[t-1].managerid) << endl;
			}
			else if(R[i].managerid > S[j].eid){
				j++;
			}
			else if(R[i].managerid < S[j].eid){
				i++;
			}
			
		}
		a += i;
		b += j;
		if(a > size_dept || b > size_emp){
			complete = true;
		}
	}
	Write_Join(join);
	
	return 0;
}

//Read dept from un-sorted file
Dept *Read_Dept(string filename){
	ifstream file(filename.c_str());
	string id, name, bud, manid;
	Dept *temp = new Dept[200];
	int i = 0;
		while(getline(file, id, ',')){
			temp[i].did = CharDel(id);
			
			getline(file, name, ',');
			temp[i].dname = CharDel(name);
			
			getline(file, bud, ',');
			temp[i].budget = CharDel(bud);
			
			getline(file, manid);
			temp[i].managerid = atoi(CharDel(manid).c_str());
			i++;
		}
	Dept *data = new Dept[i-1];
	//cout << temp[0].did + temp[0].dname << endl;
	size_dept = i-1;
	//cout << size_dept << endl;
	for(int j = 0; j < size_dept; j++){
		data[j] = temp[j];
	}
	//cout <<"data: "+ data[0].did + data[0].dname << endl;
	delete [] temp;
	return data;
}

//Read emp from un-sorted file
Emp *Read_Emp(string filename){
	ifstream file(filename.c_str());
	string id, name, age, sala;
	Emp *temp = new Emp[200];
	int i = 0;
		while(getline(file, id, ',')){
			temp[i].eid = atoi(CharDel(id).c_str());
			
			getline(file, name, ',');
			temp[i].ename = CharDel(name);
			
			getline(file, age, ',');
			temp[i].age = CharDel(age);
			
			getline(file, sala);
			temp[i].salary = CharDel(sala);
			i++;
		}
	Emp *data = new Emp[i-1];
	size_emp = i-1;
	//cout << size_emp << endl;
	//cout << temp[0].ename + temp[0].age << endl;
	for(int j = 0; j < size_emp; j++){
		data[j] = temp[j];
	}
	//cout << "data: "+data[0].ename + data[0].age << endl;
	delete [] temp;
	return data;
}

void Write_Dept(Dept *dept){
	ofstream newfile;
	newfile.open("Dept.csv");
	for(int i = 0; i < size_dept; i++){
		string temp = dept[i].did + "," + dept[i].dname + "," + dept[i].budget + "," + NumtoStr(dept[i].managerid) + "\n";
		//cout << NumtoStr(i+1)+ ". " + temp << endl;
		newfile << temp;
	}
	newfile.close();
}

void Write_Emp(Emp *emp){
	ofstream newfile;
	newfile.open("Emp.csv");
	for(int i=0; i < size_emp; i++){
		string temp = NumtoStr(emp[i].eid) + "," + emp[i].ename + "," + emp[i].age + "," + emp[i].salary + "\n";
		//cout << NumtoStr(i+1)+ ". " + temp << endl;
		newfile << temp;
	}
	newfile.close();
}

Dept* Sort_Dept(Dept *dept){
		SortTemp Temp[size_dept];
		Dept temp[size_dept];
		Dept *temp_dept = new Dept[size_dept];
		for(int i=0;i < size_dept; i++){
			Temp[i].itemid = dept[i].managerid;
			Temp[i].sortid = i;
			temp[i] = dept[i];
		}
		MergeSort(Temp, 0, size_dept-1);

		for(int i = 0; i <size_dept; i++){
			temp_dept[i] = temp[Temp[i].sortid];
		}
	return temp_dept;
}

Emp* Sort_Emp(Emp *emp){
	SortTemp Temp[size_emp];
	Emp temp[size_emp];
	Emp *temp_emp = new Emp[size_emp];
	for(int i=0;i < size_emp; i++){
		Temp[i].itemid = emp[i].eid;
		Temp[i].sortid = i;
		temp[i] = emp[i];
		}
	MergeSort(Temp, 0, size_emp-1);		
	for(int i = 0; i < size_emp; i++){
		temp_emp[i] = temp[Temp[i].sortid];
	}
	return temp_emp;
}



void Merge(SortTemp *a, int low, int high, int mid){
	
	// We have low to mid and mid+1 to high already sorted.
	int i, j, k;
	SortTemp temp[high-low+1];
	i = low;
	k = 0;
	j = mid + 1;
	
 
	// Merge the two parts into temp[].
	while (i <= mid && j <= high)
	{
		if (a[i].itemid < a[j].itemid)
		{
			temp[k] = a[i];
			k++;
			i++;
		}
		else
		{
			temp[k] = a[j];
			k++;
			j++;
		}
	}
 
	// Insert all the remaining values from i to mid into temp[].
	while (i <= mid)
	{
		temp[k] = a[i];
		k++;
		i++;
	}
 
	// Insert all the remaining values from j to high into temp[].
	while (j <= high)
	{
		temp[k] = a[j];
		k++;
		j++;
	}
 
 
	// Assign sorted data stored in temp[] to a[].
	for (i = low; i <= high; i++)
	{
		a[i] = temp[i-low];
	}
}

void MergeSort(SortTemp *a, int low, int high){
	int mid;
	if (low < high)
	{
		mid=(low+high)/2;
		// Split the data into two half.
		MergeSort(a, low, mid);
		MergeSort(a, mid+1, high);
 
		// Merge them to get sorted output.
		Merge(a, low, high, mid);
	}
}

