// OSProject2.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm> 

using namespace std;

struct Process 
{
	int id,cpubrust,arrivaltime,priority;
	//waitingtime = endtime - cpubrust - arrivaltime
	//turnaroundtime = endtime - arrivaltime
	int fcfswaitingtime, fcfsturnaroundtime, fcfsbegintime, fcfsendtime, fcfsruntime = 0;//FCFS (First Come First Serve)
	int rrwaitingtime, rrturnaroundtime, rrbegintime, rrendtime, rrruntime = 0; //RR(Round Robin)
	int psjfwaitingtime, psjfturnaroundtime, psjfbegintime, psjfendtime, psjfruntime = 0, psjfused = 0;//PSJF (Preemptive Shortest Job First)
	int nsjfwaitingtime, nsjfturnaroundtime, nsjfbegintime, nsjfendtime, nsjfruntime = 0;//NSJF (Non-preemptive Shortest Job First)
	int ppwaitingtime, ppturnaroundtime, ppbegintime, ppendtime, ppruntime = 0,ppused = 0;//PP (Preemptive Priority)
};

void file_reader(string filename, int & method, int & timeslice, vector<Process> & process1)
{

	ifstream fin;
	string line;
	char ch;
	filename = filename + ".txt";
	fin.open(filename);
	if (fin.is_open())
	{
		if (fin.good())
		{
			fin >> method >> timeslice;
			fin.get(ch);//enter
			getline(fin, line); // read"ID     CPU Burst  Arrival Time   Priority"
		}
		else
		{ 
			cout << "Bad file." << endl;
		}

		Process temp;
		while (fin >> temp.id >> temp.cpubrust >> temp.arrivaltime >> temp.priority)
		{
			process1.push_back(temp);
		}

		fin.close();

		cout << filename << endl;
		cout << method << "	" << timeslice << endl;
		cout << line << endl;
		for (int i = 0; i < process1.size(); i++)
		{
			cout << process1[i].id << "	"
				<< process1[i].cpubrust << "	"
				<< process1[i].arrivaltime << "	"
				<< process1[i].priority << endl;
		}
	}
	else
	{
		cout << "Unable to open file." << endl;
	}

}

char int_transform(int i)
{
	char c;

	if (i < 10)
	{
		c = i + 48;
		return c;
	}
	else if (i >= 10 && i <= 35)
	{
		c = i + 55;
		return c;
	}
	else
	{
		c = '?';
		return c;
	}
	
}

bool sort_id(Process a, Process b)
{
	return a.id < b.id;
}

bool sort_id_r(Process a, Process b)
{
	return a.id > b.id;
}

bool sort_arrival_time(Process a, Process b)
{
	return a.arrivaltime < b.arrivaltime;
}
bool sort_arrival_time_r(Process a, Process b)
{
	return a.arrivaltime > b.arrivaltime;
}

bool sort_cpu_burst(Process a, Process b)
{
	return a.cpubrust < b.cpubrust;
}

bool sort_psjf_remaining_time(Process a, Process b)
{
	return (a.cpubrust - a.psjfruntime) < (b.cpubrust - b.psjfruntime);
}

bool sort_psjf_used(Process a, Process b)
{
	return a.psjfused < b.psjfused;
}

bool sort_pp_used(Process a, Process b)
{
	return a.ppused < b.ppused;
}

bool sort_priority(Process a, Process b)
{
	return a.priority < b.priority;
}

void first_come_first_serve(vector<Process> & process1, string & fcfsganttchart)
{
	int currenttime=0, pos = 0;
	bool cpurunning = false;
	
	sort(process1.begin(), process1.end(), sort_id);
	sort(process1.begin(), process1.end(), sort_arrival_time);

	while (pos < process1.size())
	{
		if (cpurunning == false)
		{
			if (process1[pos].arrivaltime <= currenttime)
			{
				if (process1[pos].fcfsruntime < process1[pos].cpubrust)
				{
					if(process1[pos].fcfsruntime == 0)
					{
						process1[pos].fcfsbegintime = currenttime;
					}
					cpurunning = true;
					process1[pos].fcfsruntime++;
					currenttime++;
					//cout << process1[pos].fcfsruntime << " ";
					fcfsganttchart.push_back(int_transform(process1[pos].id));
				}
				else
				{
					cpurunning = false;
					process1[pos].fcfsendtime = currenttime;
					process1[pos].fcfswaitingtime = process1[pos].fcfsendtime - process1[pos].cpubrust - process1[pos].arrivaltime;
					process1[pos].fcfsturnaroundtime = process1[pos].fcfsendtime - process1[pos].arrivaltime;

					pos++;				
				}
			}
			else
			{
				currenttime++;
				fcfsganttchart.push_back('-');//++
			}
		}
		else
		{
			if (process1[pos].fcfsruntime < process1[pos].cpubrust)
			{
				cpurunning = true;
				process1[pos].fcfsruntime++;
				currenttime++;
				//cout << process1[pos].fcfsruntime << " ";
				fcfsganttchart.push_back(int_transform(process1[pos].id));
			}
			else
			{
				cpurunning = false;
				process1[pos].fcfsendtime = currenttime;
				process1[pos].fcfswaitingtime = process1[pos].fcfsendtime - process1[pos].cpubrust - process1[pos].arrivaltime;
				process1[pos].fcfsturnaroundtime = process1[pos].fcfsendtime - process1[pos].arrivaltime;
				pos++;
				
			}
		}
	}
}

