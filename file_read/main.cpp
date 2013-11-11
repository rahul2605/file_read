#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <conio.h>
//#include <locale>
#include <algorithm>
#include <vector>
#include <tinydir.h>


using namespace std;

int clk = 0;
int R[32], RAT_R[32], RAT_F[32]; 
float F[32], Mem[256];


string IntAdderInstrns[5] = {"beq", "bne", "add", "addi", "sub"};
string FPAdderInstrns[2] = {"add_d", "sub_d"};
string FPMulInstrns[2] = {"mult_d", "div_d"};
string LDInstrns[2] = {"ld", "sd"};
vector<string> cur_code, code, print_code;									//Contains the lines of code

bool StringIsInt (string str) {
	if (str == "")
		return true;
	else
	{
		stringstream ss;
		ss << str;
		int num = 0;
		ss >> num;
		if(ss.good()) { return false; }
		else if (num == 0 && str.at(0) != '0') { return false; }
		else return true;
	}
}
void ReplaceAll( string &s, const string &search, const string &replace );

bool found = false;
bool CodeIsIntAdderInstrn(vector<string> string_list, string &line) {
	found = false;
	for (int i=0; i<5; i++)
	{
		if ((string_list.at(0) == IntAdderInstrns[i]) && (i <= 1 || i == 3) && (string_list.size() == 4)) //If beq, bne or add.i
		{
			found = true;
			if (string_list.at(1).at(0) == 'r' && string_list.at(2).at(0) == 'r')		//If both integer registers
			{
				if (string_list.at(1).size() == 1)
					string_list.at(1) = "r0";
				if (string_list.at(2).size() == 1)
					string_list.at(2) = "r0";
				ReplaceAll(line, "r,", "r0,");

				string error = "";
				string reg1 = string_list.at(1).substr(1,string::npos);
				string reg2 = string_list.at(2).substr(1,string::npos);
				int reg1_num = atoi(reg1.c_str());
				int reg2_num = atoi(reg2.c_str());
				
				if (!StringIsInt(reg1))
					error+= string_list.at(1) + " : " + reg1 + " should be an integer.";
				if (!StringIsInt(reg2))
				{
					if (error != "")
					{
						error+="\n";
						for (int j=0; j<line.size(); j++) error+=" ";
						error+="\t: ";
					}
					error+= string_list.at(2) + " : " + reg2 + " should be an integer.";
				}

				if (StringIsInt(reg1) && StringIsInt(reg2) && StringIsInt(string_list.at(3)) && reg1_num >= 0 && reg1_num < 32 &&  reg2_num >= 0 && reg2_num < 32) { return true; } //If registers in 0-31 and offset is integer
				else
				{
					if (!(reg1_num >= 0 && reg1_num < 32))
					{
						if (error != "")
						{
							error+="\n";
							for (int j=0; j<line.size(); j++) error+=" ";
							error+="\t: ";
						}
						error+= string_list.at(1) + " : " + reg1 + " should be an integer between 0 & 31.";
					}

					if (!(reg2_num >= 0 && reg2_num < 32))
					{
						if (error != "")
						{
							error+="\n";
							for (int j=0; j<line.size(); j++) error+=" ";
							error+="\t: ";
						}
						error+= string_list.at(2) + " : " + reg2 + " should be an integer between 0 & 31.";
					}

					print_code.push_back(line + "\t: " + error);
					return false;
				}
			}
			else
			{
				string error = "";
				if (string_list.at(1).at(0) != 'r')
					error += string_list.at(1);
				if (string_list.at(2).at(0) != 'r')
				{
					if (error != "")
						error+=" and ";
					error += string_list.at(2);
				}
				print_code.push_back(line + "\t: " + error + " should be Integer Register(s)."); return false;
			}
		}

		else if ((string_list.at(0) == IntAdderInstrns[i]) && (i == 2 || i == 4) && (string_list.size() == 4)) //If add or sub
		{
			found = true;
			if (string_list.at(1).at(0) == 'r' && string_list.at(2).at(0) == 'r' && string_list.at(3).at(0) == 'r')		//If all integer registers
			{
				if (string_list.at(1).size() == 1)
					string_list.at(1) = "r0";
				if (string_list.at(2).size() == 1)
					string_list.at(2) = "r0";
				if (string_list.at(3).size() == 1)
					string_list.at(3) = "r0";
				ReplaceAll(line, "r,", "r0,");

				string error = "";
				string reg1 = string_list.at(1).substr(1,string::npos);
				string reg2 = string_list.at(2).substr(1,string::npos);
				string reg3 = string_list.at(3).substr(1,string::npos);
				int reg1_num = atoi(reg1.c_str());
				int reg2_num = atoi(reg2.c_str());
				int reg3_num = atoi(reg3.c_str());

				if (!StringIsInt(reg1))
					error+= string_list.at(1) + " : " + reg1 + " should be an integer.";
				if (!StringIsInt(reg2))
				{
					if (error != "")
					{
						error+="\n";
						for (int j=0; j<line.size(); j++) error+=" ";
						error+="\t: ";
					}
					error+= string_list.at(2) + " : " + reg2 + " should be an integer.";
				}
				if (!StringIsInt(reg3))
				{
					if (error != "")
					{
						error+="\n";
						for (int j=0; j<line.size(); j++) error+=" ";
						error+="\t: ";
					}
					error+= string_list.at(3) + " : " + reg3 + " should be an integer.";
				}
				
				if (StringIsInt(reg1) && StringIsInt(reg2) && StringIsInt(reg3) && reg1_num >= 0 && reg1_num < 32 &&  reg2_num >= 0 && reg2_num < 32 &&  reg3_num >= 0 && reg3_num < 32) { return true; }  //If registers in 0-31
				else
				{
					if (!(reg1_num >= 0 && reg1_num < 32))
					{
						if (error != "")
						{
							error+="\n";
							for (int j=0; j<line.size(); j++) error+=" ";
							error+="\t: ";
						}
						error+= string_list.at(1) + " : " + reg1 + " should be an integer between 0 & 31.";
					}

					if (!(reg2_num >= 0 && reg2_num < 32))
					{
						if (error != "")
						{
							error+="\n";
							for (int j=0; j<line.size(); j++) error+=" ";
							error+="\t: ";
						}
						error+= string_list.at(2) + " : " + reg2 + " should be an integer between 0 & 31.";
					}

					if (!(reg3_num >= 0 && reg3_num < 32))
					{
						if (error != "")
						{
							error+="\n";
							for (int j=0; j<line.size(); j++) error+=" ";
							error+="\t: ";
						}
						error+= string_list.at(3) + " : " + reg3 + " should be an integer between 0 & 31.";
					}

					print_code.push_back(line + "\t: " + error);
					return false;
				}
			}
			else
			{
				string error = "";
				if (string_list.at(1).at(0) != 'r')
					error += string_list.at(1);
				if (string_list.at(2).at(0) != 'r')
				{
					if (error != "")
						error+=", ";
					error += string_list.at(2);
				}
				if (string_list.at(3).at(0) != 'r')
				{
					if (error != "")
						error+=" and ";
					error += string_list.at(3);
				}
				print_code.push_back(line + "\t: " + error + " should be Integer Register(s)."); return false;
			}
		}
	}
	return false;
}

bool CodeIsFPAdderInstrn(vector<string> string_list, string &line) {
	if (!found)
	{
		for (int i=0; i<2; i++)
		{
			string operation = string_list[0];
			std::replace(operation.begin(), operation.end(), '.', '_');
			if ((operation == FPAdderInstrns[i])  && (string_list.size() == 4)) //If add.d or s.d
			{
				found = true;
				if (string_list.at(1).at(0) == 'f' && string_list.at(2).at(0) == 'f' && string_list.at(3).at(0) == 'f') //If all FP registers
				{
					if (string_list.at(1).size() == 1)
						string_list.at(1) = "f0";
					if (string_list.at(2).size() == 1)
						string_list.at(2) = "f0";
					if (string_list.at(3).size() == 1)
						string_list.at(3) = "f0";
					ReplaceAll(line, "f,", "f0,");

					string error = "";
					string reg1 = string_list.at(1).substr(1,string::npos);
					string reg2 = string_list.at(2).substr(1,string::npos);
					string reg3 = string_list.at(3).substr(1,string::npos);
					int reg1_num = atoi(reg1.c_str());
					int reg2_num = atoi(reg2.c_str());
					int reg3_num = atoi(reg3.c_str());

					if (!StringIsInt(reg1))
						error+= string_list.at(1) + " : " + reg1 + " should be an integer.";
					if (!StringIsInt(reg2))
					{
						if (error != "")
						{
							error+="\n";
							for (int j=0; j<line.size(); j++) error+=" ";
							error+="\t: ";
						}
						error+= string_list.at(2) + " : " + reg2 + " should be an integer.";
					}
					if (!StringIsInt(reg3))
					{
						if (error != "")
						{
							error+="\n";
							for (int j=0; j<line.size(); j++) error+=" ";
							error+="\t: ";
						}
						error+= string_list.at(3) + " : " + reg3 + " should be an integer.";
					}
				
					if (StringIsInt(reg1) && StringIsInt(reg2) && StringIsInt(reg3) && reg1_num >= 0 && reg1_num < 32  &&  reg2_num >= 0 && reg2_num < 32  &&  reg3_num >= 0 && reg3_num < 32) { return true; } //If reg between 0-31
					else
					{
						if (!(reg1_num >= 0 && reg1_num < 32))
						{
							if (error != "")
							{
								error+="\n";
								for (int j=0; j<line.size(); j++) error+=" ";
								error+="\t: ";
							}
							error+= string_list.at(1) + " : " + reg1 + " should be an integer between 0 & 31.";
						}

						if (!(reg2_num >= 0 && reg2_num < 32))
						{
							if (error != "")
							{
								error+="\n";
								for (int j=0; j<line.size(); j++) error+=" ";
								error+="\t: ";
							}
							error+= string_list.at(2) + " : " + reg2 + " should be an integer between 0 & 31.";
						}

						if (!(reg3_num >= 0 && reg3_num < 32))
						{
							if (error != "")
							{
								error+="\n";
								for (int j=0; j<line.size(); j++) error+=" ";
								error+="\t: ";
							}
							error+= string_list.at(3) + " : " + reg3 + " should be an integer between 0 & 31.";
						}

						print_code.push_back(line + "\t: " + error);
						return false;
					}
				}

				else
				{
					string error = "";
					if (string_list.at(1).at(0) != 'f')
						error += string_list.at(1);
					if (string_list.at(2).at(0) != 'f')
					{
						if (error != "")
							error+=", ";
						error += string_list.at(2);
					}
					if (string_list.at(3).at(0) != 'f')
					{
						if (error != "")
							error+=" and ";
						error += string_list.at(3);
					}
					print_code.push_back(line + "\t: " + error + " should be FP Register(s)."); return false;
				}
			}
		}
	}
	return false;
}

bool CodeIsFPMulInstrn(vector<string> string_list, string &line) {
	if (!found)
	{
		for (int i=0; i<2; i++)
		{
			string operation = string_list[0];
			std::replace(operation.begin(), operation.end(), '.', '_');
			if ((operation == FPMulInstrns[i])  && (string_list.size() == 4)) //If mult.d or div.d
			{
				found = true;
				if (string_list.at(1).at(0) == 'f' && string_list.at(2).at(0) == 'f' && string_list.at(3).at(0) == 'f') //If all FP registers
				{
					if (string_list.at(1).size() == 1)
						string_list.at(1) = "f0";
					if (string_list.at(2).size() == 1)
						string_list.at(2) = "f0";
					if (string_list.at(3).size() == 1)
						string_list.at(3) = "f0";
					ReplaceAll(line, "f,", "f0,");

					string error = "";
					string reg1 = string_list.at(1).substr(1,string::npos);
					string reg2 = string_list.at(2).substr(1,string::npos);
					string reg3 = string_list.at(3).substr(1,string::npos);
					int reg1_num = atoi(reg1.c_str());
					int reg2_num = atoi(reg2.c_str());
					int reg3_num = atoi(reg3.c_str());

					if (!StringIsInt(reg1))
						error+= string_list.at(1) + " : " + reg1 + " should be an integer.";
					if (!StringIsInt(reg2))
					{
						if (error != "")
						{
							error+="\n";
							for (int j=0; j<line.size(); j++) error+=" ";
							error+="\t: ";
						}
						error+= string_list.at(2) + " : " + reg2 + " should be an integer.";
					}
					if (!StringIsInt(reg3))
					{
						if (error != "")
						{
							error+="\n";
							for (int j=0; j<line.size(); j++) error+=" ";
							error+="\t: ";
						}
						error+= string_list.at(3) + " : " + reg3 + " should be an integer.";
					}
				
					if (StringIsInt(reg1) && StringIsInt(reg2) && StringIsInt(reg3) && reg1_num >= 0 && reg1_num < 32  &&  reg2_num >= 0 && reg2_num < 32  &&  reg3_num >= 0 && reg3_num < 32) { return true; } //If reg between 0-31
					else
					{
						if (!(reg1_num >= 0 && reg1_num < 32))
						{
							if (error != "")
							{
								error+="\n";
								for (int j=0; j<line.size(); j++) error+=" ";
								error+="\t: ";
							}
							error+= string_list.at(1) + " : " + reg1 + " should be an integer between 0 & 31.";
						}

						if (!(reg2_num >= 0 && reg2_num < 32))
						{
							if (error != "")
							{
								error+="\n";
								for (int j=0; j<line.size(); j++) error+=" ";
								error+="\t: ";
							}
							error+= string_list.at(2) + " : " + reg2 + " should be an integer between 0 & 31.";
						}

						if (!(reg3_num >= 0 && reg3_num < 32))
						{
							if (error != "")
							{
								error+="\n";
								for (int j=0; j<line.size(); j++) error+=" ";
								error+="\t: ";
							}
							error+= string_list.at(3) + " : " + reg3 + " should be an integer between 0 & 31.";
						}

						print_code.push_back(line + "\t: " + error);
						return false;
					}
				}

				else
				{
					string error = "";
					if (string_list.at(1).at(0) != 'f')
						error += string_list.at(1);
					if (string_list.at(2).at(0) != 'f')
					{
						if (error != "")
							error+=", ";
						error += string_list.at(2);
					}
					if (string_list.at(3).at(0) != 'f')
					{
						if (error != "")
							error+=" and ";
						error += string_list.at(3);
					}
					print_code.push_back(line + "\t: " + error + " should be FP Register(s)."); return false;
				}
			}
		}
	}
	return false;
}

bool CodeIsLSInstrn(vector<string> string_list, string &line) {
	if (!found)
	{
		for (int i=0; i<2; i++)
		{
			if ((string_list.at(0) == LDInstrns[i])  && (string_list.size() == 3)) //If ld or sd
			{
				found = true;
				if (string_list.at(1).at(0) == 'f' && string_list.at(2).find('(') != string::npos && string_list.at(2).at(0)!='(' && string_list.at(2).find(')') != string::npos && string_list.at(2).at(0)!=')' && string_list.at(2).at(string_list.at(2).size()-1) == ')' && string_list.at(2).at(string_list.at(2).find('(')+1) == 'r') //If first is FP and second is int reg
				{
					if (string_list.at(1).size() == 1)
						string_list.at(1) = "f0";
					ReplaceAll(line, "f,", "f0,");
					ReplaceAll(line, "(r)", "(r0)");

					string error = "";
					string reg1 = string_list.at(1).substr(1,string::npos);
					string reg2 = string_list.at(2).substr(string_list.at(2).find('(')+2, string_list.at(2).find(')')-string_list.at(2).find('(')-2);
				
					if (!StringIsInt(reg1))
						error+= string_list.at(1) + " : " + reg1 + " should be an integer.";
					if (!StringIsInt(reg2))
					{
						if (error != "")
						{
							error+="\n";
							for (int j=0; j<line.size(); j++) error+=" ";
							error+="\t: ";
						}
						error+= string_list.at(2) + " : " + reg2 + " should be an integer.";
					}

					int reg1_num = atoi(reg1.c_str());
					string offset = string_list.at(2).substr(0,string_list.at(2).find('('));
					int reg2_num = atoi(reg2.c_str());
				
					if (StringIsInt(reg1) && StringIsInt(reg2) && StringIsInt(offset) && reg1_num >= 0 && reg1_num < 32 && reg2_num >= 0 && reg2_num < 32) { return true; } //If regs between 0-31 and offset is int
					else
					{
						if (!(reg1_num >= 0 && reg1_num < 32))
						{
							if (error != "")
							{
								error+="\n";
								for (int j=0; j<line.size(); j++) error+=" ";
								error+="\t: ";
							}
							error+= string_list.at(1) + " : " + reg1 + " should be an integer between 0 & 31.";
						}

						if (!(reg2_num >= 0 && reg2_num < 32))
						{
							if (error != "")
							{
								error+="\n";
								for (int j=0; j<line.size(); j++) error+=" ";
								error+="\t: ";
							}
							error+= string_list.at(2) + " : " + reg2 + " should be an integer between 0 & 31.";
						}

						print_code.push_back(line + "\t: " + error);
						return false;
					}
				}
				
				else
				{
					print_code.push_back(line + "\t: Unrecognized format for Load/Store Instructions."); return false;
				}
			}
		}
	}

	if (!found)
	{
		string str = line + "\t: Operation not recognized or number of inputs does not match the operation.";
		print_code.push_back(str);
	}
	return false;
}



