#pragma once
#include "AuthTicketsDB.h"
#include "Logger.h"
#include "UtfConverter.h"
#include "Common.h"

void AuthTicketsTable::validateTicket(ulonglong id){
	std::stringstream eqqr;
	eqqr << "UPDATE `auth_tickets` SET `status`='" << "ACCEPTED" << "' WHERE `id` = '" << id << "';";
	Database::Query(eqqr.str());
}