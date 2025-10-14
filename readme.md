## **Dependencies**<br>

Run `git submodule update --init --recursive` in root to clone external dependencies:

Current dependencies:<br>
**Boost.Asio**

## **CLI Syntax & Cmdlets**<br>

### **Syntax**<br>

CLI command arguments must always be seperated by a single space,  
and must never have any trailing spaces i.e.  
`newUser 0gga 3`


### **Cmdlets**<br>

- Create a new user: `newUser 0gga 3`
- Remove an existing user: `rmUser 0gga`
- Shutdown the CLI connection: `shutdown`
- Get client logs: `getLog`


#### **Possible additions:**

- Get user specific logs: `getULog 0gga`
- Add new reader with accessLevel, client- and CLI port: `addReader 1 9000 9001`
