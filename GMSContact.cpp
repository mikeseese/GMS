#include "stdafx.h"
#include "GMSContact.h"
#include <includes.h>

GMSContact::GMSContact(string person, string cell)
{
	name = person;
	number = cell;
	//sprintf(number, "%s@txt.att.net,%s@myboostmobile.com,%s@cingular.com,%s@mymetropcs.com,%s@vtext.com,%s@messaging.sprintpcs.com,%s@vmobl.com,", cell, cell, cell, cell, cell, cell, cell);
}

GMSContact::GMSContact()
{
	//
}

string generateEmails(string cell)
{
	char num[256];
	sprintf(num, "%s@txt.att.net,%s@myboostmobile.com,%s@cingular.com,%s@mymetropcs.com,%s@vtext.com,%s@messaging.sprintpcs.com,%s@vmobl.com,", cell.c_str(), cell.c_str(), cell.c_str(), cell.c_str(), cell.c_str(), cell.c_str(), cell.c_str());
	string emails(num);
	return emails;
}