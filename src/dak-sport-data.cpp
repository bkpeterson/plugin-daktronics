#include "dak-sport-data.hpp"

DAKSportData::DAKSportData(std::string sport, uint32_t dataSize) : _sport(sport), _dataSize(dataSize) {}

DAKSportData::~DAKSportData()
{
	std::vector<std::unique_ptr<DAKFieldData>>().swap(_fieldData);
}

void DAKSportData::AddFieldData(uint32_t index, std::string field, uint32_t length)
{
	std::unique_ptr<DAKFieldData> newData = std::make_unique<DAKFieldData>();
	newData->_sport = _sport;
	newData->_index = index;
	newData->_field = field;
	newData->_length = length;
	_fieldData.push_back(std::move(newData));
}

std::string DAKSportData::GetSportName()
{
	return _sport;
}

uint32_t DAKSportData::GetDataSize()
{
	return _dataSize;
}

DAKFieldData *DAKSportData::GetField(uint32_t fieldIdx)
{
	for (auto const &fieldData : _fieldData) {
		if (fieldData->_index == fieldIdx)
			return fieldData.get();
	}

	return nullptr;
}

uint32_t DAKSportData::GetFieldLen(uint32_t fieldIdx)
{
	for (auto const &fieldData : _fieldData) {
		if (fieldData->_index == fieldIdx)
			return fieldData->_length;
	}

	return 0;
}

void DAKSportData::PopulateFieldProps(obs_property_t *fieldList)
{
	for (auto const &fieldData : _fieldData) {
		obs_property_list_add_int(fieldList, fieldData->_field.c_str(), fieldData->_index);
	}
}