struct TimingTable {										//Create the final IS-EX-MEM-WB-COMMIT table
	int ISSUE, EX0, EX1, MEM0, MEM1, WB, COMMIT0, COMMIT1;

	TimingTable() { ISSUE = EX0 = EX1 = MEM0 = MEM1 = WB = COMMIT0 = COMMIT1 = 0; }
public: void print(int cnt)
		{
			cout<<" ";
			if (ISSUE < 100) {cout<<" ";}
			if (ISSUE < 10) {cout<<" ";}
			cout<<ISSUE<<"  | ";

			if (EX0 < 100) {cout<<" ";}
			if (EX0 < 10) {cout<<" ";}
			if (EX0 == 0) {cout<<" ";}
			else cout<<EX0;
			if (EX0 > 0) cout<<" -";
			else cout<<"  ";
			if (EX1 < 100) {cout<<" ";}
			if (EX1 < 10) {cout<<" ";}
			if (EX1 == 0) {cout<<" ";}
			else cout<<EX1;
			cout<<" |";
			
			if (EX1+1 == WB)
			{
				cout<<"    -   ";
			}
			else
			{
				if (MEM0 < 100) {cout<<" ";}
				if (MEM0 < 10) {cout<<" ";}
				if (MEM0 == 0) cout<<" ";
				else cout<<MEM0;
				
				if ((MEM0 > 0 && MEM1 > 0) || (MEM0 == 0 && COMMIT0 > 0))
					cout<<" -";
				else
					cout<<"  ";
				if (MEM1 < 100) {cout<<" ";}
				if (MEM1 < 10) {cout<<" ";}
				if (MEM1 == 0) {cout<<" ";}
				else cout<<MEM1;
				
			}
			cout<<" | ";

			if (WB < 100) {cout<<" ";}
			if (WB < 10) {cout<<" ";}
			if (WB == 0) 
			{
				if (COMMIT0 > 0)	//Branch or Store Instruction
					cout<<"-";
				else
					cout << " ";
			}
			else cout << WB;
			cout<<"  | ";

			if (COMMIT0 < 100) {cout<<" ";}
			if (COMMIT0 < 10) {cout<<" ";}
			if (COMMIT0 == 0) {cout<<" ";}
			else cout<<COMMIT0;
			if (COMMIT0 > 0 && COMMIT1 > 0) cout<<" -";
			else cout<<"  ";
			if (COMMIT1 < 100) {cout<<" ";}
			if (COMMIT1 < 10) {cout<<" ";}
			if (COMMIT1 == 0) {cout<<" ";}
			else cout<<COMMIT1;

			cout<<" | "<<cnt<<endl;
		}
};
vector<TimingTable> FT;


struct Integer_Adder {										//Create the Integer Adder
	static int num_RS, cycles_EX, num_FU;
	static const int cycles_MEM = 0;

	Integer_Adder() {}
};
int Integer_Adder::num_RS, Integer_Adder::cycles_EX, Integer_Adder::num_FU;
int int_adder_RS_cnt = 0;


struct FP_Adder {										//Create the FP Adder
	static int num_RS, cycles_EX, num_FU;
	static const int cycles_MEM = 0;

	FP_Adder() {}
};
int FP_Adder::num_RS, FP_Adder::cycles_EX, FP_Adder::num_FU; 
int fp_adder_RS_cnt = 0;


struct FP_Multiplier {										//Create the FP Multiplier
	static int num_RS, cycles_EX, num_FU;
	static const int cycles_MEM = 0;

	FP_Multiplier() {}
};
int FP_Multiplier::num_RS, FP_Multiplier::cycles_EX, FP_Multiplier::num_FU;
int fp_mul_RS_cnt = 0;


struct LS_Unit {										//Create the Load/Store Unit
	static int num_RS, cycles_EX, cycles_MEM, num_FU;

	LS_Unit() {}
};
int LS_Unit::num_RS, LS_Unit::cycles_EX, LS_Unit::cycles_MEM, LS_Unit::num_FU;
int ls_RS_cnt = 0;



struct LS_Queue {
	string address, val, op;
	int code_cnt;

	LS_Queue () {address = ""; val = ""; op = ""; code_cnt = -1;}

public: void clear() { address = ""; val = ""; op = ""; code_cnt = -1; }

public: bool isEmpty() {
			if (code_cnt == -1)
				return true;
			else
				return false;
		}
};
vector<LS_Queue> LSQ;
int lsq_cnt = 0;

int AddLSQRow(LS_Queue qRow, int RS_num) {
	if (LSQ.size() > 0 && RS_num <= (LSQ.size()-1))
	{
		LSQ.at(RS_num).address = qRow.address;
		LSQ.at(RS_num).code_cnt = qRow.code_cnt;
		LSQ.at(RS_num).op = qRow.op;
		LSQ.at(RS_num).val = qRow.val;
		return RS_num;
	}
	else
	{
		LSQ.push_back(qRow);
		int temp = LSQ.size()-1;
		return temp;
	}
}



struct ReservationStation {										//Create the Reservation Station
	string Op, Qj, Qk;
	int Dst_Tag;
	float Vj, Vk;
	int code_cnt;

	ReservationStation() {Op = Qj = Qk = ""; Dst_Tag = code_cnt = -1; Vj = Vk = 0.0;}

public: bool isEmpty() 
	{
		if (Op == "") return true;
		else return false;
	}

public: void print(int maxVjCol, int maxIntPartVj, int cnt) 
	{
		if (!(this->isEmpty()))
		{
			cout << Op;
			for (int i=Op.size(); i<6; i++)
				cout<<" ";
			if (Dst_Tag != -1)
			{
				cout<<" |  ROB" << Dst_Tag;
				if (Dst_Tag < 10)
					cout<<" ";
			}

			cout << "  |  ";
			if (Qj == "")
				cout<<"     ";
			else
			{
				if (Qj.size() == 5)
					cout<<Qj;
				else if (Qj.size() == 4)
					cout<<Qj<<" ";
			}
			cout<< "  |  " ;
			if (Qk == "")
				cout<<"     ";
			else
			{
				if (Qk.size() == 5)
					cout<<Qk;
				else if (Qk.size() == 4)
					cout<<Qk<<" ";
			}
			cout<< "  | " ;
			if (Qj == "")
			{
				stringstream ss (stringstream::in | stringstream::out);
				ss << Vj;
				int curIntPartVj, curFractPartVj;
				if (ss.str().find('.') != string::npos)
				{
					curIntPartVj = ss.str().find('.');
					curFractPartVj = ss.str().size()-ss.str().find('.')-1;
				}
				else
				{
					curIntPartVj = ss.str().size();
					curFractPartVj = -1;
				}
				
				for (int j=0; j<maxIntPartVj-curIntPartVj; j++)
					cout<<" ";
				cout << Vj;
				for (int j=0; j<maxVjCol-maxIntPartVj-curFractPartVj-1; j++)
					cout<<" ";
			}
			else
			{
				for (int j=0; j<maxVjCol; j++) cout << " ";
			}
			cout << " | ";

			if (Qk == "")
			{
				stringstream ss (stringstream::in | stringstream::out);
				ss << Vk;
				int curIntPartVk, curFractPartVk;
				if (ss.str().find('.') != string::npos)
				{
					curIntPartVk = ss.str().find('.');
					curFractPartVk = ss.str().size()-ss.str().find('.')-1;
				}
				else
				{
					curIntPartVk = ss.str().size();
					curFractPartVk = -1;
				}
				
				for (int j=0; j<maxIntPartVj-curIntPartVk; j++)
				cout<<" ";
				cout << Vk;
				for (int j=0; j<maxVjCol-maxIntPartVj-curFractPartVk-1; j++)
					cout<<" ";
			}
			else
				for (int j=0; j<maxVjCol; j++) cout << " ";

			cout<<" | "<<cnt<<endl;
		}
		else
		{
			cout << "------ | ------- | ------- | ------- | ";
			for (int i=0; i<maxVjCol; i++) cout<<"-";
			cout<<" | ";
			for (int i=0; i<maxVjCol; i++) cout<<"-";
			cout<<" | ";
			if (cnt != -1)
				cout<<cnt;
			cout << endl;
		}
	}

public: void clear() { Op = Qj = Qk = ""; Dst_Tag = code_cnt = -1; Vj = Vk = 0.0; }
};




struct ReOrderBuffer {										//Create the Ro-order Buffer
	string Type, Dst;
	float Val;
	bool Ready;
	int code_cnt;

	ReOrderBuffer() {Type = Dst = ""; Val = 0.0; Ready = false; code_cnt = -1;}

public: bool isEmpty() 
	{
		if (Type == "") return true;
		else return false;
	}

public: void print(int i, int maxThirdCol, int maxIntPart, int curIntPart, int curFractPart, int cnt)
	{
		if (!(this->isEmpty())) { 
			cout << "ROB" << i;
			if (i < 100) cout << " ";
			if (i < 10) cout << " ";
			cout << " | " << Type << "\t | " << Dst << "\t | ";
			for (int j=0; j<maxIntPart-curIntPart; j++)
				cout<<" ";
			cout << Val;
			for (int j=0; j<maxThirdCol-maxIntPart-curFractPart-1; j++)
				cout<<" ";
			cout << " |   " << Ready << "   | " <<cnt << endl; 
		}
	}

public: void clear() { Type = Dst = ""; Val = 0.0; Ready = false; code_cnt = -1;}
};
int ROB_cnt = 0;
int ROB_entries = 0;



struct BranchTargetBuffer {
	int cur_PC, pred_PC, r1, r2;
	vector<int> code_cnt;
	bool taken;

	BranchTargetBuffer() {cur_PC = pred_PC = r1 = r2 = -1; taken = false;}
};
BranchTargetBuffer BTB[8];

bool BTBContains(int cur_code_cnt) {
	for (int i=0; i<8; i++)
		if (BTB[i].cur_PC == cur_code_cnt)
			return true;
	return false;
}

int BTBGet(int cur_code_cnt) {
	for (int i=0; i<8; i++)
		if (BTB[i].cur_PC == cur_code_cnt)
		{
			if (BTB[i].taken)
				return BTB[i].pred_PC;
			else
				return -1;
		}
}

void BTBAdd(int cur_code_cnt, int code_cnt, int pred_PC) {
	for (int i=0; i<8; i++)
		if (BTB[i].cur_PC == -1)
		{
			BTB[i].cur_PC = cur_code_cnt;
			BTB[i].code_cnt.push_back(code_cnt);
			BTB[i].pred_PC = pred_PC;
			break;
		}
}

void BTBUpdate(int cur_code_cnt, int code_cnt) {
	for (int i=0; i<8; i++)
	{
		if (BTB[i].cur_PC == cur_code_cnt)
		{BTB[i].code_cnt.push_back(code_cnt);}
	}
}

int branch_PC = 0;
bool branch_taken = false;
bool wait_for_branch = false;

struct BackupRAT {
	int backupR[32], backupF[32], code_cnt;

	BackupRAT() {
		for (int i=0; i<32; i++) backupR[i] = backupF[i] = -1;
		code_cnt = -1;
	}

	BackupRAT(int code_count) {
		for (int i=0; i<32; i++) {backupR[i] = RAT_R[i]; backupF[i] = RAT_F[i];}
		code_cnt = code_count;
	}
};
vector<BackupRAT> backups;
int misprediction_cnt = 0;


void RestoreBackup(BackupRAT backup, unsigned int &code_cnt, ReservationStation* RS_IntAdder, ReservationStation* RS_FPAdder, ReservationStation* RS_FPMultiplier, ReservationStation* RS_LSU, ReOrderBuffer* ROB);



//Function used in FormatLine(string)
bool BothAreSpaces(char lhs, char rhs) {					
	return ((lhs == rhs) && ((lhs == ' ') || (lhs == '\t'))) || ((lhs == ' ') && (rhs == '\t')) || ((lhs == '\t') && (rhs == ' ')); 
}


void ReplaceAll( string &s, const string &search, const string &replace ) {
    for( size_t pos = 0; ; pos += replace.length() ) {
        // Locate the substring to replace
        pos = s.find( search, pos );
        if( pos == string::npos ) break;
        // Replace by erasing and inserting
        s.erase( pos, search.length() );
        s.insert( pos, replace );
    }
}


//Function to remove multiple tabs and spaces
string FormatLine (string line) {
	//Replace tabs with spaces
	std::replace( line.begin(), line.end(), '\t', ' ');

	//Remove leading and trailing blanks
	string::size_type begin = line.find_first_not_of(" \t");
	string::size_type end   = line.find_last_not_of(" \t");
	line = line.substr(begin, end + 1);

	//Remove multiple instances of spaces
	string::iterator new_end = std::unique(line.begin(), line.end(), BothAreSpaces);
	line.erase(new_end, line.end()); 

	//Convert to lowercase
	//std::locale loc;
	//for (std::string::size_type i=0; i<line.length(); ++i)
		//line[i] = std::tolower(line[i],loc);
	std::transform(line.begin(), line.end(), line.begin(), ::tolower);

	//Remove spaces before and after '='
	if (line.find("rob") == string::npos)
	{
		ReplaceAll(line, " =", "=");
		ReplaceAll(line, "= ", "=");
	}
	return line;
}

//Function to break a line into it's words
vector<string> BreakLine (string line) {
	string delimiter = " ";								//Delimiter to seperate words on a line
	char extras[] = ",";								//Extra chars that have to be removed from each word
	string token = line;								//This will be looped and cut in the while loop
	string temp;
	vector<string> string_list;		
	int count = 0;
			
	while (token.find(delimiter) != string::npos)			//Loop to seperate a line at ", " - This will seperate the operands except the first one
	{
		temp = token.substr(0, token.find(delimiter));		 //Get the first word

		for (unsigned int i = 0; i < strlen(extras); ++i)   //Remove the extras
			temp.erase (std::remove(temp.begin(), temp.end(), extras[i]), temp.end());
					
		string_list.push_back(temp);
		token = token.erase(0, token.find(delimiter)+delimiter.length());  //Cut the front of the token for the next iteration
		count++;
	}
	string_list.push_back(token);
	return string_list;
}

