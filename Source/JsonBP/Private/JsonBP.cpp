// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "JsonBP.h"
#include "JsonObject.h"


void FJsonBPModule::StartupModule()
{
	UJsonValue::Test0();
}

void FJsonBPModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FJsonBPModule, JsonBP)





TSharedPtr<FJsonValue> HelperParseJSON(const FString& jsonValue)
{
	static FString Str_null("null");
	static FString Str_true("true");
	static FString Str_false("false");

	//the Decerilize() itself only handle [] or {}. so for basic types we should do it ourself
	{

		if (jsonValue.Equals(Str_null))
			return MakeShared<FJsonValueNull>();

		if (jsonValue.Equals(Str_true))
			return MakeShared<FJsonValueBoolean>(true);

		if (jsonValue.Equals(Str_false))
			return MakeShared<FJsonValueBoolean>(false);

		if (jsonValue.IsNumeric())
			return MakeShared<FJsonValueNumber>(FCString::Atof(*jsonValue));

		//check if its string type. its string if first and last characters are "
		//#TODO we don't check the characters inside of ""
		if (jsonValue.Len() >= 2 && jsonValue[0] == TCHAR('"') && jsonValue[jsonValue.Len() - 1] == TCHAR('"'))
			return MakeShared<FJsonValueString>(jsonValue.Mid(1, jsonValue.Len() - 2));
	}

	{
		//this could be another solution
		////Deserialize function only parse [] or {} so we trick
		//FString arrayedValue = FString::Printf(TEXT("[%s]"), *jsonValue);
	}

	TSharedPtr<FJsonValue> jsValue;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(jsonValue);
	if (!FJsonSerializer::Deserialize(JsonReader, jsValue) || !jsValue.IsValid())
	{
		return nullptr;
	}

	return jsValue;
}

FString HelperStringifyJSON(TSharedPtr<FJsonValue> jsValue, bool bPretty)
{
	check(jsValue.IsValid());

	if (jsValue->Type == EJson::Null)
		return FString("null");

	if (jsValue->Type == EJson::Number || jsValue->Type == EJson::Boolean)
		return jsValue->AsString();
	
	//#Fixme
	if (jsValue->Type == EJson::String)
		return FString::Printf(TEXT(R"("%s")"), *jsValue->AsString());

	FString OutputString;
	if (bPretty)
	{

		TSharedRef<TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
		if (!FJsonSerializer::Serialize(jsValue, FString(), Writer))
			return FString();
	}
	else
	{
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> > Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
		if (!FJsonSerializer::Serialize(jsValue, FString(), Writer))
			return FString();
	}
	return OutputString;
}


UJsonValue* MakeJsonValue()
{
	static FName NameJsonValue("JsonValue");
	//return NewObject<UJsonValue>((UObject*)GetTransientPackage(), UJsonValue::StaticClass(), NameJsonValue, RF_Transient);
	return NewObject<UJsonValue>();
}

UJsonValue::UJsonValue()
{
	JsonType = EJsonType::JSON_None;
}

bool UJsonValue::GetValueAsString(FString& value)
{
	if (JsonType == EJsonType::JSON_String) 
	{
		value = ValueString;
		return true;
	}
	return false;
}

bool UJsonValue::GetValueAsBoolean(bool& value)
{
	if (JsonType == EJsonType::JSON_Boolean)
	{
		value = ValueBool;
		return true;
	}
	return false;
}

bool UJsonValue::GetValueAsNumber(float& value)
{
	if (JsonType == EJsonType::JSON_Number)
	{
		value = ValueNumber;
		return true;
	}
	return false;
}

bool UJsonValue::GetValueAsArray(TArray<UJsonValue*>& value)
{
	if (JsonType == EJsonType::JSON_Array)
	{
		value = ValueArray;
		return true;
	}
	return false;
}

bool UJsonValue::GetValueAsObject(TMap<FString, UJsonValue*>& value)
{
	if (JsonType == EJsonType::JSON_Object)
	{
		value = ValueObject;
		return true;
	}
	return false;
}