void last_come_first_serve(vector<Process> & process1, string & fcfsganttchart)
{
	int currenttime=0, pos = 0, p1num = 0;

	
	sort(process1.begin(), process1.end(), sort_id_r);
	sort(process1.begin(), process1.end(), sort_arrival_time);



	vector<Process> lcfs1, lcfsres;
	


	while (p1num < process1.size() || !lcfs1.empty())
	{

		if (!lcfs1.empty())
		{
			//cout << "1" << endl;

			if (lcfs1[pos].fcfsbegintime == 0)
			{
				//cout << "2" << endl;
				lcfs1[pos].fcfsbegintime = currenttime;
			}

			//cout << "3" << endl;
			currenttime++;
			lcfs1[pos].fcfsruntime++;
			fcfsganttchart.push_back(int_transform(lcfs1[pos].id));

			if (lcfs1[pos].fcfsruntime == lcfs1[pos].cpubrust)
			{
				//cout << "5" << endl;
				lcfs1[pos].fcfsendtime = currenttime;
				lcfs1[pos].fcfswaitingtime = lcfs1[pos].fcfsendtime - lcfs1[pos].cpubrust - lcfs1[pos].arrivaltime;
				lcfs1[pos].fcfsturnaroundtime = lcfs1[pos].fcfsendtime - lcfs1[pos].arrivaltime;

				lcfsres.push_back(lcfs1[pos]);
				lcfs1.erase(lcfs1.begin());
			}
			

			while (p1num < process1.size() && process1[p1num].arrivaltime == currenttime)
			{
				lcfs1.push_back(process1[p1num]);

				if (lcfs1.size() > 1)
				{

					sort(lcfs1.begin(), lcfs1.end(), sort_id_r);
					sort(lcfs1.begin(), lcfs1.end(), sort_arrival_time_r);

					
				p1num++;
				//cout << "4" << endl;
				//cout << "p1num: " << p1num << endl;
				}
			}
		}
		else
		{
			//cout << "6" << endl;
			currenttime++;
			fcfsganttchart.push_back('-');//++

			//cout << "p1num: " << p1num << endl;
			//cout << "process1.size(): " << process1.size() << endl;
			//cout << "process1[p1num].arrivaltime: " << process1[p1num].arrivaltime << endl;
			//cout << "currenttime: " << currenttime << endl;

			while (p1num < process1.size() && process1[p1num].arrivaltime == currenttime)
			{
				lcfs1.push_back(process1[p1num]);

				if (lcfs1.size() > 1)
				{

					sort(lcfs1.begin(), lcfs1.end(), sort_id_r);
					sort(lcfs1.begin(), lcfs1.end(), sort_arrival_time_r);
				}


				p1num++;
				//cout << "4" << endl;
				//cout << "p1num: " << p1num << endl;
			}
		}
	}

	process1 = lcfsres;
}

