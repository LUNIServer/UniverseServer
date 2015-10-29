#pragma once
#include "time.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

static std::string writeTime(tm* time, std::string format){
	std::stringstream out;
	char y;
	for (unsigned int k = 0; k < format.length(); k++){
		y = format.at(k);
		switch (y){
		case 89: //Y
			out << std::setfill('0') << std::setw(4) << std::to_string(time->tm_year + 1900);
			break;
		case 109: //m
			out << std::setfill('0') << std::setw(2) << std::to_string(time->tm_mon + 1);
			break;
		case 100: //d
			out << std::setfill('0') << std::setw(2) << std::to_string(time->tm_mday);
			break;
		case 72: //H
			out << std::setfill('0') << std::setw(2) << std::to_string(time->tm_hour);
			break;
		case 77: //M
			out << std::setfill('0') << std::setw(2) << std::to_string(time->tm_min);
			break;
		case 83: //S
			out << std::setfill('0') << std::setw(2) << std::to_string(time->tm_sec);
			break;
		default:
			out << y;
		}
	}
	return out.str();
}

static std::string padd(const std::string& in, int length){

}

static tm* getTime(const std::string& time, const std::string& format){
	tm *t = new tm();
	char y;
	char s = 0;
	for (unsigned int k = 0; k < format.length(); k++){
		y = format.at(k);
		switch (y){
		case 89: //Y
			t->tm_year = std::stoi(time.substr(s, 4)) - 1900;
			s += 4;
			break;
		case 109: //m
			t->tm_mon = std::stoi(time.substr(s, 2)) - 1;
			s += 2;
			break;
		case 100: //d
			t->tm_mday = std::stoi(time.substr(s, 2));
			s += 2;
			break;
		case 72: //H
			t->tm_hour = std::stoi(time.substr(s, 2));
			s += 2;
			break;
		case 77: //M
			t->tm_min = std::stoi(time.substr(s, 2));
			s += 2;
			break;
		case 83: //S
			t->tm_sec = std::stoi(time.substr(s, 2));
			s += 2;
			break;
		default:
			s++;
		}
	}
	return t;
}

static std::string mysql_string_to_timestamp(){

}

static void getTime(){
	std::string format = "Y-m-d H:M:S";
	time_t now = time(NULL);
	tm *ntm = localtime(&now);
	std::string nstr = writeTime(ntm, format);
	if (ntm->tm_isdst){
		now = now - 3600;
		ntm = localtime(&now);
	}
	std::string nstr2 = writeTime(ntm, format);
	
	//std::cout << "NORMAL: " << nstr << std::endl;
	//std::cout << "NONDST: " << nstr2 << std::endl;

	//std::string timeS = "2015-08-07 14:01:30";
	//
	tm* t = getTime(nstr, format);
	tm* t2 = getTime(nstr2, format);
	//t->tm_isdst = true;
	//std::cout << "NORMAL: " << std::to_string(mktime(t)) << std::endl;
	//std::cout << "NONDST: " << std::to_string(mktime(t2)) << std::endl;
	//std::cout << std::to_string(time(NULL)) << std::endl;
}