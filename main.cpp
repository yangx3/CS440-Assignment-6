/*
CS 440 Assignment 6
Recovery
by Xiaoyi Yang
Implement from assignment 3, reference: stack overflow
changes from last assignment: values in three struct: match them type.
change int to long int to avoid Scientific notation
Make dname, ename char[40]. I was worry in this one from last assignment, using strcpy instead of passing pointer
Re-make merge-join. 
A new way for read and write to file, reference stack overflow and CPlusplus
Crashing Test idea comes from website.
Log idea comes from website.
*/

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<algorithm>

#include<sys/stat.h>
#include<ctime>
#include<fstream>
#include<string>
#include<cstring>
#include <unistd.h>

using namespace std;

//changes: struct from A3 for both dep and emp, values type match them, not only string applied
struct Dept 
{
	long int managerId;
    long int did;
    char dname[40];
    double budget;
   
};

//emp struct
struct Emp
{
    long int eid;
    char ename[40];
    long int age;
    double salary;
    
};

//for join strcut
struct EmpDep
{
    long int did;
    long int eid;
    char dname[40];
    char ename[40];
    double budget;
    double salary;
	long int age;
    long int managerId;    
};

//int LogNum = 0;
bool TEST_CRASH = true;  //set to false if dont want to do crash testing
int State = 50; //1/50 to crash/in testing, it is 1/20

void outPutLog(FILE* log, int depPre, int empPre, int output);
EmpDep * MergeToJoin(Dept * dep, Emp * emp);
Emp* getEmp(FILE* empFile);
Dept* getDep(FILE* depFile);


//using another way for join algorithm
struct SortTemp{
	//int num;  Using for debugging. delete it
	int dep;
	int emp;
	int join;
};


//error check functions, check then open file->
FILE* CheckOpenFile(string filename, string args){
	FILE* fp = fopen(filename.c_str(), args.c_str());
	if(!fp){
		cout << "Error found, can't read/write file "<< filename << endl;
		exit(1);
		
	}
	return fp;
}



void testCrash(){
	if(TEST_CRASH && rand() % State == 0){
		cout << endl << "******ERROR FOUND: Oh, this is a Test Crash ************" << endl;
		exit(1);
	}
}

//write to log file. Writing way reference: stack overflow
void writeLog(FILE* log, SortTemp* temp){
	if(!log)
		return;
	else
		fprintf(log, "%i, %i, %i\n", temp->dep, temp->emp, temp->join);
	//LogNum++;
}

//same way as writing. Stack overflow
SortTemp* readLog(FILE* log){
	SortTemp* temp = new SortTemp;
	if(!feof(log))
		fscanf(log, "%i, %i, %i\n", &temp->dep, &temp->emp, &temp->join);
	else
		temp = NULL;
	return temp;
	
}

//check if log file or read file exist AND can be read. if join file exist but empty, concerned as "no recording"
bool LogCheck(string filename){
	bool check_result;
	FILE* fp = fopen(filename.c_str(), "r");
	check_result = (fp != NULL);
	if(fp != NULL){
		check_result = fgetc(fp) != EOF;
		fclose(fp);}
	return check_result;
}


//create new line to log. In Order: Department, Emp and join's line number
void outPutLog(FILE* log, int depPre, int empPre, int output){
	if(!log)
		return;
	SortTemp temp;
	//temp.num = LogNum;
	temp.dep = depPre;
	temp.emp = empPre;
	temp.join = output;
	writeLog(log, &temp);
	
}


//recover from last run's position->
void recover(FILE* log, FILE* depFile, FILE* empFile, FILE* outFile){
	fseek(log, 0, SEEK_END);
	int position = ftell(log);
	cout << "restart at position: " << position << endl;
	int num = 0;
	int output;
	char character;
	fseek(log, position, SEEK_SET);
	//cout << "Test point 1" << endl;
	while(num < 2){
		output = fgetc(log);
		character = output;
		if(character == '\n')
			num+=1;
		position -= 1;
		fseek(log, position, SEEK_SET);
	}
	fseek(log, position + 2, SEEK_SET);
	SortTemp* temp = readLog(log);
	//temp->num = LogNum;
	fseek(depFile, temp->dep, SEEK_SET);
	fseek(empFile, temp->emp, SEEK_SET);
	fseek(outFile, temp->join, SEEK_SET);
	delete temp;
}


//New, easy way to read dept and emp
Dept* getDep(FILE* file){
	Dept* temp = new Dept;
	if(!feof(file))
		fscanf(file, "\"%ld\",\"%[^\"]\",\"%la\",\"%ld\"\n", &temp->did, temp->dname, &temp->budget, &temp->managerId);
	else
		temp = NULL;
    return temp;
}