UJsonValue* UJsonValue::MakeString(const FString& value)
{
	UJsonValue* pObj = MakeJsonValue();
	pObj->SetValueString(value);
	return pObj;
}

UJsonValue* UJsonValue::MakeNumber(float value)
{
	UJsonValue* pObj = MakeJsonValue();
	pObj->SetValueNumber(value);
	return pObj;
}

UJsonValue* UJsonValue::MakeBoolean(bool value)
{
	UJsonValue* pObj = MakeJsonValue();
	pObj->SetValueBoolean(value);
	return pObj;
}

UJsonValue* UJsonValue::MakeNull()
{
	UJsonValue* pObj = MakeJsonValue();
	pObj->SetValueNull();
	return pObj;
}

UJsonValue* UJsonValue::MakeArray(const TArray<UJsonValue*>& value)
{
	UJsonValue* pObj = MakeJsonValue();
	pObj->SetValueArray(value);
	return pObj;
}

UJsonValue* UJsonValue::MakeObject(const TMap<FString, UJsonValue*>& value)
{
	UJsonValue* pObj = MakeJsonValue();
	pObj->SetValueObject(value);
	return pObj;
}

UJsonValue* UJsonValue::MakeFromString(const FString& jsonValue)
{
	TSharedPtr<FJsonValue> parsed = HelperParseJSON(jsonValue);
	if (!parsed)
		return nullptr;

	return MakeFromCPPVersion(parsed);
}

bool UJsonValue::SetFieldValue(const FString& field, const UJsonValue* value)
{
	if (JsonType != EJsonType::JSON_Object)
		return false;

	ValueObject.Add(field, (UJsonValue*)value);
	return true;
}

bool UJsonValue::SetFieldString(const FString& field, const FString& value)
{
	return SetFieldValue(field, MakeString(value));
}

bool UJsonValue::SetFieldNumber(const FString& field, float value)
{
	return SetFieldValue(field, MakeNumber(value));
}

bool UJsonValue::SetFieldBoolean(const FString& field, bool value)
{
	return SetFieldValue(field, MakeBoolean(value));
}

bool UJsonValue::SetFieldNull(const FString& field)
{
	return SetFieldValue(field, MakeNull());
}

bool UJsonValue::SetFieldArray(const FString& field, const TArray<UJsonValue*>& value)
{
	return SetFieldValue(field, MakeArray(value));
}

bool UJsonValue::SetFieldObject(const FString& field, const TMap<FString, UJsonValue*>& value)
{
	return SetFieldValue(field, MakeObject(value));
}

UJsonValue* UJsonValue::GetFieldValue(const FString& field) const
{
	if (JsonType != EJsonType::JSON_Object)
		return nullptr;

	auto ppValue = ValueObject.Find(field);
	if (!ppValue)
		return nullptr;

	return *ppValue;
}

bool UJsonValue::GetFieldValueString(const FString& field, FString& value) const
{
	auto pValue = GetFieldValue(field);
	if (!pValue)
		return false;

	return pValue->GetValueAsString(value);
}

bool UJsonValue::GetFieldValueNumber(const FString& field, float& value) const
{
	auto pValue = GetFieldValue(field);
	if (!pValue)
		return false;

	return pValue->GetValueAsNumber(value);
}

bool UJsonValue::GetFieldValueBoolean(const FString& field, bool& value) const
{
	auto pValue = GetFieldValue(field);
	if (!pValue)
		return false;

	return pValue->GetValueAsBoolean(value);
}

bool UJsonValue::GetFieldValueArray(const FString& field, TArray<UJsonValue*>& value) const
{
	auto pValue = GetFieldValue(field);
	if (!pValue)
		return false;

	return pValue->GetValueAsArray(value);
}

bool UJsonValue::GetFieldValueObject(const FString& field, TMap<FString, UJsonValue*>& value) const
{
	auto pValue = GetFieldValue(field);
	if (!pValue)
		return false;

	return pValue->GetValueAsObject(value);
}

