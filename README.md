# Lunia
Lunia Emulator

# Vcpkg - Recommendation is making sure that you have the integration on to make your life easier with .libs and .objs.

1. git pull https://github.com/microsoft/vcpkg
2. go to your environment variables
3. Add a new variable called `VCPKG_DEFAULT_TRIPLET` with the value `x64-windows`
4. Run bootstrap-vcpkg.bat in a cmd from inside the newly created vcpkg folder.
5. After everything is done downloading and compiling run -> vcpkg integrate install
6. Now it's time to install our libraries. Make sure to check if they are being compiled in x64 it'll tell in the cmd

#Packages being used in our project.
vcpkg install asio:x64-windows
vcpkg install spdlog:x64-windows
vcpkg install fmt:x64-windows 
vcpkg install cpr:x86-windows

#Logger::GetInstance() from anywhere to give you access to logs that get outputed to the console.