Emp* getEmp(FILE* file){
	Emp* temp = new Emp;
	if(!feof(file))
		fscanf(file, "\"%ld\",\"%[^\"]\",\"%ld\",\"%la\"\n", &temp->eid, temp->ename, &temp->age, &temp->salary);
	else
		temp = NULL;
	return temp;
}

//another way to write on files, reference: stack overflow
void writeEmpDep(FILE* file, EmpDep* data)
{
	//const char* department = data->dname.c_str();
	//const char* employee = data->ename.c_str();
	fprintf(file,"\"%ld\",\"%ld\",\"%s\",\"%s\",\"%.21f\",\"%.21f\",\"%li\",\"%li\"\n", data->did, data->eid, data->dname, data->ename, data->budget, data->salary, data->managerId, data->age);	
    fflush(file);
}

//completely replace join algorithm-> Mine from A3 cannot do Recovery
void Mergejoin(FILE* outFile, FILE* depFile, FILE* empFile, FILE* log){
	Emp* emp = getEmp(empFile);
	Dept* dep = getDep(depFile);
	int EmpPre, DepPre;
	bool Done = false;
	if(dep == NULL || emp == NULL){
		cout << "Done: ";
		Done = true;
		cout << endl;
	}
	EmpPre = 0;
	DepPre = 0;
	int timeout = 0;
	outPutLog(log, ftell(depFile), ftell(empFile), ftell(outFile));
	//do merge join check.
	while(!Done && timeout <= 100000){
		if(emp != NULL && dep != NULL){
			if(emp->eid == dep->managerId){
				testCrash();
				EmpDep* output = MergeToJoin(dep, emp);
				writeEmpDep(outFile, output);
				outPutLog(log, DepPre, EmpPre, ftell(outFile));
				delete dep;
				dep = NULL;
				delete output;
				output = NULL;
				DepPre = ftell(depFile);
				dep = getDep(depFile);
				outPutLog(log, DepPre, EmpPre, ftell(outFile));
			}
			else if(emp->eid > dep->managerId){
				delete dep;
				dep = NULL;
				DepPre = ftell(depFile);
				dep = getDep(depFile);
				outPutLog(log, DepPre, EmpPre, ftell(outFile));
			}
			else{
				delete emp;
				emp = NULL;
				EmpPre = ftell(empFile);
				emp = getEmp(empFile);
				outPutLog(log, DepPre, EmpPre, ftell(outFile));
			}
		}
		testCrash();
		if(emp == NULL || dep == NULL){
			Done = true;
			break;}
		timeout++;
		
	
	}
	cout << "Finish at " << timeout << endl; // testing used.
}

//Move EMPDEP from main (which I have done in Assignment 3), require dep and emp not NULL #this is important!!! 
EmpDep* MergeToJoin(Dept * dep, Emp * emp){
	if(!dep || !emp)
		return NULL;
	
	EmpDep* temp = new EmpDep;
	temp->did = dep->did;
    temp->eid = emp->eid;

	strcpy(temp->dname, dep->dname);

    strcpy(temp->ename, emp->ename);
	
    temp->budget = dep->budget;
    temp->salary = emp->salary;
    temp->managerId = dep->managerId;
	temp->age = emp-> age;
	
	return temp;
}

int main(){
	srand(time(NULL));
	FILE* output, *log;
	FILE* department = CheckOpenFile("department.csv", "r");
	FILE* employee = CheckOpenFile("employee.csv", "r");
	cout << "Merge Join algorithm starting, checking the current state..." << endl;
	sleep(1);
	if(LogCheck("JOIN_LOG") && LogCheck("join.csv")){
		cout << "..." <<endl << "Re-start from last footstep" << endl;
		log = CheckOpenFile("JOIN_LOG", "a+");
		output = CheckOpenFile("join.csv", "a");
		recover(log, department, employee, output);
		cout << "Resuming..." << endl;
		sleep(1);
	}
	else{
		cout << "No recording found, starting new process" << endl;
		log = CheckOpenFile("JOIN_LOG", "w");
		output = CheckOpenFile("join.csv", "w");
		cout << "Processing..." << endl;
		sleep(1);
	}
	Mergejoin(output, department,employee, log);
	cout << "Processing complete, cleaning log..." << endl;
	fclose(output);
	fclose(log);
	fclose(department);
	fclose(employee);
	remove("JOIN_LOG");
	//remove log so it can be run again without delete log by hand.
	sleep(1);
	if(LogCheck("JOIN_LOG"))
		cout << "Remove failed, something happened! Please check JOIN_LOG" << endl;
	else
		cout << "Remove complete, have a nice day and see you next time!" << endl;
	return 0;
}


