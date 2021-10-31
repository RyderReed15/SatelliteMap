#include "InfoRetriever.h"

SGP4 Query::GetSGP4(int id) {

	std::stringstream webData;
	curl::curl_ios<std::stringstream> webStream(webData);

	curl::curl_easy curlEasy(webStream);
	std::string url = "https://celestrak.com/NORAD/elements/gp.php?CATNR=" + std::to_string(id) + "&FORMAT=TLE";
	curlEasy.add<CURLOPT_URL>(url.c_str());
	curlEasy.add<CURLOPT_FOLLOWLOCATION>(1);
	try {
		curlEasy.perform();

		std::string line, line2, line3;
		std::getline(webData, line);
		std::getline(webData, line2);
		std::getline(webData, line3);
		Tle satTLE(line, line2.substr(0, 69), line3.substr(0, 69));
		SGP4 sgp4(satTLE);

		return sgp4;
	}
	catch (curl::curl_easy_exception error)
	{
		auto errors = error.get_traceback();
		error.print_traceback();
	}

}
std::vector<Tle> Query::GetTleGroup(const std::string& groupName) {
	std::stringstream webData;
	curl::curl_ios<std::stringstream> webStream(webData);

	curl::curl_easy curlEasy(webStream);
	std::string url = "https://celestrak.com/NORAD/elements/gp.php?GROUP=" + groupName + "&FORMAT=TLE";
	curlEasy.add<CURLOPT_URL>(url.c_str());
	curlEasy.add<CURLOPT_FOLLOWLOCATION>(1);
	try {
		curlEasy.perform();
		std::vector<Tle> tles;
		while (!webData.eof()) {
			std::string line, line2, line3;
			std::getline(webData, line);
			if (webData.eof()) break;
			std::getline(webData, line2);
			std::getline(webData, line3);
			Tle satTLE(line, line2.substr(0, 69), line3.substr(0, 69));
			tles.push_back(satTLE);
		}

		return tles;
	}
	catch (curl::curl_easy_exception error)
	{
		auto errors = error.get_traceback();
		error.print_traceback();
	}
}
std::vector<SGP4> Query::GetSGP4Group(std::vector<Tle>& tles) {
	std::vector<SGP4> sgp4s;
	for (int i = 0; i < tles.size(); i++) {
		SGP4 sgp4(tles[i]);
		sgp4s.push_back(sgp4);

	}
	return sgp4s;
}
std::vector<SGP4> Query::GetSGP4Group(const std::string& groupName) {

	std::vector<Tle> group = GetTleGroup(groupName);
	return GetSGP4Group(group);

}