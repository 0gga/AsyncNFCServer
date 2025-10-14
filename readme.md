>## **Dependencies**<br>
>
>>Run `git submodule update --init --recursive` in root to clone external dependencies:
>
>
>>### >**Current dependencies:**
>>
>>-  **boost.asio**  
>>   Read documentation for info on sub-dependencies  
>>-  **nlohmann/json**  

>## **CLI Syntax & Cmdlets**<br>
>
>>### **Syntax**<br>
>>
>>_**To make the CLI commands parseable, they must be syntactically correct:**_  
>>CLI command arguments must always be seperated by a single space,  
>>and must never have any trailing spaces:  
>>`newUser ogga 3`  
>
>>Names must be either snake_case or camelCase:  
>>`newUser ooga_booga 3`  
>>`newUser oogaBooga 3`  
>>The CLI parser will look for both and ensure all entries are removed.  
>
>>### **Cmdlets**<br>
>>
>>- Create a new user: `newUser 0gga 3`
>>- Remove an existing user: `rmUser 0gga`
>>- Shutdown the CLI connection: `shutdown`
>>- Get client logs: `getLog`
>>
>>
>>#### **Possible additions:**
>>
>>- Get user specific logs: `getULog 0gga`
>>- Add new reader with accessLevel, client- and CLI port: `addReader 1 9000 9001`

>## **Compilation**<br>
>>### **Cross Compilation**<br>
>>Native cross compilation is currently not supported.  
>>Use CLion WSL toolchain for easy cross compilation.

>## **Additions before 1.0**<br>
>>- The system currently doesn't write systemwide logs nor user logs.