void UJsonValue::SetValueString(const FString& value)
{
	Clear();
	JsonType = EJsonType::JSON_String;
	ValueString = value;
}

void UJsonValue::SetValueBoolean(bool value)
{
	Clear();
	JsonType = EJsonType::JSON_Boolean;
	ValueBool = value;
}

void UJsonValue::SetValueNumber(float value)
{
	Clear();
	JsonType = EJsonType::JSON_Number;
	ValueNumber = value;
}

void UJsonValue::SetValueNull()
{
	Clear();
	JsonType = EJsonType::JSON_Null;
}

void UJsonValue::SetValueArray(const TArray<UJsonValue*>& value)
{
	Clear();
	JsonType = EJsonType::JSON_Array;
	ValueArray = value;
}

void UJsonValue::SetValueObject(const TMap<FString, UJsonValue*>& value)
{
	Clear();
	JsonType = EJsonType::JSON_Object;
	ValueObject = value;
}

void UJsonValue::Clear()
{
	JsonType = EJsonType::JSON_None;

	ValueArray.Reset();
	ValueObject.Reset();
	ValueBool = false;
	ValueNumber = 0;
	ValueString = FString();
}

FString UJsonValue::ToString(bool bPretty) const
{
	TSharedPtr<FJsonValue> jsValue = ToCPPVersion();
	if (!jsValue)
		return FString();

	return HelperStringifyJSON(jsValue, bPretty);
}

TSharedPtr<FJsonValue> UJsonValue::ToCPPVersion() const
{
	switch (JsonType)
	{
	case EJsonType::JSON_None:
		return nullptr;
	case EJsonType::JSON_Null:
		return MakeShared<FJsonValueNull>();
	case EJsonType::JSON_String:
		return MakeShared<FJsonValueString>(ValueString);
	case EJsonType::JSON_Number:
		return MakeShared<FJsonValueNumber>(ValueNumber);
	case EJsonType::JSON_Boolean:
		return MakeShared<FJsonValueBoolean>(ValueBool);
	case EJsonType::JSON_Array:
	{
		TArray<TSharedPtr<FJsonValue>> elements;
		for (const UJsonValue* pElement : ValueArray)
		{
			elements.Add(pElement ? pElement->ToCPPVersion() : nullptr);
		}

		return MakeShared<FJsonValueArray>(elements);
	}
	case EJsonType::JSON_Object:
	{
		TSharedPtr<FJsonObject> jsObject = MakeShared<FJsonObject>();
		for (const auto& pair : ValueObject) 
		{
			jsObject->Values.Add(pair.Key, pair.Value ? pair.Value->ToCPPVersion() : nullptr);
		}
		return MakeShared<FJsonValueObject>(jsObject);
	}
	}

	return nullptr;
}

UJsonValue* UJsonValue::MakeFromCPPVersion(TSharedPtr<FJsonValue> value)
{
	if (!value)
		return nullptr;


	switch (value->Type)
	{
	case EJson::None : return nullptr;
	case EJson::Null: return MakeNull();
	case EJson::String: return MakeString(value->AsString());

	case EJson::Number: 
		//#NOTE double is not supported in blueprint we should cast to float
		return MakeNumber((float)value->AsNumber());

	case EJson::Boolean: return MakeBoolean(value->AsBool());

	case EJson::Array:
	{
		const TArray<TSharedPtr<FJsonValue>>& jsArray = value->AsArray();

		UJsonValue* pArray = MakeJsonValue();
		check(pArray);
		pArray->JsonType = EJsonType::JSON_Array;
		pArray->ValueArray.AddZeroed(jsArray.Num());
		check(pArray->ValueArray.Num() == jsArray.Num());

		for(int i = 0; i < jsArray.Num(); i++)
			pArray->ValueArray[i] = MakeFromCPPVersion(jsArray[i]);
		
		return pArray;
	}
	case EJson::Object:
	{
		const TSharedPtr<FJsonObject>& jsObject = value->AsObject();

		UJsonValue* pObject = MakeJsonValue();
		check(pObject);
		pObject->JsonType = EJsonType::JSON_Object;
		
		for (const auto& pair : jsObject->Values)
			pObject->ValueObject.Add(pair.Key, MakeFromCPPVersion(pair.Value));

		return pObject;
	}

	}

	return nullptr;
}

