Thanks for considering contributing to the project. Before you get to contributing please read over our code guidelines to avoid having us redo your work later. 

These guidelines are a WIP so feel free to contribute to it if you want. I'm aware that these guidelines "go against" the UE4 guidelines, but this is the way I like my code. Feel free to clone and change it for your own project. ;)

## Formatting
- Tabs with 4 spaces
- {} on new lines
- Classes are to be prefixed with `HM` for example `AHMCharacterBase`
- No spacing between () for example `if ( cond )`
- Where there are private variables make a Getter/Setter where it makes sense
- Comment all code as much as possible
- Use m_ as a prefix for variables in the class see below - code uses the [Hungarian Notation](https://en.wikipedia.org/wiki/Hungarian_notation#Examples) format for code. (just read the codebase and you'll see)
- When exposing variables to Blueprint make sure to add the meta tag with DisplayName



### Code example
```cpp
class MyClass
{
    /** The ID of x */
    int32 m_ID;

    /** Name of x */
    FString m_Name;

    /** Is x ... */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "bTest"))
    bool m_bTest;

public:
    /** Get the ID of x */
    UFUNCTION(...)
    FORCEINLINE int32 GetID() const { return m_ID; }

    /** Get the Name of x */
    UFUNCTION(...)
    FORCEINLINE FString GetName() const { return m_Name; }

    /**
	 * Set the Name of x
	 * 
	 * @param const FString& NewName The Name that you want to set for x
	 */
    UFUNCTION(...)
    void SetName(const FString& NewName);
};

// CPP
void MyClass::SetName(const FString& NewName)
{
    if (NewName.IsEmpty()) 
    {
        return;
    }

    m_Name = NewName;
}
```

