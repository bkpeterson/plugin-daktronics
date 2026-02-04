#pragma once

#include <string>

#include <obs-module.h>
#include <dak-source-support.h>

#include "dak-sport-data.hpp"
#include "dak-data-utils.hpp"

class DAKFilter {
public:
	DAKFilter(obs_data_t *settings, obs_source_t *obs_source);
	~DAKFilter();

	obs_source_t *_source;

	static const uint32_t DAK_VISIBLE = 1;
	static const uint32_t DAK_TEXT = 2;
	static const uint32_t DAK_COLOR = 3;

	void SetValue(std::string value);

	uint32_t GetFilterType();
	uint32_t GetIndex();
	std::string GetSport();
	const char *GetSourceName();
	void Update(obs_data_t *settings);

	static void *Create(obs_data_t *settings, obs_source_t *obs_source);
	static void Destroy(void *data);
	static void Update(void *data, obs_data_t *settings);
	static const char *GetName(void *);
	static void Render(void *data, gs_effect_t *effect);
	static obs_properties_t *GetProperties(void *data);
	static void GetDefaults(obs_data_t *settings);

	//void _render();
	obs_properties_t *_getProperties();
	void doColorProps(obs_properties_t *props, std::string paramName);
	bool _DAKSportChanged(obs_properties_t *props, obs_property_t *property, obs_data_t *settings);

	static bool DAKSportChanged(obs_properties_t *props, obs_property_t *property, obs_data_t *settings);
	static bool DAKFilterChanged(void *data, obs_properties_t *props, obs_property_t *property,
				     obs_data_t *settings);
	static bool DAKParamChanged(void *data, obs_properties_t *props, obs_property_t *property,
				    obs_data_t *settings);
	static bool DAKColorParamChanged(void *data, obs_properties_t *props, obs_property_t *property,
					 obs_data_t *settings);

	void doColorParamChanged(obs_data_t *settings);

private:
	void _render();

	std::string _sport;
	uint32_t _index;
	std::string _internalValue;
	uint32_t _filterType;
	bool _invertLogic;
	std::string _paramName;
	std::string _paramColorName;
	obs_property_type _paramType;
	obs_property_t *_colorProp;
	bool _colorActive;

	int _color;
	int _origColor;
	int _colorAlpha;
	int _origColorAlpha;
};
