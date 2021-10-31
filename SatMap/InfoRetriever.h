#pragma once

#ifndef INFO_RETREIVER_H
#define INFO_RETRIEVER_H
#include <CoordTopocentric.h>
#include <CoordGeodetic.h>
#include <Observer.h>
#include <SGP4.h>

#include "curl/curl.h"
#include "curl_easy.h"
#include "curl_form.h"
#include "curl_ios.h"
#include "curl_exception.h"





namespace Query {
    SGP4 GetSGP4(int id);
    std::vector<Tle> GetTleGroup(const std::string& groupName);
    std::vector<SGP4> GetSGP4Group(std::vector<Tle>& tles);
    std::vector<SGP4> GetSGP4Group(const std::string& groupName);
}
#endif // !INFO_RETREIVER_H