void round_robin(vector<Process> & process1, string & rrganttchart, int timeslice)
{
	
	int currenttime = 0, pos = 0, curtimeslice = 0,p1num = 0;
	vector<Process> roundrobin1,rrres;

	//到達時間小~大,id小~大.
	sort(process1.begin(), process1.end(), sort_id);
	sort(process1.begin(), process1.end(), sort_arrival_time);

	while (p1num < process1.size() || !roundrobin1.empty())
	{
		//只要沒放完process或沒處理完就跑一秒.

		//cout << "roundrobin1.size(): " << roundrobin1.size() << endl;
		//cout << "rrres.size(): " << rrres.size() << endl;
		//cout << "currenttime: " << currenttime << endl;

		if (!roundrobin1.empty())
		{
			//cout << "2" << endl;

			if (roundrobin1[pos].rrruntime == 0)
			{
				//cout << "3" << endl;
				roundrobin1[pos].rrbegintime = currenttime;
			}

			//cout << "4" << endl;
			currenttime++;
			roundrobin1[pos].rrruntime++;
			curtimeslice++;
			rrganttchart.push_back(int_transform(roundrobin1[pos].id));

			while (p1num < process1.size() && process1[p1num].arrivaltime == currenttime)
			{
				//只要時間到了放進roundrobin1.
				roundrobin1.push_back(process1[p1num]);
				p1num++;
				//cout << "1" << endl;
				//cout << "p1num: " << p1num << endl;
			}

			if (roundrobin1[pos].rrruntime == roundrobin1[pos].cpubrust)
			{
				//cout << "5" << endl;
				roundrobin1[pos].rrendtime = currenttime;
				roundrobin1[pos].rrwaitingtime = roundrobin1[pos].rrendtime - roundrobin1[pos].cpubrust - roundrobin1[pos].arrivaltime;
				roundrobin1[pos].rrturnaroundtime = roundrobin1[pos].rrendtime - roundrobin1[pos].arrivaltime;

				rrres.push_back(roundrobin1[pos]);
				roundrobin1.erase(roundrobin1.begin());
				curtimeslice = 0;
			}
			if (curtimeslice == timeslice)
			{
				//cout << "6" << endl;
				Process temp1 = roundrobin1[pos];
				roundrobin1.push_back(temp1);
				roundrobin1.erase(roundrobin1.begin());
				curtimeslice = 0;
			}

		}
		else 
		{
			//cout << "7" << endl;
			currenttime++;
			rrganttchart.push_back('-');//++

			while (p1num < process1.size() && process1[p1num].arrivaltime == currenttime)
			{
				//只要時間到了放進roundrobin1.
				roundrobin1.push_back(process1[p1num]);
				p1num++;
				//cout << "1" << endl;
				//cout << "p1num: " << p1num << endl;
			}

		}

	}

	process1 = rrres;
}

