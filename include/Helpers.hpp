#pragma once
#include "Urho3D/Urho3DAll.h"


///A number of helper functions for Urho3D
namespace Helpers {

	/// Trims a string of unwanted characters, defaulting to spaces and tabs.
	void TrimString(std::string& input, const std::string& unwanted = " \t") {

		int strBegin = input.find_first_not_of(unwanted);
		int strEnd = input.find_last_not_of(unwanted);

		input = input.substr(strBegin, strEnd+1);

	}


}