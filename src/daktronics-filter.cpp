#include "daktronics-filter.hpp"

DAKFilter::DAKFilter(obs_data_t *settings, obs_source_t *source) : _source(source)
{
	_filterType = DAKFilter::DAK_TEXT;
	_internalValue = "";
	Update(this, settings);
}

DAKFilter::~DAKFilter()
{
	// Nothing to do
}

void *DAKFilter::Create(obs_data_t *settings, obs_source_t *source)
{
	auto context = new DAKFilter(settings, source);

	DAKDataUtils::AddFilter(context);
	return context;
}

void DAKFilter::Destroy(void *data)
{
	auto instance = static_cast<DAKFilter *>(data);
	DAKDataUtils::RemoveFilter(instance);
}

const char *DAKFilter::GetName(void *type_data)
{
	UNUSED_PARAMETER(type_data);
	return "Daktronics Scoreboard Filter";
}

const char *DAKFilter::GetSourceName()
{
	if (!_source)
		return "";

	const char *srcId = obs_source_get_id(_source);
	return obs_source_get_display_name(srcId);
}

void DAKFilter::SetValue(std::string newValue)
{
	_internalValue = newValue;

	if (!_source)
		return;

	obs_source_t *targetSource = obs_filter_get_parent(_source);
	obs_data_t *sourceData = obs_source_get_settings(targetSource);
	std::string sourceName = obs_source_get_name(targetSource);

	switch (_filterType) {
	case DAKFilter::DAK_VISIBLE:
		if (_internalValue == "" || _internalValue == "0:00" || _internalValue == ":00" ||
		    _internalValue == "0.0" || _internalValue == "0") {
			obs_source_set_enabled(targetSource, _invertLogic);
			std::string actionVerb = _invertLogic ? "Show  " : "Hide  ";
			DAKLogger::instance().emit logMessage(
				QString::fromStdString(actionVerb + "\"" + sourceName + "\""));
		} else {
			obs_source_set_enabled(targetSource, !_invertLogic);
			std::string actionVerb = _invertLogic ? "Hide  " : "Show  ";
			DAKLogger::instance().emit logMessage(
				QString::fromStdString(actionVerb + "\"" + sourceName + "\""));
		}
		break;

	case DAKFilter::DAK_TEXT:
		obs_data_set_string(sourceData, _paramName.c_str(), _internalValue.c_str());
		obs_source_update(targetSource, sourceData);
		DAKLogger::instance().emit logMessage(
			QString::fromStdString("Set \"" + sourceName + "\": " + _internalValue));
		break;

	case DAKFilter::DAK_COLOR:
		if (_internalValue == "" || _internalValue == "0:00" || _internalValue == ":00" ||
		    _internalValue == "0.0" || _internalValue == "0") {
			if (_paramType == OBS_PROPERTY_COLOR) {
				if (_invertLogic) {
					obs_data_set_int(sourceData, _paramColorName.c_str(), _color);
					DAKLogger::instance().emit logMessage(
						QString::fromStdString("Highlight color \"" + sourceName + "\""));
				} else {
					obs_data_set_int(sourceData, _paramColorName.c_str(), _origColor);
					DAKLogger::instance().emit logMessage(
						QString::fromStdString("Reset color \"" + sourceName + "\""));
				}
			} else if (_paramType == OBS_PROPERTY_COLOR_ALPHA) {
				if (_invertLogic) {
					obs_data_set_int(sourceData, _paramColorName.c_str(), _colorAlpha);
					DAKLogger::instance().emit logMessage(
						QString::fromStdString("Highlight color \"" + sourceName + "\""));
				} else {
					obs_data_set_int(sourceData, _paramColorName.c_str(), _origColorAlpha);
					DAKLogger::instance().emit logMessage(
						QString::fromStdString("Reset color \"" + sourceName + "\""));
				}
			}
		} else {
			if (_paramType == OBS_PROPERTY_COLOR) {
				if (_invertLogic) {
					obs_data_set_int(sourceData, _paramColorName.c_str(), _origColor);
					DAKLogger::instance().emit logMessage(
						QString::fromStdString("Reset color \"" + sourceName + "\""));
				} else {
					obs_data_set_int(sourceData, _paramColorName.c_str(), _color);
					DAKLogger::instance().emit logMessage(
						QString::fromStdString("Highlight color \"" + sourceName + "\""));
				}
			} else if (_paramType == OBS_PROPERTY_COLOR_ALPHA) {
				if (_invertLogic) {
					obs_data_set_int(sourceData, _paramColorName.c_str(), _origColorAlpha);
					DAKLogger::instance().emit logMessage(
						QString::fromStdString("Reset color \"" + sourceName + "\""));
				} else {
					obs_data_set_int(sourceData, _paramColorName.c_str(), _colorAlpha);
					DAKLogger::instance().emit logMessage(
						QString::fromStdString("Highlight color \"" + sourceName + "\""));
				}
			}
		}

		obs_source_update(targetSource, sourceData);

		break;
	}

	obs_data_release(sourceData);
}