void preemptive_shortest_job_first(vector<Process> & process1, string & psjfganttchart)
{
	int currenttime = 0, pos = 0, p1num = 0;

	vector<Process> psjf1, psjfres;
	

	sort(process1.begin(), process1.end(), sort_arrival_time);

	while (p1num < process1.size() || !psjf1.empty())
	{
		//只要沒放完process或沒處理完就跑一秒.
		//cout << "psjf1.size(): " << psjf1.size() << endl;
		//cout << "psjfres.size(): " << psjfres.size() << endl;
		//cout << "currenttime: " << currenttime << endl;
		

		if (!psjf1.empty())
		{
			//cout << "1" << endl;

			if (psjf1[pos].psjfruntime == 0)
			{
				//cout << "2" << endl;
				psjf1[pos].psjfbegintime = currenttime;
			}

			//cout << "3" << endl;
			currenttime++;
			psjf1[pos].psjfruntime++;
			psjf1[pos].psjfused = 1;
			psjfganttchart.push_back(int_transform(psjf1[pos].id));

			if (psjf1[pos].psjfruntime == psjf1[pos].cpubrust)
			{
				//cout << "5" << endl;
				psjf1[pos].psjfendtime = currenttime;
				psjf1[pos].psjfwaitingtime = psjf1[pos].psjfendtime - psjf1[pos].cpubrust - psjf1[pos].arrivaltime;
				psjf1[pos].psjfturnaroundtime = psjf1[pos].psjfendtime - psjf1[pos].arrivaltime;

				psjfres.push_back(psjf1[pos]);
				psjf1.erase(psjf1.begin());
			}
			

			while (p1num < process1.size() && process1[p1num].arrivaltime == currenttime)
			{
				psjf1.push_back(process1[p1num]);

				if (psjf1.size() > 1)
				{

					sort(psjf1.begin(), psjf1.end(), sort_id);
					sort(psjf1.begin(), psjf1.end(), sort_arrival_time);
					sort(psjf1.begin(), psjf1.end(), sort_psjf_used);
					sort(psjf1.begin(), psjf1.end(), sort_psjf_remaining_time);
					
					if ((psjf1[0].psjfruntime - psjf1[0].cpubrust) == (psjf1[1].psjfruntime - psjf1[1].cpubrust)
						&& psjf1[0].psjfused == 0)
					{
						sort(psjf1.begin() + 1, psjf1.end(), sort_id);
						sort(psjf1.begin() + 1, psjf1.end(), sort_arrival_time);
						sort(psjf1.begin() + 1, psjf1.end(), sort_psjf_used);
						sort(psjf1.begin() + 1, psjf1.end(), sort_psjf_remaining_time);
					}
					else
					{
						sort(psjf1.begin(), psjf1.end(), sort_id);
						sort(psjf1.begin(), psjf1.end(), sort_arrival_time);
						sort(psjf1.begin(), psjf1.end(), sort_psjf_used);
						sort(psjf1.begin(), psjf1.end(), sort_psjf_remaining_time);
					}
				}

				p1num++;
				//cout << "4" << endl;
				//cout << "p1num: " << p1num << endl;
			}
		}
		else
		{
			//cout << "6" << endl;
			currenttime++;
			psjfganttchart.push_back('-');//++

			//cout << "p1num: " << p1num << endl;
			//cout << "process1.size(): " << process1.size() << endl;
			//cout << "process1[p1num].arrivaltime: " << process1[p1num].arrivaltime << endl;
			//cout << "currenttime: " << currenttime << endl;

			while (p1num < process1.size() && process1[p1num].arrivaltime == currenttime)
			{
				psjf1.push_back(process1[p1num]);

				if (psjf1.size() > 1)
				{

					sort(psjf1.begin(), psjf1.end(), sort_id);
					sort(psjf1.begin(), psjf1.end(), sort_arrival_time);
					sort(psjf1.begin(), psjf1.end(), sort_psjf_used);
					sort(psjf1.begin(), psjf1.end(), sort_psjf_remaining_time);
					
					if ((psjf1[0].psjfruntime - psjf1[0].cpubrust) == (psjf1[1].psjfruntime - psjf1[1].cpubrust)
						&& psjf1[0].psjfused == 0)
					{
						sort(psjf1.begin() + 1, psjf1.end(), sort_id);
						sort(psjf1.begin() + 1, psjf1.end(), sort_arrival_time);
						sort(psjf1.begin() + 1, psjf1.end(), sort_psjf_used);
						sort(psjf1.begin() + 1, psjf1.end(), sort_psjf_remaining_time);
					}
					else
					{
						sort(psjf1.begin(), psjf1.end(), sort_id);
						sort(psjf1.begin(), psjf1.end(), sort_arrival_time);
						sort(psjf1.begin(), psjf1.end(), sort_psjf_used);
						sort(psjf1.begin(), psjf1.end(), sort_psjf_remaining_time);
					}
				}

				p1num++;
				//cout << "4" << endl;
				//cout << "p1num: " << p1num << endl;
			}
		}
	}

	process1 = psjfres;
}

