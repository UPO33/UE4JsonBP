#pragma once
 
#include "Modules/ModuleManager.h"
#include "CoreMinimal.h"
#include "Json.h"


#include "JsonBP.generated.h"



class FJsonBPModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

UENUM(BlueprintType)
enum class EJsonType : uint8
{
	JSON_None,
	JSON_Null,
	JSON_String,
	JSON_Number,
	JSON_Boolean,
	JSON_Array,
	JSON_Object
};

/*
an instance of this class represent a json value (null, boolean, number, string, ...)
use UJsonValue::Make to create the instances  
*/
UCLASS(BlueprintType, Transient, NotBlueprintable)
class JSONBP_API UJsonValue : public UObject
{
	GENERATED_BODY()

private:
	EJsonType JsonType;

public:
	UJsonValue();

	UPROPERTY(BlueprintReadWrite)
	FString ValueString;
	UPROPERTY(BlueprintReadWrite)
	bool ValueBool;
	UPROPERTY(BlueprintReadWrite)
	float ValueNumber;
	UPROPERTY(BlueprintReadWrite)
	TArray<UJsonValue*> ValueArray;
	UPROPERTY(BlueprintReadWrite)
	TMap<FString, UJsonValue*> ValueObject;
	
	
	//returns true if this is a json string
	UFUNCTION(BlueprintPure)
	bool GetValueAsString(FString& value);
	//returns true if this is a json boolean 
	UFUNCTION(BlueprintPure)
	bool GetValueAsBoolean(bool& value);
	//returns true if this is a json number
	UFUNCTION(BlueprintPure)
	bool GetValueAsNumber(float& value);
	//returns true if this is a json array
	UFUNCTION(BlueprintPure)
	bool GetValueAsArray(TArray<UJsonValue*>& value);
	//returns true if this is a json object
	UFUNCTION(BlueprintPure)
	bool GetValueAsObject(TMap<FString, UJsonValue*>& value);

	UFUNCTION(BlueprintPure)
	static UJsonValue* MakeString(const FString& value);
	UFUNCTION(BlueprintPure)
	static UJsonValue* MakeNumber(float value);
	UFUNCTION(BlueprintPure)
	static UJsonValue* MakeBoolean(bool value);
	UFUNCTION(BlueprintPure)
	static UJsonValue* MakeNull();
	UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm="value"))
	static UJsonValue* MakeArray(const TArray<UJsonValue*>& value);
	UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm="value"))
	static UJsonValue* MakeObject(const TMap<FString, UJsonValue*>& value);
	//parse the string and make a json from it. returns null of failed.
	UFUNCTION(BlueprintPure)
	static UJsonValue* MakeFromString(const FString& value);

	
	//returns true if this is a JsonObject and field was set.
	UFUNCTION(BlueprintCallable)
	bool SetFieldValue(const FString& field, const UJsonValue* value);
	UFUNCTION(BlueprintCallable)
	bool SetFieldString(const FString& field, const FString& value);
	UFUNCTION(BlueprintCallable)
	bool SetFieldNumber(const FString& field, float value);
	UFUNCTION(BlueprintCallable)
	bool SetFieldBoolean(const FString& field, bool value);
	UFUNCTION(BlueprintCallable)
	bool SetFieldNull(const FString& field);
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="value"))
	bool SetFieldArray(const FString& field, const TArray<UJsonValue*>& value);
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "value"))
	bool SetFieldObject(const FString& field, const TMap<FString, UJsonValue*>& value);



	//returns the value of the specified field if any.
	UFUNCTION(BlueprintPure)
	UJsonValue* GetFieldValue(const FString& field) const;
	//return true if this json object has the specified string field
	UFUNCTION(BlueprintPure)
	bool GetFieldValueString(const FString& field, FString& value) const;
	//return true if this json object has the specified number field
	UFUNCTION(BlueprintPure)
	bool GetFieldValueNumber(const FString& field, float& value) const;
	//return true if this json object has the specified boolean field
	UFUNCTION(BlueprintPure)
	bool GetFieldValueBoolean(const FString& field, bool& value) const;
	//return true if this json object has the specified array field
	UFUNCTION(BlueprintPure)
	bool GetFieldValueArray(const FString& field, TArray<UJsonValue*>& value) const;
	//return true if this json object has the specified object field
	UFUNCTION(BlueprintPure)
	bool GetFieldValueObject(const FString& field, TMap<FString, UJsonValue*>& value) const;



	UFUNCTION(BlueprintCallable)
	void SetValueString(const FString& value);
	UFUNCTION(BlueprintCallable)
	void SetValueBoolean(bool value);
	UFUNCTION(BlueprintCallable)
	void SetValueNumber(float value);
	UFUNCTION(BlueprintCallable)
	void SetValueNull();
	UFUNCTION(BlueprintCallable)
	void SetValueArray(const TArray<UJsonValue*>& value);
	UFUNCTION(BlueprintCallable)
	void SetValueObject(const TMap<FString, UJsonValue*>& value);

	UFUNCTION(BlueprintCallable)
	void Clear();

	UFUNCTION(BlueprintPure)
	EJsonType GetType() const { return JsonType; }

	//return an string containing json
	UFUNCTION(BlueprintPure)
	FString ToString(bool bPretty) const;

	TSharedPtr<FJsonValue> ToCPPVersion() const;
	static UJsonValue* MakeFromCPPVersion(TSharedPtr<FJsonValue> value);
	
	static void Test0();
};

JSONBP_API FString HelperStringifyJSON(TSharedPtr<FJsonValue> jsValue, bool bPretty = false);
JSONBP_API TSharedPtr<FJsonValue> HelperParseJSON(const FString& jsonValue);