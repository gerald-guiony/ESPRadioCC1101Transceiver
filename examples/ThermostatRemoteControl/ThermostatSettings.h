//************************************************************************************************************************
// ThermostatSettings.h
// Version 1.0 July, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <array>
#include <map>
#include <algorithm>


namespace TRegulationMode
{
	enum Type 											{ HG=0, ECO, CONF };
	static const std::array <Type, 3> All	 		= 	{ HG, ECO, CONF };
	// C++11 only, as it uses std::initializer_list
	static const std::map <Type, String> ToStr  	= {	{ HG,		"HG"	},
														{ ECO,		"ECO"	},
														{ CONF,		"CONF"	} };

	inline bool FromStr (String str, Type & regulMode) {
		str.trim ();
		str.toUpperCase ();
		auto it = std::find_if (TRegulationMode::ToStr.begin(), TRegulationMode::ToStr.end(),
			[&str] (const std::pair <TRegulationMode::Type, String> & p) {
				return p.second == str;
			}
		);
		if (it == TRegulationMode::ToStr.end()) return false;

		regulMode = it->first;
		return true;
	}
};


struct ThermostatSettings {
	// C++11 only, as it uses std::initializer_list
	std::map <TRegulationMode::Type, uint8_t>	tempRegulationValue = {
													{ TRegulationMode::HG,		5	},
													{ TRegulationMode::ECO,		17	},
													{ TRegulationMode::CONF,	19	}
												};
	TRegulationMode :: Type						tempRegulationMode	= TRegulationMode::HG;
	int8_t										ambientTemp			= 0;
};