void non_preemptive_shortest_job_first(vector<Process> & process1, string & nsjfganttchart)
{
	int currenttime = 0, pos = 0, p1num = 0;
	bool cpurunning = false;

	vector<Process> nsjf1, nsjfres;

	sort(process1.begin(), process1.end(), sort_arrival_time);

	while (p1num < process1.size() || !nsjf1.empty())
	{
		//cout << "nsjf1.size(): " << nsjf1.size() << endl;
		//cout << "nsjfres.size(): " << nsjfres.size() << endl;
		//cout << "currenttime: " << currenttime << endl;

		if (!nsjf1.empty())
		{
			
			//cout << "1" << endl;

			if (nsjf1[pos].nsjfruntime == 0)
			{
				//cout << "2" << endl;
				nsjf1[pos].nsjfbegintime = currenttime;
			}

			currenttime++;
			cpurunning = true;
			nsjf1[pos].nsjfruntime++;
			nsjfganttchart.push_back(int_transform(nsjf1[pos].id));


			if (nsjf1[pos].nsjfruntime == nsjf1[pos].cpubrust)
			{
				cpurunning = false;
				//cout << "5" << endl;
				nsjf1[pos].nsjfendtime = currenttime;
				nsjf1[pos].nsjfwaitingtime = nsjf1[pos].nsjfendtime - nsjf1[pos].cpubrust - nsjf1[pos].arrivaltime;
				nsjf1[pos].nsjfturnaroundtime = nsjf1[pos].nsjfendtime - nsjf1[pos].arrivaltime;

				nsjfres.push_back(nsjf1[pos]);
				nsjf1.erase(nsjf1.begin());
			}

			if (cpurunning)
			{
				while (p1num < process1.size() && process1[p1num].arrivaltime == currenttime)
				{
					//只要時間到了放進nsjf1.
					nsjf1.push_back(process1[p1num]);
					sort(nsjf1.begin()+1, nsjf1.end(), sort_id);
					sort(nsjf1.begin()+1, nsjf1.end(), sort_arrival_time);
					sort(nsjf1.begin()+1, nsjf1.end(), sort_cpu_burst);

					p1num++;
					//cout << "4" << endl;
					//cout << "p1num: " << p1num << endl;
				}
			}
			else
			{
				while (p1num < process1.size() && process1[p1num].arrivaltime == currenttime)
				{
					//只要時間到了放進nsjf1.
					nsjf1.push_back(process1[p1num]);
					sort(nsjf1.begin(), nsjf1.end(), sort_id);
					sort(nsjf1.begin(), nsjf1.end(), sort_arrival_time);
					sort(nsjf1.begin(), nsjf1.end(), sort_cpu_burst);

					p1num++;
					//cout << "4" << endl;
					//cout << "p1num: " << p1num << endl;
				}
			}
		}
		else
		{
			//還沒有process進來
			//cout << "6" << endl;
			currenttime++;
			nsjfganttchart.push_back('-');//++

			while (p1num < process1.size() && process1[p1num].arrivaltime == currenttime)
			{
				//只要時間到了放進nsjf1.
				nsjf1.push_back(process1[p1num]);
				sort(nsjf1.begin(), nsjf1.end(), sort_id);
				sort(nsjf1.begin(), nsjf1.end(), sort_arrival_time);
				sort(nsjf1.begin(), nsjf1.end(), sort_cpu_burst);
				p1num++;
				//cout << "7" << endl;
				//cout << "p1num: " << p1num << endl;
			}
		}

	}

	process1 = nsjfres;
}

void preemptive_priority(vector<Process> & process1, string & ppganttchart)
{
	int currenttime = 0, pos = 0, p1num = 0;

	vector<Process> pp1, ppres;

	sort(process1.begin(), process1.end(), sort_arrival_time);

	while (p1num < process1.size() || !pp1.empty())
	{
		//只要沒放完process或沒處理完就跑一秒.
		//cout << "pp1.size(): " << pp1.size() << endl;
		//cout << "ppres.size(): " << ppres.size() << endl;
		//cout << "currenttime: " << currenttime << endl;


		if (!pp1.empty())
		{
			//cout << "1" << endl;

			if (pp1[pos].ppruntime == 0)
			{
				//cout << "2" << endl;
				pp1[pos].ppbegintime = currenttime;
			}

			//cout << "3" << endl;
			currenttime++;
			pp1[pos].ppruntime++;
			pp1[pos].ppused = 1;
			ppganttchart.push_back(int_transform(pp1[pos].id));

			if (pp1[pos].ppruntime == pp1[pos].cpubrust)
			{
				//cout << "5" << endl;
				pp1[pos].ppendtime = currenttime;
				pp1[pos].ppwaitingtime = pp1[pos].ppendtime - pp1[pos].cpubrust - pp1[pos].arrivaltime;
				pp1[pos].ppturnaroundtime = pp1[pos].ppendtime - pp1[pos].arrivaltime;

				ppres.push_back(pp1[pos]);
				pp1.erase(pp1.begin());
			}

			while (p1num < process1.size() && process1[p1num].arrivaltime == currenttime)
			{

				pp1.push_back(process1[p1num]);
				if (pp1.size()>1)
				{
					sort(pp1.begin(), pp1.end(), sort_priority);

					if (pp1[0].priority == pp1[1].priority)
					{
						sort(pp1.begin()+1, pp1.end(), sort_id);
						sort(pp1.begin()+1, pp1.end(), sort_arrival_time);
						sort(pp1.begin()+1, pp1.end(), sort_pp_used);
						sort(pp1.begin()+1, pp1.end(), sort_priority);
					}
					else
					{
						sort(pp1.begin(), pp1.end(), sort_id);
						sort(pp1.begin(), pp1.end(), sort_arrival_time);
						sort(pp1.begin(), pp1.end(), sort_pp_used);
						sort(pp1.begin(), pp1.end(), sort_priority);
					}
				}

				p1num++;
				//cout << "4" << endl;
				//cout << "p1num: " << p1num << endl;
			}

		}
		else
		{
			//cout << "6" << endl;
			currenttime++;
			ppganttchart.push_back('-');//++

			//cout << "p1num: " << p1num << endl;
			//cout << "process1.size(): " << process1.size() << endl;
			//cout << "process1[p1num].arrivaltime: " << process1[p1num].arrivaltime << endl;
			//cout << "currenttime: " << currenttime << endl;

			while (p1num < process1.size() && process1[p1num].arrivaltime == currenttime)
			{

				pp1.push_back(process1[p1num]);
				if (pp1.size() > 1)
				{
					sort(pp1.begin(), pp1.end(), sort_priority);

					if (pp1[0].priority == pp1[1].priority)
					{
						sort(pp1.begin() + 1, pp1.end(), sort_id);
						sort(pp1.begin() + 1, pp1.end(), sort_arrival_time);
						sort(pp1.begin() + 1, pp1.end(), sort_pp_used);
						sort(pp1.begin() + 1, pp1.end(), sort_priority);
					}
					else
					{
						sort(pp1.begin(), pp1.end(), sort_id);
						sort(pp1.begin(), pp1.end(), sort_arrival_time);
						sort(pp1.begin(), pp1.end(), sort_pp_used);
						sort(pp1.begin(), pp1.end(), sort_priority);
					}
				}

				p1num++;
				//cout << "4" << endl;
				//cout << "p1num: " << p1num << endl;
			}
		}

	}

	process1 = ppres;
}