#if UE_BUILD_SHIPPING
void UJsonValue::Test0()
{

}
#else
void UJsonValue::Test0()
{
	{
		UJsonValue* madeFromStr = MakeFromString(FString(R"({asd)"));
		check(madeFromStr == nullptr);
	}
	{
		FString theNumber("123");
		UJsonValue* madeFromStr = MakeFromString(theNumber);
		ensureAlways(madeFromStr && madeFromStr->GetType() == EJsonType::JSON_Number);
		ensureAlways(madeFromStr->ToString(false).Equals(theNumber));
	}
	{
		UJsonValue* madeFromStr = MakeFromString(FString("false"));
		ensureAlways(madeFromStr && madeFromStr->GetType() == EJsonType::JSON_Boolean);
		ensureAlways(madeFromStr->ToString(false).Equals("false"));
	}
	{
		UJsonValue* madeFromStr = MakeFromString(FString("true"));
		ensureAlways(madeFromStr && madeFromStr->GetType() == EJsonType::JSON_Boolean);
		ensureAlways(madeFromStr->ToString(false).Equals("true"));
	}
	{
		UJsonValue* madeFromStr = MakeFromString(FString("null"));
		ensureAlways(madeFromStr && madeFromStr->GetType() == EJsonType::JSON_Null);
		ensureAlways(madeFromStr->ToString(false).Equals("null"));
	}
	{
		FString theJSON(R"("string")");
		UJsonValue* madeFromStr = MakeFromString(theJSON);
		ensureAlways(madeFromStr && madeFromStr->GetType() == EJsonType::JSON_String);
		ensureAlways(madeFromStr->ValueString.Equals(FString("string")));
		ensureAlways(madeFromStr->ToString(false).Equals(theJSON));
	}
	{
		FString theJSON(R"("")");
		UJsonValue* madeFromStr = MakeFromString(theJSON);
		ensureAlways(madeFromStr && madeFromStr->GetType() == EJsonType::JSON_String);
		ensureAlways(madeFromStr->ValueString.IsEmpty());
		ensureAlways(madeFromStr->ToString(false).Equals(theJSON));
	}
	{
		UJsonValue* madeFromStr = MakeFromString(FString("{}"));
		ensureAlways(madeFromStr && madeFromStr->GetType() == EJsonType::JSON_Object);
		ensureAlways(madeFromStr->ToString(false).Equals("{}"));
		
	}
	{
		UJsonValue* madeFromStr = MakeFromString(FString("[]"));
		ensureAlways(madeFromStr && madeFromStr->GetType() == EJsonType::JSON_Array);
		ensureAlways(madeFromStr->ToString(false).Equals("[]"));
	}
	{
		FString strJson0 = R"({"str":"ssss","num":22,"bool":true,"array":[1,2,3],"null":null,"obj":{"a":"a","b":"b"}})";
		UJsonValue* madeFromStr = MakeFromString(strJson0);
		check(madeFromStr);
		TSharedPtr<FJsonValue> ccped = madeFromStr->ToCPPVersion();
		UJsonValue* remade = MakeFromCPPVersion(ccped);
		FString reStringed = remade->ToString(false);
		FString reStringedPretty = remade->ToString(true);

		UJsonValue* remadeFrom_reStringed = MakeFromString(reStringed);
		FString reReStringed = remadeFrom_reStringed->ToString(false);
		ensureAlways(reStringed.Equals(reReStringed));
		ensureAlways(true);
	}
	{
	}
	//ensureAlways(false);
}
#endif
