#pragma once
#include <string>
#include <sstream>
#include <fstream>

#include "Helpers.hpp"

namespace Helpers {


	/// Parses a line of config into a map.
	/// \todo Add more variable types.
	void LineIntoMap(VariantMap& inMap, std::string input, bool verbose) {

		// Exit early if it's not a valid line or a comment.
		if (input[0] == '#' || input.find_first_of(":") == std::string::npos || input.find_first_of("=") == std::string::npos)
			return;
		std::string varName;
		std::string varValue;
		std::string type;

		int delimPos = input.find_first_of('=');

		varName = input.substr(0, delimPos);
		varValue = input.substr(delimPos + 1, input.length());

		int typeDelimPos = varName.find_first_of(':');

		type = varName.substr(typeDelimPos + 1, varName.length());

		varName = varName.substr(0, typeDelimPos);

		TrimString(varName);
		TrimString(varValue);
		TrimString(type);

		/// \todo Implement more types.
		if (type == "string") {
			if (varValue == "EMPTY")
				inMap[varName.c_str()] = "";
			else
				inMap[varName.c_str()] = varValue.c_str();
		} else if (type == "bool") {
			if (varValue == "True" || varValue == "true")
				inMap[varName.c_str()] = true;
			else
				inMap[varName.c_str()] = false;

		} else if (type == "int") {
			inMap[varName.c_str()] = std::stoi(varValue);
		} else if (type == "float") {
			inMap[varName.c_str()] = std::stof(varValue);
		}




		if (verbose)
			URHO3D_LOGINFO("Loaded " + String(varName.c_str()) + " with value " + String(varValue.c_str()) + " as " + String(type.c_str()));
	}

#pragma region Helper functions for LineIntoMap

	/// Calls LineIntoMap on each string in an array.
	void StringArrayIntoMap(VariantMap& inMap, std::string input[], bool verbose) {

		for (int i = 0; i <= input->length(); i++) {
			LineIntoMap(inMap, input[i], verbose);
		}

	}

	/// Calls LineIntoMap on each line of a string.
	void StringIntoMap(VariantMap& inMap, std::string input, bool verbose) {
		std::string temp;
		std::stringstream tempStream;
		tempStream << input;

		while (std::getline(tempStream, temp)) {
			LineIntoMap(inMap, temp, verbose);
		}

	}

	/// Calls LineIntoMap on each line of a file.
	void FileIntoMap(VariantMap& inMap, std::string fileName, bool verbose) {
		std::fstream configFile(fileName);
		std::string temp;
		while (std::getline(configFile, temp)) {
			LineIntoMap(inMap, temp, verbose);
		}
	}

#pragma endregion

}