void non_preemptive_priority(vector<Process> & process1, string & ppganttchart)
{
	int currenttime = 0, pos = 0, p1num = 0;
	bool cpurunning= false;

	vector<Process> pp1, ppres;

	sort(process1.begin(), process1.end(), sort_arrival_time);

	while (p1num < process1.size() || !pp1.empty())
	{
		//只要沒放完process或沒處理完就跑一秒.
		//cout << "pp1.size(): " << pp1.size() << endl;
		//cout << "ppres.size(): " << ppres.size() << endl;
		//cout << "currenttime: " << currenttime << endl;


		if (!pp1.empty())
		{
			//cout << "1" << endl;

			if (pp1[pos].ppruntime == 0)
			{
				//cout << "2" << endl;
				pp1[pos].ppbegintime = currenttime;
			}

			//cout << "3" << endl;
			currenttime++;
			cpurunning = true;
			pp1[pos].ppruntime++;
			pp1[pos].ppused = 1;
			ppganttchart.push_back(int_transform(pp1[pos].id));

			if (pp1[pos].ppruntime == pp1[pos].cpubrust)
			{
				//cout << "5" << endl;
				cpurunning = false;
				pp1[pos].ppendtime = currenttime;
				pp1[pos].ppwaitingtime = pp1[pos].ppendtime - pp1[pos].cpubrust - pp1[pos].arrivaltime;
				pp1[pos].ppturnaroundtime = pp1[pos].ppendtime - pp1[pos].arrivaltime;

				ppres.push_back(pp1[pos]);
				pp1.erase(pp1.begin());
			}

			while (p1num < process1.size() && process1[p1num].arrivaltime == currenttime)
			{

				pp1.push_back(process1[p1num]);
				if (pp1.size()>1)
				{
					

					if (cpurunning)
					{
						sort(pp1.begin()+1, pp1.end(), sort_id_r);
						sort(pp1.begin()+1, pp1.end(), sort_arrival_time);
						//sort(pp1.begin()+1, pp1.end(), sort_pp_used);
						sort(pp1.begin()+1, pp1.end(), sort_priority);
					}
					else
					{
						sort(pp1.begin(), pp1.end(), sort_id_r);
						sort(pp1.begin(), pp1.end(), sort_arrival_time);
						//sort(pp1.begin(), pp1.end(), sort_pp_used);
						sort(pp1.begin(), pp1.end(), sort_priority);
					}
				}

				p1num++;
				//cout << "4" << endl;
				//cout << "p1num: " << p1num << endl;
			}

		}
		else
		{
			//cout << "6" << endl;
			currenttime++;
			ppganttchart.push_back('-');//++

			//cout << "p1num: " << p1num << endl;
			//cout << "process1.size(): " << process1.size() << endl;
			//cout << "process1[p1num].arrivaltime: " << process1[p1num].arrivaltime << endl;
			//cout << "currenttime: " << currenttime << endl;

			while (p1num < process1.size() && process1[p1num].arrivaltime == currenttime)
			{

				pp1.push_back(process1[p1num]);
				if (pp1.size() > 1)
				{
					

					if (cpurunning)
					{
						sort(pp1.begin() + 1, pp1.end(), sort_id_r);
						sort(pp1.begin() + 1, pp1.end(), sort_arrival_time);
						//sort(pp1.begin() + 1, pp1.end(), sort_pp_used);
						sort(pp1.begin() + 1, pp1.end(), sort_priority);
					}
					else
					{
						sort(pp1.begin(), pp1.end(), sort_id_r);
						sort(pp1.begin(), pp1.end(), sort_arrival_time);
						//sort(pp1.begin(), pp1.end(), sort_pp_used);
						sort(pp1.begin(), pp1.end(), sort_priority);
					}
				}

				p1num++;
				//cout << "4" << endl;
				//cout << "p1num: " << p1num << endl;
			}
		}

	}

	process1 = ppres;
}