//Function to parse the line
int line_cnt = 0;
int code_start = 0;
void ParseLine(string line) {
	line_cnt++;
	vector<string> string_list = BreakLine(line);	//Function to break a line into it's words	

	if (string_list[0] == "rob")
	{
		ROB_entries = atoi(string_list[3].c_str());
	}
	else if (string_list[0] == "integer" && string_list[1] == "adder")
	{
		Integer_Adder::num_FU = atoi(string_list[4].c_str());
		Integer_Adder::cycles_EX = atoi(string_list[3].c_str());
		Integer_Adder::num_RS = atoi(string_list[2].c_str());
	}
	else if (string_list[0] == "fp" && string_list[1] == "adder")
	{
		FP_Adder::num_FU = atoi(string_list[4].c_str());
		FP_Adder::cycles_EX = atoi(string_list[3].c_str());
		FP_Adder::num_RS = atoi(string_list[2].c_str());
	}
	else if (string_list[0] == "fp" && string_list[1] == "multiplier")
	{
		FP_Multiplier::num_FU = atoi(string_list[4].c_str());
		FP_Multiplier::cycles_EX = atoi(string_list[3].c_str());
		FP_Multiplier::num_RS = atoi(string_list[2].c_str());
	}
	else if (string_list[0] == "load/store" && string_list[1] == "unit")
	{
		LS_Unit::num_FU = atoi(string_list[5].c_str());
		LS_Unit::cycles_MEM = atoi(string_list[4].c_str());
		LS_Unit::cycles_EX = atoi(string_list[3].c_str());
		LS_Unit::num_RS = atoi(string_list[2].c_str());
	}
	else if (string_list[0].find('=') != string::npos)
	{
		for (int i=0; i < string_list.size(); i++)
		{
			if (string_list[i].at(0) == 'r')
			{
				int reg_num = atoi(string_list[i].substr(1,string_list[i].find('=')-1).c_str());
				int reg_val = atoi(string_list[i].substr(string_list[i].find('=')+1,string::npos).c_str());
				R[reg_num] = reg_val;
			}
			else if (string_list[i].at(0) == 'f')
			{
				int reg_num = atoi(string_list[i].substr(1,string_list[i].find('=')-1).c_str());
				float reg_val = std::stof(string_list[i].substr(string_list[i].find('=')+1,string::npos));
				F[reg_num] = reg_val;
			}
			else if (string_list[i].at(0) == 'm')
			{
				int mem_add = atoi(string_list[i].substr(4,string_list[i].find(']')-1).c_str());
				float mem_val = atof(string_list[i].substr(string_list[i].find('=')+1,string::npos).c_str());
				Mem[mem_add] = mem_val;
			}
		}
	}
	else if (!string_list.empty() && (string_list.size() < 5) && (string_list.size() > 2))
	{
		if (CodeIsIntAdderInstrn(string_list, line) || CodeIsFPAdderInstrn(string_list, line) || CodeIsFPMulInstrn(string_list, line) || CodeIsLSInstrn(string_list, line))
		{
			if (code_start == 0)
				code_start = line_cnt;
			cur_code.push_back(line);
			print_code.push_back(line);
		}
	}
	else if (!string_list.empty() && line_cnt > code_start && code_start > 0)
		print_code.push_back(line + "\t:Unrecognized Format. Too many or too few words.");
}