uint32_t DAKFilter::GetIndex()
{
	return _index;
}

std::string DAKFilter::GetSport()
{
	return _sport;
}

void DAKFilter::Render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(effect);

	auto &instance = *static_cast<DAKFilter *>(data);
	instance._render();
}

void DAKFilter::_render()
{
	obs_source_skip_video_filter(_source);
}

void DAKFilter::Update(void *data, obs_data_t *settings)
{
	auto &instance = *static_cast<DAKFilter *>(data);
	instance.Update(settings);
}

void DAKFilter::Update(obs_data_t *settings)
{
	if (!_source)
		return;

	DAKDataUtils::RemoveFilter(this);

	_sport = (std::string)obs_data_get_string(settings, "dak_sport_type");
	_index = (uint32_t)obs_data_get_int(settings, "dak_field_list");
	_filterType = (uint32_t)obs_data_get_int(settings, "dak_filter_list");
	_invertLogic = obs_data_get_bool(settings, "dak_invert_logic");
	_paramName = (std::string)obs_data_get_string(settings, "dak_param_list");
	_paramColorName = (std::string)obs_data_get_string(settings, "dak_param_list_color");

	_color = (int)obs_data_get_int(settings, "dak_color");
	_colorAlpha = (int)obs_data_get_int(settings, "dak_color_alpha");

	obs_source_t *targetSource = obs_filter_get_parent(_source);

	if (!targetSource)
		return;

	obs_properties_t *sourceProps = obs_source_properties(targetSource);
	obs_property_t *targetProp = obs_properties_get(sourceProps, _paramColorName.c_str());
	_paramType = obs_property_get_type(targetProp);
	obs_properties_destroy(sourceProps);

	if (_filterType == DAKFilter::DAK_COLOR) {
		obs_data_t *sourceData = obs_source_get_settings(targetSource);
		_origColor = (int)obs_data_get_int(sourceData, _paramColorName.c_str());
		_origColorAlpha = (int)obs_data_get_int(sourceData, _paramColorName.c_str());
		obs_data_release(sourceData);
	} else {
		_origColor = _color;
		_origColorAlpha = _colorAlpha;
	}

	DAKDataUtils::AddFilter(this);
}

void DAKFilter::GetDefaults(obs_data_t *settings)
{
	obs_data_set_default_string(settings, "dak_sport_type", "Basketball");
	obs_data_set_default_int(settings, "dak_field_list", 1);
	obs_data_set_default_int(settings, "dak_filter_list", DAKFilter::DAK_VISIBLE);
	obs_data_set_default_bool(settings, "dak_invert_logic", false);
	obs_data_set_default_string(settings, "dak_param_list", "");
	obs_data_set_default_string(settings, "dak_parama_list_color", "");
	obs_data_set_default_int(settings, "dak_color", 0xFFFFFF);
	obs_data_set_default_int(settings, "dak_color_alpha", 0xFFFFFFFF);
}

obs_properties_t *DAKFilter::GetProperties(void *data)
{
	auto &instance = *static_cast<DAKFilter *>(data);
	return instance._getProperties();
}

obs_properties_t *DAKFilter::_getProperties()
{
	obs_source_t *targetSource = obs_filter_get_parent(_source);
	obs_properties_t *sourceProps = obs_source_properties(targetSource);

	obs_properties_t *props = obs_properties_create();

	obs_property_t *sport_type =
		obs_properties_add_list(props, "dak_sport_type", "Sport", OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_STRING);

	DAKDataUtils::PopulateSportProps(sport_type);

	obs_property_set_modified_callback(sport_type, DAKFilter::DAKSportChanged);

	obs_properties_add_list(props, "dak_field_list", "Scoreboard Data Field", OBS_COMBO_TYPE_LIST,
				OBS_COMBO_FORMAT_INT);

	obs_property_t *filter_type = obs_properties_add_list(props, "dak_filter_list", "Filter Type",
							      OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);
	obs_property_list_add_int(filter_type, "Show/Hide", DAKFilter::DAK_VISIBLE);
	obs_property_list_add_int(filter_type, "Update Text", DAKFilter::DAK_TEXT);
	obs_property_list_add_int(filter_type, "Change Color", DAKFilter::DAK_COLOR);

	obs_property_set_modified_callback2(filter_type, DAKFilter::DAKFilterChanged, this);

	obs_property_t *invert =
		obs_properties_add_bool(props, "dak_invert_logic", "Apply filter when field is blank (vs not blank)");

	obs_property_t *param_type = obs_properties_add_list(props, "dak_param_list", "Property to Modify",
							     OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_STRING);

	for (obs_property_t *prop = obs_properties_first(sourceProps); prop != NULL; obs_property_next(&prop)) {
		const char *prop_name = obs_property_name(prop);
		if (obs_property_get_type(prop) == OBS_PROPERTY_TEXT)
			obs_property_list_add_string(param_type, prop_name, prop_name);
	}

	obs_property_t *param_type_color = obs_properties_add_list(props, "dak_param_list_color", "Property to Modify",
								   OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_STRING);

	for (obs_property_t *prop = obs_properties_first(sourceProps); prop != NULL; obs_property_next(&prop)) {
		const char *prop_name = obs_property_name(prop);
		if (obs_property_get_type(prop) == OBS_PROPERTY_COLOR ||
		    obs_property_get_type(prop) == OBS_PROPERTY_COLOR_ALPHA)
			obs_property_list_add_string(param_type_color, prop_name, prop_name);
	}

	obs_property_set_modified_callback2(param_type_color, DAKFilter::DAKParamChanged, this);

	obs_property_t *colorChoice = obs_properties_add_color(props, "dak_color", "Filter color");
	obs_property_t *alphaChoice = obs_properties_add_color_alpha(props, "dak_color_alpha", "Filter color");

	std::string info2 =
		"<a href=\"https://github.com/bkpeterson/plugin-daktronics\">Daktronics Source</a> (1.0) by bkpeterson";
	obs_properties_add_text(props, "plugin_info2", info2.c_str(), OBS_TEXT_INFO);

	obs_property_set_visible(invert, false);
	obs_property_set_visible(param_type, false);
	obs_property_set_visible(param_type_color, false);
	obs_property_set_visible(colorChoice, false);
	obs_property_set_visible(alphaChoice, false);

	obs_properties_destroy(sourceProps);

	return props;
}