void output(vector<Process> process1, string fcfsganttchart, string rrganttchart, string psjfganttchart, string nsjfganttchart, string ppganttchart)
{

	cout << "==    FCFS==" << endl;
	cout << fcfsganttchart << endl;
	cout << "==      RR==" << endl;
	cout << rrganttchart << endl;
	cout << "==    PSJF==" << endl;
	cout << psjfganttchart << endl;
	cout << "==Non-PSJF==" << endl;
	cout << nsjfganttchart << endl;
	cout << "== Priority==" << endl;
	cout << ppganttchart << endl;
	cout << "===========================================================" << endl << endl;
	cout << "Waiting Time" << endl;
	cout << "ID      FCFS    RR      PSJF    NPSJF   Priority" << endl;
	cout << "===========================================================" << endl;
	
	sort(process1.begin(), process1.end(), sort_id);

	for (int i = 0; i < process1.size(); i++)
	{
		cout << process1[i].id << "	"
			<< process1[i].fcfswaitingtime << "	"
			<< process1[i].rrwaitingtime << "	"
			<< process1[i].psjfwaitingtime << "	"
			<< process1[i].nsjfwaitingtime << "	"
			<< process1[i].ppwaitingtime << endl;
	}

	cout << "===========================================================" << endl << endl;
	cout << "Turnaround Time" << endl;
	cout << "ID      FCFS    RR      PSJF    NPSJF   Priority" << endl;
	cout << "===========================================================" << endl;

	for (int i = 0; i < process1.size(); i++)
	{
		cout << process1[i].id << "	"
			<< process1[i].fcfsturnaroundtime << "	"
			<< process1[i].rrturnaroundtime << "	"
			<< process1[i].psjfturnaroundtime << "	"
			<< process1[i].nsjfturnaroundtime << "	"
			<< process1[i].ppturnaroundtime << endl;
	}

	cout << "===========================================================" << endl << endl;

}

string give_space(int num)
{
	string s = "";
	for (int i = 0; i < num ; i++)
	{
		s = s + " ";
	}

	return s;
}

void file_printer (string filename, vector<Process> process1, string fcfsganttchart, string rrganttchart, string psjfganttchart, string nsjfganttchart, string ppganttchart)
{

	ofstream fout;
	filename = "out_" + filename + ".txt";
	fout.open(filename);

	fout << "==    FCFS==" << endl;
	fout << fcfsganttchart << endl;
	fout << "==      RR==" << endl;
	fout << rrganttchart << endl;
	fout << "==    PSJF==" << endl;
	fout << psjfganttchart << endl;
	fout << "==Non-PSJF==" << endl;
	fout << nsjfganttchart << endl;
	fout << "== Priority==" << endl;
	fout << ppganttchart << endl;
	fout << "===========================================================" << endl << endl;
	fout << "Waiting Time" << endl;
	fout << "ID      FCFS    RR      PSJF    NPSJF   Priority" << endl;
	fout << "===========================================================" << endl;

	sort(process1.begin(), process1.end(), sort_id);

	for (int i = 0; i < process1.size(); i++)
	{
		fout << process1[i].id << give_space(8- to_string(process1[i].id).length())
			<< process1[i].fcfswaitingtime << give_space(8 - to_string(process1[i].fcfswaitingtime).length())
			<< process1[i].rrwaitingtime << give_space(8 - to_string(process1[i].rrwaitingtime).length())
			<< process1[i].psjfwaitingtime << give_space(8 - to_string(process1[i].psjfwaitingtime).length())
			<< process1[i].nsjfwaitingtime << give_space(8 - to_string(process1[i].nsjfwaitingtime).length())
			<< process1[i].ppwaitingtime << endl;
	}

	fout << "===========================================================" << endl << endl;
	fout << "Turnaround Time" << endl;
	fout << "ID      FCFS    RR      PSJF    NPSJF   Priority" << endl;
	fout << "===========================================================" << endl;

	for (int i = 0; i < process1.size(); i++)
	{
		fout << process1[i].id << give_space(8 - to_string(process1[i].id).length())
			<< process1[i].fcfsturnaroundtime << give_space(8 - to_string(process1[i].fcfsturnaroundtime).length())
			<< process1[i].rrturnaroundtime << give_space(8 - to_string(process1[i].rrturnaroundtime).length())
			<< process1[i].psjfturnaroundtime << give_space(8 - to_string(process1[i].psjfturnaroundtime).length())
			<< process1[i].nsjfturnaroundtime << give_space(8 - to_string(process1[i].nsjfturnaroundtime).length())
			<< process1[i].ppturnaroundtime << endl;
	}

	fout << "===========================================================" << endl;

	fout.close();
}

