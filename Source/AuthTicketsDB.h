#include "Database.h"

class AuthTicketsTable : public MySQLTable{
public:
	static void validateTicket(ulonglong id);
};