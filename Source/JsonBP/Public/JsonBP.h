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
	
	

	UFUNCTION(BlueprintCallable)
	bool GetValueAsString(FString& value);
	UFUNCTION(BlueprintCallable)
	bool GetValueAsBoolean(bool& value);
	UFUNCTION(BlueprintCallable)
	bool GetValueAsNumber(float& value);
	UFUNCTION(BlueprintCallable)
	bool GetValueAsArray(TArray<UJsonValue*>& value);
	UFUNCTION(BlueprintCallable)
	bool GetValueAsObject(TMap<FString, UJsonValue*>& value);

	UFUNCTION(BlueprintPure)
	static UJsonValue* MakeString(const FString& value);
	UFUNCTION(BlueprintPure)
	static UJsonValue* MakeNumber(float value);
	UFUNCTION(BlueprintPure)
	static UJsonValue* MakeBoolean(bool value);
	UFUNCTION(BlueprintPure)
	static UJsonValue* MakeNull();
	UFUNCTION(BlueprintPure)
	static UJsonValue* MakeArray(const TArray<UJsonValue*>& value);
	UFUNCTION(BlueprintPure)
	static UJsonValue* MakeObject(const TMap<FString, UJsonValue*>& value);
	UFUNCTION(BlueprintPure)
	static UJsonValue* MakeFromString(const FString& jsonValue);

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
	UFUNCTION(BlueprintCallable)
	bool SetFieldArray(const FString& field, const TArray<UJsonValue*>& value);

	UFUNCTION(BlueprintPure)
	UJsonValue* GetFieldValue(const FString& field) const;
	UFUNCTION(BlueprintPure)
	bool GetFieldValueString(const FString& field, FString& value) const;
	UFUNCTION(BlueprintPure)
	bool GetFieldValueNumber(const FString& field, float& value) const;
	UFUNCTION(BlueprintPure)
	bool GetFieldValueBoolean(const FString& field, bool& value) const;


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

	UFUNCTION(BlueprintPure)
	FString ToString(bool bPretty) const;

	TSharedPtr<FJsonValue> ToCPPVersion() const;
	static UJsonValue* MakeFromCPPVersion(TSharedPtr<FJsonValue> value);
	
	static void Test0();

};

JSONBP_API FString HelperStringifyJSON(TSharedPtr<FJsonValue> jsValue, bool bPretty);
JSONBP_API FString HelperStringifyJSON(TSharedPtr<FJsonValue> jsValue, bool bPretty);