int main()
{
	string filename;

	cout << "FileName(ex:input,input1... 0 to exit): ";
	cin >> filename;
	while (filename != "0")
	{
		int method, timeslice;
		vector<Process> process1;
		string fcfsganttchart, rrganttchart, psjfganttchart, nsjfganttchart, ppganttchart;

		file_reader(filename, method, timeslice, process1);

		if (method == 1)
		{
			first_come_first_serve(process1, fcfsganttchart);
		}
		else if (method == 2)
		{
			round_robin(process1, rrganttchart, timeslice);
		}
		else if (method == 3)
		{
			preemptive_shortest_job_first(process1, psjfganttchart);
		}
		else if (method == 4)
		{
			non_preemptive_shortest_job_first(process1, nsjfganttchart);
		}
		else if (method == 5)
		{
			preemptive_priority(process1, ppganttchart);
		}
		else if (method == 6)
		{
			first_come_first_serve(process1, fcfsganttchart);
			round_robin(process1, rrganttchart, timeslice);
			preemptive_shortest_job_first(process1, psjfganttchart);
			non_preemptive_shortest_job_first(process1, nsjfganttchart);
			preemptive_priority(process1, ppganttchart);
		}
		else if (method == 7)
		{
			last_come_first_serve(process1, fcfsganttchart);
			round_robin(process1, rrganttchart, timeslice);
			preemptive_shortest_job_first(process1, psjfganttchart);
			non_preemptive_shortest_job_first(process1, nsjfganttchart);
			preemptive_priority(process1, ppganttchart);
		}
		else if (method == 8)
		{
			first_come_first_serve(process1, fcfsganttchart);
			round_robin(process1, rrganttchart, timeslice);
			preemptive_shortest_job_first(process1, psjfganttchart);
			non_preemptive_shortest_job_first(process1, nsjfganttchart);
			non_preemptive_priority(process1, ppganttchart);
		}
		else
		{
			cout << "Method error!" << endl;
			return 0;
		}

		

		output(process1, fcfsganttchart, rrganttchart, psjfganttchart, nsjfganttchart, ppganttchart);
		file_printer(filename, process1, fcfsganttchart, rrganttchart, psjfganttchart, nsjfganttchart, ppganttchart);

		cout << endl << "FileName(ex:input,input1...0 to exit): ";
		cin >> filename;
	}

	return 0;




	// 執行程式: Ctrl + F5 或 [偵錯] > [啟動但不偵錯] 功能表
	// 偵錯程式: F5 或 [偵錯] > [啟動偵錯] 功能表

	// 開始使用的秘訣: 
	//   1. 使用 [方案總管] 視窗，新增/管理檔案
	//   2. 使用 [Team Explorer] 視窗，連線到原始檔控制
	//   3. 使用 [輸出] 視窗，參閱組建輸出與其他訊息
	//   4. 使用 [錯誤清單] 視窗，檢視錯誤
	//   5. 前往 [專案] > [新增項目]，建立新的程式碼檔案，或是前往 [專案] > [新增現有項目]，將現有程式碼檔案新增至專案
	//   6. 之後要再次開啟此專案時，請前往 [檔案] > [開啟] > [專案]，然後選取 .sln 檔案

}