string SelectFile(char InputFolder[]) {
	cout<<"Select Input File:" << endl << endl;
	tinydir_dir dir;
	tinydir_open(&dir, InputFolder);
	int file_count = 1;

	while (dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		
		stringstream ss;
		string s;
		ss << file.name;
		ss >> s;

		if (s.at(0) != '.' && !file.is_dir)
		{
			if (file_count < 10)
				cout<<" ";
			printf("%d - %s",file_count++, file.name);
			cout<<endl;
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
	
	int file_num = 0;
	bool temp_file = true;
	while(true)
	{
		if (temp_file)
		{
			cout << endl << "Please Enter Input File Number: ";
			temp_file = false;
		}
	    if ((cin >> file_num) && (file_num > 0) && (file_num < file_count)) //true if a leading integer has entered the stream
		break ;
	    else
	    {
		  cout << "Invalid Input. Please select a number between 1 and "<<file_count-1<<": ";
		  cin.clear() ;
		  cin.ignore((std::numeric_limits<streamsize>::max)(), '\n') ;
	    }
	}
			


	tinydir_open(&dir, InputFolder);
	file_count = 1;

	tinydir_file file;
	while (dir.has_next && file_count<=file_num)
	{
		tinydir_readfile(&dir, &file);
		
		stringstream ss;
		string s;
		ss << file.name;
		ss >> s;

		if (s.at(0) != '.' && !file.is_dir)
		{
			file_count++;
		}

		tinydir_next(&dir);
	}

	stringstream ss;
	string file_path = InputFolder;
	//ss << InputFolder;
	//ss >> file_path;
	
	string token = file_path;
	file_path = "";
	while (token.find("\\") != string::npos)
	{
		file_path += token.substr(0, token.find("\\")+1);
		token = token.substr(token.find("\\")+1, token.size()-1);
	}
	//file_path += token;

	ss << file.name;
	string file_name;
	ss >> file_name;

	file_path += file_name;
	system("cls");
	return file_path;
}

char last_op = ' ';
bool last_print = false;
void print_screen(ReservationStation* RS_IntAdder, ReservationStation* RS_FPAdder, ReservationStation* RS_FPMultiplier, ReservationStation* RS_LSU, ReOrderBuffer* ROB);


int main() 
{
	char InputFolder[] = "\\\\psf\\Home\\Desktop\\Input Files\\";
	string file_path = SelectFile(InputFolder);

	R[0] = 0;
	for (int i=0; i<32; i++) {RAT_R[i] = RAT_F[i] = -1;}
	ifstream myfile(file_path.c_str());		//Open the input file
	
	if (myfile.is_open())										//If file can be opened, start reading line by line
	{
		string line;
		while (getline(myfile, line))							//Loop to get next line
		{
			if (!line.empty())
			{
				line = FormatLine(line);						//Function to remove extra tabs and spaces and convert to lowercase
				ParseLine(line);								//Function to parse the line
			}
		}
		myfile.close();
	} 
	else														//If file cannot be opened, show error
	{
		cout << "Error opening file.";
		_getch();
		exit(0);
	}

	//Print the initial state before starting the algorithm to confirm that data is parsed correctly
	cout<<endl<<"File opened: "<<file_path<<endl<<endl;
	cout<<endl<<"                # of rs    Cycles in Ex    Cycles in Mem    # of FUs"<<endl;
	cout<<"Integer Adder\t  "<<Integer_Adder::num_RS<<"\t\t"<<Integer_Adder::cycles_EX<<"\t\t"<<Integer_Adder::cycles_MEM<<"\t\t"<<Integer_Adder::num_FU<<endl;
	cout<<"FP Adder\t  "<<FP_Adder::num_RS<<"\t\t"<<FP_Adder::cycles_EX<<"\t\t"<<FP_Adder::cycles_MEM<<"\t\t"<<FP_Adder::num_FU<<endl;
	cout<<"FP Multiplier\t  "<<FP_Multiplier::num_RS<<"\t\t"<<FP_Multiplier::cycles_EX<<"\t\t"<<FP_Multiplier::cycles_MEM<<"\t\t"<<FP_Multiplier::num_FU<<endl;
	cout<<"L/S Unit\t  "<<LS_Unit::num_RS<<"\t\t"<<LS_Unit::cycles_EX<<"\t\t"<<LS_Unit::cycles_MEM<<"\t\t"<<LS_Unit::num_FU<<endl<<endl;
	cout<<"ROB entries = "<<ROB_entries<<endl<<endl;
	for (int i=0; i<32; i++)
	{
		if (R[i] != 0)
			cout << "R" << i <<" = " << R[i] << "\n";
	}
	cout<<endl;

	for (int i=0; i<32; i++)
	{
		if (F[i] != 0)
			cout << "F" << i <<" = " << F[i] << "\n";
	}
	cout<<endl;

	for (int i=0; i<256; i++)
	{
		if (Mem[i] != 0)
			cout << "Mem[" << i <<"] = " << Mem[i] << "\n";
	}
	cout<<endl<<endl<<"Code:"<<endl;
	for (int i=0; i<print_code.size(); i++)
		cout<<print_code.at(i)<<endl;
	cout<<endl;
	cout << "Continue? (y/n)";
	char input;
	cin >> input;
	if (input != 'y' && input != 'Y')
		exit(0);


	ReOrderBuffer* ROB = new ReOrderBuffer[ROB_entries];
	ReservationStation* RS_IntAdder = new ReservationStation[Integer_Adder::num_RS*Integer_Adder::num_FU];
	ReservationStation* RS_FPAdder = new ReservationStation[FP_Adder::num_RS*FP_Adder::num_FU];
	ReservationStation* RS_FPMultiplier = new ReservationStation[FP_Multiplier::num_RS*FP_Multiplier::num_FU];
	ReservationStation* RS_LSU = new ReservationStation[LS_Unit::num_RS*LS_Unit::num_FU];

	///////////////NOW WE HAVE THE DATA//////////////////////////


	unsigned int code_cnt=0;
	unsigned int cur_code_cnt = 0;

	while ((FT.size() == 0) || (FT.at(FT.size()-1).COMMIT0 == 0) || (misprediction_cnt >= clk))
	{
		if (last_op != 'E')
			print_screen(RS_IntAdder, RS_FPAdder, RS_FPMultiplier, RS_LSU, ROB);
		clk++;
		//////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////COMMIT////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////
		if (code_cnt > 0)
		{
			for (int i=0; i<FT.size(); i++)
			{
				if (FT.at(i).COMMIT0 == 0)
				{
					if (FT.at(i).WB != 0)
					{
						FT.at(i).COMMIT0 = clk;
						for (int j=0; j<ROB_entries; j++)
						{
							if (!ROB[j].isEmpty() && ROB[j].code_cnt == i)
							{
								if (ROB[j].Dst.at(0) == 'r')
								{
									ROB[j].Ready = true;
									int num = atoi(ROB[j].Dst.substr(1, string::npos).c_str());
									if (RAT_R[num] == j)
										RAT_R[num] = -1;

									if (backups.size() > 0)
									{
										int counter = backups.size()-1;
										while (counter >=0 && backups.at(counter).code_cnt > i)
										{
											if (backups.at(counter).backupR[num] == j)
												backups.at(counter).backupR[num] = -1;
											counter--;
										}
									}

									R[num] = ROB[j].Val;
									ROB[j].clear();
									ROB_cnt--;
									break;
								}
								else if (ROB[j].Dst.at(0) == 'f')
								{
									ROB[j].Ready = true;
									int num = atoi(ROB[j].Dst.substr(1, string::npos).c_str());
									if (RAT_F[num] == j)
										RAT_F[num] = -1;
										

									if (backups.size() > 0)
									{
										int counter = backups.size()-1;
										while (counter >=0 && backups.at(counter).code_cnt > i)
										{
											if (backups.at(counter).backupF[num] == j)
												backups.at(counter).backupF[num] = -1;
											counter--;
										}
									}

									F[num] = ROB[j].Val;
									ROB[j].clear();
									ROB_cnt--;
									break;
								}
							}
						}
					}

					else if (FT.at(i).WB == 0 && FT.at(i).EX1 < clk && FT.at(i).EX1 > 0)
					{
						for (int j=0; j<ROB_entries; j++)
						{
							string temp = ROB[j].Type;
							int tem = ROB[j].code_cnt;
							if (!ROB[j].isEmpty() && (ROB[j].code_cnt == i) && (ROB[j].Type == "store")) //STORE instruction
							{
								FT.at(i).COMMIT0 = clk;
								FT.at(i).COMMIT1 = clk + LS_Unit::cycles_MEM - 1	;
								ROB[j].Ready = true;
								int num = atoi(ROB[j].Dst.substr(3, string::npos).c_str());			//Get LSQ entry number
								int add;															//Get Mem address from LSQ
								if (LSQ.at(num).address.find('+') != string::npos)					//Calculate address if it's not calculated
								{
									int offset = atoi(LSQ.at(num).address.substr(0, LSQ.at(num).address.find('+')).c_str());
									int rob_val = atoi(LSQ.at(num).address.substr(LSQ.at(num).address.find('B')+1, string::npos).c_str());
									add = offset + ROB[rob_val].Val;
								}
								else
									add = atoi(LSQ.at(num).address.c_str());

								int r_num;
								float val;
								if (LSQ.at(num).val.find('R') != string::npos)						//If LSQ has ROB number	
								{
									r_num = atof(LSQ.at(num).val.substr(3, string::npos).c_str());	//Get ROB number from LSQ,
									string temp_code = code.at(LSQ.at(num).code_cnt);
									int r2_num = atoi(temp_code.substr(temp_code.find('r')+1,temp_code.find(')')-temp_code.find('r')-1).c_str());
									val = ROB[r_num].Val;											//Get val from ROB
								}		
								else																//Else if LSQ has actual value
								{
									val = atof(LSQ.at(num).val.c_str());							//Get val number from LSQ
								}
									
								Mem[add] = val;														//Update Mem with val
								ROB[j].Val = val;

								//LSQ.at(num).clear();
								//LSQ.erase(LSQ.begin()+num);												//Clear LSQ
								for (int k=0; k<LS_Unit::num_RS*LS_Unit::num_FU; k++)				//Clear RS
								{
									if (RS_LSU[k].code_cnt == ROB[j].code_cnt)
									{
										LSQ.at(num).clear();
										RS_LSU[k].clear();
										ls_RS_cnt--;
										break;
									}
								}
								ROB[j].clear();
								ROB_cnt--;
							}

							else if (!ROB[j].isEmpty() && (ROB[j].code_cnt == i) && (ROB[j].Type == "branch"))			//BRANCH instruction
							{
								for (int k=0; k<Integer_Adder::num_FU*Integer_Adder::num_RS; k++)
								{
									if (!RS_IntAdder[k].isEmpty() && RS_IntAdder[k].code_cnt == i)
									{
										RS_IntAdder[k].clear();
										int_adder_RS_cnt--;
										break;
									}
								}
								FT.at(i).COMMIT0 = clk;
								ROB[j].clear();
								ROB_cnt--;
							}
						}
					}
					break;
				}
			}
		}






		///////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////MEMORY/////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////
		bool mem_done = false;
		for (int i=0; i<LS_Unit::num_RS*LS_Unit::num_FU; i++)
		{
			if (!RS_LSU[i].isEmpty() && RS_LSU[i].Qj == "" && RS_LSU[i].Qk == "")														//Find a ready LSU RS
			{
				if (FT.at(RS_LSU[i].code_cnt).MEM0 == 0 && FT.at(RS_LSU[i].code_cnt).EX1 != 0 && FT.at(RS_LSU[i].code_cnt).EX1 < clk)	//If the corresponding function has executed
				{
					for (int j=0; j<LSQ.size(); j++)																					//Get the corresponding LSQ
					{
						if (LSQ.at(j).code_cnt == RS_LSU[i].code_cnt && RS_LSU[i].Op == "ld")											//If LOAD function
						{
							int add = atoi(LSQ.at(j).address.c_str());

							bool can_load = true;														//Flag to see if the addresses of all Stores above it are known
							
							for (int k=0; k<LSQ.size(); k++)
							{
								if (LSQ.at(k).code_cnt < LSQ.at(j).code_cnt && LSQ.at(k).op == "S" && (LSQ.at(k).address.find('+') != string::npos || LSQ.at(k).val.find('R') != string::npos))
								{can_load = false; break;}
							}
							
							//for (int l=0; l<j; l++)
								//if (LSQ.at(l).op == "S" && LSQ.at(l).address.find('+') != string::npos)	{can_load = false; break;}

							if (can_load)
							{
								FT.at(RS_LSU[i].code_cnt).MEM0 = clk;										//Update MEM feilds
								FT.at(RS_LSU[i].code_cnt).MEM1 = clk + LS_Unit::cycles_MEM - 1;

								bool found = false;								//Flag to see if forwarding-from-store is needed
								int found_num = 0;								//LSQ number from where to get data if F-F-S is required
								for (int k=0; k<LSQ.size(); k++)						//Check LSQ starting from the most recent entry
								{
									if (LSQ.at(k).address == LSQ.at(j).address && LSQ.at(k).code_cnt<LSQ.at(j).code_cnt) //If address of another entry matches, check if it is a STORE
									{
										for (int l=0; l<LS_Unit::num_RS*LS_Unit::num_FU; l++)   //Get the RS associated with the matching LSQ entry
										{
											if ((RS_LSU[l].code_cnt == LSQ.at(k).code_cnt) && (RS_LSU[l].Op == "sd")) //If it is a store,
											{
												found_num = k;					//LSQ number that contains the value
												found = true;					//Found dependency - F-F-S is needed
												break;							//Don't look anymore
											}
										}
										if (found)								//If dependency found,
											break;								//Don't look anymore
									}
								}

								string val = "";
								stringstream ss (stringstream::in | stringstream::out);
								if (!found)										//If dependency not found, get value from Memory
								{	
									ss << Mem[add];
									val = ss.str();
								}
								else											//Else if dependency found, get value from previous LSQ row
								{
									val = LSQ.at(found_num).val;
									FT.at(RS_LSU[i].code_cnt).MEM1 = FT.at(RS_LSU[i].code_cnt).MEM1 - 3;  //Just keep 1 cycle for Memory access time
								}

								LSQ.at(j).val = val;
								mem_done = true;
							}
							break;
						}

						else if (LSQ.at(j).code_cnt == RS_LSU[i].code_cnt && RS_LSU[i].Op == "sd")				//Else if STORE function
						{
							FT.at(RS_LSU[i].code_cnt).MEM0 = 0;
							FT.at(RS_LSU[i].code_cnt).MEM1 = 0;
							break;
						}
					}
				}
			}
			if (mem_done)
				break;
		}









		///////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////EXECUTE////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////
		
		int FU_IA = 0;
		int FU_FA = 0;
		int FU_FM = 0;
		int FU_LS = 0;

		for (int i=0; i<FT.size(); i++)								//Check FT for row with EX0=0
		{
			if (FT.at(i).EX0 == 0)
			{
				for (int j=0; j<Integer_Adder::num_RS*Integer_Adder::num_FU; j++)	//If it's an Integer Adder
				{
					if (FU_IA < Integer_Adder::num_FU && !RS_IntAdder[j].isEmpty() && RS_IntAdder[j].code_cnt == i && RS_IntAdder[j].Qj == "" && RS_IntAdder[j].Qk == "") //It it's ready to execute
					{
						FT.at(i).EX0 = clk;
						FT.at(i).EX1 = clk + Integer_Adder::cycles_EX - 1;
						FU_IA++;
					}
				}

				for (int j=0; j<FP_Adder::num_RS*FP_Adder::num_FU; j++)	//If it's a FP Adder
				{
					if (FU_FA < FP_Adder::num_FU && !RS_FPAdder[j].isEmpty() && RS_FPAdder[j].code_cnt == i && RS_FPAdder[j].Qj == "" && RS_FPAdder[j].Qk == "") //It it's ready to execute
					{
						FT.at(i).EX0 = clk;
						FT.at(i).EX1 = clk + FP_Adder::cycles_EX - 1;
						FU_FA++;
					}
				}

				for (int j=0; j<FP_Multiplier::num_RS*FP_Multiplier::num_FU; j++)	//If it's a FP Multiplier
				{
					if (FU_FM < FP_Multiplier::num_FU && !RS_FPMultiplier[j].isEmpty() && RS_FPMultiplier[j].code_cnt == i && RS_FPMultiplier[j].Qj == "" && RS_FPMultiplier[j].Qk == "") //It it's ready to execute
					{
						FT.at(i).EX0 = clk;
						if (RS_FPMultiplier[j].Op == "mult.d")
							FT.at(i).EX1 = clk + FP_Multiplier::cycles_EX - 1;
						else if (RS_FPMultiplier[j].Op == "div.d")
							FT.at(i).EX1 = clk + 40 - 1;
						FU_FM++;
					}
				}

				bool address_ready = true;
				for (int j=0; j<LS_Unit::num_RS*LS_Unit::num_FU; j++)	//If it's a LS Unit
				{
					if (FU_LS < LS_Unit::num_FU && !RS_LSU[j].isEmpty() && RS_LSU[j].code_cnt == i && RS_LSU[j].Qj == "" && RS_LSU[j].Qk == "") //It it's ready to execute
					{
						for (int k=0; k<LSQ.size(); k++)
						{
							if (LSQ.at(k).code_cnt == i)
								if (LSQ.at(k).address.find('R') != string::npos)
								{address_ready = false; break;}
						}
						if (address_ready)
						{
							FT.at(i).EX0 = clk;
							FT.at(i).EX1 = clk + LS_Unit::cycles_EX - 1;
							FU_LS++;
							/*if (RS_LSU[j].Op == "ld")									
							{
								int add = RS_LSU[j].Vj + RS_LSU[j].Vk;
								stringstream ss;
								ss << add;
								string str_add = ss.str();
								for (int k=0; k<LSQ.size(); k++)
								{
									if (LSQ.at(k).code_cnt == RS_LSU[j].code_cnt)
										LSQ.at(k).address = str_add;
								}
							}*/
						}
					}
				}
			}

			if (FT.at(i).EX1 == clk)
			{
				for (int j=0; j<Integer_Adder::num_RS*Integer_Adder::num_FU; j++)	//If it's an Integer Adder
				{
					if (!RS_IntAdder[j].isEmpty() && RS_IntAdder[j].code_cnt == i && (RS_IntAdder[j].Op == "bne" || RS_IntAdder[j].Op == "beq")) //If it's BRANCH & ready to execute
					{
						int BTB_num = -1;										//For BRANCH instructions, get BTB entry corresponding to current FT entry
						for (int k=0; k<8; k++)
						{
							if (BTB[k].code_cnt.size() > 0)
								for (int l=0; l<BTB[k].code_cnt.size(); l++)
									if (BTB[k].code_cnt.at(l) == i)
										BTB_num = k;
						}


						if (RS_IntAdder[j].Op == "bne")							//Check if the branch should actually be taken or not
						{
							if (RS_IntAdder[j].Vj != RS_IntAdder[j].Vk)	
								branch_taken = true; 
							else
								branch_taken = false;
						}

						else if (RS_IntAdder[j].Op == "beq")
						{
							if (RS_IntAdder[j].Vj == RS_IntAdder[j].Vk)
								branch_taken = true;
							else
								branch_taken = false;
						}

						if (BTB_num != -1)												//If BTB had the value,				
						{
							if ((BTB[BTB_num].taken && !branch_taken) || (BTB[BTB_num].taken && !branch_taken))	//If prediction has changed,
							{
								BackupRAT bkup;
								for (int k=0; k<backups.size(); k++)
									if (backups.at(k).code_cnt == i)
										{bkup = backups.at(k); break;}

								RestoreBackup(bkup, code_cnt, RS_IntAdder, RS_FPAdder, RS_FPMultiplier, RS_LSU, ROB); //Restore from backup

																											//Restore PC
								if (branch_taken)
									cur_code_cnt = BTB[BTB_num].pred_PC;

								else if (!branch_taken)
									cur_code_cnt = BTB[BTB_num].cur_PC+1;
								
								misprediction_cnt = clk+2;
							}
						}
						BTB[BTB_num].taken = branch_taken;
					}
				}


				for (int j=0; j<LS_Unit::num_RS*LS_Unit::num_FU; j++)	//If it's a LS Unit
				{
					if (!RS_LSU[j].isEmpty() && RS_LSU[j].code_cnt == i && (RS_LSU[j].Op == "ld" || RS_LSU[j].Op == "sd")) //It it's ready to execute
					{
						if (RS_LSU[j].Op == "ld")									
						{
							int add = RS_LSU[j].Vj + RS_LSU[j].Vk;
							stringstream ss;
							ss << add;
							string str_add = ss.str();
							for (int k=0; k<LSQ.size(); k++)
							{
								if (LSQ.at(k).code_cnt == RS_LSU[j].code_cnt)
									LSQ.at(k).address = str_add;
							}
						}

						else if (RS_LSU[j].Op == "sd")
						{
							int add;
							for (int k=0; k<LSQ.size(); k++)
							{
								if (LSQ.at(k).code_cnt == RS_LSU[j].code_cnt)
								{

									add = atoi(LSQ.at(k).address.substr(0, LSQ.at(k).address.find('+')).c_str()) + atoi(LSQ.at(k).address.substr(LSQ.at(k).address.find('+')+1, LSQ.at(k).address.size()-1).c_str());
									ostringstream convert; 
									convert << add;
									LSQ.at(k).address = convert.str();
								}
							}
						}
					}
				}

			}
		}
		












		/////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////ISSUE////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////
		
		if (!wait_for_branch && cur_code_cnt < cur_code.size() && misprediction_cnt <=clk)
		{
			vector<string> string_list = BreakLine(cur_code.at(cur_code_cnt));			//This contains the line in parts
			
			string operation = string_list[0];
			std::replace(operation.begin(), operation.end(), '.', '_');

			bool inc_flag = false;									//Flag that tells us if we should increment the PC during this clk cycle - defalut=don't
			if (ROB_cnt < ROB_entries)
			{
				if ((operation == FPMulInstrns[0] || operation == FPMulInstrns[1]) && fp_mul_RS_cnt < FP_Multiplier::num_RS*FP_Multiplier::num_FU)  //FP Multiplier
				{
					fp_mul_RS_cnt++;

					TimingTable thisRow;								//Add new row to FT
					thisRow.ISSUE = clk;							//Update ISSUE of new row
					FT.push_back(thisRow);
					code.push_back(cur_code.at(cur_code_cnt));
					inc_flag = true;								//Flag that tells us if we should increment the PC during this clk cycle

					for (int i=0; i<FP_Multiplier::num_RS*FP_Multiplier::num_FU; i++)
					{
						if (RS_FPMultiplier[i].isEmpty())			//Get empty RS
						{	
							RS_FPMultiplier[i].Op = string_list[0]; //Update OP of RS
							RS_FPMultiplier[i].code_cnt = code_cnt;
						
							string reg_name = string_list[2];       //Get first SRC operand and update RS from ROB or RAT or F
							int reg_num = atoi(reg_name.substr(1,string::npos).c_str());
						
							if (RAT_F[reg_num] == -1)				//If RAT is empty, get value from ARF - in Vj
								RS_FPMultiplier[i].Vj = F[reg_num];
							else									//Else if RAT is not empty,
							{
								if (ROB[RAT_F[reg_num]].Ready)		//If ROB has value, get from there - in Vj
									RS_FPMultiplier[i].Vj = ROB[RAT_F[reg_num]].Val;
								else								//Else update Qj
									RS_FPMultiplier[i].Qj = "ROB" + std::to_string((long long)RAT_F[reg_num]);
							}

						
							reg_name = string_list[3];				//Get second SRC operand and update RS from ROB or RAT or F
							reg_num = atoi(reg_name.substr(1,string::npos).c_str());
						
							if (RAT_F[reg_num] == -1)				//If RAT is empty, get value from ARF - in Vk
								RS_FPMultiplier[i].Vk = F[reg_num];
							else									//Else if RAT is not empty,
							{
								if (ROB[RAT_F[reg_num]].Ready)		//If ROB has value, get from there - in Vk
									RS_FPMultiplier[i].Vk = ROB[RAT_F[reg_num]].Val;
								else								//Else update Qk
									RS_FPMultiplier[i].Qk = "ROB" + std::to_string((long long)RAT_F[reg_num]);
							}

							reg_name = string_list[1];				//Get DEST for ROB
							reg_num = atoi(reg_name.substr(1,string::npos).c_str());

							for (int j=0; j<ROB_entries; j++)
							{
								if (ROB[j].isEmpty())				//Get empty ROB
								{
									RS_FPMultiplier[i].Dst_Tag = j; //Update DEST_TAG for RS
									ROB[j].Dst = reg_name;			//Update DEST for ROB
									ROB[j].Type = "reg";			//Update TYPE for ROB
									ROB[j].Ready = false;			//Update READY flag for ROB
									RAT_F[reg_num] = j;				//Update RAT
									ROB_cnt++;
									ROB[j].code_cnt = code_cnt;
									break;
								}
							}

							break;	
						}
					}
				}

				else
				{
					bool break_flag = false;
					for (int i=0; i<5; i++)
					{
						if (operation == IntAdderInstrns[i] && int_adder_RS_cnt < Integer_Adder::num_RS*Integer_Adder::num_FU)  //Integer Adder
						{
							int_adder_RS_cnt++;

							TimingTable thisRow;								//Add new row to FT
							thisRow.ISSUE = clk;							//Update ISSUE of new row
							FT.push_back(thisRow);
							code.push_back(cur_code.at(cur_code_cnt));
							inc_flag = true;								//Flag that tells us if we should increment the PC during this clk cycle

							for (int i=0; i<Integer_Adder::num_RS*Integer_Adder::num_FU; i++)
							{
								if (RS_IntAdder[i].isEmpty())				//Get empty RS
								{	
									RS_IntAdder[i].Op = string_list[0];	    //Update OP of RS
									RS_IntAdder[i].code_cnt = code_cnt;
								
									string reg1, reg2;
									int reg1_num, reg2_num, immediate, offset;
									if ((string_list[0] == "beq") || (string_list[0] == "bne")) //If BRANCH instrn, get the 2 operands and offset(dest)
									{
										
										reg1 = string_list[1];
										reg2 = string_list[2];
										offset = atoi(string_list[3].c_str())/4;
										branch_PC = cur_code_cnt+1+offset;
										reg1_num = atoi(reg1.substr(1,string::npos).c_str());
										reg2_num = atoi(reg2.substr(1,string::npos).c_str());

										if (RAT_R[reg1_num] == -1)									//Get first SRC operand and update RS - V & Q vals
											RS_IntAdder[i].Vj = R[reg1_num];						//If RAT is empty, get value from ARF - in Vj
										else														//Else if RAT is not empty,
										{
											if (ROB[RAT_R[reg1_num]].Ready)							//If ROB has value, get from there - in Vj
												RS_IntAdder[i].Vj = ROB[RAT_R[reg1_num]].Val;
											else													//Else update Qj
												RS_IntAdder[i].Qj = "ROB" + std::to_string((long long)RAT_R[reg1_num]);
										}

										if (RAT_R[reg2_num] == -1)									//Get first SRC operand and update RS - V & Q vals
											RS_IntAdder[i].Vk = R[reg2_num];						//If RAT is empty, get value from ARF - in Vj
										else														//Else if RAT is not empty,
										{
											if (ROB[RAT_R[reg2_num]].Ready)							//If ROB has value, get from there - in Vj
												RS_IntAdder[i].Vk = ROB[RAT_R[reg2_num]].Val;
											else													//Else update Qj
												RS_IntAdder[i].Qk = "ROB" + std::to_string((long long)RAT_R[reg2_num]);
										}

										if (BTBContains(cur_code_cnt))						//If BTB contains current PC,
										{
											BackupRAT bkup(code_cnt);
											backups.push_back(bkup);
											BTBUpdate(cur_code_cnt, code_cnt);				//Add current code_cnt to BTB entry

											int temp_branch_PC = BTBGet(cur_code_cnt);
											if (temp_branch_PC != -1)						//It predicted taken,
												cur_code_cnt = temp_branch_PC-1;			//Change PC to new value-1
										}
										else												//If BTB doesn't contain current PC,
										{
											BTBAdd(cur_code_cnt, code_cnt, branch_PC);		//Add current value to BTB
											wait_for_branch = true;							//Wait for execution to complete
										}
									}
									else														//If NOT BRANCH, get the 2 operands and immediate val(only for addi)
									{
										reg1 = string_list[2];
										reg1_num = atoi(reg1.substr(1,string::npos).c_str());
										if (string_list[0] == "addi")							//If operation is ADDI,
											immediate = atoi(string_list[3].c_str());			//Get immediate value
										else
										{
											reg2 = string_list[3];
											reg2_num = atoi(reg2.substr(1,string::npos).c_str());
										}

										if (RAT_R[reg1_num] == -1)									//Get first SRC operand and update RS - V & Q vals
											RS_IntAdder[i].Vj = R[reg1_num];						//If RAT is empty, get value from ARF - in Vj
										else														//Else if RAT is not empty,
										{
											if (ROB[RAT_R[reg1_num]].Ready)							//If ROB has value, get from there - in Vj
												RS_IntAdder[i].Vj = ROB[RAT_R[reg1_num]].Val;
											else													//Else update Qj
												RS_IntAdder[i].Qj = "ROB" + std::to_string((long long)RAT_R[reg1_num]);
										}

										if (string_list[0] == "addi")								//Get second SRC operand and update RS - V & Q vals
										{
											RS_IntAdder[i].Vk = immediate;							//If operation is ADDI, get immediate val - in Vk
										}
										else
										{
											if (RAT_R[reg2_num] == -1)								//If RAT is empty, get value from ARF - in Vk								
												RS_IntAdder[i].Vk = R[reg2_num];
											else													//Else if RAT is not empty,
											{
												if (ROB[RAT_R[reg2_num]].Ready)						//If ROB has value, get from there - in Vk
													RS_IntAdder[i].Vk = ROB[RAT_R[reg2_num]].Val;
												else												//Else update Qk
													RS_IntAdder[i].Qk = "ROB" + std::to_string((long long)RAT_R[reg2_num]);
											}
										}
									}

								

									reg1 = string_list[1];										//Get DEST for ROB for NON-BRANCH instrns
									reg1_num = atoi(reg1.substr(1,string::npos).c_str());

									for (int j=0; j<ROB_entries; j++)
									{
										if (ROB[j].isEmpty())									//Get empty ROB
										{
											RS_IntAdder[i].Dst_Tag = j;							//Update DEST_TAG for RS
											ROB[j].Ready = false;								//Update READY flag for ROB
											if ((string_list[0] == "beq") || (string_list[0] == "bne"))  
											{
												ROB[j].Dst = "";
												ROB[j].Type = "branch";
												ROB_cnt++;
												ROB[j].code_cnt = code_cnt;
											}
											else
											{
												ROB[j].Dst = reg1;									//Update DEST for ROB
												ROB[j].Type = "reg";								//Update TYPE for ROB
												RAT_R[reg1_num] = j;								//Update RAT
												ROB_cnt++;
												ROB[j].code_cnt = code_cnt;
											}
											break;
										}
									}

									break;	
								}
							}

							break;
						}
						if (i < 2)
						{
							if ( operation == FPAdderInstrns[i] && fp_adder_RS_cnt < FP_Adder::num_RS*FP_Adder::num_FU) // FP Adder
							{
								fp_adder_RS_cnt++;

								TimingTable thisRow;								//Add new row to FT
								thisRow.ISSUE = clk;							//Update ISSUE of new row
								FT.push_back(thisRow);
								code.push_back(cur_code.at(cur_code_cnt));
								inc_flag = true;								//Flag that tells us if we should increment the PC during this clk cycle

								for (int i=0; i<FP_Adder::num_RS*FP_Adder::num_FU; i++)
								{
									if (RS_FPAdder[i].code_cnt == code_cnt)
										break;
									if (RS_FPAdder[i].isEmpty())				//Get empty RS
									{	
										RS_FPAdder[i].Op = string_list[0];		//Update OP of RS
										RS_FPAdder[i].code_cnt = code_cnt;
									
										string reg_name = string_list[2];       //Get first SRC operand and update RS from ROB or RAT or F
										int reg_num = atoi(reg_name.substr(1,string::npos).c_str());
										if (RAT_F[reg_num] == -1)				//If RAT is empty, get value from ARF - in Vj
											RS_FPAdder[i].Vj = F[reg_num];
										else									//Else if RAT is not empty,
										{
											if (ROB[RAT_F[reg_num]].Ready)		//If ROB has value, get from there - in Vj
												RS_FPAdder[i].Vj = ROB[RAT_F[reg_num]].Val;
											else								//Else update Qj
												RS_FPAdder[i].Qj = "ROB" + std::to_string((long long)RAT_F[reg_num]);
										}

										reg_name = string_list[3];				//Get second SRC operand and update RS from ROB or RAT or F
										reg_num = atoi(reg_name.substr(1,string::npos).c_str());
										if (RAT_F[reg_num] == -1)				//If RAT is empty, get value from ARF - in Vk
											RS_FPAdder[i].Vk = F[reg_num];
										else									//Else if RAT is not empty,
										{
											if (ROB[RAT_F[reg_num]].Ready)		//If ROB has value, get from there - in Vk
												RS_FPAdder[i].Vk = ROB[RAT_F[reg_num]].Val;
											else								//Else update Qk
												RS_FPAdder[i].Qk = "ROB" + std::to_string((long long)RAT_F[reg_num]);
										}

										reg_name = string_list[1];				//Get DEST for ROB
										reg_num = atoi(reg_name.substr(1,string::npos).c_str());

										for (int j=0; j<ROB_entries; j++)
										{
											if (ROB[j].isEmpty())				//Get empty ROB
											{
												RS_FPAdder[i].Dst_Tag = j;		//Update DEST_TAG for RS
												ROB[j].Dst = reg_name;			//Update DEST for ROB
												ROB[j].Type = "reg";			//Update TYPE for ROB
												ROB[j].Ready = false;			//Update READY flag for ROB
												RAT_F[reg_num] = j;				//Update RAT
												ROB_cnt++;
												ROB[j].code_cnt = code_cnt;
												break;
											}
										}

										break;	
									}
								}

								break_flag = true;
							}
							else if (operation == LDInstrns[i] && ls_RS_cnt < LS_Unit::num_RS*LS_Unit::num_FU)    //LS Unit
							{
								ls_RS_cnt++;

								TimingTable thisRow;								//Add new row to FT
								thisRow.ISSUE = clk;							//Update ISSUE of new row
								FT.push_back(thisRow);
								code.push_back(cur_code.at(cur_code_cnt));
								inc_flag = true;								//Flag that tells us if we should increment the PC during this clk cycle

								LS_Queue qRow;

								for (int i=0; i<LS_Unit::num_RS*LS_Unit::num_FU; i++)
								{
									if (RS_LSU[i].isEmpty())						//Get empty RS
									{	
										RS_LSU[i].Op = string_list[0];				//Update OP of RS
										RS_LSU[i].code_cnt = code_cnt;
									

										string reg1, reg2, offset_in_string;
										int reg1_num, reg2_num, offset;

										reg1 = string_list[1];						//FP Register
										reg2 = string_list[2].substr(string_list[2].find('(')+1, string_list[2].find(')')-string_list[2].find('(')-1); //Integer Register

										reg1_num = atoi(reg1.substr(1,string::npos).c_str()); //FP Register number
										reg2_num = atoi(reg2.substr(1,string::npos).c_str()); //Integer Register number
										offset_in_string = string_list[2].substr(0, string_list[2].find('(')); //Offset
										offset = atoi(offset_in_string.c_str());	//Offset

										if (string_list[0] == "ld")					//If LOAD, update RS with offset and SRC reg value
										{
											qRow.op = "L";
											if (RAT_R[reg2_num] == -1)				//If RAT is empty, get value from ARF - in Vj
												RS_LSU[i].Vj = R[reg2_num];
											else									//Else if RAT is not empty,
											{
												if (ROB[RAT_R[reg2_num]].Ready)		//If ROB has value, get from there - in Vj
													RS_LSU[i].Vj = ROB[RAT_R[reg2_num]].Val;
												else								//Else update Qj
													RS_LSU[i].Qj = "ROB" + std::to_string((long long)RAT_R[reg2_num]);
											}
											RS_LSU[i].Vk = offset;					//Update Vk with offset
										}
										else if (string_list[0] == "sd")			//If STORE, update RS with SRC reg value
										{
											qRow.op = "S";
											stringstream ss (stringstream::in | stringstream::out);
											ss << F[reg1_num];
										
											if (RAT_F[reg1_num] == -1)				//If RAT is empty, get value from ARF - in Vj and QROW.VAL
											{RS_LSU[i].Vj = F[reg1_num]; qRow.val = ss.str();}
											else									//Else if RAT is not empty,
											{
												if (ROB[RAT_F[reg1_num]].Ready)		//If ROB has value, get from there - in Vj and QROW.VAL
												{
													RS_LSU[i].Vj = ROB[RAT_F[reg1_num]].Val;
													stringstream ss (stringstream::in | stringstream::out);
													ss << ROB[RAT_F[reg1_num]].Val;
													qRow.val = ss.str();
												}
												else								//Else update Qj
													RS_LSU[i].Qj = qRow.val = "ROB" + std::to_string((long long)RAT_F[reg1_num]);
											}
										}

										
																					//NOW WE UPDATE THE qRow ADDRESS
										if (RAT_R[reg2_num] == -1)					//If RAT is empty, get value from ARF - QROW.ADDRESS
											qRow.address = std::to_string((long long)(R[reg2_num] + offset));
										else										//Else if RAT is not empty,
										{
											if (ROB[RAT_R[reg2_num]].Ready)			//If ROB has value, get from there - QROW.ADDRESS
												qRow.address = std::to_string((long long)(ROB[RAT_R[reg2_num]].Val + offset));
											else									//Else update with string of address - QROW.ADDRESS
												qRow.address = offset_in_string + "+ROB" + std::to_string((long long)RAT_R[reg2_num]);
										}
										qRow.code_cnt = code_cnt;					//Updater code_cnt of QROW
										//LSQ.push_back(qRow);						//Add the row to LSQ
										int lsq_num = AddLSQRow(qRow, i);

										for (int j=0; j<ROB_entries; j++)
										{
											if (ROB[j].isEmpty())				//Get empty ROB
											{
												RS_LSU[i].Dst_Tag = j;			//Update DEST_TAG for RS
												if (string_list[0] == "sd")
												{
													ROB[j].Dst = "lsq" + to_string((long long)lsq_num); //Update DEST for ROB
													ROB[j].Type = "store";		//Update TYPE for ROB
													ROB[j].Ready = false;		//Update READY flag for ROB
													ROB[j].code_cnt = code_cnt;
												}
												else if (string_list[0] == "ld")
												{
													ROB[j].Dst = reg1;			//Update DEST for ROB
													ROB[j].Type = "load";		//Update TYPE for ROB
													ROB[j].Ready = false;		//Update READY flag for ROB
													RAT_F[reg1_num] = j;		//Update RAT
													ROB[j].code_cnt = code_cnt;
												}
												ROB_cnt++;
												break;
											}
										}

										break;	
									}
								}

								break_flag = true;
							}

							if (break_flag) break;
						}
					}
				}

				if (inc_flag && !wait_for_branch)
				{cur_code_cnt++; code_cnt++;}
				
			}
		}
		else if (wait_for_branch)
		{
			if (FT.at(FT.size()-1).EX1 == clk)
			{
				wait_for_branch = false;
				if (branch_taken) {cur_code_cnt = branch_PC;}
				else {cur_code_cnt++;}
				code_cnt++;
			}
		}






		//////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////WRITE BACK////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////

		bool CBD_full = false;
		if (code_cnt > 0)
		{
			for (int i=0; i<FT.size(); i++)
			{
				if (!CBD_full && FT.at(i).WB == 0 && FT.at(i).EX1 != 0 && FT.at(i).EX1 < clk && FT.at(i).COMMIT0 == 0)
				{
					//FT.at(i).WB = clk;
					for (int j=0; j<Integer_Adder::num_RS*Integer_Adder::num_FU; j++)
					{
						string op = RS_IntAdder[j].Op;
						if (!RS_IntAdder[j].isEmpty() && RS_IntAdder[j].code_cnt == i)
						{
							if (RS_IntAdder[j].Op == "add" || RS_IntAdder[j].Op == "addi")
							{
								FT.at(i).WB = clk;
								ROB[RS_IntAdder[j].Dst_Tag].Val = RS_IntAdder[j].Vj + RS_IntAdder[j].Vk;
								ROB[RS_IntAdder[j].Dst_Tag].Ready = true;
							}
							else if (RS_IntAdder[j].Op == "sub")
							{
								FT.at(i).WB = clk;
								ROB[RS_IntAdder[j].Dst_Tag].Val = RS_IntAdder[j].Vj - RS_IntAdder[j].Vk;
								ROB[RS_IntAdder[j].Dst_Tag].Ready = true;
							}
							else if (RS_IntAdder[j].Op == "beq")
							{
								FT.at(i).WB = 0;
							}
							else if (RS_IntAdder[j].Op == "bne")
							{
								FT.at(i).WB = 0;
							}

							
							stringstream ss;
							ss << RS_IntAdder[j].Dst_Tag;
							string str = ss.str();
							string name = "ROB" + str;

							for (int k=0; k<Integer_Adder::num_RS*Integer_Adder::num_FU; k++)
							{
								if (RS_IntAdder[k].Qj == name)
								{
									RS_IntAdder[k].Qj = ""; RS_IntAdder[k].Vj = ROB[RS_IntAdder[j].Dst_Tag].Val;
								}
								if (RS_IntAdder[k].Qk == name)
								{
									RS_IntAdder[k].Qk = ""; RS_IntAdder[k].Vk = ROB[RS_IntAdder[j].Dst_Tag].Val;
								}
							}
							for (int k=0; k<FP_Adder::num_RS*FP_Adder::num_FU; k++)
							{
								if (RS_FPAdder[k].Qj == name)
								{
									RS_FPAdder[k].Qj = ""; RS_FPAdder[k].Vj = ROB[RS_IntAdder[j].Dst_Tag].Val;
								}
								if (RS_FPAdder[k].Qk == name)
								{
									RS_FPAdder[k].Qk = ""; RS_FPAdder[k].Vk = ROB[RS_IntAdder[j].Dst_Tag].Val;
								}
							}
							for (int k=0; k<FP_Multiplier::num_RS*FP_Multiplier::num_FU; k++)
							{
								if (RS_FPMultiplier[k].Qj == name)
								{
									RS_FPMultiplier[k].Qj = ""; RS_FPMultiplier[k].Vj = ROB[RS_IntAdder[j].Dst_Tag].Val;
								}
								if (RS_FPMultiplier[k].Qk == name)
								{
									RS_FPMultiplier[k].Qk = ""; RS_FPMultiplier[k].Vk = ROB[RS_IntAdder[j].Dst_Tag].Val;
								}
							}
							for (int k=0; k<LS_Unit::num_RS*LS_Unit::num_FU; k++)
							{
								if (RS_LSU[k].Qj == name)
								{
									RS_LSU[k].Qj = ""; RS_LSU[k].Vj = ROB[RS_IntAdder[j].Dst_Tag].Val;
								}
								if (RS_LSU[k].Qk == name)
								{
									RS_LSU[k].Qk = ""; RS_LSU[k].Vk = ROB[RS_IntAdder[j].Dst_Tag].Val;
								}
							}

							for (int k=0; k<LSQ.size(); k++)
							{
								if (LSQ.at(k).code_cnt != -1)
								{
									if (LSQ.at(k).address.find(name) != string::npos)
									{
										//int add = atoi(LSQ.at(k).address.substr(0, LSQ.at(k).address.find('+')).c_str()) + ROB[RS_IntAdder[j].Dst_Tag].Val;
										ostringstream convert; 
										convert << ROB[RS_IntAdder[j].Dst_Tag].Val;
										string val = LSQ.at(k).address.substr(0, LSQ.at(k).address.find('+')) + "+" + convert.str();
										//stringstream ss1;
										//ss1 << add;
										//string str_val = ss1.str();
										//LSQ.at(k).address = str_val;
										LSQ.at(k).address = val;
									}

									if (LSQ.at(k).val == name)
									{
										int add = ROB[RS_IntAdder[j].Dst_Tag].Val;
										stringstream ss1;
										ss1 << add;
										string str_val = ss1.str();
										LSQ.at(k).val = str_val;
									}
								}
							}

							RS_IntAdder[j].clear();
							int_adder_RS_cnt--;
							CBD_full = true;
							break;
						}
					}

					for (int j=0; j<FP_Adder::num_RS*FP_Adder::num_FU; j++)
					{
						if (!RS_FPAdder[j].isEmpty() && RS_FPAdder[j].code_cnt == i)
						{
							if (RS_FPAdder[j].Op == "add.d")
							{
								ROB[RS_FPAdder[j].Dst_Tag].Val = RS_FPAdder[j].Vj + RS_FPAdder[j].Vk;
								ROB[RS_FPAdder[j].Dst_Tag].Ready = true;
							}
							else if (RS_FPAdder[j].Op == "sub.d")
							{
								ROB[RS_FPAdder[j].Dst_Tag].Val = RS_FPAdder[j].Vj - RS_FPAdder[j].Vk;
								ROB[RS_FPAdder[j].Dst_Tag].Ready = true;
							}
							
							FT.at(i).WB = clk;
							stringstream ss;
							ss << RS_FPAdder[j].Dst_Tag;
							string str = ss.str();
							string name = "ROB" + str;
							for (int k=0; k<Integer_Adder::num_RS*Integer_Adder::num_FU; k++)
							{
								if (RS_IntAdder[k].Qj == name)
								{
									RS_IntAdder[k].Qj = ""; RS_IntAdder[k].Vj = ROB[RS_FPAdder[j].Dst_Tag].Val;
								}
								if (RS_IntAdder[k].Qk == name)
								{
									RS_IntAdder[k].Qk = ""; RS_IntAdder[k].Vk = ROB[RS_FPAdder[j].Dst_Tag].Val;
								}
							}
							for (int k=0; k<FP_Adder::num_RS*FP_Adder::num_FU; k++)
							{
								if (RS_FPAdder[k].Qj == name)
								{
									RS_FPAdder[k].Qj = ""; RS_FPAdder[k].Vj = ROB[RS_FPAdder[j].Dst_Tag].Val;
								}
								if (RS_FPAdder[k].Qk == name)
								{
									RS_FPAdder[k].Qk = ""; RS_FPAdder[k].Vk = ROB[RS_FPAdder[j].Dst_Tag].Val;
								}
							}
							for (int k=0; k<FP_Multiplier::num_RS*FP_Multiplier::num_FU; k++)
							{
								if (RS_FPMultiplier[k].Qj == name)
								{
									RS_FPMultiplier[k].Qj = ""; RS_FPMultiplier[k].Vj = ROB[RS_FPAdder[j].Dst_Tag].Val;
								}
								if (RS_FPMultiplier[k].Qk == name)
								{
									RS_FPMultiplier[k].Qk = ""; RS_FPMultiplier[k].Vk = ROB[RS_FPAdder[j].Dst_Tag].Val;
								}
							}
							for (int k=0; k<LS_Unit::num_RS*LS_Unit::num_FU; k++)
							{
								if (RS_LSU[k].Qj == name)
								{
									RS_LSU[k].Qj = ""; RS_LSU[k].Vj = ROB[RS_FPAdder[j].Dst_Tag].Val;
								}
								if (RS_LSU[k].Qk == name)
								{
									RS_LSU[k].Qk = ""; RS_LSU[k].Vk = ROB[RS_FPAdder[j].Dst_Tag].Val;
								}
							}

							for (int k=0; k<LSQ.size(); k++)
							{
								if (LSQ.at(k).code_cnt != -1)
								{
									if (LSQ.at(k).address.find(name) != string::npos)
									{
										//int add = atoi(LSQ.at(k).address.substr(0, LSQ.at(k).address.find('+')).c_str()) + ROB[RS_FPAdder[j].Dst_Tag].Val;
										ostringstream convert; 
										convert << ROB[RS_FPAdder[j].Dst_Tag].Val;
										string val = LSQ.at(k).address.substr(0, LSQ.at(k).address.find('+')) + "+" + convert.str();
										//stringstream ss1;
										//ss1 << add;
										//string str_val = ss1.str();
										//LSQ.at(k).address = str_val;
										LSQ.at(k).address = val;
									}

									if (LSQ.at(k).val == name)
									{
										int add = ROB[RS_FPAdder[j].Dst_Tag].Val;
										stringstream ss1;
										ss1 << add;
										string str_val = ss1.str();
										LSQ.at(k).val = str_val;
									}
								}
							}

							RS_FPAdder[j].clear();
							fp_adder_RS_cnt--;
							CBD_full = true;
							break;
						}
					}

					for (int j=0; j<FP_Multiplier::num_RS*FP_Multiplier::num_FU; j++)
					{
						if (!RS_FPMultiplier[j].isEmpty() && RS_FPMultiplier[j].code_cnt == i)
						{
							if (RS_FPMultiplier[j].Op == "mult.d")
							{
								ROB[RS_FPMultiplier[j].Dst_Tag].Val = RS_FPMultiplier[j].Vj * RS_FPMultiplier[j].Vk;
								ROB[RS_FPMultiplier[j].Dst_Tag].Ready = true;
							}
							else if (RS_FPMultiplier[j].Op == "div.d")
							{
								ROB[RS_FPMultiplier[j].Dst_Tag].Val = RS_FPMultiplier[j].Vj / RS_FPMultiplier[j].Vk;
								ROB[RS_FPMultiplier[j].Dst_Tag].Ready = true;
							}

							
							FT.at(i).WB = clk;
							stringstream ss;
							ss << RS_FPMultiplier[j].Dst_Tag;
							string str = ss.str();
							string name = "ROB" + str;
							for (int k=0; k<Integer_Adder::num_RS*Integer_Adder::num_FU; k++)
							{
								if (RS_IntAdder[k].Qj == name)
								{
									RS_IntAdder[k].Qj = ""; RS_IntAdder[k].Vj = ROB[RS_FPMultiplier[j].Dst_Tag].Val;
								}
								if (RS_IntAdder[k].Qk == name)
								{
									RS_IntAdder[k].Qk = ""; RS_IntAdder[k].Vk = ROB[RS_FPMultiplier[j].Dst_Tag].Val;
								}
							}
							for (int k=0; k<FP_Adder::num_RS*FP_Adder::num_FU; k++)
							{
								if (RS_FPAdder[k].Qj == name)
								{
									RS_FPAdder[k].Qj = ""; RS_FPAdder[k].Vj = ROB[RS_FPMultiplier[j].Dst_Tag].Val;
								}
								if (RS_FPAdder[k].Qk == name)
								{
									RS_FPAdder[k].Qk = ""; RS_FPAdder[k].Vk = ROB[RS_FPMultiplier[j].Dst_Tag].Val;
								}
							}
							for (int k=0; k<FP_Multiplier::num_RS*FP_Multiplier::num_FU; k++)
							{
								if (RS_FPMultiplier[k].Qj == name)
								{
									RS_FPMultiplier[k].Qj = ""; RS_FPMultiplier[k].Vj = ROB[RS_FPMultiplier[j].Dst_Tag].Val;
								}
								if (RS_FPMultiplier[k].Qk == name)
								{
									RS_FPMultiplier[k].Qk = ""; RS_FPMultiplier[k].Vk = ROB[RS_FPMultiplier[j].Dst_Tag].Val;
								}
							}
							for (int k=0; k<LS_Unit::num_RS*LS_Unit::num_FU; k++)
							{
								if (RS_LSU[k].Qj == name)
								{
									RS_LSU[k].Qj = ""; RS_LSU[k].Vj = ROB[RS_FPMultiplier[j].Dst_Tag].Val;
								}
								if (RS_LSU[k].Qk == name)
								{
									RS_LSU[k].Qk = ""; RS_LSU[k].Vk = ROB[RS_FPMultiplier[j].Dst_Tag].Val;
								}
							}

							for (int k=0; k<LSQ.size(); k++)
							{
								if (LSQ.at(k).code_cnt != -1)
								{
									if (LSQ.at(k).address.find(name) != string::npos)
									{
										//int add = atoi(LSQ.at(k).address.substr(0, LSQ.at(k).address.find('+')).c_str()) + ROB[RS_FPMultiplier[j].Dst_Tag].Val;
										ostringstream convert; 
										convert << ROB[RS_FPMultiplier[j].Dst_Tag].Val;
										string val = LSQ.at(k).address.substr(0, LSQ.at(k).address.find('+')) + "+" + convert.str();
										//stringstream ss1;
										//ss1 << add;
										//string str_val = ss1.str();
										//LSQ.at(k).address = str_val;
										LSQ.at(k).address = val;
									}

									if (LSQ.at(k).val == name)
									{
										int add = ROB[RS_FPMultiplier[j].Dst_Tag].Val;
										stringstream ss1;
										ss1 << add;
										string str_val = ss1.str();
										LSQ.at(k).val = str_val;
									}
								}
							}

							RS_FPMultiplier[j].clear();
							fp_mul_RS_cnt--;
							CBD_full = true;
							break;
						}
					}

					for (int j=0; j<LS_Unit::num_RS*LS_Unit::num_FU; j++)
					{
						if (!RS_LSU[j].isEmpty() && RS_LSU[j].code_cnt == i)
						{
							if (FT.at(i).MEM1 != 0 && FT.at(i).MEM1 < clk)
							{
								float val;
								int num;
								for (int k=0; k<LSQ.size(); k++)
								{
									if (LSQ.at(k).code_cnt == i)
									{
										num = k;
										val = atof(LSQ.at(k).val.c_str());
										break;
									}
								}
								if (RS_LSU[j].Op == "ld")
								{
									FT.at(i).WB = clk;
									ROB[RS_LSU[j].Dst_Tag].Val = val;
									ROB[RS_LSU[j].Dst_Tag].Ready = true;
									//LSQ.erase(LSQ.begin() + num);
									LSQ.at(num).clear();
									/*for (int k=0; k<ROB_entries; k++)
									{
										if (ROB[k].code_cnt == i)
											ROB[k].Dst = "";
									}*/

									stringstream ss;
									ss << RS_LSU[j].Dst_Tag;
									string str = ss.str();
									string name = "ROB" + str;
									for (int k=0; k<Integer_Adder::num_RS*Integer_Adder::num_FU; k++)
									{
										if (RS_IntAdder[k].Qj == name)
										{
											RS_IntAdder[k].Qj = ""; RS_IntAdder[k].Vj = ROB[RS_LSU[j].Dst_Tag].Val;
										}
										if (RS_IntAdder[k].Qk == name)
										{
											RS_IntAdder[k].Qk = ""; RS_IntAdder[k].Vk = ROB[RS_LSU[j].Dst_Tag].Val;
										}
									}
									for (int k=0; k<FP_Adder::num_RS*FP_Adder::num_FU; k++)
									{
										if (RS_FPAdder[k].Qj == name)
										{
											RS_FPAdder[k].Qj = ""; RS_FPAdder[k].Vj = ROB[RS_LSU[j].Dst_Tag].Val;
										}
										if (RS_FPAdder[k].Qk == name)
										{
											RS_FPAdder[k].Qk = ""; RS_FPAdder[k].Vk = ROB[RS_LSU[j].Dst_Tag].Val;
										}
									}
									for (int k=0; k<FP_Multiplier::num_RS*FP_Multiplier::num_FU; k++)
									{
										if (RS_FPMultiplier[k].Qj == name)
										{
											RS_FPMultiplier[k].Qj = ""; RS_FPMultiplier[k].Vj = ROB[RS_LSU[j].Dst_Tag].Val;
										}
										if (RS_FPMultiplier[k].Qk == name)
										{
											RS_FPMultiplier[k].Qk = ""; RS_FPMultiplier[k].Vk = ROB[RS_LSU[j].Dst_Tag].Val;
										}
									}
									for (int k=0; k<LS_Unit::num_RS*LS_Unit::num_FU; k++)
									{
										if (RS_LSU[k].Qj == name)
										{
											RS_LSU[k].Qj = ""; RS_LSU[k].Vj = ROB[RS_LSU[j].Dst_Tag].Val;
										}
										if (RS_LSU[k].Qk == name)
										{
											RS_LSU[k].Qk = ""; RS_LSU[k].Vk = ROB[RS_LSU[j].Dst_Tag].Val;
										}
									}

									for (int k=0; k<LSQ.size(); k++)
									{
										if (LSQ.at(k).code_cnt != -1)
										{
											if (LSQ.at(k).address.find(name) != string::npos)
											{
												//int add = atoi(LSQ.at(k).address.substr(0, LSQ.at(k).address.find('+')).c_str()) + ROB[RS_LSU[j].Dst_Tag].Val;
												ostringstream convert; 
												convert << ROB[RS_LSU[j].Dst_Tag].Val;
												string val = LSQ.at(k).address.substr(0, LSQ.at(k).address.find('+')) + "+" + convert.str();
												//stringstream ss1;
												//ss1 << add;
												//string str_val = ss1.str();
												//LSQ.at(k).address = str_val;
												LSQ.at(k).address = val;
											}

											if (LSQ.at(k).val == name)
											{
												int add = ROB[RS_LSU[j].Dst_Tag].Val;
												stringstream ss1;
												ss1 << add;
												string str_val = ss1.str();
												LSQ.at(k).val = str_val;
											}
										}
									}

									RS_LSU[j].clear();
									ls_RS_cnt--;
									CBD_full = true;
									break;
								}
								else if (RS_LSU[j].Op == "sd")
									FT.at(i).WB = 0;
								
							}
							else
								FT.at(i).WB = 0;
						}
					}
				}
			}
		}
	}
	
	last_print = true;
	print_screen(RS_IntAdder, RS_FPAdder, RS_FPMultiplier, RS_LSU, ROB);
	return 0;
}


void print_screen(ReservationStation* RS_IntAdder, ReservationStation* RS_FPAdder, ReservationStation* RS_FPMultiplier, ReservationStation* RS_LSU, ReOrderBuffer* ROB)
{
	system("cls");
	vector<vector<string>> whole_code;						//Entire code as a 2D array
	for (int j=0; j<FT.size(); j++)
	{
		vector<string> string_list = BreakLine(code.at(j));		//This will contain the line in parts
		whole_code.push_back(string_list);
	}

	int max_len[4] = {0, 0, 0, 0};						//Maximum lengths of the 4 words in each line
	vector<int> char_cnt, ls_size_cnt;
	for (int j=0; j<whole_code.size(); j++)
	{
		for (int i=0; i<4; i++)
		{
			if (whole_code.at(j).size() >= i+1)
			{
				
				if (i == 2 && whole_code.at(j).size() == 3)
				{
					ls_size_cnt.push_back(whole_code.at(j).at(i).size());
				}
				if (whole_code.at(j).at(i).size() > max_len[i])
				{
					if (i == 2 && whole_code.at(j).size() == 3)
						{}
					else
						max_len[i] = whole_code.at(j).at(i).size();
				}
			}
		}
	}



	int maxLSlength = 0;
	for (int i=0; i<ls_size_cnt.size(); i++)
	{
		if (ls_size_cnt.at(i) > maxLSlength)
			maxLSlength = ls_size_cnt.at(i);
	}

	int maxCodeLength = 0;
	if (max_len[2]+max_len[3]+2 > maxLSlength)
		maxCodeLength+=6+max_len[0]+max_len[1]+max_len[2]+max_len[3];
	else
		maxCodeLength+=4+max_len[0]+max_len[1]+maxLSlength;

	

	cout<<" Timing Table:"<<endl;
	for (int i=0; i<maxCodeLength; i++)
		cout<<" ";
	cout<<" ______________________________________________"<<endl;
	for (int i=0; i<maxCodeLength; i++)
		cout<<" ";
	cout<<"| ISSUE |    EX    |   MEM   |  WB  |  COMMIT  |"<<endl;
	for (int i=0; i<maxCodeLength; i++)
		cout<<" ";
	cout<<"|-------|----------|---------|------|----------|"<<endl;
	for (int i=0; i<FT.size(); i++)
	{
		cout<<whole_code.at(i).at(0);
		for (int j=whole_code.at(i).at(0).size(); j<=max_len[0]; j++) {cout<<" ";}
		for (int j=whole_code.at(i).at(1).size(); j<max_len[1]; j++) {cout<<" ";}
		cout<<whole_code.at(i).at(1);
		cout<<", ";
		if (whole_code.at(i).size() > 3)
		{
			for (int j=whole_code.at(i).at(2).size(); j<max_len[2]; j++) {cout<<" ";}
			cout<<whole_code.at(i).at(2);
			cout<<", ";
			for (int j=whole_code.at(i).at(3).size(); j<max_len[3]; j++) {cout<<" ";}
			cout<<whole_code.at(i).at(3);
			for (int j=max_len[2]+max_len[3]+2; j<maxLSlength; j++) {cout<<" ";}
		}
		else if (whole_code.at(i).size() == 3)
		{
			int temp = maxLSlength;
			if (max_len[2]+max_len[3]+2 > maxLSlength)
				temp = max_len[2]+max_len[3]+2;
			for (int j=whole_code.at(i).at(2).size(); j<temp; j++)
				cout<<" ";
			cout<<whole_code.at(i).at(2);
		}
		
		cout<<" | ";
		FT.at(i).print(i);
	}
	for (int i=0; i<maxCodeLength; i++)
		cout<<" ";
	cout<<"|_______|__________|_________|______|__________|"<<endl;



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	int maxIntPartVj = 0;
	int maxFractPartVj = 0;
	int maxIntPartVk = 0;
	int maxFractPartVk = 0;
	int maxVjCol = 0;
	
	int totalRScnt = int_adder_RS_cnt + fp_adder_RS_cnt + fp_mul_RS_cnt;// + ls_RS_cnt;
	if (totalRScnt != 0)
	{
		for (int i=0; i<Integer_Adder::num_RS*Integer_Adder::num_FU; i++) 
		{
			if (!RS_IntAdder[i].isEmpty())
			{
				stringstream ss (stringstream::in | stringstream::out);
				ss << RS_IntAdder[i].Vj;
				if (ss.str().find('.') != string::npos)
				{
					if (ss.str().find('.') > maxIntPartVj)
						maxIntPartVj = ss.str().find('.');
					if (ss.str().size()-ss.str().find('.')-1 > maxFractPartVj)
						maxFractPartVj = ss.str().size()-ss.str().find('.')-1;
				}
				else
				{
					if (ss.str().size() > maxIntPartVj)
						maxIntPartVj = ss.str().size();
				}

				stringstream ss1 (stringstream::in | stringstream::out);
				ss1 << RS_IntAdder[i].Vk;
				if (ss1.str().find('.') != string::npos)
				{
					if (ss1.str().find('.') > maxIntPartVk)
						maxIntPartVk = ss1.str().find('.');
					if (ss1.str().size()-ss1.str().find('.')-1 > maxFractPartVk)
						maxFractPartVk = ss1.str().size()-ss1.str().find('.')-1;
				}
				else
				{
					if (ss1.str().size() > maxIntPartVk)
						maxIntPartVk = ss1.str().size();
				}
			}
		}

		for (int i=0; i<FP_Adder::num_RS*FP_Adder::num_FU; i++)
		{
			if (!RS_FPAdder[i].isEmpty())
			{
				stringstream ss (stringstream::in | stringstream::out);
				ss << RS_FPAdder[i].Vj;
				if (ss.str().find('.') != string::npos)
				{
					if (ss.str().find('.') > maxIntPartVj)
						maxIntPartVj = ss.str().find('.');
					if (ss.str().size()-ss.str().find('.')-1 > maxFractPartVj)
						maxFractPartVj = ss.str().size()-ss.str().find('.')-1;
				}
				else
				{
					if (ss.str().size() > maxIntPartVj)
						maxIntPartVj = ss.str().size();
				}

				stringstream ss1 (stringstream::in | stringstream::out);
				ss1 << RS_FPAdder[i].Vk;
				if (ss1.str().find('.') != string::npos)
				{
					if (ss1.str().find('.') > maxIntPartVk)
						maxIntPartVk = ss1.str().find('.');
					if (ss1.str().size()-ss1.str().find('.')-1 > maxFractPartVk)
						maxFractPartVk = ss1.str().size()-ss1.str().find('.')-1;
				}
				else
				{
					if (ss1.str().size() > maxIntPartVk)
						maxIntPartVk = ss1.str().size();
				}
			}
		}

		for (int i=0; i<FP_Multiplier::num_RS*FP_Multiplier::num_FU; i++)
		{
			if (!RS_FPMultiplier[i].isEmpty())
			{
				stringstream ss (stringstream::in | stringstream::out);
				ss << RS_FPMultiplier[i].Vj;
				if (ss.str().find('.') != string::npos)
				{
					if (ss.str().find('.') > maxIntPartVj)
						maxIntPartVj = ss.str().find('.');
					if (ss.str().size()-ss.str().find('.')-1 > maxFractPartVj)
						maxFractPartVj = ss.str().size()-ss.str().find('.')-1;
				}
				else
				{
					if (ss.str().size() > maxIntPartVj)
						maxIntPartVj = ss.str().size();
				}

				stringstream ss1 (stringstream::in | stringstream::out);
				ss1 << RS_FPMultiplier[i].Vk;
				if (ss1.str().find('.') != string::npos)
				{
					if (ss1.str().find('.') > maxIntPartVk)
						maxIntPartVk = ss1.str().find('.');
					if (ss1.str().size()-ss1.str().find('.')-1 > maxFractPartVk)
						maxFractPartVk = ss1.str().size()-ss1.str().find('.')-1;
				}
				else
				{
					if (ss1.str().size() > maxIntPartVk)
						maxIntPartVk = ss1.str().size();
				}
			}
		}

		/*for (int i=0; i<LS_Unit::num_RS*LS_Unit::num_FU; i++)
		{
			if (!RS_LSU[i].isEmpty())
			{
				stringstream ss (stringstream::in | stringstream::out);
				ss << RS_LSU[i].Vj;
				if (ss.str().find('.') != string::npos)
				{
					if (ss.str().find('.') > maxIntPartVj)
						maxIntPartVj = ss.str().find('.');
					if (ss.str().size()-ss.str().find('.')-1 > maxFractPartVj)
						maxFractPartVj = ss.str().size()-ss.str().find('.')-1;
				}
				else
				{
					if (ss.str().size() > maxIntPartVj)
						maxIntPartVj = ss.str().size();
				}

				stringstream ss1 (stringstream::in | stringstream::out);
				ss1 << RS_LSU[i].Vk;
				if (ss1.str().find('.') != string::npos)
				{
					if (ss1.str().find('.') > maxIntPartVk)
						maxIntPartVk = ss1.str().find('.');
					if (ss1.str().size()-ss1.str().find('.')-1 > maxFractPartVk)
						maxFractPartVk = ss1.str().size()-ss1.str().find('.')-1;
				}
				else
				{
					if (ss1.str().size() > maxIntPartVk)
						maxIntPartVk = ss1.str().size();
				}
			}
		}*/
	}


	if (maxIntPartVj < maxIntPartVk)
		maxIntPartVj = maxIntPartVk;
	if (maxFractPartVj < maxFractPartVk)
		maxFractPartVj = maxFractPartVk;
	maxVjCol = maxIntPartVj;
	if (maxFractPartVj > 0)
		maxVjCol+= maxFractPartVj+1;
	if (maxVjCol < 2)
		maxVjCol = 2;




	if (totalRScnt != 0)
	{
		int j = 0;
		cout << endl << endl << " Reservation Stations:" << endl;
		cout<<"         _______________________________________";
		for (int i=0; i<2*maxVjCol+5; i++) cout<<"_";
		cout<<endl;
		cout<<"        |   Op   | Dst_Tag |   Qj    |   Qk    | ";

		for (int i=0; i<(maxVjCol-2)/2; i++) cout<<" ";
		cout<<"Vj";
		for (int i=0; i<(maxVjCol-2)/2; i++) cout<<" ";
		if ((maxVjCol-2)%2 != 0)
			cout<<" ";
		cout<<" | ";

		for (int i=0; i<(maxVjCol-2)/2; i++) cout<<" ";
		cout<<"Vk";
		for (int i=0; i<(maxVjCol-2)/2; i++) cout<<" ";
		if ((maxVjCol-2)%2 != 0)
			cout<<" ";
		cout<<" |"<<endl;

		cout<<"        |--------|---------|---------|---------|";
		for (int i=0; i<maxVjCol+2; i++) cout<<"-";
		cout<<"|";
		for (int i=0; i<maxVjCol+2; i++) cout<<"-";
		cout<<"|"<<endl;

		int num_RS = 0;
		int num2_RS = 0;
		
		for (int i=0; i<Integer_Adder::num_RS*Integer_Adder::num_FU; i++) 
		{
			if (!RS_IntAdder[i].isEmpty())
			{
				num2_RS++;
				num_RS++;
				cout << "RS_IA" << j; 
				if (j < 10)
					cout<<" ";
				cout<< " | "; RS_IntAdder[i].print(maxVjCol, maxIntPartVj, RS_IntAdder[i].code_cnt);
			}
			j++;
		}

		for (int i=0; i<FP_Adder::num_RS*FP_Adder::num_FU; i++)
		{
			if (!RS_FPAdder[i].isEmpty() && num2_RS > 0)
			{
				num2_RS = 0;
				cout<<"--------|--------|---------|---------|---------|";
				for (int i=0; i<maxVjCol+2; i++) cout<<"-";
				cout<<"|";
				for (int i=0; i<maxVjCol+2; i++) cout<<"-";
				cout<<"|----"<<endl;
				break;
			}
		}



		j=0; num_RS = 0;
		for (int i=0; i<FP_Adder::num_RS*FP_Adder::num_FU; i++)
		{
			if (!RS_FPAdder[i].isEmpty())
			{
				num2_RS++;
				num_RS++;
				cout << "RS_FA" << j;
				if (j < 10)
					cout<<" ";
				cout << " | "; RS_FPAdder[i].print(maxVjCol, maxIntPartVj, RS_FPAdder[i].code_cnt);
			}
			j++;
		}
		


		for (int i=0; i<FP_Multiplier::num_RS*FP_Multiplier::num_FU; i++)
		{
			if (!RS_FPMultiplier[i].isEmpty() && num2_RS > 0)
			{
				num2_RS = 0;
				cout<<"--------|--------|---------|---------|---------|";
				for (int i=0; i<maxVjCol+2; i++) cout<<"-";
				cout<<"|";
				for (int i=0; i<maxVjCol+2; i++) cout<<"-";
				cout<<"|----"<<endl;
				break;
			}
		}



		j=0; num_RS = 0;
		for (int i=0; i<FP_Multiplier::num_RS*FP_Multiplier::num_FU; i++)
		{
			if (!RS_FPMultiplier[i].isEmpty())
			{
				num2_RS++;
				num_RS++;
				cout << "RS_FM" << j;
				if (j < 10)
					cout<<" ";
				cout << " | "; RS_FPMultiplier[i].print(maxVjCol, maxIntPartVj, RS_FPMultiplier[i].code_cnt);
			}
			j++;
		}



		/*for (int i=0; i<LS_Unit::num_RS*LS_Unit::num_FU; i++)
		{
			if (!RS_LSU[i].isEmpty() && num2_RS > 0)
			{
				num2_RS = 0;
				cout<<"--------|--------|---------|---------|---------|";
				for (int i=0; i<maxVjCol+2; i++) cout<<"-";
				cout<<"|";
				for (int i=0; i<maxVjCol+2; i++) cout<<"-";
				cout<<"|----"<<endl;
				break;
			}
		}



		j=0; num_RS = 0;
		for (int i=0; i<LS_Unit::num_RS*LS_Unit::num_FU; i++)
		{
			if (!RS_LSU[i].isEmpty())
			{
				num2_RS++;
				num_RS++;
				cout << "RS_LS" << j;
				if (j < 10)
					cout<<" ";
				cout << " | "; RS_LSU[i].print(maxVjCol, maxIntPartVj, RS_LSU[i].code_cnt);
			}
			j++;
		}*/

		cout<<"        |________|_________|_________|_________|_";
		for (int i=0; i<maxVjCol; i++) cout<<"_";
		cout<<"_|_";
		for (int i=0; i<maxVjCol; i++) cout<<"_";
		cout<<"_|" << endl;
	}




	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	int maxIntPart = 0;
	int maxFractPart = 0;
	int maxROBpart = 0;
	int maxAddSize = 0;
	int diff = 0;
	int maxThirdCol = 0;

	for (int i=0; i<LSQ.size(); i++)
	{
		if (LSQ.at(i).code_cnt != -1)
		{
			if (LSQ.at(i).address.size() > maxAddSize)
				maxAddSize = LSQ.at(i).address.size();
			if (LSQ.at(i).val.find('R') == string::npos)
			{
				if (LSQ.at(i).val.find('.') != string::npos)
				{
					if (LSQ.at(i).val.find('.') > maxIntPart)
						maxIntPart = LSQ.at(i).val.size();
					if (LSQ.at(i).val.size()-LSQ.at(i).val.find('.')-1 > maxFractPart)
						maxFractPart = LSQ.at(i).val.size()-LSQ.at(i).val.find('.')-1;
				}
				else
				{
					if (LSQ.at(i).val.size() > maxIntPart)
						maxIntPart = LSQ.at(i).val.size();
				}
			}
			else if (LSQ.at(i).val.find('R') != string::npos && LSQ.at(i).val.size() > maxROBpart)
			{ maxROBpart = LSQ.at(i).val.size();}
		}
	}

	diff = 0;
	maxThirdCol = maxIntPart;
	if (maxFractPart > 0)
		maxThirdCol+= maxFractPart+1;
	if (maxROBpart > maxThirdCol)
		maxThirdCol = maxROBpart;
	if (maxThirdCol < 5)
	{
		diff = (5 - maxThirdCol)/2;
		maxThirdCol = 5; 
	}
	if (maxAddSize < 7)
		maxAddSize = 7;


	bool first = true;
	for (int j=0; j<LSQ.size(); j++)
	{
		if (LSQ.at(j).code_cnt != -1)
		{
			int curIntPart, curFractPart, curAddSize;
			curAddSize = LSQ.at(j).address.size();
			if (LSQ.at(j).val.find('R') == string::npos)
			{
				stringstream ss (stringstream::in | stringstream::out);
				ss << LSQ.at(j).val;
				if (ss.str().find('.') != string::npos)
				{
					curIntPart = ss.str().find('.');
					curFractPart = ss.str().size()-ss.str().find('.')-1;
				}
				else
				{
					curIntPart = ss.str().size();
					curFractPart = -1;
				}
			}
			else if (LSQ.at(j).val.find('R') != string::npos)
			{
				curIntPart = maxIntPart;
				curFractPart = 0;
			}


			if (first)
			{
				cout<<endl<<endl<<" Load/Store Queue"<<endl;
				cout<<"\t ";
				for (int i=0; i<maxThirdCol+maxAddSize+16; i++) cout<<"_";
				cout<<endl;
				cout<<"\t| Num | Op | ";
				for (int i=0; i<(maxAddSize-7)/2; i++) cout<<" ";
				cout<<"Address";
				for (int i=0; i<(maxAddSize-6)/2; i++) cout<<" ";
				cout<<" | ";
				for (int i=0; i<(maxThirdCol-5)/2; i++) cout<<" ";
				cout<<"Value";
				for (int i=0; i<(maxThirdCol-4)/2; i++) cout<<" ";
				cout<< " |"<<endl;
				cout<<"\t|-----|----|---------|";
				for (int i=0; i<maxThirdCol+2; i++) cout<<"-";
				cout<<"|"<<endl;
				first = false;
			}
			cout<<"\t| ";
			if (j < 100) cout<<" ";
			if (j < 10) cout<<" ";
			cout<<j<<" | ";
			cout<<LSQ.at(j).op<<"  | ";

			if (LSQ.at(j).address.find('+') == string::npos)
			{
				for (int i=0; i<((maxAddSize-curAddSize)/2); i++) cout<<" ";
				if (atoi(LSQ.at(j).address.c_str()) < 100) cout<<" ";
				if (atoi(LSQ.at(j).address.c_str()) < 10) cout<<" ";
				cout<<LSQ.at(j).address;
				for (int i=0; i<(maxAddSize-curAddSize)/2; i++) cout<<" ";
				cout<<" | ";
			}
			else
			{
				cout<<LSQ.at(j).address;
				for (int i=0; i<maxAddSize-curAddSize; i++) cout<<" ";
				cout<<" | ";
			}

			for (int j=0; j<maxIntPart-curIntPart+diff; j++)
				cout<<" ";
			cout << LSQ.at(j).val;
			if (LSQ.at(j).val.find('R') == string::npos)
				for (int k=0; k<maxThirdCol-maxIntPart-curFractPart-1-diff; k++)
					cout<<" ";
			else if (LSQ.at(j).val.find('R') != string::npos)
				for (int k=0; k<maxThirdCol-LSQ.at(j).val.size()-diff; k++)
					cout<<" ";
			cout << " | " << LSQ.at(j).code_cnt << endl;
		}
	}
	if (!first)
	{	
		cout<<"\t|_____|____|_";
		for (int i=0; i<maxAddSize; i++) cout<<"_";
		cout<<"_|_";
		for (int i=0; i<maxThirdCol ; i++) cout<<"_";
		cout<<"_|"<<endl;
	}





	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	maxIntPart = 0;
	maxFractPart = 0;

	for (int i=0; i<ROB_entries; i++)
	{
		if (!ROB[i].isEmpty())
		{
			stringstream ss (stringstream::in | stringstream::out);
			ss << ROB[i].Val;
			if (ss.str().find('.') != string::npos)
			{
				if (ss.str().find('.') > maxIntPart)
					maxIntPart = ss.str().size();
				if (ss.str().size()-ss.str().find('.')-1 > maxFractPart)
					maxFractPart = ss.str().size()-ss.str().find('.')-1;
			}
			else
			{
				if (ss.str().size() > maxIntPart)
					maxIntPart = ss.str().size();
			}
		}
	}
	maxThirdCol = maxIntPart;
	if (maxFractPart > 0)
		maxThirdCol+= maxFractPart+1;
	if (maxThirdCol < 6)
		maxThirdCol = 6;



	first = true;
	for (int i=0; i<ROB_entries; i++)
	{
		if (!ROB[i].isEmpty())
		{
			stringstream ss (stringstream::in | stringstream::out);
			ss << ROB[i].Val;
			int curIntPart, curFractPart;
			if (ss.str().find('.') != string::npos)
			{
				curIntPart = ss.str().find('.');
				curFractPart = ss.str().size()-ss.str().find('.')-1;
			}
			else
			{
				curIntPart = ss.str().size();
				curFractPart = -1;
			}

			if (first)
			{
				cout << endl << endl << " Re-Order Buffer:" << endl;
				cout<<"        __________________";
				for (int i=0; i<maxThirdCol+2; i++) cout<<"_";
				cout<<"________"<<endl;
				cout<<"       |  Type   | Dest  | ";
				for (int i=0; i<(maxThirdCol-6)/2; i++) cout<<" ";
				cout<<"Result";
				for (int i=0; i<(maxThirdCol-6)/2; i++) cout<<" ";
				if ((maxThirdCol-6)%2 != 0)
					cout<<" ";
				cout<<" | Ready |"<<endl;
				cout<<"       |---------|-------|";
				for (int i=0; i<maxThirdCol+2; i++) cout<<"-";
				cout<<"|-------|"<<endl;
				first = false;
			}
			ROB[i].print(i, maxThirdCol, maxIntPart, curIntPart, curFractPart, ROB[i].code_cnt);
		}
	}
	if (!first)
	{
		cout<<"       |_________|_______|";
		for (int i=0; i<maxThirdCol+2; i++) cout<<"_";
		cout<<"|_______|";
	}
	cout<<endl<<endl<<endl;





	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





	maxIntPart = 0;
	maxFractPart = 0;
	for (int i=0; i<32; i++)
	{
		if ((R[i] != 0) || RAT_R[i] != -1)
		{
			stringstream ss (stringstream::in | stringstream::out);
			ss << R[i];
			if (ss.str().size() > maxIntPart)
				maxIntPart = ss.str().size();
		}
	}


	for (int i=0; i<32; i++)
	{
		if ((F[i] != 0) || RAT_F[i] != -1)
		{
			stringstream ss (stringstream::in | stringstream::out);
			ss << F[i];
			if (ss.str().find('.') != string::npos)
			{
				if (ss.str().find('.') > maxIntPart)
					maxIntPart = ss.str().size();
				if (ss.str().size()-ss.str().find('.')-1 > maxFractPart)
					maxFractPart = ss.str().size()-ss.str().find('.')-1;
			}
			else
			{
				if (ss.str().size() > maxIntPart)
					maxIntPart = ss.str().size();
			}
		}
	}
	maxThirdCol = maxIntPart;
	if (maxFractPart > 0)
		maxThirdCol+= maxFractPart+1;
	if (maxThirdCol < 3)
		maxThirdCol = 3;


	cout<<" Non-zero Registers and RAT files:"<<endl<<endl;
	cout<<"              Integer Registers:\t\tFP Registers:"<<endl;
	first = true;
	bool Rdone = false;
	bool Fdone = false;
	for (int i=0, ii=0; (i<32 || ii<32); i++, ii++)
	{
		if (((i<32) && (R[i] != 0 || RAT_R[i] != -1)) || ((ii<32) && (F[ii] != 0 || RAT_F[ii] != -1)))
		{
			if (first)
			{
				cout<<"\t _________________";
				for (int i=0; i<maxThirdCol+2; i++) cout<<"_";

				cout<<"          _________________";
				for (int i=0; i<maxThirdCol+2; i++) cout<<"_";


				cout<<endl<<"\t|  RAT  |  R[i]  | ";
				for (int i=0; i<(maxThirdCol-3)/2; i++) cout<<" ";
				cout<<"ARF";
				for (int i=0; i<(maxThirdCol-3)/2; i++) cout<<" ";
				if ((maxThirdCol-3)%2 != 0)
					cout<<" ";
				cout<<" |";

				cout<<"        |  RAT  |  F[i]  | ";
				for (int i=0; i<(maxThirdCol-3)/2; i++) cout<<" ";
				cout<<"ARF";
				for (int i=0; i<(maxThirdCol-3)/2; i++) cout<<" ";
				if ((maxThirdCol-3)%2 != 0)
					cout<<" ";
				cout<<" |";


				cout<<endl<<"\t|-------|--------|";
				for (int i=0; i<maxThirdCol+2; i++) cout<<"-";
				cout<<"|";

				cout<<"        |-------|--------|";
				for (int i=0; i<maxThirdCol+2; i++) cout<<"-";
				cout<<"|";


				cout<<endl;
				first = false;
			}

			if ((i<32 && ii<32) && ((R[i] != 0 || RAT_R[i] != -1) && (F[ii] != 0 || RAT_F[ii] != -1)))
			{
				stringstream ssR (stringstream::in | stringstream::out);
				ssR << R[i];
				int curIntPartR = ssR.str().size();

				stringstream ssF (stringstream::in | stringstream::out);
				ssF << F[ii];
				int curIntPartF, curFractPartF;

				if (ssF.str().find('.') != string::npos)
				{
					curIntPartF = ssF.str().find('.');
					curFractPartF = ssF.str().size()-ssF.str().find('.')-1;
				}
				else
				{
					curIntPartF = ssF.str().size();
					curFractPartF = -1;
				}

				
				if (RAT_R[i] != -1)
				{
					cout << "\t| ROB" << RAT_R[i];
					if (RAT_R[i] < 10) cout<<" ";
				}
				else
					cout << "\t|      ";
				cout << " |  R[";
				if (i<10) cout<<"0";
				cout<< i << "] | ";
				for (int j=0; j<maxIntPart-curIntPartR; j++)
					cout<<" ";
				cout << R[i];
				for (int j=0; j<maxThirdCol-maxIntPart; j++)
					cout<<" ";
				cout << " |" << "        ";

				if (RAT_F[ii] != -1)
				{
					cout << "| ROB" << RAT_F[ii];
					if (RAT_F[ii] < 10)
						cout<<" ";
				}
				else
					cout << "|      ";
				cout << " |  F[";
				if (ii<10)
					cout<<"0";
				cout<< ii << "] | ";
				for (int j=0; j<maxIntPart-curIntPartF; j++)
					cout<<" ";
				cout << F[ii];
				for (int j=0; j<maxThirdCol-maxIntPart-curFractPartF-1; j++)
					cout<<" ";
				cout << " |" << endl;
			}

			else if (i<32 && (R[i] != 0 || RAT_R[i] != -1))
			{
				stringstream ssR (stringstream::in | stringstream::out);
				ssR << R[i];
				int curIntPartR = ssR.str().size();

				if (RAT_R[i] != -1)
				{
					cout << "\t| ROB" << RAT_R[i];
					if (RAT_R[i] < 10) cout<<" ";
				}
				else
					cout << "\t|      ";
				cout << " |  R[";
				if (i<10) cout<<"0";
				cout<< i << "] | ";
				for (int j=0; j<maxIntPart-curIntPartR; j++)
					cout<<" ";
				cout << R[i];
				for (int j=0; j<maxThirdCol-maxIntPart; j++)
					cout<<" ";
				cout << " |" << "        ";

				while (ii < 32 && F[ii] == 0) { ii++; }
				if (ii < 32) {
					stringstream ssF (stringstream::in | stringstream::out);
					ssF << F[ii];
					int curIntPartF, curFractPartF;

					if (ssF.str().find('.') != string::npos)
					{
						curIntPartF = ssF.str().find('.');
						curFractPartF = ssF.str().size()-ssF.str().find('.')-1;
					}
					else
					{
						curIntPartF = ssF.str().size();
						curFractPartF = -1;
					}

					if (RAT_F[ii] != -1)
					{
						cout << "| ROB" << RAT_F[ii];
						if (RAT_F[ii] < 10)
							cout<<" ";
					}
					else
						cout << "|      ";
					cout << " |  F[";
					if (ii<10)
						cout<<"0";
					cout<< ii << "] | ";
					for (int j=0; j<maxIntPart-curIntPartF; j++)
						cout<<" ";
					cout << F[ii];
					for (int j=0; j<maxThirdCol-maxIntPart-curFractPartF-1; j++)
						cout<<" ";
					cout << " |" << endl;
				}

				else if (ii >= 32 && !Fdone) {
					Fdone = true;
					cout<<"|_______|________|";
					for (int i=0; i<maxThirdCol+2; i++) cout<<"_";
					cout<<"|"<<endl;
				}

				else
					cout << endl;
			}

			else if (ii<32 && (F[ii] != 0 || RAT_F[ii] != -1))
			{
				while (i < 32 && R[i] == 0) { i++; }
				if (i < 32) {
					stringstream ssR (stringstream::in | stringstream::out);
					ssR << R[i];
					int curIntPartR = ssR.str().size();

					if (RAT_R[i] != -1)
					{
						cout << "\t| ROB" << RAT_R[i];
						if (RAT_R[i] < 10) cout<<" ";
					}
					else
						cout << "\t|      ";
					cout << " |  R[";
					if (i<10) cout<<"0";
					cout<< i << "] | ";
					for (int j=0; j<maxIntPart-curIntPartR; j++)
						cout<<" ";
					cout << R[i];
					for (int j=0; j<maxThirdCol-maxIntPart; j++)
						cout<<" ";
					cout << " |        ";
				}

				else if (i >= 32 && !Rdone) {
					Rdone = true;
					cout<<"\t|_______|________|";
					for (int i=0; i<maxThirdCol+2; i++) cout<<"_";
					cout<<"|        ";
				}

				else {
					cout << "\t                   ";
					for (int j=0; j<maxThirdCol; j++)	cout<<" ";
					cout << "          ";
				}


				stringstream ssF (stringstream::in | stringstream::out);
				ssF << F[ii];
				int curIntPartF, curFractPartF;

				if (ssF.str().find('.') != string::npos)
				{
					curIntPartF = ssF.str().find('.');
					curFractPartF = ssF.str().size()-ssF.str().find('.')-1;
				}
				else
				{
					curIntPartF = ssF.str().size();
					curFractPartF = -1;
				}

				if (RAT_F[ii] != -1)
				{
					cout << "| ROB" << RAT_F[ii];
					if (RAT_F[ii] < 10)
						cout<<" ";
				}
				else
					cout << "|      ";
				cout << " |  F[";
				if (ii<10)
					cout<<"0";
				cout<< ii << "] | ";
				for (int j=0; j<maxIntPart-curIntPartF; j++)
					cout<<" ";
				cout << F[ii];
				for (int j=0; j<maxThirdCol-maxIntPart-curFractPartF-1; j++)
					cout<<" ";
				cout << " |" << endl;
			}
		}
	}

	if (!Rdone && !Fdone) {
		cout<<"\t|_______|________|";
		for (int i=0; i<maxThirdCol+2; i++) cout<<"_";
		cout<<"|";

		cout<<"        |_______|________|";
		for (int i=0; i<maxThirdCol+2; i++) cout<<"_";
		cout<<"|"<<endl;
	}

	else if (!Rdone) {
		Rdone = true;
		cout<<"\t|_______|________|";
		for (int i=0; i<maxThirdCol+2; i++) cout<<"_";
		cout<<"|"<<endl;
	}

	else if (!Fdone) {
		Fdone = true;
		cout<<"\t";
		for (int i=0; i<maxThirdCol+21; i++) cout<<" ";

		cout<<"        |_______|________|";
		for (int i=0; i<maxThirdCol+2; i++) cout<<"_";
		cout<<"|"<<endl;
	}




	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





	maxIntPart = 0;
	maxFractPart = 0;

    for (int i=0; i<256; i++)
	{
		if (Mem[i] != 0)
		{
			stringstream ss (stringstream::in | stringstream::out);
			ss << Mem[i];
			if (ss.str().find('.') != string::npos)
			{
				if (ss.str().find('.') > maxIntPart)
					maxIntPart = ss.str().size();
				if (ss.str().size()-ss.str().find('.')-1 > maxFractPart)
					maxFractPart = ss.str().size()-ss.str().find('.')-1;
			}
			else
			{
				if (ss.str().size() > maxIntPart)
					maxIntPart = ss.str().size();
			}
		}
	}

	diff = 0;
	maxThirdCol = maxIntPart;
	if (maxFractPart > 0)
		maxThirdCol+= maxFractPart+1;
	if (maxThirdCol < 5)
	{
		diff = (5 - maxThirdCol)/2;
		maxThirdCol = 5; 
	}


	first = true;
	for (int i=0; i<256; i++)
	{
		if (Mem[i] != 0.0)
		{
			stringstream ss (stringstream::in | stringstream::out);
			ss << Mem[i];
			int curIntPart, curFractPart;
			if (ss.str().find('.') != string::npos)
			{
				curIntPart = ss.str().find('.');
				curFractPart = ss.str().size()-ss.str().find('.')-1;
			}
			else
			{
				curIntPart = ss.str().size();
				curFractPart = -1;
			}

			if (first)
			{
				cout<<endl<<endl<<" Non-zero Memory Locations:"<<endl;
				cout<<"\t ___________";
				for (int i=0; i<maxThirdCol+2; i++) cout<<"_";
				cout<<endl<<"\t|  Mem[i]  | ";
				for (int i=0; i<(maxThirdCol-5)/2; i++) cout<<" ";
				cout<<"Value";
				for (int i=0; i<(maxThirdCol-5)/2; i++) cout<<" ";
				if ((maxThirdCol-5)%2 != 0)
					cout<<" ";
				cout<<" |";
				cout<<endl<<"\t|----------|";
				for (int i=0; i<maxThirdCol+2; i++) cout<<"-";
				cout<<"|"<<endl;
				first = false;
			}
			cout<<"\t| Mem[";
			if (i < 100) cout <<"0";
			if (i < 10) cout <<"0";
			cout<<i<<"] | ";
			for (int j=0; j<maxIntPart-curIntPart+diff; j++)
				cout<<" ";
			cout << Mem[i];
			for (int j=0; j<maxThirdCol-maxIntPart-curFractPart-1-diff; j++)
				cout<<" ";
			cout << " |" << endl;
		}
	}
	if (!first)
	{
		cout<<"\t|__________|";
		if (maxThirdCol < 5)
			cout<<"_______";
		else
			for (int i=0; i<maxThirdCol+2; i++) cout<<"_";
		cout<<"|"<<endl;
	}
	




	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	first = true;
	for (int i=0; i<8; i++)
	{
		if (BTB[i].cur_PC != -1)
		{
			if (first)
			{
				cout<<endl<<endl<<" Branch Target Buffer"<<endl;
				cout<<"\t ___________________________________________"<<endl;
				cout<<"\t| Current PC | Predicted PC | Branch Taken? |"<<endl;
				cout<<"\t|------------|--------------|---------------|"<<endl;
				first = false;
			}
			cout<<"\t|     ";
			if (BTB[i].cur_PC < 100) cout << " ";
			if (BTB[i].cur_PC < 10) cout << " ";
			cout << BTB[i].cur_PC<<"    |     ";
			if (BTB[i].cur_PC < 100) cout << " ";
			if (BTB[i].cur_PC < 10) cout << " ";
			cout << BTB[i].pred_PC<<"      |       ";
			if (BTB[i].taken == true) cout << "Y";
			else if (BTB[i].taken == false) cout << "N";
			cout<<"       |"<<endl;
		}
	}
	if (!first)
		cout<<"\t|____________|______________|_______________|"<<endl;
	


		
	cout<<endl<<endl<<" Total clock cycles = "<<clk<<endl<<endl;
	if (!last_print)
		cout<<"Press any key to continue (e to jump to output)...";
	else
		cout<<"Press any key to continue...";
	last_op = _getch();
	last_op = toupper(last_op);
}


void RestoreBackup(BackupRAT backup, unsigned int &code_cnt, ReservationStation* RS_IntAdder, ReservationStation* RS_FPAdder, ReservationStation* RS_FPMultiplier, ReservationStation* RS_LSU, ReOrderBuffer* ROB) {
	for (int i=0; i<32; i++)			//Restore RAT
	{
		RAT_R[i] = backup.backupR[i];
		RAT_F[i] = backup.backupF[i];
	}

										//Restore RS

	int_adder_RS_cnt = fp_adder_RS_cnt = fp_mul_RS_cnt = ls_RS_cnt = ROB_cnt = 0;

	for (int i=0; i<Integer_Adder::num_FU*Integer_Adder::num_RS; i++)
	{
		if (!RS_IntAdder[i].isEmpty() && RS_IntAdder[i].code_cnt >= backup.code_cnt)
			RS_IntAdder[i].clear();
		if (!RS_IntAdder[i].isEmpty())
			int_adder_RS_cnt++;
	}

	for (int i=0; i<FP_Adder::num_FU*FP_Adder::num_RS; i++)
	{
		if (!RS_FPAdder[i].isEmpty() && RS_FPAdder[i].code_cnt >= backup.code_cnt)
			RS_FPAdder[i].clear();
		if (!RS_FPAdder[i].isEmpty())
			fp_adder_RS_cnt++;
	}

	for (int i=0; i<FP_Multiplier::num_FU*FP_Multiplier::num_RS; i++)
	{
		if (!RS_FPMultiplier[i].isEmpty() && RS_FPMultiplier[i].code_cnt >= backup.code_cnt)
			RS_FPMultiplier[i].clear();
		if (!RS_FPMultiplier[i].isEmpty())
			fp_mul_RS_cnt++;
	}

	for (int i=0; i<LS_Unit::num_FU*LS_Unit::num_RS; i++)
	{
		if (!RS_LSU[i].isEmpty() && RS_LSU[i].code_cnt >= backup.code_cnt)
			RS_LSU[i].clear();
		if (!RS_LSU[i].isEmpty())
			ls_RS_cnt++;
	}

										//Restore LSQ
	for (int i=0; i<LSQ.size(); i++)
	{
		if (LSQ.at(i).code_cnt >= backup.code_cnt)
			LSQ.at(i).clear();
	}

										//Restore ROB
	for (int i=0; i<ROB_entries; i++)
	{
		if (!ROB[i].isEmpty() && ROB[i].code_cnt > backup.code_cnt)
			ROB[i].clear();
		if (!ROB[i].isEmpty())
			ROB_cnt++;
	}

										//Restore FT
	int temp = FT.size();
	for (int i=backup.code_cnt+1; i<temp; i++)
	{
		FT.pop_back();
		code.pop_back();
		code_cnt--;
	}
	
}