void DAKFilter::doColorProps(obs_properties_t *props, std::string paramName)
{
	obs_source_t *targetSource = obs_filter_get_parent(_source);
	obs_properties_t *sourceProps = obs_source_properties(targetSource);

	obs_property_t *targetProp = obs_properties_get(sourceProps, paramName.c_str());
	obs_property_t *color = obs_properties_get(props, "dak_color");
	obs_property_t *color_alpha = obs_properties_get(props, "dak_color_alpha");

	switch (obs_property_get_type(targetProp)) {
	case OBS_PROPERTY_COLOR:
		obs_property_set_visible(color, true);
		obs_property_set_visible(color_alpha, false);
		break;

	case OBS_PROPERTY_COLOR_ALPHA:
		obs_property_set_visible(color, false);
		obs_property_set_visible(color_alpha, true);
		break;

	default:
		obs_property_set_visible(color, false);
		obs_property_set_visible(color_alpha, false);
	}

	obs_properties_destroy(sourceProps);
}

bool DAKFilter::DAKSportChanged(obs_properties_t *props, obs_property_t *property, obs_data_t *settings)
{
	UNUSED_PARAMETER(property);

	std::string sport_type = (std::string)obs_data_get_string(settings, "dak_sport_type");

	obs_property_t *list = obs_properties_get(props, "dak_field_list");
	obs_property_list_clear(list);

	DAKSportData *sportData = DAKDataUtils::getSportData(sport_type);
	sportData->PopulateFieldProps(list);

	return true;
}

bool DAKFilter::DAKFilterChanged(void *data, obs_properties_t *props, obs_property_t *property, obs_data_t *settings)
{
	UNUSED_PARAMETER(property);

	uint32_t filter_type = (uint32_t)obs_data_get_int(settings, "dak_filter_list");
	obs_property_t *invert = obs_properties_get(props, "dak_invert_logic");
	obs_property_t *list = obs_properties_get(props, "dak_param_list");
	obs_property_t *listColor = obs_properties_get(props, "dak_param_list_color");
	obs_property_t *color = obs_properties_get(props, "dak_color");
	obs_property_t *color_alpha = obs_properties_get(props, "dak_color_alpha");

	switch (filter_type) {
	case DAKFilter::DAK_VISIBLE:
		obs_property_set_visible(invert, true);
		obs_property_set_visible(list, false);
		obs_property_set_visible(listColor, false);
		obs_property_set_visible(color, false);
		obs_property_set_visible(color_alpha, false);
		break;

	case DAKFilter::DAK_TEXT:
		obs_property_set_visible(invert, false);
		obs_property_set_visible(list, true);
		obs_property_set_visible(listColor, false);
		obs_property_set_visible(color, false);
		obs_property_set_visible(color_alpha, false);
		break;

	case DAKFilter::DAK_COLOR:
		obs_property_set_visible(invert, true);
		obs_property_set_visible(list, false);
		obs_property_set_visible(listColor, true);
		DAKFilter::DAKParamChanged(data, props, list, settings);
		break;
	}

	return true;
}

bool DAKFilter::DAKParamChanged(void *data, obs_properties_t *props, obs_property_t *property, obs_data_t *settings)
{
	UNUSED_PARAMETER(property);

	DAKFilter *instance = (DAKFilter *)data;
	std::string paramName = obs_data_get_string(settings, "dak_param_list_color");
	instance->doColorProps(props, paramName);

	return true;
}
