#pragma once

#include "Common.h"
#include "User.h"

#include "RakNet\RakNetTypes.h"

#include <unordered_map>
#include <algorithm>

// Use that for managing online users
class UsersPool {
	private:
		std::unordered_map< unsigned long long, Ref< User > > _pool; // This is an unordered_map of the online Users
		
		//Not used but could be useful
		static SystemAddress decompress(unsigned long long cAddress){
			SystemAddress addr;
			addr.binaryAddress = (unsigned long)(cAddress >> 16);
			addr.port = (unsigned short)(cAddress & 0xFFFF);
			return addr;
		}

		//compression
		static unsigned long long compress(SystemAddress address){
			return (((unsigned long long) address.binaryAddress) << 16) + address.port;
		}
	public:
		// This inserts a new user into the user pool
		void Insert(const Ref< User >& usr, const SystemAddress& sysaddr) {
			_pool.insert(std::make_pair(compress(sysaddr), usr));
		}

		// This returns the count with the number of users online
		int Count() { return _pool.size(); }

		// Searches the user pool for a specific user (by their SystemAddress)
		// Returns null if user is not found
		Ref< User > Find(const SystemAddress& sysaddr) {
			std::unordered_map< unsigned long long, Ref< User > >::iterator it = std::find_if(_pool.begin(), _pool.end(), [&](std::pair<unsigned long long, Ref< User >> o){
				return (compress(sysaddr) == o.first);
			});
			if (it == _pool.end()) return NULL;
			return it->second;
		}

		// Remove a user from the user pool
		// Returns true if user is removed
		bool Remove(const SystemAddress& sysaddr) {
			using namespace std;
			auto usr = Find(sysaddr);
			if (usr == NULL || (--usr->numredir) > 0) return false;
			_pool.erase(compress(sysaddr));
			return true;
		}
};