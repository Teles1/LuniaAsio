# Lunia
Lunia Emulator
Recommendation is to make sure that you have the integration enabled to make your life easier with .libs and .objs.

# Vcpkg

1. git pull https://github.com/microsoft/vcpkg
2. go to your environment variables
3. Add a new variable called `VCPKG_DEFAULT_TRIPLET` with the value `x64-windows`
4. Run bootstrap-vcpkg.bat in a cmd from inside the newly created vcpkg folder.
5. After everything is done downloading and compiling run -> vcpkg integrate install
6. Now it's time to install our libraries. Make sure to check if they are being compiled in x64 it'll tell in the cmd
7. Go to cmd and run -> vcpkg integrate project

# Packages being used in our project.
- vcpkg install asio:x64-windows
- vcpkg install spdlog:x64-windows
- vcpkg install fmt:x64-windows 
- vcpkg install cpr:x64-windows
- vcpkg install expat:x64-windows [ on add this you need reimplement in your project bro ]


# Singletons
Logger::GetInstance() -> Gives you access to our logging system  
Lunia::Net::UserRegistry::GetInstance()  -> To give you access to user management 
