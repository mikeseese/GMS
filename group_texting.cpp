// GMS - Group Messaging Service
/*Copyright (C) <2011>  <Mike Seese>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
	
	Contact me at seesemichaelj@gmail.com with any questions or concerns.*/

#include "stdafx.h"
#include "GMSContact.h"
#include <includes.h>
#include <functions.h>
#include <ipworks.h>
#include <vector>

POP pop; //POP object
SMTP smtp; //SMTP object

string convertInt(int number)
{
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

int _tmain(int argc, _TCHAR* argv[])
{
	int ret_code = 0;
	int msgnum = 1;
	int msgcount;
	int groupnum;
	string from;
	string groupEmail;
	string msgtxt;
	vector<vector<GMSContact>> groups;

	if (ret_code = pop.SetMailServer("pop.mikeseese.com")) goto done; //set the POP server, exit if you cant
	pop.SetUser("groups);
	pop.SetPassword("**********");
	if(ret_code = pop.Connect()) goto done; //connect to the POP server, exit if you cant

	//Connect to SMTP server
	if (ret_code = smtp.SetMailServer("smtp.mikeseese.com")) goto done;
	smtp.SetUser("groups");
	smtp.SetPassword("**********");

	while(1)
	{
		msgcount = pop.GetMessageCount();
		while(msgnum <= msgcount)
		{
			pop.SetMessageNumber(msgnum);
			msgtxt = pop.GetMessageText();
			string subject = pop.GetMessageSubject();
			if(subject.compare("Returned mail: see transcript for details") == 0) goto loop;
			cout << msgtxt << endl;
			from = pop.GetMessageFrom();
			from = from.substr(0, 10);
			groupEmail = pop.GetMessageTo();
			if(groupEmail.compare("groups@mikeseese.com") == 0)
			{
				//if they're texting the original email, form a group
				vector<GMSContact> group;
				int pos;
				
				//add initiator to the group
				pos = msgtxt.find_first_of(',');
				string name = msgtxt.substr(0, pos);
				msgtxt = msgtxt.substr(pos+1);
				GMSContact initiator(name, from);
				group.push_back(initiator);

				//add friends to group
				while(msgtxt.length() > 5)
				{
					pos = msgtxt.find_first_of(',');
					string name = msgtxt.substr(0, pos);
					msgtxt = msgtxt.substr(pos+1);
					pos = msgtxt.find_first_of(',');
					string number = msgtxt.substr(0, pos);
					msgtxt = msgtxt.substr(pos+1);
					GMSContact c(name, number);
					group.push_back(c);
				}
				groups.push_back(group);

				//text back to the initiator with new group
				groupnum = groups.size() - 1;
				string groupNumber = convertInt(groupnum);
				while(groupNumber.length() < 3)
				{
					groupNumber = "0" + groupNumber;
				}
				groupEmail = "groups+" + groupNumber + "@mikeseese.com";
				smtp.SetFrom(groupEmail.c_str());
				smtp.SetReplyTo(groupEmail.c_str());
				string toAddress = generateEmails(from);
				smtp.SetSendTo(toAddress.c_str());
				smtp.SetMessageText("Your texting group has been created! Simply reply to this text to start chatting!");
				smtp.Send();
			}
			else
			{
				//if they're texting a group, manage the SMS message
				groupEmail = groupEmail.substr(7, 3);
				groupnum = atoi(groupEmail.c_str());
				int contactPos;
				if(groupnum >= groups.size()) goto loop; //if someone inputs an invalid group number (too big), delete it
				vector<GMSContact> group = groups.at(groupnum);
				string toAddresses;
				string fromName;
				for(int i = 0; i < group.size(); i++)
				{
					if(group.at(i).number.compare(from) != 0)
					{
						toAddresses = toAddresses + generateEmails(group.at(i).number);;
					}
					else
					{
						fromName = group.at(i).name;
					}
				}
				smtp.SetSendTo(toAddresses.c_str());
				msgtxt = fromName + ": " + msgtxt;
				while(msgtxt.length() > 0)
				{
					string msg = msgtxt.substr(0, 160);
					smtp.SetMessageText(msg.c_str());
					smtp.Send();
					if(msgtxt.length() > 160)
						msgtxt = msgtxt.substr(159); //if it's too long, let's loop around and send the next message
					else
						break;
				}
			}
loop:
			pop.Delete();

			msgnum++;
		}
		msgnum = 1; //reset the message number
		pop.Disconnect();
		pop.Connect();
		Sleep(1000);
	}

done:
	system("pause");
	return 0;
}

